//*-- Author  : Georgy Kornakov
//*-- Created : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetStrawDigitizer
//
//  This class digitizes the Forward Straw detector data
//
//  Produce calibrated Time and energy loss and the straw number
//
/////////////////////////////////////////////////////////////

using namespace std;

#include "hfwdetstrawdigitizer.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hdebug.h"
#include "hspectrometer.h"
#include "hevent.h"
#include "hcategory.h"
#include "fwdetdef.h"
#include "hgeantkine.h"
#include "hgeantfwdet.h"
#include "hfwdetdetector.h"
#include "hfwdetstrawcalsim.h"
#include "hfwdetstrawdigipar.h"
//#include "TRandom.h"

ClassImp(HFwDetStrawDigitizer)

HFwDetStrawDigitizer::HFwDetStrawDigitizer(void) {
    // default constructor
    initVariables();
}

HFwDetStrawDigitizer::HFwDetStrawDigitizer(const Text_t *name, const Text_t *title)
                    : HReconstructor(name, title) {
    // constructor
    initVariables();
}

void HFwDetStrawDigitizer::initVariables(void) {
    // initialize the variables in constructor 
    fGeantFwDetCat    = 0;
    fFwDetStrawCalCat = 0;
    fStrawDigiPar     = 0;
    fLoc.setNIndex(2);
    fLoc.set(2,0,0);
}

Bool_t HFwDetStrawDigitizer::init(void) {
    // initializes the task

    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet){
        Error("FwDetStrawDigitizer::init","No Forward Detector found");
        return kFALSE;
    }

    // GEANT input data
    fGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!fGeantFwDetCat) {
      Error("HFwDetStrawDigitizer::init()","HGeant FwDet input missing");
      return kFALSE;
    }

    // build the Calibration category
    fFwDetStrawCalCat=pFwDet->buildCategory(catFwDetStrawCal);
    if (!fFwDetStrawCalCat) {
        Error("HFwDetStrawDigitizer::init()","Cal category not created");
        return kFALSE;
    }

    // create the parameter container
    fStrawDigiPar = (HFwDetStrawDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetStrawDigiPar");
    if (!fStrawDigiPar) {
         Error("HFwDetStrawDigitizer::init()","Parameter container for digitizer not created");
         return kFALSE;
    }
        
    return kTRUE;
}

Int_t HFwDetStrawDigitizer::execute(void) {
    // Digitization of GEANT hits and storage in HFwDetStrawCalSim

    HGeantFwDet* ghit = 0;
    HFwDetStrawCalSim* cal = 0;

    Int_t entries = fGeantFwDetCat->getEntries();
    for(Int_t i=0;i<entries;i++) {
	ghit = (HGeantFwDet*)fGeantFwDetCat->getObject(i);
	if(ghit) {
	    ghit->getAddress(module,geantCell);
            if(module>FWDET_STRAW_MAX_MODULES) continue; // skip the other detectors of the FwDet
	    ghit->getHit(xHit, yHit,  zHit, pxHit, pyHit, pxHit, tofHit, trackLength, eHit);
	    trackNumber = ghit->getTrackNumber();
            // calculate straw number and set location indexes
            strawNum = geantCell;  // preliminary!!!!!!!!!
            if (strawNum >= FWDET_STRAW_MAX_CELLS) continue;
            fLoc[0] = module;
	    fLoc[1] = strawNum;
            // do something with the Geant hit...
            //
            // fill the cal hit:
            // check if the object is already existing in the cal category
	    cal = (HFwDetStrawCalSim*)fFwDetStrawCalCat->getSlot(fLoc);
	    if (cal) {
		cal = new(cal) HFwDetStrawCalSim;
		cal->setAddress(module,geantCell,strawNum);
		cal->setHit(tofHit,eHit);
                cal->setTrack(trackNumber);
	    }
	}
    }
    return 0;

}



