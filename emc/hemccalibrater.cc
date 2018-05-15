//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////
//
//  HEmcCalibrater:
//
//  Calibrates all fired cells in EmcRaw category and fills
//  the EmcCal category
//
//  embedding = 0 no embedding
//            = 1 embedding
////////////////////////////////////////////////////////////////

#include "hemccalibrater.h"
#include "emcdef.h"
#include "hemcraw.h"
#include "hemccalsim.h"
#include "hemcdetector.h"
#include "hemccalpar.h"
#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hrun.h"
#include "hspectrometer.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

ClassImp(HEmcCalibrater)
HEmcCalibrater::HEmcCalibrater(void)
{
    // default constructor
    pRawCat = NULL;
    pCalCat = NULL;
    iter    = NULL;
    pCalpar = NULL;
    pDet    = NULL;
    embedding = 0;
}

HEmcCalibrater::HEmcCalibrater(const Text_t *name, const Text_t *title)
: HReconstructor(name, title)
{
    // constructor
    pRawCat = NULL;
    pCalCat = NULL;
    iter    = NULL;
    pCalpar = NULL;
    pDet    = NULL;
    embedding = 0;
}

HEmcCalibrater::~HEmcCalibrater(void)
{
    //destructor deletes the iterator on the raw category
    if (NULL != iter) {
	delete iter;
	iter = NULL;
    }
}

Bool_t HEmcCalibrater::init(void)
{
    // gets the calibration parameter container
    // gets the EmcRaw category and creates the EmcCal category
    // creates an iterator which loops over all fired cells in EmcRaw
    pDet = (HEmcDetector*)gHades->getSetup()->getDetector("Emc");
    if (!pDet) {
	Error("init", "No Emc found.");
	return kFALSE;
    }

    pCalpar = (HEmcCalPar*)gHades->getRuntimeDb()->getContainer("EmcCalPar");
    if (!pCalpar) return kFALSE;

    pRawCat = gHades->getCurrentEvent()->getCategory(catEmcRaw);
    if (!pRawCat) {
	Error("init()", "HEmcRaw category not available!");
	return kFALSE;
    }

    if(gHades->getEmbeddingMode()>0) embedding = 1;

    pCalCat = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catEmcCal));
    if(pCalCat == NULL) {
	if (embedding == 0) pCalCat = pDet->buildMatrixCategory("HEmcCal",0.5);
	else                pCalCat = pDet->buildMatrixCategory("HEmcCalSim",0.5);
	if(pCalCat == NULL) return kFALSE;
	gHades->getCurrentEvent()->addCategory(catEmcCal,pCalCat,"Emc");
    }

    iter = (HIterator*)pRawCat->MakeIterator();
    loc.set(2, 0, 0);
    fActive = kTRUE;

    return kTRUE;
}

Int_t HEmcCalibrater::execute(void)
{
    // calibrates all fired cells
    HEmcRaw *pRaw = NULL;
    HEmcCal *pCal = NULL;

    Int_t  sec   = -1;
    Int_t  cell  = 0;
    Char_t row   = -1;
    Char_t col   = -1;
    UInt_t  nfasthits = 0, nslowhits=0;

    Double_t rawTimeLeading   = 0.;
    Double_t rawTimeTrailing= 0.;
    Double_t rawWidth  = 0.;
    Double_t calTimeLeading   = 0.;
    Double_t calTimeTrailing   = 0.;
    Double_t calCharge = 0.;

    Float_t parCellData[4] = {1.F, 0.F, 1.F, 0.F};

    //Fill cal category
    iter->Reset();
    while ((pRaw = (HEmcRaw *)iter->Next()) != 0) {
	nfasthits = pRaw->getFastMultiplicity();
	nslowhits = pRaw->getFastMultiplicity();

	pRaw->getAddress(sec, cell, row, col);
	loc[0] = sec;
	loc[1] = cell;

	if (sec >= 0 && nfasthits > 0 && nslowhits>0) {
	    pCal = (HEmcCal*)pCalCat->getObject(loc);
	    if (!pCal) {
		pCal = (HEmcCal *)pCalCat->getSlot(loc);
		if (pCal) {
		    if(embedding == 0) pCal = new(pCal) HEmcCal;
		    else               pCal = new(pCal) HEmcCalSim;

		    pCal->setAddress((Char_t)sec, (UChar_t)cell, row, col);
		} else {
		    Error("execute()", "Can't get slot sector=%i, cell=%i", sec, cell);
		    return -1;
		}
	    } else {
		Error("execute()", "Slot already exists for sector=%i, cell=%i", sec, cell);
		return -1;
	    }

	    // get the calibration parameters
	    HEmcCalParCell &pPar = (*pCalpar)[sec][cell];
	    pPar.getData(parCellData);


	    // JAM this is just a placeholder treating the first hit only!
	    // loop over number of hits in raw level
	    // here in the simplified version only the first hit is calibrated
	    //pRaw->getFastTimeAndWidth(1, rawTime, rawWidth);
	    // <JAM original concept, but we take width and time from different tdc channels!


	    // TODO: JAM filter out the relevant message inside the time window condition
	    // which message is the one we want to take for tot and time info?
	    // for the moment, we take the first accepted message (index 0) only...

	    // TODO: evaluate time and energy from fast and slow channels
	    // time: take time from fast channel
	    // energy: take from slow channel?
	    rawTimeLeading = pRaw->getFastTimeLeading(0);
	    rawWidth       = pRaw->getSlowWidth(0);
	    // TODO: check which fast and slow hits belong together?

	    // correct the time and width
	    calTimeLeading   = parCellData[0] * rawTimeLeading + parCellData[1];
	    calTimeTrailing  = parCellData[0] * rawTimeTrailing + parCellData[1];
	    calCharge        = parCellData[2] * rawWidth + parCellData[3];
	    // fill cal hits -
	    // TODO: ???? change definition of emc cal category? conflict with simulation...
	    pCal->setTime1(calTimeLeading);
	    pCal->setTime1(calTimeTrailing);
	    pCal->setEnergy(calCharge);
	    pCal->setNHits(1);
	    // we might want to have emc cal category element that can take more than one hit
	    ////////// END placeholder






	}
    }
    return 0;
}
