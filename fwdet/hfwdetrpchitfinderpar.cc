//*-- Author  : Rafal Lalik
//*-- Created : 10.01.2017

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcHitFinderPar
//
//  Container class of Forward Rpc Hit Finder par
//
/////////////////////////////////////////////////////////////

#include "hfwdetrpchitfinderpar.h"
#include "hparamlist.h"

#include <iostream>

ClassImp(HFwDetRpcHitFinderPar);

HFwDetRpcHitFinderPar::HFwDetRpcHitFinderPar(const Char_t* name, const Char_t* title,
                       const Char_t* context) : HParCond(name, title, context)
{
    clear();
}

HFwDetRpcHitFinderPar::~HFwDetRpcHitFinderPar()
{
}

void HFwDetRpcHitFinderPar::clear()
{
    // clears the container
    fMatchRadius = 0.0;
    fMatchTime = 0.0;
    status = kFALSE;
    resetInputVersions();
    changed = kFALSE;
}

void HFwDetRpcHitFinderPar::putParams(HParamList* l)
{
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("fMatchRadius", fMatchRadius );
    l->add("fMatchTime", fMatchTime );
}

Bool_t HFwDetRpcHitFinderPar::getParams(HParamList* l)
{
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!( l->fill("fMatchRadius", &fMatchRadius))) return kFALSE;
    if (!( l->fill("fMatchTime", &fMatchTime))) return kFALSE;
    return kTRUE;
}

void HFwDetRpcHitFinderPar::printParam() const
{
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
    std::cout << "Radius matching:       " << fMatchRadius << std::endl;
    std::cout << "Time matching:         " << fMatchTime << std::endl;
}
