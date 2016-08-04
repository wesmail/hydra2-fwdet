#include "honlineclientservercom.h"
#include "TThread.h"
#include "TStopwatch.h"

#include "hades.h"
#include "hdatasource.h"

#include <iostream>
#include <vector>
using namespace std;

ClassImp(HOnlineClientServerCom);

Int_t   HOnlineClientServerCom::fEvtCtInterval = 100;
Float_t HOnlineClientServerCom::fSpeed         = 0;
Bool_t  HOnlineClientServerCom::isInit         = kFALSE;
Bool_t  HOnlineClientServerCom::doStop         = kFALSE;


HOnlineClientServerCom::HOnlineClientServerCom(TString name, TString hostname, Int_t port) {
    this->SetName(name);
    monitor = 0;
    serversocket = 0;
    fPort = port;
    fHostname = hostname;
    fTimeout = 1000;
    fDebug = kFALSE;
    fEvtCtInterval = 100;
    fSpeed         = 0;
    eventLoopThread = NULL;
    isInit = kFALSE;
    doStop = kFALSE;
    masterClient = NULL;
}
//--------------------------------------------------------------------------
Bool_t HOnlineClientServerCom::init() {
    serversocket = new TServerSocket(fPort, kFALSE);  // before true
    if (!serversocket->IsValid()) {
	return kFALSE;
    }
    monitor = new TMonitor;
    monitor->Add(serversocket);
    return kTRUE;
}
//--------------------------------------------------------------------------
HOnlineClientServerCom::~HOnlineClientServerCom()
{
    // destroys the object. the eventloop thread
    // is deleted too
    TThread* self = TThread::Self();
    self->Lock();
    serversocket->Close();
    delete monitor;
    socketlist.Delete();
    delete serversocket;
    serversocket = 0;
    monitor = 0;

    TThread::Delete(eventLoopThread);
    delete eventLoopThread;

    self->UnLock();
}
//--------------------------------------------------------------------------
Bool_t HOnlineClientServerCom::sendListToClient(TSocket* socket, TList* list)
{
    if (!socket->IsValid()) return kFALSE;

    TMessage answer(kMESS_OBJECT);
    answer.WriteObject(list);

    if (socket->Send(answer) <= 0) return kFALSE;

    return kTRUE;
}
//--------------------------------------------------------------------------
Bool_t HOnlineClientServerCom::sendHistToClient(TSocket* socket, HOnlineMonHistAddon* hist) {
    if (!socket->IsValid()) return kFALSE;

    TMessage answer(kMESS_OBJECT);
    answer.WriteObject(hist);

    if (socket->Send(answer) <= 0) return kFALSE;

    return kTRUE;
}
//--------------------------------------------------------------------------
Bool_t HOnlineClientServerCom::sendTextToClient(TSocket* socket, TString msg) {
    if (!socket->IsValid()) return kFALSE;

    TMessage answer(kMESS_STRING);
    answer.WriteTString(msg);

    if (socket->Send(answer) <= 0) return kFALSE;

    return kTRUE;
}
//--------------------------------------------------------------------------
Bool_t HOnlineClientServerCom::processClientRequests()
{
    // evalutates the list of connections to the server

    TList* wr = new TList();  // sockets waiting for writing
    TList* rr = new TList();  // sockets waiting for reading

    // number of sockets waiting
    Int_t numSockets = monitor->Select(rr, wr, fTimeout);

    if(fDebug) TThread::Printf("HOnlineClientServerCom:: number of sockets waiting: %d", numSockets);

    if (numSockets == 0) {
	// nothing to do
	return kFALSE;
    }

    // sockets are waiting for reading
    for(Int_t i=0;i<rr->LastIndex()+1;i++) {
	// process sockets
	TSocket* s = (TSocket*) rr->At(i);

	if(s->IsA() == TServerSocket::Class()) {
	    // open connection
	    TSocket *sock = ((TServerSocket*)s)->Accept();
	    monitor->Add(sock); // Add socket to monitor
	    socketlist.Add(sock); // Add socket to list
	    TThread::Printf("HOnlineClientServerCom:: connection from client %s accepted",(sock->GetInetAddress()).GetHostName());

            if(!masterClient) masterClient = sock;

	} else {
	    TMessage* mess = 0;
	    Int_t rcode = s->Recv(mess);

	    if (rcode <= 0) {
		// close connection
		TString client = (s->GetInetAddress()).GetHostName();
		monitor->Remove(s);
		socketlist.Remove(s);
		s->Close();
		delete s;
		s = 0;
		delete mess;
		mess = 0;
		TThread::Printf("HOnlineClientServerCom:: connection from client %s closed",client.Data());
	    } else {
		// process request
		if (mess->What() == kMESS_STRING) {
		    TString str;
		    mess->ReadTString(str);
		    TObjString* objstr = new TObjString();
		    objstr->SetString(str);
		    clientRequests.Add(objstr, s);
		}

		delete mess;
		mess = 0;
	    }
	}
    }

    delete wr;
    delete rr;

    return kTRUE;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
Int_t HOnlineClientServerCom::processClients(TList& histpool) {
    //-----------------------------------------------------------------------
    // process client requests and send histos to client
    // the known commands ( start with : CMD ..) are processed
    // first and second the request for the histograms to
    // be transfered to the client. The requests are handled
    // asynchoniously to the eventloop (the eventloop is not stopped
    // while processing).


    while(isInit == kFALSE) {
	TThread::Printf("HOnlineClientServerCom:: Waiting for first eventLoop ...");
	TThread::Sleep(1);
    }


    while(1)
    {

	if (processClientRequests())
	{

	    TIterator* myiter = clientRequests.MakeIterator();
	    TObjString* mapkey;
	    TString histname2send;
	    TSocket* socket;
	    HOnlineMonHistAddon* hist2send;

	    //-----------------------------------------------------------------------
            // first process commands
	    while ((mapkey=(TObjString*)myiter->Next()) != 0)
	    {
		socket = (TSocket*) clientRequests.GetValue(mapkey);
		TString recvstring = mapkey->GetString();

		if(recvstring.BeginsWith("CMD "))
		{

		    //-----------------------------------------------------------------------
		    // PROCESSING COMMAND

		    //-----------------------------------------------------------------------
		    // COMMAND: CMD speed
                    if (recvstring.CompareTo("CMD speed") == 0) {
			sendTextToClient(socket,Form("%5.2f evts/s",fSpeed));
		    }
		    //-----------------------------------------------------------------------
		    // COMMAND: CMD filename
                    if (recvstring.CompareTo("CMD filename") == 0) {
                        TString filename = "no file yet";
			HDataSource* source = gHades->getDataSource();
			if(source){
			    filename = gHades->getDataSource()->getCurrentFileName();
			}
			sendTextToClient(socket,filename);
		    }
		    //-----------------------------------------------------------------------
		    // COMMAND: CMD ismaster
		    if (recvstring.CompareTo("CMD ismaster") == 0) {
			TThread::Printf("HOnlineClientServerCom:: received command '%s' from host '%s'", recvstring.Data(), (socket->GetInetAddress()).GetHostName());

			TString message = "no";
			if(socket == masterClient ) {
			    message = "yes";
			    TThread::Printf("HOnlineClientServerCom:: This client is considered as Master client");
			} else {
                           TThread::Printf("HOnlineClientServerCom:: This client is considered as Slave client");
			}
			sendTextToClient(socket,message);
		    }
		    //-----------------------------------------------------------------------
		    // COMMAND: CMD setmaster
                    if (recvstring.CompareTo("CMD setmaster") == 0) {
			TThread::Printf("HOnlineClientServerCom:: received command '%s' from host '%s'", recvstring.Data(), (socket->GetInetAddress()).GetHostName());
                        TThread::Printf("HOnlineClientServerCom:: This client is set as Master client");
			masterClient = socket;
			TString message = "ok";
			sendTextToClient(socket,message);
		    }
		    //-----------------------------------------------------------------------
		    // COMMAND: CMD histlist
		    if (recvstring.CompareTo("CMD histlist") == 0) {
			// create and send TList* with histogram names to client
			TList* listhistname = new TList();
			for(Int_t i=0;i<histpool.LastIndex()+1;i++) {
			    TString tmpname = ((HOnlineMonHistAddon*)histpool.At(i))->GetName();
			    TObjString* tmpobj = new TObjString();
			    tmpobj->SetString(tmpname);
			    listhistname->Add(tmpobj);
			}
			if (!sendListToClient(socket, listhistname)) {
			    TThread::Printf("HOnlineClientServerCom:: Error: could not send list of histogram names to client");
			}
			listhistname->Delete();
			listhistname = 0;
		    }

		    //-----------------------------------------------------------------------
		    // COMMAND: CMD stop
		    if (recvstring.CompareTo("CMD stop") == 0) {

			// received command, process command
			TThread::Printf("HOnlineClientServerCom:: received command '%s' from host '%s'", recvstring.Data(), (socket->GetInetAddress()).GetHostName());

			if(masterClient!=socket){
 
			    TThread::Printf("HOnlineClientServerCom:: THIS CLIENT IS NOT THE MASTER! WILL BE IGNORED!");
			    sendTextToClient(socket,"IGNORED");
			} else {


			    // stop the server
			    TThread::Printf("HOnlineClientServerCom:: stopping server... Received command 'stop' from host '%s'", (socket->GetInetAddress()).GetHostName());

			    // stop the eventloop thread
			    doStop = kTRUE;
			    Int_t ct = 0;

			    TThread::EState state = eventLoopThread->GetState();
			    while(state != TThread::kCanceledState && ct < 10) {
				ct++;
				TThread::Printf("HOnlineClientServerCom:: Waiting for eventLoop to finish .... n time %i",ct);
				state = eventLoopThread->GetState();
				TThread::Sleep(1);
			    }
			    if(ct == 10) {
				TThread::Printf("HOnlineClientServerCom:: EventLoop seems to be hanging .... finishing anyway.");
			    }
			    TThread::Ps();

			    return -1;
			}
		    }

		    //-----------------------------------------------------------------------
		    // COMMAND: CMD resetall
		    if (recvstring.CompareTo("CMD resetall") == 0) {

			TThread::Printf("HOnlineClientServerCom:: reset all histograms ... Received command 'resetall' from host '%s'", (socket->GetInetAddress()).GetHostName());

			if(masterClient!=socket){
 
			    TThread::Printf("HOnlineClientServerCom:: THIS CLIENT IS NOT THE MASTER! WILL BE IGNORED!");
			    sendTextToClient(socket,"IGNORED");

			} else {
			    // reset histograms
			    TThread* self = TThread::Self();
			    self->Lock();

			    HOnlineMonHistAddon* addon;
			    TIterator* it  = histpool.MakeIterator();
			    while( ((addon = (HOnlineMonHistAddon*) it->Next()) != 0) ) {
				if(addon->getActive() != 0) addon->reset(0,0); // full reset no matter if is resetable
			    }
			    delete it;
			    sendTextToClient(socket,"resetdone");
			    self->UnLock();
			}
		    }

		    delete (TObjString*) (clientRequests.Remove(mapkey));
		}
	    }
	    //-----------------------------------------------------------------------



	    //-----------------------------------------------------------------------
            // second process histogram requests
	    myiter->Reset();

 	    while ((mapkey=(TObjString*)myiter->Next()) != 0)
	    {
		socket = (TSocket*) clientRequests.GetValue(mapkey);
		TString recvstring = mapkey->GetString();

		if(recvstring.BeginsWith("CMD ")) {
                   // do nothing
		} else {

		    //-----------------------------------------------------------------------
		    // PROCESSING HISTOGRAM REQUEST

		    // received no command, it must be the name of a histogram
		    // which should be send to the client
		    TString histname2send = recvstring;
		    hist2send = (HOnlineMonHistAddon*) histpool.FindObject(histname2send.Data());
		    if (hist2send == 0) {
			TString errmsg = "ERROR: Can't find requested histogram '";
			errmsg += histname2send;
			errmsg += "' in list of active histograms.";
			TThread::Printf("HOnlineClientServerCom:: %s",errmsg.Data());

			if (!sendTextToClient(socket, errmsg)) {
			    // error occurred
			    TThread::Printf("HOnlineClientServerCom:: ERROR: could not send text %s to client %s '",errmsg.Data(),(socket->GetInetAddress()).GetHostName());
			}
			delete (TObjString*) (clientRequests.Remove(mapkey));
		    } else {
			if (!sendHistToClient(socket, hist2send)) {
			    // error occurred
			    TThread::Printf("HOnlineClientServerCom:: ERROR: could not send histogram %s to client %s",hist2send->GetName(),(socket->GetInetAddress()).GetHostName());
			}
			delete (TObjString*) (clientRequests.Remove(mapkey));
		    }
		}
	    } // end mapping keys
	    //-----------------------------------------------------------------------


	    delete myiter;


	} // end process clients
	//-----------------------------------------------------------------------


    } // end while

    return 0;
}
//--------------------------------------------------------------------------

/* -------------------------------------------------------------------------- */
void* HOnlineClientServerCom::ThreadEventLoop(void* arg) {
    
    // This function is executed in a separat thread.
    // it will run the HADES eventloop in an enless loop
    // for 1 event and call the fill functions provided
    // by the list. The loop is finished if the last event
    // is reached or the client request for stop server.

    vector < fillptrHOnlineClientServerCom >& fillfunctions = *((vector < fillptrHOnlineClientServerCom >*) arg);
    Int_t count = 0;
    Bool_t rc = 0;

    TStopwatch timer;
    timer.Start();
    while(rc == 0 && !doStop) {

	//------------------- EVENTLOOP---------------------------------------
	if(!gHades->eventLoop(1)) {

	    TThread::Printf("HOnlineClientServerCom:: Reached last Event!");
	    rc = -1 ; continue ;
	}
	count++;
	if(count%100 == 0){
           Float_t time = timer.RealTime();
	   if(time > 0.0001) fSpeed = 100./time;
           else              fSpeed = 0;
	   timer.Reset();
	   timer.Start();
	}

        if(count>=1) isInit = kTRUE;

	if(fEvtCtInterval!=-1 && count%fEvtCtInterval == 0 ){ TThread::Printf("Event %i",count);}

	//TThread* self = TThread::Self();
	//self->Lock();

	for(UInt_t i = 0; i < fillfunctions.size(); i++){ // call the fill functionwith event number
	    fillptrHOnlineClientServerCom func = fillfunctions[i];
	    (*func)(count);
	}

	//self->UnLock();
	//--------------------------------------------------------------------

    }
    TThread::Printf("HOnlineClientServerCom:: eventLoopThread finsihed!");
    return 0;
}

void HOnlineClientServerCom::eventLoop(vector < fillptrHOnlineClientServerCom >*  fillfunctions)
{
    // thread to run the HADES eventloop in backgorund
    // while HOnlineClientServerCom communicates with the
    // clients

     // start eventloop thread
    if (!eventLoopThread) {
        eventLoopThread = new TThread("eventloopThread", ( void(*) (void*) )&ThreadEventLoop, (void*) fillfunctions);
        eventLoopThread->Run();
        TThread::Ps();
    }
}

