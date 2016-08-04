#include "honlinemonclienttabwin.h"

#include "honlinemonclientmain.h"
#include "honlinemonclienttab.h"

ClassImp(HOnlineMonClientTabWin);

/* -------------------------------------------------------------------------- */
HOnlineMonClientTabWin::HOnlineMonClientTabWin() {
    mainFrame = 0;
    tabContainerFrame = 0;
    tabContainer = 0;
    statBar = 0;
    detectorname = "";
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientTabWin::~HOnlineMonClientTabWin() {
    if (mainFrame) {
	mainFrame->Cleanup(); // deletes tabContainerFrame
	delete tabContainer;
	tabContainer = 0;
	tabContainerFrame = 0;
	delete mainFrame;
	mainFrame = 0;
    }

    listTabs.Delete();
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTabWin::CreateWindow(HOnlineMonClientMain* clientmain, TString detectorName) {

    detectorname = detectorName;

    mainFrame = new TGMainFrame(gClient->GetRoot());

    tabContainerFrame = new TGCompositeFrame(mainFrame, 500, 300, kVerticalFrame);
    tabContainer = new TGTab(tabContainerFrame);

    for (Int_t i=0;i<listTabs.LastIndex()+1;i++) {
	HOnlineMonClientTab *clienttab = (HOnlineMonClientTab*) listTabs.At(i);
	clienttab->CreateTab(clientmain, this);
    }

    tabContainerFrame->AddFrame(tabContainer, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    mainFrame->AddFrame(tabContainerFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));


    // create status bar
    statBar = new TGStatusBar(mainFrame, 50, 10, kVerticalFrame);
    statBar->SetText("", 0);
    mainFrame->AddFrame(statBar, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));

    mainFrame->SetName(this->GetName());
    mainFrame->SetWindowName(this->GetTitle());
    mainFrame->Connect("CloseWindow()", "HOnlineMonClientMain", clientmain, ((TString) "HandleSingleWindowClose(=\"" + this->GetName() + "\", \"" + detectorname.Data() + "\")").Data());
    mainFrame->MapSubwindows();
    mainFrame->MapWindow();
    mainFrame->Resize(mainFrame->GetDefaultSize());

    this->SetActive(kTRUE);
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTabWin::DestroyWindow(HOnlineMonClientMain* clientmain) {
    TThread* main = clientmain->getMainThread();
    main->Lock();

    this->SetActive(kFALSE);

    if (mainFrame) {

	for (Int_t i=0;i<listTabs.LastIndex()+1;i++) {
	    HOnlineMonClientTab *clienttab = (HOnlineMonClientTab*) listTabs.At(i);
	    clienttab->DestroyTab(clientmain);
	}

	mainFrame->Cleanup(); // removes all objects added via AddFrame()
	delete tabContainer;
	tabContainer = 0;
	delete mainFrame;
	mainFrame = 0;
    }

    main->UnLock();
}
/* -------------------------------------------------------------------------- */
TString HOnlineMonClientTabWin::GetLastUpdate() {
    return lastUpdate;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTabWin::SetLastUpdate() {
    TTimeStamp* timestamp = new TTimeStamp();
    lastUpdate = timestamp->AsString("l");
    delete timestamp;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTabWin::SetStatusBarText(TString text) {
    if (statBar) {
	statBar->SetText(text, 0);
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTabWin::SetActive(Bool_t state) {
    for (Int_t i=0;i<listTabs.LastIndex()+1;i++) {
	HOnlineMonClientTab *clienttab = (HOnlineMonClientTab*) listTabs.At(i);
	clienttab->SetActive(state);
    }

    this->active = state;
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientTabWin::IsActive() {
    return this->active;
}
/* -------------------------------------------------------------------------- */

