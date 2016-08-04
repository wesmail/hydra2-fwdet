#ifndef HONLINEMONCLIENTMAIN_H
#define HONLINEMONCLIENTMAIN_H

#include "TList.h"
#include "TGButton.h"
#include "TApplication.h"
#include "TSocket.h"
#include "TMessage.h"
#include "TThread.h"
#include "TSystem.h"
#include "TGMsgBox.h"

#include <map>
#include <time.h>

#include "honlinemonhist.h"
#include "honlinemonstack.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"


class HOnlineMonClientXML;
class HOnlineMonClientMainWin;
class HOnlineMonClientDet;
class HOnlineMonClientTabWin;
class HOnlineMonClientSimpleWin;
class HOnlineMonClientTab;
class HOnlineMonClientCanvas;
class HOnlineMonClientHist;


using namespace std;

class HOnlineMonClientMain {
public:
	
	HOnlineMonClientMain(TString filename="");
	virtual ~HOnlineMonClientMain();
	
	// if parameters are given, connect to specified server and send command and exit
	// else connect to server specified in configfile and start client
	void Init(Bool_t sendCmdOnly=kFALSE, TString host="", Int_t port=0, TString cmd="");

	void SetServerhost(TString host);
	TString GetServerhost();
	
	void SetServerport(Int_t port);
	Int_t GetServerport();
        Int_t GetUpdateInterval() { return UpdateInterval;}
 	
 	static void  SetAutoSaveInterval(Int_t minute) {AutoSaveInterval = minute;}
 	static void  SetAutoSaveIntervalOnline(Int_t minute) {AutoSaveIntervalOnline = minute;}
       
	// add / remove histos+canvases from the global lists/map
	void AddHist(HOnlineMonClientCanvas* canvas, HOnlineMonClientHist* hist);
	void RemoveHist(HOnlineMonClientCanvas* canvas, HOnlineMonClientHist* hist);
	
	// gui event handler, args must be char*
	void HandleControlbuttonClick(char *detectorname);
	void HandleSingleWindowClose(char *windowname, char* detectorname); 
	void HandleRefreshRateChange();
	void HandleAutoSaveRateChange();
	void HandleMainWindowClose();
	void HandleSendStopServer();      // send stop command to server
	void HandleSendResetHists();      // send rest all hists
	void HandleSnapshotSave();
	void HandleSnapshotSaveOnline();
	void HandleConnectionError();
	void HandleTryClose();
	TList listDetectors;		// list of all detectors
	HOnlineMonClientMainWin* clientmainwin;	// pointer to mainwindow class
	TThread* getMainThread() { return mainThread; }
	Int_t    IsMasterClient() { return isMasterClient <= 0 ? kFALSE : kTRUE ;}
	Int_t    GetMasterClient(){ return isMasterClient ;}
        void     SetIsMasterClient(Bool_t master) {  isMasterClient = master;}
        Int_t    GetReconnect(){ return doReconnect;}
        void     SetReconnect(Int_t recon) { doReconnect = recon;}

private:
	void CreateClientConfig(TString xmlfile);
	void CreateDetGui(HOnlineMonClientDet* detector);
	void DestroyDetGui(HOnlineMonClientDet* detector);
	void DestroySingleWindow(HOnlineMonClientSimpleWin* window);
	void DestroySingleWindow(HOnlineMonClientTabWin* window);
	
	// global hist lists/map
	map <TString, TString> pad2hist;	// provide mapping between subpadname and histogram name
	TList listHists;			// global list to store the opened histograms.
						// all histos in this list will be updated
	TList listPads;				// global list to store the canvas subpads
						// all subpads in this list will be updated
	
	// network, histograms
	Bool_t Connect(TString host, Int_t port);
	Bool_t ReConnect(TString host, Int_t port);
	HOnlineMonHistAddon* GetHist(TString histoname); // get histogram from network
	Bool_t UpdateHist(HOnlineMonClientHist* hist); // update a single histogram, calls GetHist to get new histo
	void UpdateHists(); // update all hists: iterates over visible histos and calls UpdateHist for each hist
	
	// thread
	static TThread* mainThread  ; // thread for the client program
	TThread* updateThread; // thread to update histograms
	static void* ThreadUpdateHists(void* arg); // thread function, runs into the updateThread
	
	// network vars
	TSocket* socket;		// socket for network communication
	TString serverhost;
	Int_t serverport;
	
	// send commands to server
	TList*  GetListOfHistograms(); // get list of all available histograms from server
	TString SendCmdToServer(TString cmd); // send command to server and get a TString from server back
	
	
	void SetUpdateInterval(Int_t msec);
	Int_t UpdateInterval; // update interval in secs.

	static Int_t AutoSaveInterval;        // auto save interval in minutes.
	static Int_t AutoSaveIntervalOnline;  // auto save interval in minutes.
        static time_t lastSaved;        // remember the time when last saved (in seconds since 1970)
        static time_t lastSavedOnline;  // remember the time when last saved (in seconds since 1970)
	static ULong64_t lastSumBytes ; // remember last number of bytes recieved

	static Int_t isMasterClient;    // remember if it was a master client
        Int_t  doReconnect;             // 0 = stop at crash, 1 = ask for reconnect (default), 2 = auto reconnect

	TString configfile; // name of the xml config file
	
	Bool_t updateComplete;	// after complete update of histograms, thread sets this to true
				// thread checks updateComplete before calling the update function to
				// ensure that the last update was completed. So no call to update function is possible,
				// if update function has not finished yet.

	ClassDef(HOnlineMonClientMain, 1);

};

#endif

