//*-- Author   : Ilse Koenig
//*-- Created  : 19.02.2016 

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
//  HFwDetScinDigitizer
//
//  This class digitizes the Forward Scintillator detector data
//
//  Produces calibrated time and energy loss for each Scintillator cell
//
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "hfwdetscindigitizer.h"
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
#include "hfwdetscincalsim.h"
#include "hfwdetscindigipar.h"
//#include "TRandom.h"

ClassImp(HFwDetScinDigitizer)

HFwDetScinDigitizer::HFwDetScinDigitizer(void) {
    // default constructor
    initVariables();
}

HFwDetScinDigitizer::HFwDetScinDigitizer(const Text_t *name, const Text_t *title)
                    : HReconstructor(name, title) {
    // constructor
    initVariables();
}

void HFwDetScinDigitizer::initVariables(void) {
    // initialize the variables in constructor 
    fGeantFwDetCat    = 0;
    fFwDetScinCalCat = 0;
    fScinDigiPar     = 0;
    fLoc.setNIndex(2);
    fLoc.set(2,0,0);
}

Bool_t HFwDetScinDigitizer::init(void) {
    // initializes the task

    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet){
        Error("FwDetScinDigitizer::init","No Forward Detector found");
        return kFALSE;
    }

    // GEANT input data
    fGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!fGeantFwDetCat) {
      Error("HFwDetScinDigitizer::init()","HGeant FwDet input missing");
      return kFALSE;
    }

    // build the Calibration category
    fFwDetScinCalCat=pFwDet->buildCategory(catFwDetScinCal);
    if (!fFwDetScinCalCat) {
        Error("HFwDetScinDigitizer::init()","Cal category not created");
        return kFALSE;
    }

    // create the parameter container
    fScinDigiPar = (HFwDetScinDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetScinDigiPar");
    if (!fScinDigiPar) {
         Error("HFwDetScinDigitizer::init()","Parameter container for digitizer not created");
         return kFALSE;
    }
        
    return kTRUE;
}

Int_t HFwDetScinDigitizer::execute(void) {
    // Digitization of GEANT hits and storage in HFwDetScinCalSim

    HGeantFwDet* ghit = 0;
    HFwDetScinCalSim* cal = 0;

    Int_t entries = fGeantFwDetCat->getEntries();
    for(Int_t i=0;i<entries;i++) {
	ghit = (HGeantFwDet*)fGeantFwDetCat->getObject(i);
	if(ghit) {
	    ghit->getAddress(geantModule,geantCell);
            if(geantModule < 4 || geantModule > 5) continue; // skip the other detectors of the FwDet
	    ghit->getHit(xHit, yHit,  zHit, pxHit, pyHit, pxHit, tofHit, trackLength, eHit);
	    trackNumber = ghit->getTrackNumber();
            // calculate scintillator cell number and set location indexes
            module = geantModule - 4;
            scinNum = geantCell;  // preliminary!!!!!!!!!
            if (module >= FWDET_SCIN_MAX_MODULES || scinNum >= FWDET_SCIN_MAX_CELLS) continue;
       fLoc[0] = module;
	    fLoc[1] = scinNum;
            // do something with the Geant hit...
            //
            // fill the cal hit:
            // check if the object is already existing in the cal category
	    cal = (HFwDetScinCalSim*)fFwDetScinCalCat->getSlot(fLoc);
	    if(cal) {
		cal = new(cal) HFwDetScinCalSim;
		cal->setAddress(module,geantCell,scinNum);
		cal->setHit(tofHit,eHit);
                cal->setTrack(trackNumber);
	    }
	}
    }
    return 0;

}



