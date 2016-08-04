//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
// HOnlineMonClientMain
//
// Main class of the HADES Online Monitoring Client.
// It contains a separat Thread in which the histograms are updated.
// The canvases are updated in the main thread, because the
// canvas-update is not threadsafe.
////////////////////////////////////////////////////////////////////////////

#include "honlinemonclientmain.h"

#include "honlinemonclientxml.h"
#include "honlinemonclientmainwin.h"
#include "honlinemonclientdet.h"
#include "honlinemonclienttabwin.h"
#include "honlinemonclientsimplewin.h"
#include "honlinemonclienttab.h"
#include "honlinemonclientcanvas.h"
#include "honlinemonclienthist.h"

#include "TClass.h"
#include "TObject.h"
#include "TPad.h"
#include "TTimeStamp.h"
#include "TObjString.h"
//#include "TGMsgBox.h"

#include <cstdlib>

ClassImp(HOnlineMonClientMain);

 Int_t    HOnlineMonClientMain::AutoSaveInterval       = -1;  // auto save interval in minutes.
 Int_t    HOnlineMonClientMain::AutoSaveIntervalOnline = -1;  // auto save interval in minutes.
 time_t   HOnlineMonClientMain::lastSaved        = 0;
 time_t   HOnlineMonClientMain::lastSavedOnline  = 0;
TThread*  HOnlineMonClientMain::mainThread       = NULL;
ULong64_t HOnlineMonClientMain::lastSumBytes     = 0;
Int_t     HOnlineMonClientMain::isMasterClient   = -1;

