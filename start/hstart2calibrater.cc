//*-- Created : 04/12/2009 by I.Koenig
//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////
//
//  HStart2Calibrater:
//
//  Calibrates all fired cells in Start2Raw category and fills
//  the Start2Cal category
//
////////////////////////////////////////////////////////////////

#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hrun.h"
#include "hspectrometer.h"
#include "hstart2cal.h"
#include "hstart2calibrater.h"
#include "hstart2calpar.h"
#include "hstart2detector.h"
#include "hstart2raw.h"
#include "hstartdef.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

ClassImp(HStart2Calibrater)


Bool_t HStart2Calibrater::doCorrection = kTRUE;

HStart2Calibrater::HStart2Calibrater(void)
{
   // default constructor
   rawCat = NULL;
   calCat = NULL;
   iter   = NULL;
   calpar = NULL;
   runpar = NULL;
   correction.clear();
   memset(corrs,0,128*sizeof(Double_t));

}

HStart2Calibrater::HStart2Calibrater(const Text_t *name, const Text_t *title) :
   HReconstructor(name, title)
{
   // constructor
   rawCat = NULL;
   calCat = NULL;
   iter   = NULL;
   calpar = NULL;
   runpar = NULL;
   correction.clear();
   memset(corrs,0,128*sizeof(Double_t));

}

HStart2Calibrater::~HStart2Calibrater(void)
{
   //destructor deletes the iterator on the raw category
   if (NULL != iter) {
      delete iter;
      iter = NULL;
   }
}

void HStart2Calibrater::setCorrection(Bool_t corr)
{
    // kTRUE = use HStart2CalRunPar (default)
    doCorrection = corr;
}

Bool_t HStart2Calibrater::init(void)
{
   // gets the calibration parameter container
   // gets the Start2Raw category and creates the Start2Cal category
   // creates an iterator which loops over all fired cells in Start2Raw
   calpar = (HStart2Calpar*)gHades->getRuntimeDb()->getContainer("Start2Calpar");
   if (!calpar) return kFALSE;

   if(doCorrection){
       runpar = (HStart2CalRunPar*)gHades->getRuntimeDb()->getContainer("Start2CalRunPar");
       if (!runpar) return kFALSE;
   }

   HStart2Detector* det = (HStart2Detector*)gHades->getSetup()->getDetector("Start");
   if (!det) {
      Error("init", "No Start Detector found.");
      return kFALSE;
   }
   rawCat = gHades->getCurrentEvent()->getCategory(catStart2Raw);
   if (!rawCat) {
      Error("init()", "HStart2Raw category not available!");
      exit(1);
   }
   calCat = det->buildCategory(catStart2Cal);
   if (!calCat) return kFALSE;

   iter = (HIterator*)rawCat->MakeIterator();
   loc.set(2, 0, 0);
   fActive = kTRUE;

   return kTRUE;
}

Bool_t HStart2Calibrater::reinit(void)
{
   // gets the calibration parameter container

    if(doCorrection){
	if (!runpar) return kFALSE;

	Int_t runID = gHades->getRuntimeDb()->getCurrentRun()->getRunId();
	correction.clear();
	runpar->getRun(runID,&correction);
	memset(corrs,0,128*sizeof(Double_t));
	for(Int_t i=0;i<correction.getNVals();i++){
	    corrs[i] = correction.getVal(i);
	}
    }
    return kTRUE;
}

Int_t HStart2Calibrater::execute(void)
{
   // calibrates all fired cells
   HStart2Raw *pRaw = 0;
   HStart2Cal *pCal = 0;

   Bool_t bhits  = kFALSE;

   Int_t mod     = 0;
   Int_t strip   = 0;
   Int_t nhits   = 0;

   Float_t rawTime  = 0.;
   Float_t rawWidth = 0.;
   Float_t calTime  = 0.;
   Float_t calWidth = 0.;

   Float_t parCellData[4] = {0., 0., 0., 0.};


   //Fill cal category
   iter->Reset();
   while ((pRaw = (HStart2Raw *)iter->Next()) != 0) {

      pRaw->getAddress(mod, strip);
      loc[0] = mod;
      loc[1] = strip;

      if (loc[0] >= 0) {
         pCal = (HStart2Cal*)calCat->getObject(loc);
         if (!pCal) {
            pCal = (HStart2Cal *)calCat->getSlot(loc);
            if (pCal) {
               pCal = new(pCal) HStart2Cal;
               pCal->setAddress(loc[0], loc[1]);
            } else {
               Error("execute()", "Can't get slot mod=%i, chan=%i", loc[0], loc[1]);
               return -1;
            }
         } else {
            Error("execute()", "Slot already exists for mod=%i, chan=%i", loc[0], loc[1]);
            return -1;
         }

         // get the calibration parameters
         HStart2CalparCell &pPar = (*calpar)[mod][strip];
         pPar.getData(parCellData);

         // loop over number of hits
         nhits = pRaw->getMultiplicity();
         for (Int_t i = 0; i < nhits; i++) {

            // get raw time and width (only stored for 4 hits!)
            if (i < pRaw->getMaxMultiplicity()) {
               pRaw->getTimeAndWidth(i + 1, rawTime, rawWidth);
            }

            // else set dummy values (only for counting hits!)
            else {
               rawTime  = 0.;
               rawWidth = 0.;
            }

            // correct the time and width
            calTime  = parCellData[0] * rawTime + parCellData[1] + corrs[loc[0]*16+loc[1]];
            calWidth = parCellData[2] * rawWidth + parCellData[3];

            // fill cal hits
            bhits = pCal->setTimeAndWidth(calTime, calWidth);

            // check nhits
            if (i >= pRaw->getMaxMultiplicity() && bhits == kTRUE) {
               Error("execute()", "Start module %d strip %d :  hit multiplicity > %d (unrecognized)", mod, strip, pRaw->getMaxMultiplicity());
            }
         }
      }
   }
   return 0;
}
