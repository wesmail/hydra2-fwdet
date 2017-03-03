//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HFwDetStrawVectorFinderPar
//
// Container class for PionTracker hit finder parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hfwdetstrawvectorfinderpar.h"
#include "hparamlist.h"
#include <iostream>

ClassImp (HFwDetStrawVectorFinderPar)

HFwDetStrawVectorFinderPar::HFwDetStrawVectorFinderPar(const Char_t * name, const Char_t * title, const Char_t * context)
    : HParCond (name, title, context)
{
    clear();
}

void HFwDetStrawVectorFinderPar::clear()
{
    // clears the container
    fCutX = 0.0;
    fCutY = 0.0;
    fLRCutChi2 = 0.0;
    fHRCutChi2 = 0.0;
    fLRErrU = 0.0;
    fHRErrU = 0.0;
    nPass = 0;
}

void HFwDetStrawVectorFinderPar::putParams(HParamList * l)
{
    // add the parameters to the list for writing
    if (!l) return;

    l->add("fCutX", fCutX);
    l->add("fCutY", fCutY);
    l->add("fLRCutChi2", fLRCutChi2);
    l->add("fHRCutChi2", fHRCutChi2);
    l->add("fLRErrU", fLRErrU);
    l->add("fHRErrU", fHRErrU);
    l->add("nPass", nPass);
}

Bool_t HFwDetStrawVectorFinderPar::getParams(HParamList * l)
{
    // gets the parameters from the list (read from input)
    if (!l) return kFALSE;

    if (!l->fill ("fCutX", &fCutX)) return kFALSE;
    if (!l->fill ("fCutY", &fCutY)) return kFALSE;
    if (!l->fill ("fLRCutChi2", &fLRCutChi2)) return kFALSE;
    if (!l->fill ("fHRCutChi2", &fHRCutChi2)) return kFALSE;
    if (!l->fill ("fLRErrU", &fLRErrU)) return kFALSE;
    if (!l->fill ("fHRErrU", &fHRErrU)) return kFALSE;
    if (!l->fill ("nPass", &nPass)) return kFALSE;

    return kTRUE;
}