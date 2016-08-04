#include "honlinemonclientmainwin.h"

#include "honlinemonclientmain.h"

ClassImp(HOnlineMonClientMainWin);

/* -------------------------------------------------------------------------- */
HOnlineMonClientMainWin::HOnlineMonClientMainWin() {
    width = 0;
    height = 0;
    clientMainWindow = 0;
    clientMain       = 0;
    // refresh rate
    gFrameRefreshRate = 0;
    numberEntryRefreshRate = 0;

    // auto save rate
    numberEntryAutoSaveRate = 0;

    // snapshot
    gFrameSnapshot = 0;
    buttonSnapshot = 0;

    // detectors
    gFrameDetectors = 0;

    SnapShotPath ="";

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMainWin::CreateMainWin(HOnlineMonClientMain* clientmain) {

    /* MAIN WINDOW */
    clientMainWindow = new TGMainFrame(gClient->GetRoot());
    clientMain = clientmain;
    /* DETECTORS CHECKBUTTONS */
    gFrameDetectors = new TGGroupFrame(clientMainWindow, "detectors", kVerticalFrame);
    gFrameDetectors->SetLayoutManager(new TGVerticalLayout(gFrameDetectors));
    // for each detector one checkbox
    for (Int_t i=0;i<clientmain->listDetectors.LastIndex()+1;i++) {
	TString detname = clientmain->listDetectors.At(i)->GetName();
	TGCheckButton *checkbutton = new TGCheckButton(gFrameDetectors, detname);
	checkbutton->SetName(detname);
	checkbutton->Connect("Clicked()", "HOnlineMonClientMain", clientmain, ((TString) "HandleControlbuttonClick(=\"" + detname + "\")").Data());
	listControlbuttons.Add(checkbutton);

        if (listOpenDetectors.FindObject(detname.Data()) || listOpenDetectors.FindObject("all") ) checkbutton->SetOn(kTRUE);

	gFrameDetectors->AddFrame(checkbutton, new TGLayoutHints(kLHintsLeft,2,2,2,2));
    }

    // add GroupFrame to MainFrame
    clientMainWindow->AddFrame(gFrameDetectors, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    /* REFRESH RATE */
    gFrameRefreshRate = new TGGroupFrame(clientMainWindow, "refresh rate (s) | auto save (m)", kHorizontalFrame);
    gFrameRefreshRate->SetLayoutManager(new TGHorizontalLayout(gFrameRefreshRate));
    numberEntryRefreshRate = new TGNumberEntry(gFrameRefreshRate,clientmain->GetUpdateInterval(), 5, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMin, 1);
    numberEntryRefreshRate->Connect("ValueSet(Long_t)", "HOnlineMonClientMain", clientmain, "HandleRefreshRateChange()");
    (numberEntryRefreshRate->GetNumberEntry())->Connect("ReturnPressed()", "HOnlineMonClientMain", clientmain, "HandleRefreshRateChange()");
    // add numberEntry to GroupFrame
    gFrameRefreshRate->AddFrame(numberEntryRefreshRate, new TGLayoutHints(kLHintsLeft, 5, 5, 3, 4));

    /* AUTOSAVE RATE */
    numberEntryAutoSaveRate = new TGNumberEntry(gFrameRefreshRate, -1, 5, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAAnyNumber, TGNumberFormat::kNELLimitMin, 1);
    numberEntryAutoSaveRate->SetLimits(TGNumberFormat::kNELLimitMinMax, -1,300);
    numberEntryAutoSaveRate->Connect("ValueSet(Long_t)", "HOnlineMonClientMain", clientmain, "HandleAutoSaveRateChange()");
    (numberEntryAutoSaveRate->GetNumberEntry())->Connect("ReturnPressed()", "HOnlineMonClientMain", clientmain, "HandleAutoSaveRateChange()");

    // add numberEntry to GroupFrame
    gFrameRefreshRate->AddFrame(numberEntryAutoSaveRate, new TGLayoutHints(kLHintsRight, 5, 5, 3, 4));
    numberEntryAutoSaveRate->GetNumberEntry()->SetIntNumber(GetAutoSaveRate());

    // add GroupFrame to MainFrame
    clientMainWindow->AddFrame(gFrameRefreshRate, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    /* SNAPSHOT */
    gFrameSnapshot = new TGGroupFrame(clientMainWindow, "actions", kHorizontalFrame);
    gFrameSnapshot->SetLayoutManager(new TGHorizontalLayout(gFrameSnapshot));


    // 'stop'-button
    TGTextButton* buttonStop = new TGTextButton(gFrameSnapshot, "STOP");
    buttonStop->SetToolTipText("Click here to stop the hadesonlineserver. \n"
			       "Only super user should stop the server since it \n"
			       "will affect all other monitoring clients! \n"
			       "USE With CARE!");
    buttonStop->Connect("Clicked()", "HOnlineMonClientMain", clientmain, "HandleSendStopServer()");
    gFrameSnapshot->AddFrame(buttonStop, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    // 'reset'-button
    TGTextButton* buttonReset = new TGTextButton(gFrameSnapshot, "RESET");
    buttonReset->SetToolTipText("Click here to reset hists on the hadesonlineserver. \n"
				"This will affect all other monitoring clients! \n"
				"USE With CARE!");
    buttonReset->Connect("Clicked()", "HOnlineMonClientMain", clientmain, "HandleSendResetHists()");
    gFrameSnapshot->AddFrame(buttonReset, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    // 'save'-button
    buttonSnapshot = new TGTextButton(gFrameSnapshot, "SAVE");
    buttonSnapshot->SetToolTipText("Click here to store all monitoring screens to a \n"
				   "pdf file in the snapshot dir. The file name is \n"
				   "composed filename_yyyy-m-dd_hh:minmin:ss.pdf \n"
				   "ONLY HISTOGRAMS OF OPEN MONITORS WILL BE SAVED!");
    buttonSnapshot->Connect("Clicked()", "HOnlineMonClientMain", clientmain, "HandleSnapshotSave()");
    // add button to GroupFrame
    gFrameSnapshot->AddFrame(buttonSnapshot, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
    // add GroupFrame to MainFrame
    clientMainWindow->AddFrame(gFrameSnapshot, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

    // current file name
    fTextFilename = new TGTextEntry(clientMainWindow );
    clientMainWindow->AddFrame(fTextFilename,new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 5));

    // path for snapshot files
    fTextSnapshotPath = new TGTextEntry(clientMainWindow );
    clientMainWindow->AddFrame(fTextSnapshotPath,new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 5));
    fTextSnapshotPath->SetText(GetSnapShotPath().Data());

    // speed of event server
    fTextSpeed = new TGTextEntry(clientMainWindow );
    clientMainWindow->AddFrame(fTextSpeed,new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 5));

    // speed of event server
    fDataRate = new TGTextEntry(clientMainWindow );
    clientMainWindow->AddFrame(fDataRate,new TGLayoutHints(kLHintsExpandX, 5, 5, 3, 5));

    clientMainWindow->SetName(this->GetName());
    clientMainWindow->SetWindowName(this->GetTitle());
    clientMainWindow->MapSubwindows();
    clientMainWindow->MapWindow();
    clientMainWindow->Resize(this->GetWidth(), this->GetHeight());
    clientMainWindow->Connect("CloseWindow()", "HOnlineMonClientMain", clientmain, "HandleMainWindowClose()");
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMainWin::DestroyMainWin(HOnlineMonClientMain* clientmain) {
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientMainWin::~HOnlineMonClientMainWin() {

    // control button list
    // Remove all objects from the list AND delete all heap based objects.
    // the destructor of the elements is called
    // no need to delete the controlbuttons by hand
    listControlbuttons.Delete();

    // MainWindow
    clientMainWindow->Cleanup();
    delete clientMainWindow;
    clientMainWindow = 0;
    clientMain       = 0;

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMainWin::SetWidth(Int_t width) {
    this->width = width;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientMainWin::GetWidth() {
    return this->width;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMainWin::SetHeight(Int_t height) {
    this->height = height;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientMainWin::GetHeight() {
    return this->height;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMainWin::SetSnapShotPath(TString path) {
    SnapShotPath = path;

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientMainWin::SetAutoSaveRate(Int_t rate) {
    autoSaveRate = rate;

}
/* -------------------------------------------------------------------------- */
