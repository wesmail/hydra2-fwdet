#ifndef HPIONTRACKERCALIBRATER_H
#define HPIONTRACKERCALIBRATER_H

#include "hreconstructor.h"
#include "hpiontrackercalrunpar.h"

class HCategory;
class HIterator;
class HPionTrackerCalPar;
class HPionTrackerHitFPar;
class HPionTrackerCalRunPar;

class HPionTrackerCalibrater : public HReconstructor {
protected:
  HCategory *pRawCat;          // pointer to the raw data
  HCategory *pCalCat;          // pointer to the cal data
  HIterator *iter;             // iterator on raw data.
  HLocation loc;               // location for new cal object
  HPionTrackerCalPar *pCalpar; // pointer to calibration parameters
  HPionTrackerHitFPar *pHitfpar; // pinter to hit parameters
  HPionTrackerCalRunPar *pRunpar; //pionter to calibration paramters
  HPionTrackerCalRunPars correction; //!corrections per strip 
  Double_t corrs[4]; // corrections for global temperature offset (runID)
  Double_t charge_offset[4]; // corrections for global temperature offset (by comparing modules) 
  Double_t calib_charge[4]; // switch on/off usage of adc in hitfinder

public:
  HPionTrackerCalibrater(void);
  HPionTrackerCalibrater(const Text_t* name, const Text_t* title);
  ~HPionTrackerCalibrater(void);
  Bool_t init(void);
  Bool_t reinit(void);
  Int_t  execute(void);
  Bool_t finalize(void) {return kTRUE;}

  ClassDef(HPionTrackerCalibrater, 0) // Calibrater raw->cal for PionTracker data
};

#endif /* !HPIONTRACKERCALIBRATER_H */
