/* -------------------------------------------------------------------------- */
HOnlineMonClientMain::HOnlineMonClientMain(TString filename) {
    // filename is the name of the xml configuration file
    
    configfile = filename;
    socket = 0;
    updateComplete = kTRUE;
    updateThread = 0;
    UpdateInterval = 4;
    serverhost = "";
    serverport = 0;\
    doReconnect = 1;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::Init(Bool_t sendCmdOnly, TString host, Int_t port, TString cmd) {
    // Initialize program.
    // If there is a command, execute it and exit.
    // Else start client program with graphical user interface
    
    // check if there is a command to execute
    if (sendCmdOnly) {
        // connect and send command to server
        Bool_t validcmd = kFALSE;
        if ( Connect(host, port) ) {
            TThread::Printf("connected to server '%s' on port '%d'.", host.Data(), port);
        } else {
            TThread::Printf("failed to connect to server '%s' on port '%d'.", host.Data(), port);
            return;
        }
        
        /* CMD list */
        if (cmd.CompareTo("list")==0) {
            validcmd = kTRUE;
            TThread::Printf("list of available histograms from server '%s' on port '%d'", host.Data(), port);
            TThread::Printf("=========================================================");
            TList* histolist = GetListOfHistograms();
            if (histolist) {
                for(Int_t i=0;i<histolist->LastIndex()+1;i++) {
                    TString tempname = ((TObjString*) histolist->At(i))->GetString();
                    TThread::Printf("%s\n",tempname.Data());
                }
                TThread::Printf("=========================================================");
                histolist->Delete();
                delete histolist;
                histolist = 0;
            } else {
                TThread::Printf("ERROR: Received invalid list of histograms from server for command '%s'", cmd.Data());
                return;
            }
        }
        
        /* CMD stop */
        if (cmd.CompareTo("stop")==0) {
            validcmd = kTRUE;
            TThread::Printf("stop server '%s' on port '%d'", host.Data(), port);
            TString answer = SendCmdToServer("CMD stop");
        }
        
        socket->Close();
        delete socket;
        socket = 0;
        
        /* invalid cmd */
        if (!validcmd) {
            // invalid command
            TThread::Printf("Error: Unknown command '%s'", cmd.Data());
            return;
        }
        
        return;
    }
    
    
    // no command to execute, START CLIENT
    TThread::Printf("Create MainClient");
     
    clientmainwin = new HOnlineMonClientMainWin();
    clientmainwin->SetMainClient(this);

    CreateClientConfig(configfile);

    mainThread = TThread::Self();

    clientmainwin->CreateMainWin(this);
    
    if ( Connect(serverhost, serverport) ) {
        TThread::Printf("connected to server '%s' on port '%d'.", serverhost.Data(), serverport);
    } else {
        TThread::Printf("failed to connect to server '%s' on port '%d'.", serverhost.Data(), serverport);
        exit(1);
    }

    TString master = SendCmdToServer("CMD ismaster");
    if(master == "yes") {
	SetIsMasterClient(kTRUE);
	TThread::Printf("Running as Master client");
    } else {
        TThread::Printf("Running as Slave client");
    }

    // start update thread
    if (!updateThread) {
	TThread::Printf("start Update");
        updateThread = new TThread("updateThread", ( void(*) (void*) )&ThreadUpdateHists, (void*) this);
        updateThread->Run();
        TThread::Ps();
    }

     mainThread->Lock();
 
    //---------------------------------------------
    // create all detector GUIS
    TThread::Printf("Create Detetctor GUIs");
    for (Int_t i=0;i<listDetectors.LastIndex()+1;i++) {
	TString detname = listDetectors.At(i)->GetName();
	HOnlineMonClientDet *det = (HOnlineMonClientDet*) listDetectors.At(i);
	TGCheckButton *checkbutton = (TGCheckButton *)clientmainwin->listControlbuttons.FindObject(detname);
	if(checkbutton->IsOn()){
	  CreateDetGui(det);
	}
    }
    //---------------------------------------------

    mainThread->UnLock();

    // canvas/subpad update function should NOT be called
    // in a Thread, because it is NOT threadsafe

    TThread::Printf("Before Loop");
    lastSaved       = time(NULL);
    lastSavedOnline = time(NULL);

    // canvas / subpad update loop
    while(kTRUE) {

	mainThread->Lock();

        gSystem->ProcessEvents();
        
        for (Int_t i=0;i<listPads.LastIndex()+1;i++) {
            if (listPads.At(i)->IsA() == TCanvas::Class()) {
                // unsplit canvas, update the canvas itself
                TCanvas* cnv = (TCanvas*) listPads.At(i);
		cnv->cd();
                HOnlineMonClientHist* hist = (HOnlineMonClientHist*) listHists.FindObject(pad2hist[cnv->GetName()]);
                if (hist&&hist->IsActive() && hist->histo) {
                    if (cnv->IsModified()) {
                        cnv->Update();
                    }
                }
            } else {
                // split canvas, update the subpads
                TPad* subpad = (TPad*) listPads.At(i);
                subpad->cd();
                HOnlineMonClientHist* hist = (HOnlineMonClientHist*) listHists.FindObject(pad2hist[subpad->GetName()]);
                if (hist&&hist->IsActive() && hist->histo) {
                    if (subpad->IsModified()) {
                        subpad->Update();
                    }
                }
            }
        }
	mainThread->UnLock();
	TThread::Sleep(0,1000000*1); // sleep ms  to give the UpdateHists Thread a chance :-)
    }
}
/* -------------------------------------------------------------------------- */
void* HOnlineMonClientMain::ThreadUpdateHists(void* arg) {
    
    // This function is executed in a separat thread.
    // It will update all histograms in the global histogram list.
    // The code between TThread::Lock() and TThread::UnLock() is
    // executed without interruption. The Thread will only be locked
    // before TThread::Lock() or after TThread::UnLock().
    // This ensures that the histogram update is performed in one step
    // without interruption.
    
    HOnlineMonClientMain *clientmain = (HOnlineMonClientMain*) arg;
    
    while(1) {
        gSystem->Sleep(clientmain->UpdateInterval*1000);
        //gSystem->ProcessEvents();

	mainThread->Lock();

	//--------------------------------------------------------------------------
	if(clientmain->updateComplete) {
	    clientmain->updateComplete = kFALSE;
	    clientmain->UpdateHists();
	    clientmain->updateComplete = kTRUE;
	}
 	//--------------------------------------------------------------------------


	//--------------------------------------------------------------------------
	if(AutoSaveInterval > 0){
	    time_t now = time(NULL);
	    if(now - lastSaved > AutoSaveInterval*60){

		clientmain->HandleSnapshotSave();

		lastSaved = time(NULL);
	    }
	}
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	if(AutoSaveIntervalOnline > 0){
	    time_t now = time(NULL);
	    if(now - lastSavedOnline > AutoSaveIntervalOnline*60){

		clientmain->HandleSnapshotSaveOnline();

		lastSavedOnline = time(NULL);
	    }
	}
	//--------------------------------------------------------------------------

	mainThread->UnLock();
   }
    return 0;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::UpdateHists() {
    
    // update all hists in the global list of histograms.
    // This function is called by the thread-function
    // and is executed in the update-Thread.


    TString filename = SendCmdToServer("CMD filename");
    TString speed    = SendCmdToServer("CMD speed");
    filename = gSystem->BaseName(filename.Data());

    ULong64_t totBytesRecieved = socket->GetSocketBytesRecv();
    Float_t bytesPerSecond = (0.000001*(totBytesRecieved-lastSumBytes))/UpdateInterval;
    lastSumBytes = totBytesRecieved;

    clientmainwin->fTextFilename->SetText(filename.Data());
    clientmainwin->fTextSpeed   ->SetText(speed.Data());
    clientmainwin->fDataRate    ->SetText(Form("%10.4f Mb/s",bytesPerSecond));


    for (Int_t i=0;i<listPads.LastIndex()+1;i++) {
	if (listPads.At(i)->IsA() == TCanvas::Class()) {
            // unsplit canvas
            TCanvas* cnv = (TCanvas*) listPads.At(i);
	    // change to the canvas in which the histogram must be displayed
	    if(cnv == 0) TThread::Printf("UpdateHists() : canvas == 0");
	    cnv->cd();
            HOnlineMonClientHist* hist = (HOnlineMonClientHist*) listHists.FindObject(pad2hist[cnv->GetName()]);
	    //if(hist == 0) TThread::Printf("UpdateHists() : hist == 0"); // needs investigation ???
	    if (hist&&hist->IsActive() && hist->histo) {
                if (!UpdateHist(hist)) {
                    // error occurred
                    TThread::Printf("ERROR: Update of histogram '%s' failed.", hist->GetName());
		}
            }
        } else {
            // split canvas
            TPad* subpad = (TPad*) listPads.At(i);
	    if(subpad == 0) TThread::Printf("UpdateHists() : subpad == 0");
	    subpad->cd();
            HOnlineMonClientHist* hist = (HOnlineMonClientHist*) listHists.FindObject(pad2hist[subpad->GetName()]);
	    //if(hist == 0) TThread::Printf("UpdateHists() : hist == 0");  // needs investigation ???
	    if (hist&&hist->IsActive() && hist->histo) {
                if (!UpdateHist(hist)) {
                    // error occurred
                    TThread::Printf("ERROR: Update of histogram '%s' failed.", hist->GetName());
		}
	    }

        }
    }

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::CreateClientConfig(TString xmlfile) {
        // Parse configuration file
	
	HOnlineMonClientXML* xmlparser = new HOnlineMonClientXML();
        Bool_t r = xmlparser->ParseXMLFile(xmlfile, this);

        if (!r) {
            TThread::Printf("Error: could not parse xml file.");
            delete xmlparser;
            xmlparser = 0;
            exit(1);
        }
        
        delete xmlparser;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::CreateDetGui(HOnlineMonClientDet *detector) {
    // Create and show all windows from given detector

    mainThread->Lock();
    
    detector->CreateDet(this);
   
    // unlock update thread
    mainThread->UnLock();

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::DestroyDetGui(HOnlineMonClientDet *detector) {
    // Hide and destroy all windows from given detector
    mainThread->Lock();

    // the TCanvas objects will be deleted automatically, when TGMainFrame is destroyed.
    detector->DestroyDet(this);

    // unlock update thread
    mainThread->UnLock();
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::DestroySingleWindow(HOnlineMonClientSimpleWin *window) {
    // Hide and destroy a single simple window

    HOnlineMonClientDet *detector = (HOnlineMonClientDet *) listDetectors.FindObject(window->detectorname);
    if(detector) {
	TGCheckButton *button = 0;
	button = (TGCheckButton*) clientmainwin->listControlbuttons.FindObject(window->detectorname);
	if(button){
	    button->SetEnabled(kFALSE);

	    DestroyDetGui(detector);

	    button->SetEnabled(kTRUE);
	    button->SetOn(kFALSE);

	}
    }
    else { TThread::Printf("Error : DestroySingleWindow () retrieved NULL pointer for detector %s .",window->detectorname.Data());}

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::DestroySingleWindow(HOnlineMonClientTabWin *window) {
    // Hide and destroy a single tab window


    HOnlineMonClientDet *detector = (HOnlineMonClientDet *) listDetectors.FindObject(window->detectorname);
    if(detector) {

	TGCheckButton *button = (TGCheckButton*) clientmainwin->listControlbuttons.FindObject(window->detectorname);
	if(button){
	    button->SetEnabled(kFALSE);

	    DestroyDetGui(detector);

	    button->SetEnabled(kTRUE);
	    button->SetOn(kFALSE);
	}
    }
    else { TThread::Printf("Error : DestroySingleWindow () retrieved NULL pointer for detector %s .",window->detectorname.Data());}


}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleRefreshRateChange() {
    // GUI Event Handler to handle change of refresh rate
    SetUpdateInterval(clientmainwin->numberEntryRefreshRate->GetNumberEntry()->GetIntNumber());
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleAutoSaveRateChange() {
    // GUI Event Handler to handle change of refresh rate
    SetAutoSaveInterval(clientmainwin->numberEntryAutoSaveRate->GetNumberEntry()->GetIntNumber());
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleSendStopServer(){
    TString returnval = SendCmdToServer("CMD stop");
    TThread::Printf("server responded on command stop = %s !", returnval.Data());
    if(returnval != "IGNORED"){
	TThread::Printf("Close Monitor application !");
	HandleMainWindowClose();
    }
    if(returnval == "IGNORED"){
	Int_t retval = 0;
	new TGMsgBox(gClient->GetRoot(), clientmainwin->GetMainWindow(),
		     "ERROR MSG", "YOU ARE NOT RUNNING AS MASTER AND CANNOT STOP THE SERVER!"
		     ,kMBIconExclamation,kMBCancel,&retval);//,
    }

}

/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleSendResetHists(){
    TString returnval = SendCmdToServer("CMD resetall");
    TThread::Printf("server responded on command resetall = %s !", returnval.Data());
    if(returnval == "IGNORED"){
	Int_t retval = 0;
	new TGMsgBox(gClient->GetRoot(), clientmainwin->GetMainWindow(),
		     "ERROR MSG", "YOU ARE NOT RUNNING AS MASTER AND CANNOT RESET HISTOGRAMS!"
		     ,kMBIconExclamation,kMBCancel,&retval);//,
    }

}

/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleConnectionError()
{
    TThread::Printf("server responded connection Error!" );

    Int_t retval = -1;

    //clientmainwin->GetMainWindow()->Disconnect("CloseWindow()");
    //clientmainwin->GetMainWindow()->Connect("CloseWindow()", "HOnlineMonClientMain", this, "HandleTryClose()");
    //new TGMsgBox(gClient->GetRoot(), clientmainwin->GetMainWindow(),
    //    	 "ERROR MSG", "CONNECTION TO SERVER LOST!"
    //    	 ,kMBIconStop,kMBClose,&retval);
    TGMsgBox* box = NULL;
    if(doReconnect == 1){
	box = new TGMsgBox(gClient->GetRoot(), clientmainwin->GetMainWindow(),
			   "ERROR MSG", "CONNECTION TO SERVER LOST! \n PRESS YES TO EXIT OR NO TO WAIT FOR RECONNECT!"
			   ,kMBIconStop,kMBYes|kMBNo ,&retval);
    }

    if(doReconnect == 0) {
	HandleMainWindowClose();
        return;
    }

    while ( ReConnect(serverhost,serverport) == kFALSE ){
        gSystem->ProcessEvents();
	TThread::Sleep(1);
        gSystem->ProcessEvents();
	TThread::Sleep(1);
	TThread::Printf("Try to reconnect!" );
    }

    if( IsMasterClient() ) {

	TString message = SendCmdToServer("CMD setmaster");
	if(message == "ok") {
           TThread::Printf("Running as Master client");
	} else {
	    TThread::Printf("Running as Slave client");
	}
    }

    TThread::Printf("Succesfully reconnected to server %s , port %i!",serverhost.Data(),serverport);


    if(doReconnect == 1 && box && retval == -1 && retval != kMBClose){ // nothing done to the window or closed
	box->CloseWindow();
    }


    if(doReconnect == 1 && retval == kMBYes) {
	HandleMainWindowClose();
    }
}
/* -------------------------------------------------------------------------- */

void HOnlineMonClientMain::HandleTryClose()
{
    Int_t retval = 0;
    //clientmainwin->Connect("CloseWindow()", "TestMsgBox", this, "TryToClose()");
    new TGMsgBox(gClient->GetRoot(), clientmainwin->GetMainWindow(),
		 "ERROR MSG", "CANNOT CLOSE NOW, STILL A MESSAGE BOX OPEN!"
		 ,kMBIconExclamation,kMBCancel,&retval);//,
    HandleConnectionError();
}

/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleSnapshotSave() {
    // GUI Event Handler to handle snapshot button click
    // Save all active canvases in a ps file in the same folder as the program.
    // The filename contains a timestamp

    if (listPads.LastIndex() == -1) return; // return if no window is open

    Bool_t doQuit = kFALSE;

    mainThread->Lock();
    
    // loop over the pads, not over the canvases. But the print-function
    // prints the complete canvas into the file. If there is a split canvas,
    // the print-function prints the canvas several times (nx*ny) into the file.
    // Prevent this by adding the name of each printed canvas into this map and
    // check the map before each canvas is printed.

    map <TString, Bool_t> printedCanvases;
    
    // create timestamp
    UInt_t year = 0;
    UInt_t month = 0;
    UInt_t day = 0;
    UInt_t hour = 0;
    UInt_t min = 0;
    UInt_t sec = 0;
    
    TTimeStamp* timestamp = new TTimeStamp();
    timestamp->GetDate(kFALSE, 0, &year, &month, &day);
    timestamp->GetTime(kFALSE, 0, &hour, &min, &sec);
    delete timestamp;
    TString  path      = clientmainwin->fTextSnapshotPath->GetText();
    TString currentRun = clientmainwin->fTextFilename->GetText();

    if(path      .Contains("ERROR")) doQuit = kTRUE;
    if(currentRun.Contains("ERROR")) doQuit = kTRUE;

    if(!doQuit)
    {

	if(path != ""){
	    path += "/";
	    path.ReplaceAll("//","/");
	    if(gSystem->AccessPathName(path.Data()) != 0 ){ // path not found
		TThread::Printf("HandleSnapshotSave() : Path = %s not found! Will use currentdirectory.",path.Data());
		path = "";
	    }
	}
	TString currentRun = clientmainwin->fTextFilename->GetText();


	TString filename = "";
	filename.Form("%s_%d-%02d-%02d_%02d%02d%02d.ps", currentRun.Data(), year, month, day, hour, min, sec);

	TString pdf = filename;
	pdf.ReplaceAll(".ps",".pdf");

	TString input  = path + filename;
	TString output = path + pdf;


	// get first subpad
	TPad* subpad = (TPad*) listPads.At(0);

	// open file without printing anything
	subpad->GetCanvas()->Print(filename + TString("["));

	for (Int_t i=0;i<listPads.LastIndex()+1;i++) {
	    TPad* subpad = (TPad*) listPads.At(i);
	    TCanvas* cnv = subpad->GetCanvas();
	    if (!printedCanvases[cnv->GetName()]) {  // canvas is not yet printed
		cnv->Print(filename);
		printedCanvases[cnv->GetName()] = kTRUE;
	    }
	}

	// close file without printing anything
	subpad->GetCanvas()->Print(filename + TString("]"));



	if (gSystem->AccessPathName(filename.Data()) == 0 ){ // file exists
	    gSystem->Exec(Form("ps2pdf %s %s",filename.Data(),pdf.Data()));
	    if (gSystem->AccessPathName(pdf.Data()) == 0 ){
		gSystem->Exec(Form("rm -f %s",filename.Data()));
		if(path != "") gSystem->Exec(Form("mv %s %s/",pdf.Data(),path.Data()));
		TThread::Printf("HandleSnapshotSave() : Snapshot File has been saved in %s",Form("%s%s",path.Data(),pdf.Data()));
	    } else {
		TThread::Printf("HandleSnapshotSave() : Could not convert ps file!");
	    }
	}
	lastSaved = time(NULL);

	TThread::Printf("SnapShot finished!");
    } else {

       	TThread::Printf("HandleSnapshotSave() : Could not convert ps file because of lost connection to server! Will QUIT! BYE ...");
	mainThread->UnLock();
	HandleMainWindowClose();
    }
    if(!doQuit) mainThread->UnLock();
}

/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleSnapshotSaveOnline() {
    // GUI Event Handler to handle snapshot button click
    // Save all active canvases in png file in the online snapshot path.

    if (listPads.LastIndex() == -1) return; // return if no window is open


    mainThread->Lock();
    
    // loop over the pads, not over the canvases. But the print-function
    // prints the complete canvas into the file. If there is a split canvas,
    // the print-function prints the canvas several times (nx*ny) into the file.
    // Prevent this by adding the name of each printed canvas into this map and
    // check the map before each canvas is printed.

    map <TString, Bool_t> printedCanvases;
    
    TString  path      = clientmainwin->fTextSnapshotPathOnline;

    if(path != ""){
	path += "/";
	path.ReplaceAll("//","/");
	if(gSystem->AccessPathName(path.Data()) != 0 ){ // path not found
	    TThread::Printf("HandleSnapshotSave() : Path = %s not found! Will use currentdirectory.",path.Data());
	    path = "";
	}
    }

    for (Int_t i=0;i<listPads.LastIndex()+1;i++) {
	TPad* subpad = (TPad*) listPads.At(i);
	TCanvas* cnv = subpad->GetCanvas();
	if (!printedCanvases[cnv->GetName()]) {  // canvas is not yet printed
	    cnv->SaveAs(Form("%s%s.png",path.Data(),cnv->GetName()));
	    printedCanvases[cnv->GetName()] = kTRUE;
	}
    }

    lastSavedOnline = time(NULL);

    TThread::Printf("SnapShot finished!");

    mainThread->UnLock();
}

/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleMainWindowClose() {
    // GUI Event Handler to handle close-event of the main window

    mainThread->Lock();

    for(Int_t i=0;i<listDetectors.LastIndex()+1;i++) {
	HOnlineMonClientDet* detector = (HOnlineMonClientDet*) listDetectors.At(i);
	if (detector->IsActive()) {
	    detector->DestroyDet(this);
	}
    }
    listDetectors.Delete();
    listHists.Delete();
    listPads.Delete();
    TThread::Delete(updateThread);
    delete updateThread;
    socket->Close();
    delete socket;

    mainThread->UnLock();
   
    gApplication->Terminate();
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientMain::~HOnlineMonClientMain() {

    mainThread->Lock();

    for(Int_t i=0;i<listDetectors.LastIndex()+1;i++) {
	    HOnlineMonClientDet* detector = (HOnlineMonClientDet*) listDetectors.At(i);
	    if (detector->IsActive()) {
	        detector->DestroyDet(this);
	    }
    }
    listDetectors.Delete();
    listHists.Delete();
    listPads.Delete();
	
    TThread::Delete(updateThread);
    delete updateThread;
    if (socket) {
        socket->Close();
        delete socket;
        socket = 0;
    }

    mainThread->UnLock();

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleControlbuttonClick(char *detectorname) {

    TGCheckButton *button = (TGCheckButton*) clientmainwin->listControlbuttons.FindObject(detectorname);
    Bool_t buttonOn = button->IsOn();
    
    if (buttonOn) {
	button->SetEnabled(kFALSE);
	HOnlineMonClientDet *det = (HOnlineMonClientDet*) listDetectors.FindObject(detectorname);

	CreateDetGui(det);
	button->SetEnabled(kTRUE);

    } else {
	button->SetEnabled(kFALSE);
	HOnlineMonClientDet *det = (HOnlineMonClientDet*) listDetectors.FindObject(detectorname);
	DestroyDetGui(det);
	button->SetEnabled(kTRUE);
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::SetUpdateInterval(Int_t msec) {
    UpdateInterval = msec;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::HandleSingleWindowClose(char *windowname, char* detectorname) {

    mainThread->Lock();

    HOnlineMonClientDet *det = (HOnlineMonClientDet*) listDetectors.FindObject(detectorname);
    if (det->listWindows.FindObject(windowname)->IsA() == HOnlineMonClientTabWin::Class()) {
        HOnlineMonClientTabWin *win = (HOnlineMonClientTabWin*) det->listWindows.FindObject(windowname);
        DestroySingleWindow(win);
    } else {
        HOnlineMonClientSimpleWin *win = (HOnlineMonClientSimpleWin*) det->listWindows.FindObject(windowname);
        DestroySingleWindow(win);
    }
    mainThread->UnLock();
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientMain::Connect(TString host, Int_t port) {
    socket = new TSocket(host, port);
    if (socket->IsValid()) {
        return kTRUE;
    }
    
    return kFALSE;
}

/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientMain::ReConnect(TString host, Int_t port) {


    if (socket) {
	socket->Close();
	delete socket;
	socket = 0;
    }
    lastSumBytes = 0 ;
    socket = new TSocket(host, port);
    if (socket->IsValid()) {
        return kTRUE;
    }
    
    return kFALSE;
}

/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientMain::UpdateHist(HOnlineMonClientHist *hist) { 
    // update a single histogram and draw it on the current canvas.
    // A timestamp is added to the histogram title

    HOnlineMonHistAddon* newhisto = GetHist(hist->GetRealName());
    if (newhisto == 0) { return kFALSE; }
    
    // create timestamp
    UInt_t year = 0;
    UInt_t month = 0;
    UInt_t day = 0;
    UInt_t hour = 0;
    UInt_t min = 0;
    UInt_t sec = 0;
    TString update_on;

    TTimeStamp* timestamp = new TTimeStamp();
    timestamp->GetDate(kFALSE, 0, &year, &month, &day);
    timestamp->GetTime(kFALSE, 0, &hour, &min, &sec);
    delete timestamp;
    timestamp = 0;
    
    update_on.Form(" %d.%d.%d %d:%d:%d", day, month, year, hour, min, sec);
     
    // cast the HOnlineMonHistAddon pointer, add timestamp to title, draw histogram
    if (hist->histo->IsA() == HOnlineMonHist::Class()) {
        HOnlineMonHist* h = (HOnlineMonHist*) hist->histo;
        h->getP()->SetTitle(newhisto->getP()->GetTitle() + update_on);
	h->getP()->Reset();
        h->getP()->Add(newhisto->getP());
        delete (HOnlineMonHist*) newhisto;

	h->draw();
        return kTRUE;
    }

    // cast the HOnlineMonHistAddon pointer, add timestamp to title, draw histogram
    if (hist->histo->IsA() == HOnlineMonStack::Class()) {
        HOnlineMonStack* h = (HOnlineMonStack*) hist->histo;
	h->reset(100,0);  // >10 = only here
	h->add(newhisto);
	h->addTimeStamp(update_on);
	delete (HOnlineMonStack*) newhisto;

	h->draw();
        return kTRUE;
    }
    
    if (hist->histo->IsA() == HOnlineMonHist2::Class()) {
        HOnlineMonHist2* h = (HOnlineMonHist2*) hist->histo;
        h->getP()->SetTitle(newhisto->getP()->GetTitle() + update_on);
	h->getP()->Reset();
	h->getP()->Add(newhisto->getP());
	delete (HOnlineMonHist2*) newhisto;

	h->draw();
        return kTRUE;
    }
    
    if (hist->histo->IsA() == HOnlineTrendHist::Class()) {
        HOnlineTrendHist* h = (HOnlineTrendHist*) hist->histo;
        h->getP()->SetTitle(newhisto->getP()->GetTitle() + update_on);
	h->getP()->Reset();
	h->getP()->Add(newhisto->getP());
	delete (HOnlineTrendHist*) newhisto;

	h->draw();
        return kTRUE;
    }
    
    if (hist->histo->IsA() == HOnlineHistArray::Class()) {
        HOnlineHistArray* h = (HOnlineHistArray*) hist->histo;

	h->reset(0,0);
        h->add(newhisto);

	Int_t s = hist->GetArrayIndexS();
        Int_t m = hist->GetArrayIndexM();
        
        Int_t i;
        Int_t j;
        h->getDim(&i, &j);
        
        if ( (s != -1) || (m != -1) ) {
            // if indices in configfile given, draw specific histogram only.
            // otherwise call draw-function from HOnline* object.
            
            // check if s,m in range of array
            if( (s < i) || (m < j) ) {
                h->getP(s,m)->SetTitle(newhisto->getP(s,m)->GetTitle() + update_on);
		h->getP(s,m)->Draw();  // ignoriert die Optionen (OPT#) aus Histogrammdefinitionsdatei onlinehists.txt
	    } else {
                TThread::Printf("Histogram ERROR: histogram array HOnlineHistArray '%s', index s='%d' or m='%d' out of bound. Dimension of the array is ('%d','%d').", hist->GetName(), s, m, i, j);
                exit(1);
            }
        } else {
            // get the last histogram in the array
            // and add the timestamp to the title.
            h->getP(0,i*j)->SetTitle(newhisto->getP(0,i*j)->GetTitle() + update_on);
	    h->draw();
	}

	delete (HOnlineHistArray*) newhisto;
        return kTRUE;
    }

    if (hist->histo->IsA() == HOnlineHistArray2::Class()) {
        HOnlineHistArray2* h = (HOnlineHistArray2*) hist->histo;

	h->reset(0,0);
        h->add(newhisto);

	Int_t s = hist->GetArrayIndexS();
        Int_t m = hist->GetArrayIndexM();
        
        Int_t i;
        Int_t j;
        h->getDim(&i, &j);
        
        if ( (s != -1) || (m != -1) ) {
            // if indices in configfile given, draw specific histogram only.
            // otherwise call draw-function from HOnline* object.
            
            // check if s,m in range of array
            if( (s < i) || (m < j) ) {
                h->getP(s,m)->SetTitle(newhisto->getP(s,m)->GetTitle() + update_on);
		h->getP(s,m)->Draw();  // ignoriert die Optionen (OPT#) aus Histogrammdefinitionsdatei onlinehists.txt
	    } else {
                TThread::Printf("Histogram ERROR: histogram array HOnlineHistArray2 '%s', index s='%d' or m='%d' out of bound. Dimension of the array is ('%d','%d').", hist->GetName(), s, m, i, j);
                exit(1);
            }
        } else {
            // get the last histogram in the array
            // and add the timestamp to the title.
            h->getP(0,i*j)->SetTitle(newhisto->getP(0,i*j)->GetTitle() + update_on);
	    h->draw();
	}

	delete (HOnlineHistArray2*) newhisto;
        return kTRUE;
    }

    if (hist->histo->IsA() == HOnlineTrendArray::Class()) {
        HOnlineTrendArray* h = (HOnlineTrendArray*) hist->histo;

	h->reset(0,0);
        h->add(newhisto);
	Int_t s = hist->GetArrayIndexS();
        Int_t m = hist->GetArrayIndexM();
        
        Int_t i;
        Int_t j;
        h->getDim(&i, &j);
        
        if ( (s != -1) || (m != -1) ) { 
            // if indices in configfile given, draw specific histogram only.
            // otherwise call draw-function from HOnline* object.
            
            // check if s,m in range of array
            if( (s < i) || (m < j) ) {
                h->getP(s,m)->SetTitle(newhisto->getP(s,m)->GetTitle() + update_on);
        	h->getP(s,m)->Draw();  // ignoriert die Optionen (OPT#) aus Histogrammdefinitionsdatei onlinehists.txt
	    } else {
                TThread::Printf("Histogram ERROR: histogram array HOnlineTrendArray '%s', index s='%d' or m='%d' out of bound. Dimension of the array is ('%d','%d').", hist->GetName(), s, m, i, j);
                exit(1);
            }
        } else {
            // get the last histogram in the array
            // and add the timestamp to the title.
            h->getP(0,i*j)->SetTitle(newhisto->getP(0,i*j)->GetTitle() + update_on);
	    h->draw();
	}

	delete (HOnlineTrendArray*) newhisto;
        return kTRUE;
    }
    
    TThread::Printf("Histogram ERROR: histogram '%s' is of unknown type", hist->histo->GetName());
    return kFALSE;

}
/* -------------------------------------------------------------------------- */

TString HOnlineMonClientMain::SendCmdToServer(TString cmd) {
    if (!socket->IsValid()) {
        TString s;
	s.Form("ERROR: Socket invalid");
	HandleConnectionError();
        return s;
    }

    TMessage* recvmess = 0;
    TMessage sendmess(kMESS_STRING);
    sendmess.WriteTString(cmd);
    Int_t scode = socket->Send(sendmess); // sendcode
    if (scode <= 0) {
        TString s;
        s.Form("socket sent code %d for command '%s'.", scode, cmd.Data());
        return s;
    }
    
    Int_t rcode = socket->Recv(recvmess); // receivecode
    
    if (rcode <= 0) {
        TString s;
        s.Form("socket received code %d for command %s", rcode, cmd.Data());
        delete recvmess;
        return s;
    }

    if (recvmess->What() == kMESS_STRING) {
        TString str;
        recvmess->ReadTString(str);
        delete recvmess;
        return str;
    } else {
        TString s;
        s.Form("ERROR: type of message received from server for command '%s' is invalid.", cmd.Data());
        delete recvmess;
        return s;
    }

    delete recvmess;
    TString errormsg = "ERROR: message received from server is invalid";
    return errormsg;
}
/* -------------------------------------------------------------------------- */
TList* HOnlineMonClientMain::GetListOfHistograms() {
    if (!socket->IsValid()) {
        TThread::Printf("ERROR: Socket invalid");
        return 0;
    }
    
    TString cmd = "CMD histlist";
    TMessage* recvmess = 0;
    TList* hlist = new TList();
    
    TMessage sendmess(kMESS_STRING);
    sendmess.WriteTString(cmd);
    Int_t scode = socket->Send(sendmess); // sendcode
    if (scode <= 0) {
        TThread::Printf("socket sent code %d for command '%s'.", scode, cmd.Data());
        return 0;
    }
    
    Int_t rcode = socket->Recv(recvmess); // receivecode
    
    if (rcode <= 0) {
        TThread::Printf("socket received code %d for command %s", rcode, cmd.Data());
        delete recvmess;
        return 0;
    }

    if (recvmess->What() == kMESS_STRING) {
        TString str;
        recvmess->ReadTString(str);
        TThread::Printf("message received from server for command '%s': %s", cmd.Data(), str.Data());
        delete recvmess;
        return 0;
    } else {
        if (recvmess->What() == kMESS_OBJECT) {
            if ( (hlist = (TList*) recvmess->ReadObjectAny(TList::Class())) == 0) {
                TThread::Printf("ERROR: Received invalid data for command '%s' from server.", cmd.Data());       
                delete recvmess;
                return 0;
            }
        } else {
            TThread::Printf("ERROR: type of message received from server for command '%s' is invalid.", cmd.Data());
            delete recvmess;
            return 0;
        }
    }

    delete recvmess;
    return hlist;  
}
/* -------------------------------------------------------------------------- */
HOnlineMonHistAddon* HOnlineMonClientMain::GetHist(TString histoname) {

    if (!socket->IsValid()) {
        TThread::Printf("ERROR: Socket invalid");
        return 0;
    }
    
    
    // TMessage object to receive object from server
    TMessage* recvmess = 0;  // receiving message
    HOnlineMonHistAddon* newhist = 0;
    
    
    // TMessage object to send TString to server
    // can NOT be a TMessage* object, because of TSocket::Receive(TMessage&) 
    TMessage sendmess(kMESS_STRING); 
    sendmess.WriteTString(histoname);
    
    Int_t scode = socket->Send(sendmess);  // sendcode
    
    if (scode <= 0) {
        TThread::Printf("socket sent code %d histo %s", scode, histoname.Data());
        return 0;
    }
    
    Int_t rcode = socket->Recv(recvmess); // receivecode
    
    if (rcode <= 0) {
        TThread::Printf("socket received code %d histo %s", rcode, histoname.Data());
        delete recvmess;
        return 0;
    }
    

    if (recvmess->What() == kMESS_STRING) {
        TString str;
        recvmess->ReadTString(str);
        TThread::Printf("message received from server: %s", str.Data());
        delete recvmess;
        return 0;
    } else {
        if (recvmess->What() == kMESS_OBJECT) {
            if ( (newhist = (HOnlineMonHistAddon*) recvmess->ReadObjectAny(HOnlineMonHistAddon::Class())) == 0) {
                TThread::Printf("ERROR: Received invalid data for histogram '%s' from server.", histoname.Data());       
                delete recvmess;
                return 0;
            }
        } else {
            TThread::Printf("ERROR: type of message received from server is invalid.");
            delete recvmess;
            return 0;
        }
    }

    delete recvmess;
    return newhist;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::AddHist(HOnlineMonClientCanvas* canvas, HOnlineMonClientHist* hist) {
    // Put the histogram and the corresponding canvas into the global
    // lists listHists / listPads.
    // All histograms and canvases/pads in these lists are updated
    // by the update-function

    listHists.Add(hist);

    // get an initial histogram from server. The real name must be send.
    HOnlineMonHistAddon* newhist = this->GetHist(hist->GetRealName());
    
    if (newhist == 0) {
        TThread::Printf("ERROR: Can't add histogram '%s' to the update-list. Received invalid initial histogram from server.", hist->GetName());
        return;
    }
    
    hist->SetHist(newhist);

    // get the subpad and apply grid- and log-values
    if (hist->GetSubpadnumber() == 0) {
    	// unsplit canvas, apply options to the canvas
    	TCanvas* cnv = canvas->GetCanvas();
        cnv->SetGrid(hist->GetHist()->getGridX(), hist->GetHist()->getGridY());
        cnv->SetLogx(hist->GetHist()->getLogX());
        cnv->SetLogy(hist->GetHist()->getLogY());
        cnv->SetLogz(hist->GetHist()->getLogZ());
        listPads.Add(cnv);
        pad2hist[cnv->GetName()] = hist->GetName();
    } else {
    	// split canvas, apply options to the subpad, not to the canvas
        TString padnr = "";
        padnr += hist->GetSubpadnumber();
        
        TString padname = canvas->GetName() + TString("_") + padnr;
        
        TPad* pad=(TPad*) canvas->GetCanvas()->GetPrimitive(padname.Data());
        pad->SetGrid(hist->GetHist()->getGridX(), hist->GetHist()->getGridY());
        pad->SetLogx(hist->GetHist()->getLogX());
        pad->SetLogy(hist->GetHist()->getLogY());
        pad->SetLogz(hist->GetHist()->getLogZ());
        
        listPads.Add(pad);
        pad2hist[padname] = hist->GetName();
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::RemoveHist(HOnlineMonClientCanvas* canvas, HOnlineMonClientHist* hist) {
    // Remove canvas and hist from the global list. Then, the hist and the canvas
    // are no longer updated

    if (hist->GetSubpadnumber() == 0) {
    	// unsplit canvas
    	TCanvas* cnv = canvas->GetCanvas();
        listHists.Remove(hist);
        listPads.Remove(cnv);
        pad2hist.erase(cnv->GetName());
    } else {
        TString padnr = "";
        padnr += hist->GetSubpadnumber();
        
        TString padname = canvas->GetName() + TString("_") + padnr;
        
        TPad* pad=(TPad*) canvas->GetCanvas()->GetPrimitive(padname.Data());
        listHists.Remove(hist);
        listPads.Remove(pad);
        pad2hist.erase(pad->GetName());
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::SetServerhost(TString host) {
    serverhost = host;
}
/* -------------------------------------------------------------------------- */
TString HOnlineMonClientMain::GetServerhost() {
    return serverhost;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMain::SetServerport(Int_t port) {
    serverport = port;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientMain::GetServerport() {
    return serverport;
}
/* -------------------------------------------------------------------------- */


