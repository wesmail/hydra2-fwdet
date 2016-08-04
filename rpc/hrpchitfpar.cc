//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : P. Cabanelas
////////////////////////////////////////////////////////////////////////////
// HRpcHitFPar
//
// Container class for RPC hit finder parameters
//
// (Condition Style)
//
// Slewing Correction parameterization (time=f(Q))
//  - to a fifth order pol function from 0 to fQcut
//  - to a straight line (1st order pol) after fQcut
//
////////////////////////////////////////////////////////////////////////////
#include "hrpchitfpar.h"
#include "hparamlist.h"

ClassImp(HRpcHitFPar)

HRpcHitFPar::HRpcHitFPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{
    fPol5Pars.Set(1152*6);
    fPol1Pars.Set(1152*2);
    clear();
}
HRpcHitFPar::~HRpcHitFPar()
{
  // destructor
}
void HRpcHitFPar::clear()
{
    for(Int_t i=0;i<1152*6;i++) {
			fPol5Pars[i]=0.;
    }
    for(Int_t i=0;i<1152*2;i++) {
			fPol1Pars[i]=0.;
    }
    fQcut=0.;
    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}
void HRpcHitFPar::putParams(HParamList* l)
{
    // Puts all params of HRpcHitFPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("fPol5Pars",fPol5Pars);
    l->add("fPol1Pars",fPol1Pars);
    l->add("fQcut",    fQcut);

}
Bool_t HRpcHitFPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fPol5Pars",&fPol5Pars))) return kFALSE;
    if(!( l->fill("fPol1Pars",&fPol1Pars))) return kFALSE;
    if(!( l->fill("fQcut",    &fQcut)))     return kFALSE;

    return kTRUE;
}
