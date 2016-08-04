// $Id: htimelvl1evtfilter.cc,v 1.4 2009-07-15 11:35:01 halo Exp $
// Last update by Filip Krizek: 24/11/06 
//
using namespace std;
#include "htimelvl1evtfilter.h"

#include "hades.h"
#include "hcategory.h"
#include "hevent.h"
#include "hiterator.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "htofhitsim.h"
#include "tofdef.h"

#include "TRandom.h"

#include <iostream>
#include <iomanip>

ClassImp(HTimeLvl1EvtFilter)

    HTimeLvl1EvtFilter::HTimeLvl1EvtFilter(const Text_t *name,const Text_t *title,TString opt,Int_t m,Float_t timecut, Float_t MultSmearing) :
HReconstructor(name,title)
{
    filterOption=opt;
    nMult=m;
    fTimeCut=timecut;
    fMultSmearing=MultSmearing;
    fTofHits=NULL;
    fTofIter=NULL;
}


HTimeLvl1EvtFilter::~HTimeLvl1EvtFilter(void) {

}

Bool_t HTimeLvl1EvtFilter::init() {
    if (gHades) {
	HEvent *event=gHades->getCurrentEvent();
	if (event) {

	    if (filterOption.Contains("tofmult"))
	    {
		fTofHits=event->getCategory(catTofHit);
		if (!fTofHits) {
		    Error("init","No TOF input");
		    return kFALSE;
		}
		fTofIter=(HIterator *)fTofHits->MakeIterator();
	    }

	}

    }

    resetCounters();
    return kTRUE;
}

Bool_t HTimeLvl1EvtFilter::finalize() {
    cout<<"Number of rejected evts: "<<nCounterNbRejectedEvts<<endl;
    cout<<"Number of remaining evts: "<<nCntProcessedEvents-nCounterNbRejectedEvts<<endl;
    cout<<"Percentage of rejected evts: "<<(((Float_t)nCounterNbRejectedEvts)/((Float_t)nCntProcessedEvents))*100<<"%"<<endl;
    return kTRUE;
}

Int_t HTimeLvl1EvtFilter::execute()
{
    // refer to isFilteredEvt(swt) for filter options
    nCntProcessedEvents++;

    Int_t kReturnValue=0;

    if (isFilteredEvt(filterOption))
    {
	nCounterNbRejectedEvts++;
	kReturnValue = kSkipEvent;
    }
    else if (isSkipNextTask(0)) kReturnValue = 1;


    return kReturnValue;
}

void HTimeLvl1EvtFilter::resetCounters()
{
    nCntProcessedEvents=0;
    nCounterNbRejectedEvts=0;
}

Bool_t HTimeLvl1EvtFilter::isFilteredEvt(TString opt)
{

    // if function returns kTRUE evt is skipped
    Bool_t kSwitch=kFALSE;

    if (opt.Contains("tofmult"))
    {
	if (!isTofMultTrigger()) kSwitch=kTRUE;
    }

    return kSwitch;
}

Bool_t HTimeLvl1EvtFilter::isSkipNextTask(Int_t swt)
{
    Bool_t kSwitch=kFALSE;

    switch (swt)
    {
    case 0:

	break;

    default:
	kSwitch=kFALSE;

    }
    return kSwitch;
}




Bool_t HTimeLvl1EvtFilter::isTofMultTrigger()
{


    Bool_t r=kFALSE;

    if(!fTofIter) return r;


    Int_t nTH=0;
    Float_t tof;
    Float_t random_mult; //should include effects of electronics

    HTofHit *pTof = 0;

    fTofIter->Reset();
    while((pTof = (HTofHit *)fTofIter->Next()))
    {
	tof=pTof->getTof();
	if(tof < fTimeCut) nTH++;
    }

    random_mult=gRandom->Gaus(0,fMultSmearing);
    //    cout<<"TOF Mult: "<<nTH<<"   and Tofino Mult: "<<nTiH<<endl;
    if ((nTH) >= (nMult+random_mult)){
	r=kTRUE;
    }
    return r;
}



