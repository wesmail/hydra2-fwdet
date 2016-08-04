//*-- Author  : Georgy Kornakov
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

ClassImp(HFwDetRpcDigiPar)

HFwDetRpcDigiPar::HFwDetRpcDigiPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context) {
    clear();
}

void HFwDetRpcDigiPar::clear() {
  // clears the container
    nRpcCells = 0;
    rpcRadius = 0.F;
    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}

void HFwDetRpcDigiPar::putParams(HParamList* l) {
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("nRpcCells", nRpcCells );
    l->add("rpcRadius", rpcRadius );
}

Bool_t HFwDetRpcDigiPar::getParams(HParamList* l) {
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!( l->fill("nRpcCells", &nRpcCells))) return kFALSE;
    if (!( l->fill("rpcRadius", &rpcRadius))) return kFALSE;
    return kTRUE;
}

