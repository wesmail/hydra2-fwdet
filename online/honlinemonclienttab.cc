#include "honlinemonclienttab.h"

#include "honlinemonclientmain.h"
#include "honlinemonclientcanvas.h"

ClassImp(HOnlineMonClientTab);

/* -------------------------------------------------------------------------- */
HOnlineMonClientTab::HOnlineMonClientTab() {
    tabFrame = 0;
    active = kFALSE;
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientTab::~HOnlineMonClientTab() {
    // tabFrame is deleted by tab widget
    listCanvases.Delete();
    tabFrame = 0;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTab::CreateTab(HOnlineMonClientMain* clientmain, HOnlineMonClientTabWin* clienttabwin) {
    tabFrame = clienttabwin->tabContainer->AddTab(this->GetTitle());
    tabFrame->SetName(this->GetName());

    for (Int_t i=0;i<listCanvases.LastIndex()+1;i++) {
	HOnlineMonClientCanvas *clientcanvas = (HOnlineMonClientCanvas*) listCanvases.At(i);
	clientcanvas->CreateCanvas(clientmain, (TGWindow*) tabFrame);
	tabFrame->AddFrame(clientcanvas->GetRCanvas(), new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
    }
    this->SetActive(kTRUE);

}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTab::DestroyTab(HOnlineMonClientMain* clientmain) {
    this->SetActive(kFALSE);

    for (Int_t i=0;i<listCanvases.LastIndex()+1;i++) {
	HOnlineMonClientCanvas *clientcanvas = (HOnlineMonClientCanvas*) listCanvases.At(i);
	clientcanvas->DestroyCanvas(clientmain);
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientTab::SetActive(Bool_t state) {
    for (Int_t i=0;i<listCanvases.LastIndex()+1;i++) {
	HOnlineMonClientCanvas* clientcanvas = (HOnlineMonClientCanvas*) listCanvases.At(i);
	clientcanvas->SetActive(state);
    }
    this->active = state;
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientTab::IsActive() {
    return this->active;
}
/* -------------------------------------------------------------------------- */

