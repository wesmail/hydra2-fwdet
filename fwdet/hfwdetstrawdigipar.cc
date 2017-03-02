//*-- Author  : Georgy Kornakov
//*-- Created : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetStrawDigiGeomPar
//
//  Container class of Forward Straw detector Digi par
//
/////////////////////////////////////////////////////////////

#include "hfwdetstrawdigipar.h"
#include "hparamlist.h"

ClassImp(HFwDetStrawDigiPar);

HFwDetStrawDigiPar::HFwDetStrawDigiPar(const Char_t* name, const Char_t* title,
                    const Char_t* context) : HParCond(name, title, context)
{
    clear();
}

HFwDetStrawDigiPar::~HFwDetStrawDigiPar()
{
}

void HFwDetStrawDigiPar::clear()
{
    fTimeReso = 0.0;
    fElossReso = 0.0;
    fDriftReso = 0.0;
    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;
}

void HFwDetStrawDigiPar::putParams(HParamList* l)
{
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("fTimeReso", fTimeReso);
    l->add("fElossReso", fElossReso);
    l->add("fDriftReso", fDriftReso);
}

Bool_t HFwDetStrawDigiPar::getParams(HParamList* l)
{
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!( l->fill("fTimeReso", &fTimeReso) )) return kFALSE;
    if (!( l->fill("fElossReso", &fElossReso) )) return kFALSE;
    if (!( l->fill("fDriftReso", &fDriftReso) )) return kFALSE;
    return kTRUE;
}
