//*-- Author   : Ilse Koenig
//*-- Created  : 19.02.2016 

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetScinDigiGeomPar
//
//  Container class of Digitization parameters of the Forward Scintillator detector
//
/////////////////////////////////////////////////////////////

#include "hfwdetscindigipar.h"
#include "hparamlist.h"

ClassImp(HFwDetScinDigiPar)

HFwDetScinDigiPar::HFwDetScinDigiPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context) {
    scinSize.Set(2);
    clear();
}

void HFwDetScinDigiPar::clear() {
  // clears the container
    nScinCells = 0;
    scinSize.Reset();
    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}

void HFwDetScinDigiPar::putParams(HParamList* l) {
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("nScinCells", nScinCells );
    l->add("scinSize",   scinSize );
}

Bool_t HFwDetScinDigiPar::getParams(HParamList* l) {
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!( l->fill("nScinCells", &nScinCells))) return kFALSE;
    if (!( l->fill("scinSize",   &scinSize))) return kFALSE;
    return kTRUE;
}

