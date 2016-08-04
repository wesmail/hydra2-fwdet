
#ifndef HRPCCALIBRATER_H
#define HRPCCALIBRATER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HIterator;
class HRpcCalPar;
class HRpcChargeOffsetPar;

class HRpcCalibrater : public HReconstructor {
protected:
  HCategory *pRawCat;             // pointer to the raw data
  HCategory *pCalCat;             // pointer to the cal data
  HIterator *iter;                // iterator on raw data.
  HLocation loc;                  // location for new cal object
  HRpcCalPar* pCalPar;            // calibration parameters
  HRpcChargeOffsetPar* pChOffPar; // charge offset parameters

public:
  HRpcCalibrater(void);
  HRpcCalibrater(const Text_t* name,const Text_t* title);
  ~HRpcCalibrater(void);
  Bool_t init(void);
  Bool_t finalize(void) { return kTRUE; }
  Int_t execute(void);
public:
  ClassDef(HRpcCalibrater,0) // Calibrater raw->cal for RPC data
};

#endif /* !HRPCCALIBRATER_H */
















