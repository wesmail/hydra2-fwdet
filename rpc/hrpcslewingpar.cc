//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : G. Kornakov
////////////////////////////////////////////////////////////////////////////
// HRpcSlewingPar
//
// Container class for RPC slewing correction parameters
// used by HRpcHitF
//
// (Condition Style)
//
// Slewing Correction parameterization (time=f(Q))
// Two exponentials describe the charge-time dependence
//
////////////////////////////////////////////////////////////////////////////
#include "hrpcslewingpar.h"
#include "hparamlist.h"

ClassImp(HRpcSlewingPar)

HRpcSlewingPar::HRpcSlewingPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{
    fExpo1.Set(1152*3);
    fExpo2.Set(1152*3);
    clear();
}
HRpcSlewingPar::~HRpcSlewingPar()
{
  // destructor
}
void HRpcSlewingPar::clear()
{

    fExpo1.Reset(0.);
    fExpo2.Reset(0.);
    fQcut = 0.;
    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;

}
void HRpcSlewingPar::putParams(HParamList* l)
{
    // Puts all params of HRpcSlewingPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("fExpo1", fExpo1);
    l->add("fExpo2", fExpo2);
    l->add("fQcut",   fQcut);

}
Bool_t HRpcSlewingPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fExpo1", &fExpo1))) return kFALSE;
    if(!( l->fill("fExpo2", &fExpo2))) return kFALSE;
    if(!( l->fill("fQcut",   &fQcut))) return kFALSE;

    return kTRUE;
}
