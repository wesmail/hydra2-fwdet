#include "honlinemonclientsimplewin.h"

#include "honlinemonclientmain.h"
#include "honlinemonclientcanvas.h"

ClassImp(HOnlineMonClientSimpleWin);

/* -------------------------------------------------------------------------- */
HOnlineMonClientSimpleWin::HOnlineMonClientSimpleWin() {
    mainFrame = 0;
    statBar = 0;
    active = kFALSE;
    lastUpdate = "";
    detectorname = "";
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientSimpleWin::~HOnlineMonClientSimpleWin() {
    if (mainFrame) {
	mainFrame->Cleanup();
	delete mainFrame;
	mainFrame = 0;
	statBar = 0;
    }

    listCanvases.Delete();
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientSimpleWin::CreateWindow(HOnlineMonClientMain* clientmain, TString detectorName) {

    detectorname = detectorName;

    mainFrame = new TGMainFrame(gClient->GetRoot());

    for (Int_t i=0;i<listCanvases.LastIndex()+1;i++) {
	HOnlineMonClientCanvas *clientcanvas = (HOnlineMonClientCanvas*) listCanvases.At(i);
	clientcanvas->CreateCanvas(clientmain, (TGWindow*) mainFrame);
	mainFrame->AddFrame(clientcanvas->GetRCanvas(), new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    }

    // create status bar
    statBar = new TGStatusBar(mainFrame, 50, 10, kVerticalFrame);
    statBar->SetText("", 0);
    mainFrame->AddFrame(statBar, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));

    mainFrame->SetName(this->GetName());
    mainFrame->SetWindowName(this->GetTitle());
    //mainFrame->Connect("CloseWindow()", "HOnlineMonClientMain", clientmain, ((TString) "HandleSingleWindowClose(=\"" + this->GetName() + "\", \"" + detectorname.Data() + "\")").Data());
    mainFrame->MapSubwindows();
    mainFrame->MapWindow();
    mainFrame->Resize(mainFrame->GetDefaultSize());

    this->SetActive(kTRUE);
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientSimpleWin::DestroyWindow(HOnlineMonClientMain* clientmain) {

    TThread* main = clientmain->getMainThread();
    main->Lock();

    this->SetActive(kFALSE);

    if (mainFrame) {


	for (Int_t i=0;i<listCanvases.LastIndex()+1;i++) {
	    HOnlineMonClientCanvas* clientcanvas = (HOnlineMonClientCanvas*) listCanvases.At(i);
	    clientcanvas->DestroyCanvas(clientmain);
	}
	mainFrame->Cleanup(); // removes all objects added via AddFrame()

	delete mainFrame;
	mainFrame = 0;
    }
    main->UnLock();

}
/* -------------------------------------------------------------------------- */
TString HOnlineMonClientSimpleWin::GetLastUpdate() {
    return lastUpdate;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientSimpleWin::SetLastUpdate() {
    TTimeStamp* timestamp = new TTimeStamp();
    lastUpdate = timestamp->AsString("l");
    delete timestamp;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientSimpleWin::SetStatusBarText(TString text) {
    if (statBar) {
	statBar->SetText(text, 0);
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientSimpleWin::SetActive(Bool_t state) {
    for (Int_t i=0;i<listCanvases.LastIndex()+1;i++) {
	HOnlineMonClientCanvas* clientcanvas = (HOnlineMonClientCanvas*) listCanvases.At(i);
	clientcanvas->SetActive(state);
    }
    this->active = state;
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientSimpleWin::IsActive() {
    return this->active;
}
/* -------------------------------------------------------------------------- */

