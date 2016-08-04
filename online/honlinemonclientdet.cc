#include "honlinemonclientdet.h"

#include "honlinemonclientsimplewin.h"
#include "honlinemonclienttabwin.h"

#include <cstdlib>

ClassImp(HOnlineMonClientDet);

/* -------------------------------------------------------------------------- */
HOnlineMonClientDet::HOnlineMonClientDet() {
    active = kFALSE;
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientDet::~HOnlineMonClientDet() {
    listWindows.Delete();
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientDet::CreateDet(HOnlineMonClientMain *clientmain) {
    for (Int_t i=0;i<listWindows.LastIndex()+1;i++) {
	if (listWindows.At(i)->IsA() == HOnlineMonClientSimpleWin::Class()) {
	    HOnlineMonClientSimpleWin* simplewin = (HOnlineMonClientSimpleWin*) listWindows.At(i);
	    simplewin->CreateWindow(clientmain, this->GetName());
	    simplewin->SetActive(kTRUE);
	} else {

	    if (listWindows.At(i)->IsA() == HOnlineMonClientTabWin::Class()) {
		HOnlineMonClientTabWin* tabwin = (HOnlineMonClientTabWin*) listWindows.At(i);
		tabwin->CreateWindow(clientmain, this->GetName());
		tabwin->SetActive(kTRUE);
	    } else {
		Printf("Unknown window class in detector '%s'", this->GetName());
		exit(1);
	    }

	}
    }

    this->SetActive(kTRUE);
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientDet::DestroyDet(HOnlineMonClientMain *clientmain) {
    this->SetActive(kFALSE);

    for (Int_t i=0;i<listWindows.LastIndex()+1;i++) {
	if (listWindows.At(i)->IsA() == HOnlineMonClientSimpleWin::Class()) {
	    HOnlineMonClientSimpleWin* simplewin = (HOnlineMonClientSimpleWin*) listWindows.At(i);
	    simplewin->DestroyWindow(clientmain);
	} else {

	    if (listWindows.At(i)->IsA() == HOnlineMonClientTabWin::Class()) {
		HOnlineMonClientTabWin* tabwin = (HOnlineMonClientTabWin*) listWindows.At(i);
		tabwin->DestroyWindow(clientmain);
	    }

	}
    }
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientDet::SetActive(Bool_t state) {
    for (Int_t i=0;i<listWindows.LastIndex()+1;i++) {
	if (listWindows.At(i)->IsA() == HOnlineMonClientSimpleWin::Class()) {
	    HOnlineMonClientSimpleWin* simplewin = (HOnlineMonClientSimpleWin*) listWindows.At(i);
	    simplewin->SetActive(state);
	} else {

	    if (listWindows.At(i)->IsA() == HOnlineMonClientTabWin::Class()) {
		HOnlineMonClientTabWin* tabwin = (HOnlineMonClientTabWin*) listWindows.At(i);
		tabwin->SetActive(state);
	    }

	}
    }

    this->active = state;
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientDet::IsActive() {
    return this->active;
}
/* -------------------------------------------------------------------------- */

