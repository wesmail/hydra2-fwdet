//*-- Author  : Luis Silva
//*-- Created : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcDigiGeomPar
//
//  Container class of Forward Rpc detector Digi par
//
/////////////////////////////////////////////////////////////

#include "hfwdetrpcdigipar.h"
#include "hparamlist.h"

ClassImp(HFwDetRpcDigiPar);

HFwDetRpcDigiPar::HFwDetRpcDigiPar(const Char_t* name, const Char_t* title,
                       const Char_t* context) : HParCond(name, title, context)
{
    clear();
}

HFwDetRpcDigiPar::~HFwDetRpcDigiPar()
{
}

void HFwDetRpcDigiPar::clear()
{
    // clears the container
    nDigiModel = 0;
    fTimeReso = 0.0;
    fTimeProp = 0.0;
    fTimeOffset = 0.0;

    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;
}

void HFwDetRpcDigiPar::putParams(HParamList* l)
{
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("nDigiModel", nDigiModel);
    l->add("fTimeReso", fTimeReso);
    l->add("fTimeProp", fTimeProp);
    l->add("fTimeOffset", fTimeOffset);
}

Bool_t HFwDetRpcDigiPar::getParams(HParamList* l)
{
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!( l->fill("nDigiModel", &nDigiModel))) return kFALSE;
    if (!( l->fill("fTimeReso", &fTimeReso))) return kFALSE;
    if (!( l->fill("fTimeProp", &fTimeProp))) return kFALSE;
    if (!( l->fill("fTimeOffset", &fTimeOffset))) return kFALSE;
    return kTRUE;
}
