#ifndef HTOFCALIBRATER_H
#define HTOFCALIBRATER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HIterator;
class HTofCalPar;

class HTofCalibrater : public HReconstructor {
protected:
  HLocation fLoc;      //Location for new object
  HIterator *fIter;    //!Iterator on raw data
  HCategory *fRawCat;  //!Pointer to the raw data category
  HCategory *fCalCat;  //!Pointer to the cal data category
  HTofCalPar *fCalPar; //!Calibration parameters
public:
  HTofCalibrater(void);
  HTofCalibrater(const Text_t *name,const Text_t *title);
  ~HTofCalibrater(void);
  Bool_t init(void);
  void initParContainer();
  Bool_t finalize(void) {return kTRUE;}
  Int_t execute(void);
  HTofCalibrater &operator=(HTofCalibrater &c);
  ClassDef(HTofCalibrater,1) //Calibrater of Tof data.
};

#endif /* !HTOFCALIBRATER_H */
