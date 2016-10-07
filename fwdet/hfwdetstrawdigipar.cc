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
    nStrawCells = 0;
    fStrawRadius = 0.;
    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;
}

void HFwDetStrawDigiPar::putParams(HParamList* l)
{
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("nStrawCells", nStrawCells);
    l->add("fStrawRadius", fStrawRadius);
}

Bool_t HFwDetStrawDigiPar::getParams(HParamList* l)
{
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!( l->fill("nStrawCells", &nStrawCells) )) return kFALSE;
    if (!( l->fill("fStrawRadius", &fStrawRadius) )) return kFALSE;
    return kTRUE;
}
