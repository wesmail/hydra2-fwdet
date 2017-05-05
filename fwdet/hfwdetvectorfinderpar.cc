//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HFwDetVectorFinderPar
//
// Container class for VectorFinder hit finder parameters
//
/////////////////////////////////////////////////////////////

#include "hfwdetvectorfinderpar.h"
#include "hparamlist.h"

ClassImp (HFwDetVectorFinderPar)

HFwDetVectorFinderPar::HFwDetVectorFinderPar(const Char_t * name, const Char_t * title, const Char_t * context)
    : HParCond (name, title, context)
{
    clear();
}

void HFwDetVectorFinderPar::clear()
{
    // Clear the container
    fMatchR = 0.0;
    fMatchCross = 0.0;
    fCutX = 0.0;
    fCutY = 0.0;
    fLRCutChi2 = 0.0;
    fHRCutChi2 = 0.0;
    fTanCut = 0.0;
    fTxyCut = 0.0;
    fLRErrU = 0.0;
    fHRErrU = 0.0;
    nMaxBest = 0;
    nPass = 0;
}

void HFwDetVectorFinderPar::putParams(HParamList * l)
{
    // Add the parameters to the list for writing
    if (!l) return;

    l->add("fMatchR", fMatchR);
    l->add("fMatchCross", fMatchCross);
    l->add("fCutX", fCutX);
    l->add("fCutY", fCutY);
    l->add("fLRCutChi2", fLRCutChi2);
    l->add("fHRCutChi2", fHRCutChi2);
    l->add("fTanCut", fTanCut);
    l->add("fTxyCut", fTxyCut);
    l->add("fLRErrU", fLRErrU);
    l->add("fHRErrU", fHRErrU);
    l->add("nMaxBest", nMaxBest);
    l->add("nPass", nPass);
}

Bool_t HFwDetVectorFinderPar::getParams(HParamList * l)
{
    // Get the parameters from the list (read from input)
    if (!l) return kFALSE;

    if (!l->fill ("fMatchR", &fMatchR)) return kFALSE;
    if (!l->fill ("fMatchCross", &fMatchCross)) return kFALSE;
    if (!l->fill ("fCutX", &fCutX)) return kFALSE;
    if (!l->fill ("fCutY", &fCutY)) return kFALSE;
    if (!l->fill ("fLRCutChi2", &fLRCutChi2)) return kFALSE;
    if (!l->fill ("fHRCutChi2", &fHRCutChi2)) return kFALSE;
    if (!l->fill ("fTanCut", &fTanCut)) return kFALSE;
    if (!l->fill ("fTxyCut", &fTxyCut)) return kFALSE;
    if (!l->fill ("fLRErrU", &fLRErrU)) return kFALSE;
    if (!l->fill ("fHRErrU", &fHRErrU)) return kFALSE;
    if (!l->fill ("nMaxBest", &nMaxBest)) return kFALSE;
    if (!l->fill ("nPass", &nPass)) return kFALSE;

    return kTRUE;
}
