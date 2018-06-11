//_HADES_CLASS_DESCRIPTION//////////////////////////////////////////////////////////////////
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
#include "hemccalqa.h"
#include "hemccalsim.h"
#include "hemcdetector.h"
#include "hemccalpar.h"
#include "hemccellstatuspar.h"
#include "hemccalibraterpar.h"
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

  pStatuspar = (HEmcCellStatusPar*)gHades->getRuntimeDb()->getContainer("EmcCellStatusPar");
  if (!pStatuspar) return kFALSE;

  pCalibpar = (HEmcCalibraterPar*)gHades->getRuntimeDb()->getContainer("EmcCalibraterPar");
  if (!pCalibpar) return kFALSE;

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

#ifdef WITHCALQA
  pQACat = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catEmcCalQA));
  if(pQACat == NULL) {
    pQACat = pDet->buildMatrixCategory("HEmcCalQA",0.5);
    if(pQACat == NULL) return kFALSE;
    gHades->getCurrentEvent()->addCategory(catEmcCalQA, pQACat,"Emc");
  }
#endif

  iter = (HIterator*)pRawCat->MakeIterator();
  loc.set(2, 0, 0);
  fActive = kTRUE;

  return kTRUE;
}

Float_t twcCorrection(Float_t t, Float_t * twcPars)
{
    return twcPars[0] + twcPars[1] * t + twcPars[2] * t * t;
}

Int_t HEmcCalibrater::execute(void) {
  // calibrates all fired cells
  HEmcRaw *pRaw = NULL;
  HEmcCal *pCal = NULL;

  Int_t  sec   = -1;
  Int_t  cell  = 0;
  Int_t  pos   = 0;
  Char_t row   = -1;
  Char_t col   = -1;
  UInt_t nfasthits = 0, nslowhits=0;

  Float_t rawTimeLeading   = 0.;
  Float_t twcTimeLeading   = 0.;
  Float_t rawTimeLeadingS  = 0.;
  Float_t rawWidth         = 0.;
  Float_t twcWidth         = 0.;
  Float_t calTimeLeading   = 0.;
  Float_t calTimeLeadingS  = 0.;
  Float_t calTime   = 0.F;
  Float_t calEnergy = 0.F;

  Float_t parCellData[7] = {1.F, 0.F, 1.F, 0.F, 0.F, 0.F, 0.F};

  //Fill cal category
  iter->Reset();
  while ((pRaw = (HEmcRaw *)iter->Next()) != 0) {
    nfasthits = pRaw->getFastMultiplicity();
    nslowhits = pRaw->getSlowMultiplicity();

    if (nfasthits == 0 && nslowhits == 0)
        continue;

    pRaw->getAddress(sec, cell, row, col);

    if (sec < 0)
        continue;

    pos = HEmcDetector::getPositionFromCell(cell);

    if (pStatuspar->getCellStatus(sec, pos-1) == 0)
        continue;

    loc[0] = sec;
    loc[1] = cell;

    // get the calibration parameters
    HEmcCalParCell &pPar = (*pCalpar)[sec][cell];
    pPar.getData(parCellData);

    // index for slow hits
    UInt_t j = 0;

    Float_t diff_min = pCalibpar->getMatchWindowMin();
    Float_t diff_max = pCalibpar->getMatchWindowMax();

    Int_t hits_cnt = 0;

#ifdef WITHCALQA
    HEmcCalQA *pQA = (HEmcCalQA*)pQACat->getObject(loc);
    if (!pQA) {
      pQA = (HEmcCalQA *)pQACat->getSlot(loc);
      if (pQA) {
        pQA = new(pQA) HEmcCalQA;
        pQA->setAddress((Char_t)sec, (UChar_t)cell, row, col);
      } else {
        Error("execute()", "Can't get slot for CalQA sector=%i, cell=%i", sec, cell);
        return -1;
      }
    } else {
      Error("execute()", "Slot already exists for CalQA sector=%i, cell=%i", sec, cell);
      return -1;
    }
#endif
    // loop over number of fast hits in raw level
    for (UInt_t i = 0; i < nfasthits; ++i)
    {
      // calc Time Walk Correction
      Float_t twc = twcCorrection(rawTimeLeading, (parCellData+4));

      rawTimeLeading = pRaw->getFastTimeLeading(i);
      twcTimeLeading = rawTimeLeading + twc;

      // correct the time and width
      calTimeLeading = parCellData[0] * twcTimeLeading + parCellData[1];

#ifdef WITHCALQA
      pQA->addFastHit(calTimeLeading, pRaw->getFastWidth(i));
#endif
      Bool_t has_match = kFALSE;
      while (j < nslowhits)
      {
          rawTimeLeadingS = pRaw->getSlowTimeLeading(j);
          calTimeLeadingS = parCellData[0] * rawTimeLeadingS + parCellData[1];

          Float_t diff = calTimeLeadingS - calTimeLeading;

#ifdef WITHCALQA
          pQA->addSlowHit(calTimeLeadingS, pRaw->getSlowWidth(j));
#endif
          // if slow is before fast, go to the next slow
          if (diff < diff_min)
          {
              ++j;
              continue;
          }

          // if slow is inside the window, mark as a match and proceed
          if (diff < diff_max)
          {
              has_match = kTRUE;
              break;
          }

          // if slow is after fast, break and go to the next fast
          break;
      }

      if (has_match)
      {
        ++hits_cnt;
        rawWidth = pRaw->getSlowTimeTrailing(j) - rawTimeLeading;
        twcWidth = rawWidth - twc;

        calTime = calTimeLeading;
        calEnergy = parCellData[2] * twcWidth + parCellData[3];

#ifdef WITHCALQA
        pQA->addMatchedHit(calTime, calEnergy);
#endif
      }
    }

    // store the last found hit
    if (hits_cnt)
    {
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

      // fill cal hits
      pCal->setTime(calTime);
      pCal->setEnergy(calEnergy);

      pCal->setNHits(hits_cnt);
    }

#ifdef WITHCALQA
    for (; j < nslowhits; ++j)
    {
      rawTimeLeadingS = pRaw->getSlowTimeLeading(j);
      calTimeLeadingS = parCellData[0] * rawTimeLeadingS + parCellData[1];

      pQA->addSlowHit(calTimeLeadingS, pRaw->getSlowWidth(j));
    }
#endif
  }
  return 0;
}
