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
    fDriftTimePars.Set(dt_pars_num);
    fDriftRadiusPars.Set(dt_pars_num);
    clear();
}

HFwDetStrawDigiPar::~HFwDetStrawDigiPar()
{
}

void HFwDetStrawDigiPar::clear()
{
    fAnalogReso = 0.0;
    fElossSlope = 0.0;
    fElossOffset = 0.0;
    fTimeReso = 0.0;
    fDriftRadiusPars = 0.0;
    fDriftTimePars = 0.0;
    fStartOffset = 0.0;
    fThreshold = 0.0;
    fEfficiency = 0.0;

    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;
}

void HFwDetStrawDigiPar::putParams(HParamList* l)
{
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("fAnalogReso", fAnalogReso);
    l->add("fElossSlope", fElossSlope);
    l->add("fElossOffset", fElossOffset);
    l->add("fTimeReso", fTimeReso);
    l->add("fDriftRadiusPars", fDriftRadiusPars);
    l->add("fDriftTimePars", fDriftTimePars);
    l->add("fStartOffset", fStartOffset);
    l->add("fThreshold", fThreshold);
    l->add("fEfficiency", fEfficiency);
}

Bool_t HFwDetStrawDigiPar::getParams(HParamList* l)
{
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!( l->fill("fAnalogReso", &fAnalogReso) )) return kFALSE;
    if (!( l->fill("fElossSlope", &fElossSlope) )) return kFALSE;
    if (!( l->fill("fElossOffset", &fElossOffset) )) return kFALSE;
    if (!( l->fill("fTimeReso", &fTimeReso) )) return kFALSE;
    if (!( l->fill("fDriftRadiusPars", &fDriftRadiusPars) )) return kFALSE;
    if (!( l->fill("fDriftTimePars", &fDriftTimePars) )) return kFALSE;
    if (!( l->fill("fStartOffset", &fStartOffset) )) return kFALSE;
    if (!( l->fill("fThreshold", &fThreshold) )) return kFALSE;
    if (!( l->fill("fEfficiency", &fEfficiency) )) return kFALSE;
    return kTRUE;
}

void HFwDetStrawDigiPar::setDriftTimePar(Int_t idx, Float_t p)
{
    if (idx < dt_pars_num)
        fDriftTimePars[idx] = p;
}

Float_t HFwDetStrawDigiPar::getDriftTimePar(Int_t idx) const
{
    if (idx < dt_pars_num)
        return fDriftTimePars[idx];
    else
        return 0.0;
}

void HFwDetStrawDigiPar::setDriftRadiusPar(Int_t idx, Float_t p)
{
    if (idx < dt_pars_num)
        fDriftRadiusPars[idx] = p;
}

Float_t HFwDetStrawDigiPar::getDriftRadiusPar(Int_t idx) const
{
    if (idx < dt_pars_num)
        return fDriftRadiusPars[idx];
    else
        return 0.0;
}
