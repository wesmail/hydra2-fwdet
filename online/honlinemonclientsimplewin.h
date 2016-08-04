#ifndef HONLINEMONCLIENTSIMPLEWIN_H
#define HONLINEMONCLIENTSIMPLEWIN_H

#include "TNamed.h"
#include "TList.h"
#include "TGFrame.h"
#include "TGStatusBar.h"
#include "TString.h"
#include "TTimeStamp.h"

class HOnlineMonClientMain;
class HOnlineMonClientCanvas;

class HOnlineMonClientSimpleWin : public TNamed {
public:
	TList listCanvases;	// list of canvases
	TString detectorname;   // store the detector name
	HOnlineMonClientSimpleWin();
	~HOnlineMonClientSimpleWin();
	
	void SetActive(Bool_t state);
	Bool_t IsActive();
	
	TString GetLastUpdate();
	void SetLastUpdate();	// erzeugt TTimeStamp*.
				// Schreibt Timestamp in
				// lastUpdate-String.
				// Loescht TTimeStamp*.

	void SetStatusBarText(TString text);	// set the status bar text

	void CreateWindow(HOnlineMonClientMain* clientmain, TString detectorname);
	void DestroyWindow(HOnlineMonClientMain* clientmain);
	
private:
	TGMainFrame* mainFrame;		// main frame
	TGStatusBar* statBar;		// status bar to display update timestamp
	Bool_t active;			// window open or not 
	TString lastUpdate;		// last update text
	
	ClassDef(HOnlineMonClientSimpleWin, 1);
};
#endif

