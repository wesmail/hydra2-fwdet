#ifndef HONLINEMONCLIENTMAINWIN_H
#define HONLINEMONCLIENTMAINWIN_H

#include "TNamed.h"
#include "TString.h"
#include "TList.h"
#include "TGFrame.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"
#include "TGStatusBar.h"
#include "TGTextEntry.h"


class HOnlineMonClientMain;

class HOnlineMonClientMainWin : public TNamed {
public:

	TList listControlbuttons;	// holds the control checkboxes - each detector has a
					// controlbutton in the MainWindow to switch on/off
    	TList listOpenDetectors;        // list of open detectors  (startup)
	HOnlineMonClientMainWin();
	~HOnlineMonClientMainWin();

	void CreateMainWin(HOnlineMonClientMain* clientmain);
	void DestroyMainWin(HOnlineMonClientMain* clientmain);
	
	void    SetWidth(Int_t width);
	Int_t   GetWidth();
	
	void    SetHeight(Int_t height);
	Int_t   GetHeight();
	void    SetSnapShotPath(TString path);
	TString GetSnapShotPath() { return SnapShotPath; }

	void    SetAutoSaveRate(Int_t rate) ;
        Int_t   GetAutoSaveRate() { return autoSaveRate;}
	HOnlineMonClientMain* GetMainClient() {return clientMain;}
        void    SetMainClient(HOnlineMonClientMain* main) {  clientMain = main;}

	// refresh rate
	TGGroupFrame*  gFrameRefreshRate;
	TGNumberEntry* numberEntryRefreshRate; // number entry for refresh rate

	TGNumberEntry* numberEntryAutoSaveRate;// number entry for auto save rate
	TGTextEntry*   fTextFilename;          // widget for file name
	TGTextEntry*   fTextSnapshotPath;      // widget for snapshot path
	TString        fTextSnapshotPathOnline; // snapshot path online
	TGTextEntry*   fTextSpeed;             // widget for speed [evts/s]
	TGTextEntry*   fDataRate;              // widget for kBytes/s
private:
	// main window elements	
        TGMainFrame* clientMainWindow;
        HOnlineMonClientMain* clientMain;
	Int_t   width;
	Int_t   height;
	TString SnapShotPath;
        Int_t   autoSaveRate;
	
	// snapshot
	TGGroupFrame* gFrameSnapshot;
	TGTextButton* buttonSnapshot;
	
	// detectors
	TGGroupFrame* gFrameDetectors;
public:
	TGMainFrame*   GetMainWindow() { return clientMainWindow ;}
	
	ClassDef(HOnlineMonClientMainWin, 1);
};
#endif
