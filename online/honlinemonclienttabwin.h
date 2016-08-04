#ifndef HONLINEMONCLIENTTABWIN_H
#define HONLINEMONCLIENTTABWIN_H

#include "TNamed.h"
#include "TList.h"
#include "TGFrame.h"
#include "TGTab.h"
#include "TGStatusBar.h"
#include "TString.h"
#include "TTimeStamp.h"

class HOnlineMonClientMain;
class HOnlineMonClientTab;

class HOnlineMonClientTabWin : public TNamed {
public:
        TList listTabs;
        TString detectorname;                   // store detectorname
	TGTab* tabContainer;			// the tab container, holds the tabs


	HOnlineMonClientTabWin();
	~HOnlineMonClientTabWin();
	
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
	TGMainFrame* mainFrame;			// main frame
	TGCompositeFrame* tabContainerFrame;	// frame for tab container

	TGStatusBar* statBar;		// status bar to display update timestamp
	Bool_t active;			// window open or not 
	TString lastUpdate;		// last update text
	
	ClassDef(HOnlineMonClientTabWin, 1);
};
#endif

