#ifndef __MAKEGUI__
#define __MAKEGUI__



#include "TRootBrowser.h"
#include "TString.h"
#include "TSystem.h"

#include "TEveManager.h"
#include "TEveBrowser.h"

#include "TGFrame.h"
#include "TGButton.h"
#include "TGColorDialog.h"
#include "TGColorSelect.h"

#include "hedgui.h"


#include "nextEvent.C"

//---------------------------------------------------------
//             MAKE GUI
// Create the GUI for Display setup in Eve. Connect
// "nex Event" button to HEDEvtNavHandler defined in
//  nextEvent.C
//---------------------------------------------------------


void make_GUI(){

    TEveBrowser* browser = gEve->GetBrowser();
    browser->StartEmbedding(TRootBrowser::kLeft);
    TGMainFrame* frmMain = new TGMainFrame(gClient->GetRoot(), 1000, 600);
    frmMain->SetWindowName("XX GUI");
    frmMain->SetCleanup(kDeepCleanup);

    //-------------------------------------------------------------
    // next event button
    Int_t width  = 300;
    Int_t height = 100;

    HEDSetup* setup = HEDSetup::getEDSetup(frmMain,width,height);
    frmMain->AddFrame(setup,new TGLayoutHints(kLHintsCenterX | kLHintsTop, 0, 0, 1, 1));


    //-------------------------------------------------------------
    TGHorizontalFrame *fHL4 = new TGHorizontalFrame(frmMain, width, height, kFixedWidth);

    HEDEvtNavHandler    *fhandler = HEDEvtNavHandler::getHandler();
    setup->fNext = new TGTextButton   (fHL4,"next Event", setup->IDs.GetUnID());
    setup->fNext->SetToolTipText("Click here to load next Event. \n"
				 "If the loop check box is set it will \n"
				 " loop for events. Uncheck the box to \n"
				 "stop. The loop will stop also if \n"
				 "nextEvent() return with non ZERO.");
    setup->fNext->Connect("Clicked()", "HEDEvtNavHandler", fhandler, "selectEvent()");
    fHL4->AddFrame(setup->fNext, new TGLayoutHints(kLHintsExpandX));
    frmMain->AddFrame(fHL4, new TGLayoutHints(kLHintsCenterX, 1, 1, 1, 1));

    //-------------------------------------------------------------

    frmMain->MapSubwindows();
    frmMain->Resize();
    frmMain->MapWindow();

    browser->StopEmbedding();
    browser->SetTabTitle("Event Control", 0);

}
#endif

