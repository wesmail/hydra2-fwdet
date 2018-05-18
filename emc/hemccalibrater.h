#ifndef HEMCCALIBRATER_H
#define HEMCCALIBRATER_H

#include "hreconstructor.h"

class HCategory;
class HIterator;
class HEmcCalPar;
class HEmcCellStatusPar;
class HEmcCalibraterPar;

#define WITHCALQA

class HEmcCalibrater : public HReconstructor {
protected:
  HCategory *pRawCat;   //! pointer to the raw data
  HCategory *pCalCat;   //! pointer to the cal data
#ifdef WITHCALQA
  HCategory *pQACat;    //! pointer to the cal data
#endif
  HIterator *iter;      //! iterator on raw data.
  HLocation loc;        // location for new cal object
  HEmcCalPar *pCalpar;  //! pointer to calibration parameters
  HEmcCellStatusPar *pStatuspar;    //! pointer to calibration parameters
  HEmcCalibraterPar *pCalibpar;     //! pointer to calibrater params
  Int_t embedding;      // flag is set if real data should be embedded into simulation data
public:
  HEmcCalibrater(void);
  HEmcCalibrater(const Text_t* name, const Text_t* title);
  ~HEmcCalibrater(void);
  Bool_t init(void);
  Int_t  execute(void);
  Bool_t finalize(void) {return kTRUE;}
  ClassDef(HEmcCalibrater, 0) // Calibrater raw->cal for EMC data
};

#endif /* !HEMCCALIBRATER_H */
















