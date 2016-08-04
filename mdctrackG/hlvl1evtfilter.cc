using namespace std;
//*-- Author : Stefano Spataro

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////
// HLvl1EvtFilter
// Emulator of first level trigger.
// How to use it:
//
// HTaskSet *evtfilter = new HTaskSet("","");
// HLvl1EvtFilter *evtflt = new HLvl1EvtFilter("eventfilter","eventfilter",options,multMeta,multTtof);
// evtfilter->connect(evtflt);
// masterTaskSet->add(evtflt);
//
// where options stays for:
// "metamult"   ->  META multiplicity   >= multMeta
// "tofmult"    ->  TOF multiplicity    >= multTof
// "opsec"  ->  Opposite sectors in META
// "sim"        ->  Simulation mode ON

#include "hlvl1evtfilter.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hades.h"
#include "htofraw.h"
#include "tofdef.h"
#include "TFile.h"
#include <iostream>

ClassImp(HLvl1EvtFilter)
    HLvl1EvtFilter::HLvl1EvtFilter(const Text_t *name,const Text_t *title,TString opt,Int_t multMeta, Int_t multTof)
    : HReconstructor(name,title)
{
    // constructor  options:
    // "metamult"   ->  META multiplicity   >= multMeta
    // "tofmult"    ->  TOF multiplicity    >= multTof
    // "opsec"      ->  Opposite sectors in META
    // "sim"        ->  Simulation mode ON

    nMetaMult = 0;
    nTofMult = 0;
    kOpSec = kFALSE;
    isSimulation = kFALSE;
    fTofIter = NULL;
    fTofRaw  = NULL;

    if ((opt.Contains("metamult"))   && (multMeta!=0))   nMetaMult=multMeta;
    if ((opt.Contains("tofmult"))    && (multTof!=0))    nTofMult=multTof;
    if  (opt.Contains("opsec"))      kOpSec = kTRUE;
    if  (opt.Contains("sim"))        isSimulation = kTRUE;

} // end of constructor

HLvl1EvtFilter::HLvl1EvtFilter()
{
    // Default constructor

    nMetaMult = 0;
    nTofMult = 0;
    kOpSec = kFALSE;
    isSimulation = kFALSE;
    fTofIter = NULL;
    fTofRaw  = NULL;

}


HLvl1EvtFilter::~HLvl1EvtFilter(void) {
    // destructor
}

Bool_t HLvl1EvtFilter::init() {
    // initialization function

    if (gHades) {
	HEvent *event=gHades->getCurrentEvent();
	if (event) {

	    fTofRaw=event->getCategory(catTofRaw);
	    if (!fTofRaw) {
		Error("init","No TOF input");
		return kFALSE;
	    }
	    fTofIter=(HIterator *)fTofRaw->MakeIterator();

	} // end of event && rtdb condition
    } // end of gHades

    cout << " HLvl1EvtFilter: *** 1st level trigger event filter initialization ***" << endl;
    cout << " META >= " << nMetaMult << "\tTOF >= " << nTofMult<< "\t Opposite sector " << kOpSec << "\t Simulation: "<< isSimulation << endl;

    resetCounters();

    return kTRUE;
} // end of initialization

Bool_t HLvl1EvtFilter::finalize()
{
    // Finalize: write number of rejected events
    cout<<"Number of rejected evts: "<<nCounterNbRejectedEvts<<endl;
    cout<<"Number of remaining evts: "<<nCntProcessedEvents-nCounterNbRejectedEvts<<endl;
    cout<<"Percentage of rejected evts: "<<(((Float_t)nCounterNbRejectedEvts)/((Float_t)nCntProcessedEvents))*100<<"%"<<endl;

    return kTRUE;

}

Int_t HLvl1EvtFilter::execute()
{
    // Execute function: refer to isFilteredEvt(swt) for filter options
    nCntProcessedEvents++;

    Int_t kReturnValue=0;

    if (isFilteredEvt())
    {
	nCounterNbRejectedEvts++;
	kReturnValue = kSkipEvent;
    }

    return kReturnValue;
}

void HLvl1EvtFilter::resetCounters()
{
    // Reset counters of processed and rejected events

    nCntProcessedEvents=0;
    nCounterNbRejectedEvts=0;
}

Bool_t HLvl1EvtFilter::isFilteredEvt()
{
    // if function returns kTRUE evt is skipped

    Int_t TofLeftMult[6] = {0,0,0,0,0,0};
    Int_t TofRightMult[6] = {0,0,0,0,0,0};
    Bool_t MetaMult[6]   = {kFALSE,kFALSE,kFALSE,kFALSE,kFALSE,kFALSE};
    Int_t tofraw_count = 0,  meta_mult = 0, tof_mult = 0;
    Bool_t opposite_sector = kFALSE;
    Bool_t kSwitch = kFALSE;


    if(!fTofIter ) return kSwitch;
    HTofRaw *TofRaw = 0;

    fTofIter->Reset();
    while ((TofRaw=(HTofRaw *)fTofIter->Next()) != 0) // start TofRaw Loop
    {
	if (TofRaw->getRightTime()>0.)
	{
	    TofRightMult[(Int_t)TofRaw->getSector()]++;
	    tofraw_count++;
	}
	if (TofRaw->getLeftTime()>0.)
	{
	    TofLeftMult[(Int_t)TofRaw->getSector()]++;
	    tofraw_count++;
	}
    } // end of HTofRaw loop


    for (Int_t yyy=0; yyy<6; yyy++) MetaMult[yyy]=(((TofRightMult[yyy]+TofLeftMult[yyy])>=2));

    opposite_sector = ((MetaMult[0]&&MetaMult[3]) || (MetaMult[1]&&MetaMult[4]) || (MetaMult[2]&&MetaMult[5]));
    meta_mult = int(tofraw_count/2);
    tof_mult = int(tofraw_count/2);

    if ((meta_mult<nMetaMult) || (tof_mult<nTofMult) || (!opposite_sector&&kOpSec))
	kSwitch = kTRUE; // if at least one condition is not fulfilled, turn on skip flag

    return kSwitch;
}

