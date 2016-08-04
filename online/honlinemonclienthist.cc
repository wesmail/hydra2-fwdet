#include "honlinemonclienthist.h"

ClassImp(HOnlineMonClientHist);

/* -------------------------------------------------------------------------- */
HOnlineMonClientHist::HOnlineMonClientHist() {
	histo = 0;
	subpadnumber = 0;
	active = kFALSE;
	index_s = -1;
	index_m = -1;
	type = "";
        guiname ="";
}
/* -------------------------------------------------------------------------- */
HOnlineMonClientHist::~HOnlineMonClientHist() {
    if (histo) {
        // cast the HOnlineMonHistAddon pointer
        if (histo->IsA() == HOnlineMonHist::Class()) {
            delete (HOnlineMonHist*) histo;
            histo = 0;
        }
        
        if (histo->IsA() == HOnlineMonHist2::Class()) {
            delete (HOnlineMonHist2*) histo;
            histo = 0;
        }
        
        if (histo->IsA() == HOnlineTrendHist::Class()) {
            delete (HOnlineTrendHist*) histo;
            histo = 0;
        }
           
        if (histo->IsA() == HOnlineHistArray::Class()) {
            delete (HOnlineHistArray*) histo;
            histo = 0;
        }
        
        if (histo->IsA() == HOnlineHistArray2::Class()) {
            delete (HOnlineHistArray2*) histo;
            histo = 0;
        }
        
        if (histo->IsA() == HOnlineTrendArray::Class()) {
            delete (HOnlineTrendArray*) histo;
            histo = 0;
        }
        
    }
	
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetHist(HOnlineMonHistAddon* hist) {
    if (hist) {
	if (this->histo) {
	    delete this->histo;
	    this->histo = 0;
	}
	this->histo = hist;
    }
}
/* -------------------------------------------------------------------------- */
HOnlineMonHistAddon* HOnlineMonClientHist::GetHist() {
    if (this->histo) {
	return histo;
    }
    return 0;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetSubpadnumber(Int_t nr) {
    this->subpadnumber = nr;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientHist::GetSubpadnumber() {
    return subpadnumber;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetActive(Bool_t state) {
    this->active = state;
}
/* -------------------------------------------------------------------------- */
Bool_t HOnlineMonClientHist::IsActive() {
    return active;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetArrayIndexS(Int_t s) {
    this->index_s = s;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientHist::GetArrayIndexS() {
    return this->index_s;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetArrayIndexM(Int_t m) {
    this->index_m = m;
}
/* -------------------------------------------------------------------------- */
Int_t HOnlineMonClientHist::GetArrayIndexM() {
    return this->index_m;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetType(TString histotype) {
    this->type = histotype;
}
/* -------------------------------------------------------------------------- */
TString HOnlineMonClientHist::GetType() {
    return this->type;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetRealName(TString rname) {
    this->realname = rname;
}
/* -------------------------------------------------------------------------- */
TString HOnlineMonClientHist::GetRealName() {
    return this->realname;
}
/* -------------------------------------------------------------------------- */
void HOnlineMonClientHist::SetGuiName(TString rname) {
    this->guiname = rname;
}
/* -------------------------------------------------------------------------- */
TString HOnlineMonClientHist::GetGuiName() {
    return this->guiname;
}
/* -------------------------------------------------------------------------- */
