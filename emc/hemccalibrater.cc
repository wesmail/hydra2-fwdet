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
HEmcCalibrater::HEmcCalibrater(void) {
  // default constructor
  pRawCat = NULL;
  pCalCat = NULL;
  iter    = NULL;
  pCalpar = NULL;
  embedding = 0;
}

HEmcCalibrater::HEmcCalibrater(const Text_t *name, const Text_t *title)
                      : HReconstructor(name, title) {
  // constructor
  pRawCat = NULL;
  pCalCat = NULL;
  iter    = NULL;
  pCalpar = NULL;
  embedding = 0;
}

HEmcCalibrater::~HEmcCalibrater(void) {
  //destructor deletes the iterator on the raw category
  if (NULL != iter) {
    delete iter;
    iter = NULL;
  }
}

Bool_t HEmcCalibrater::init(void) {
  // gets the calibration parameter container
  // gets the EmcRaw category and creates the EmcCal category
  // creates an iterator which loops over all fired cells in EmcRaw
  HEmcDetector* pDet = (HEmcDetector*)gHades->getSetup()->getDetector("Emc");
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

Int_t HEmcCalibrater::execute(void) {
  // calibrates all fired cells
  HEmcRaw *pRaw = NULL;
  HEmcCal *pCal = NULL;

  Int_t  sec   = -1;
  Int_t  cell  = 0;
  Char_t row   = -1;
  Char_t col   = -1;
  Int_t  nhits = 0;

  Float_t rawTime   = 0.F;
  Float_t rawWidth  = 0.F;
  Float_t calTime   = 0.F;
  Float_t calCharge = 0.F;

  Float_t parCellData[4] = {1.F, 0.F, 1.F, 0.F};

  //Fill cal category
  iter->Reset();
  while ((pRaw = (HEmcRaw *)iter->Next()) != 0) {
    nhits = pRaw->getTotMultiplicity();
    pRaw->getAddress(sec, cell);
    loc[0] = sec;
    loc[1] = cell;

    if (sec >= 0 && nhits > 0) {
      pCal = (HEmcCal*)pCalCat->getObject(loc);
      if (!pCal) {
        pCal = (HEmcCal *)pCalCat->getSlot(loc);
        if (pCal) {
          if(embedding == 0) pCal = new(pCal) HEmcCal;
          else               pCal = new(pCal) HEmcCalSim;
          pDet->getRowCol(cell,row,col);
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

      // loop over number of hits in raw level
      // here in the simplified version only the first hit is calibrated
      pRaw->getTimeAndWidth(1, rawTime, rawWidth);
      // correct the time and width
      calTime   = parCellData[0] * rawTime + parCellData[1];
      calCharge = parCellData[2] * rawWidth + parCellData[3];
      // fill cal hits
      pCal->setTime1(calTime);
      pCal->setEnergy(calCharge);
      pCal->setNHits(1);
    }
  }
  return 0;
}
