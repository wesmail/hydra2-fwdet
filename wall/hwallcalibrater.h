//*-- AUTHOR Bjoern Spruck
//*-- created : 24.03.06
// Modified for Wall by M.Golubeva 01.11.2006

#ifndef HWALLCALIBRATER_H
#define HWALLCALIBRATER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HIterator;
class HWallCalPar;

class HWallCalibrater : public HReconstructor {
 protected:
  HCategory *rawCat;          //! pointer to the raw data
  HCategory *calCat;          //! pointer to the cal data
  HIterator *iter;            //! iterator on raw data.
  HLocation loc;              //! location for new cal object
  HWallCalPar* pWallCalPar;        //! calibration parameters
  
 public:
  HWallCalibrater(void);
  HWallCalibrater(const Text_t* name,const Text_t* title);
  ~HWallCalibrater(void);
  Bool_t init(void);
  Bool_t finalize(void) { return kTRUE; }
  Int_t execute(void);

 public:
  ClassDef(HWallCalibrater,0) // Calibrater raw->cal for WALL data
};

#endif /* !HWALLCALIBRATER_H */
















