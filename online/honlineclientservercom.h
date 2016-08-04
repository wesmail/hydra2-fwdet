#ifndef HONLINECLIENTSERVERCOM_H
#define HONLINECLIENTSERVERCOM_H

#include "TList.h"
#include "TSocket.h"
#include "TServerSocket.h"
#include "TMonitor.h"
#include "TNamed.h"
#include "honlinemonhist.h"
#include "TMessage.h"
#include "TMap.h"
#include "TObjString.h"
#include "TThread.h"

#include <vector>
using namespace std;

typedef Bool_t (*fillptrHOnlineClientServerCom)(Int_t); // prototype for fill functions

class HOnlineClientServerCom : public TNamed {
public:
    

    TMap clientRequests;

    HOnlineClientServerCom(TString name, TString hostname, Int_t port);
    virtual ~HOnlineClientServerCom();

    Bool_t init();
    Bool_t processClientRequests();
    Int_t processClients(TList& histPool);

    // send histogram to client
    Bool_t sendHistToClient(TSocket* socket, HOnlineMonHistAddon* hist);

    // send text to client
    Bool_t sendTextToClient(TSocket* socket, TString msg);

    // send a list to client, i.e. list of histogram names
    Bool_t sendListToClient(TSocket* socket, TList* list);

    void setDebug(Bool_t dodebug) { fDebug = dodebug; }

    // set the counter interval to print the eventnumber
    static void setEvtCounterInterval(Int_t ct = 100) { fEvtCtInterval = ct ;}

    // run event loop in Thread
    void  eventLoop(vector < fillptrHOnlineClientServerCom >*  fillfunctions);

private:

    TMonitor *monitor;
    TServerSocket *serversocket;
    TList socketlist;
    TSocket* masterClient;         // master client connection (first socket)
    Int_t fPort;	           // server port number
    TString fHostname;
    Int_t fTimeout;	           // timeout for socket handling
    Bool_t fDebug;                 // do some prints in debug mode
    static Int_t fEvtCtInterval;   // set the counter interval to print the eventnumber (-1 = no print)
    static Float_t fSpeed;         // speed in evts/s

    // thread
    static Bool_t isInit;
    static Bool_t doStop;
    TThread* eventLoopThread;                  // thread to run the eventloop
    static void* ThreadEventLoop(void* arg);   // thread function, runs into the eventLoopThread
    ClassDef(HOnlineClientServerCom, 1);

};

#endif

