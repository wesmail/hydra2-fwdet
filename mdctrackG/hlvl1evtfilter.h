#ifndef HLVL1EVTFILTER_H
#define HLVL1EVTFILTER_H

#include "hreconstructor.h"

class HCategory;
class HIterator;
class HLvl1EvtFilter : public HReconstructor {
public:

    HLvl1EvtFilter();
    HLvl1EvtFilter(const Text_t *name,const Text_t *title,TString opt, Int_t multMeta=0, Int_t multTof=0);
    ~HLvl1EvtFilter();

    Bool_t init();
    Bool_t finalize();
    Int_t execute();

    void resetCounters();

    Bool_t isFilteredEvt();

private:

    HCategory* fTofRaw; //!
    HIterator* fTofIter; //!

    Int_t nCntProcessedEvents;//!
    Int_t nCounterNbRejectedEvts;//!

    Bool_t kOpSec;//!
    Bool_t isSimulation;//!
    Int_t nMetaMult;//!
    Int_t nTofinoMult;//!
    Int_t nTofMult;//!

    ClassDef(HLvl1EvtFilter,0) //ROOT extension
};

#endif /* !HLVL1EVTFILTER_H */
