//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : G. Kornakov
////////////////////////////////////////////////////////////////////////////
// HRpcTimePosPar
//
// Container class for RPC position-time correction parameters
// used by HRpcHitF
//
// (Condition Style)
//
// Time-position Correction parameterization
// 5 harmonics are used to desribe the correction
//
////////////////////////////////////////////////////////////////////////////
#include "hrpctimepospar.h"
#include "hparamlist.h"

ClassImp(HRpcTimePosPar)

HRpcTimePosPar::HRpcTimePosPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{
    f5harmonics_16.Set(1152*16);
    clear();
}
HRpcTimePosPar::~HRpcTimePosPar()
{
  // destructor
}
void HRpcTimePosPar::clear()
{

    f5harmonics_16.Reset(0.);
    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;

}
void HRpcTimePosPar::putParams(HParamList* l)
{
    // Puts all params of HRpcTimePosPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("f5harmonics_16", f5harmonics_16);

}
Bool_t HRpcTimePosPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("f5harmonics_16", &f5harmonics_16))) return kFALSE;

    return kTRUE;
}
