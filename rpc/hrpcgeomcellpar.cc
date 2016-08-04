//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : P. Cabanelas
////////////////////////////////////////////////////////////////////////////
// HRpcGeomCellPar
//
// Container class for RPC cells geometry parameters
//
// (Condition Style)
//
////////////////////////////////////////////////////////////////////////////
#include "hrpcgeomcellpar.h"
#include "hparamlist.h"
#include <stdlib.h>
#include "TString.h"
#include "hpario.h"
#include "hdetpario.h"


ClassImp(HRpcGeomCellPar)

HRpcGeomCellPar::HRpcGeomCellPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{
    fXcell.Set(192);
    fYcell.Set(192);
    fLcell.Set(192);
    fWcell.Set(192);
    clear();
}
HRpcGeomCellPar::~HRpcGeomCellPar()
{
  // destructor
}
void HRpcGeomCellPar::clear()
{
    for(Int_t i=0;i<192;i++) {
      fXcell[i]=fYcell[i]=fLcell[i]=fWcell[i]=0.;
    }
    fDPlanes=0.;
    fDeltaZ =0.;
    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}
void HRpcGeomCellPar::putParams(HParamList* l)
{
    // Puts all params of HRpcGeomCellPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("fXcell",  fXcell);
    l->add("fYcell",  fYcell);
    l->add("fLcell",  fLcell);
    l->add("fWcell",  fWcell);
    l->add("fDPlanes",fDPlanes);
    l->add("fDeltaZ", fDeltaZ);

}
Bool_t HRpcGeomCellPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fXcell",  &fXcell)))   return kFALSE;
    if(!( l->fill("fYcell",  &fYcell)))   return kFALSE;
    if(!( l->fill("fLcell",  &fLcell)))   return kFALSE;
    if(!( l->fill("fWcell",  &fWcell)))   return kFALSE;
    if(!( l->fill("fDPlanes",&fDPlanes))) return kFALSE;
    if(!( l->fill("fDeltaZ", &fDeltaZ)))  return kFALSE;

    return kTRUE;
}
