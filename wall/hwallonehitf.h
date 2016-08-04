//*-- AUTHOR Bjoern Spruck
//*-- created : 24.03.06
// created by M.Golubeva 01.11.2006

#ifndef HWALLONEHITF_H
#define HWALLONEHITF_H

#include "hreconstructor.h"
#include "hlocation.h"
#include <iostream> 
#include <iomanip>

class HCategory;
class HIterator;
class HWallCalPar;
class HWallRefWinPar;
class HWallRefWinMod;
class HRuntimeDb;
class HWallOneHit;
class HWallRaw;
class HWallCal;

class HWallOneHitF : public HReconstructor {
protected:
  HCategory *fCalCat;          //! pointer to the cal data
  HCategory *fOneHitCat;          //! pointer to the hitone data
  HIterator *iter;            //! iterator on raw data.
  HLocation fLoc;              //! location for new cal object
  HWallRefWinPar *pWallRefWinPar;     //! tdc Window params

  virtual void fillHit(HWallOneHit *, HWallCal *); //should be changed to HWallCal 

public:
  HWallOneHitF(void);
  HWallOneHitF(const Text_t* name,const Text_t* title);
  ~HWallOneHitF(void);
  void   initParContainer();
  Bool_t init(void);
  Bool_t finalize(void) { return kTRUE; }
  Int_t  execute(void);
public:
  ClassDef(HWallOneHitF,0) // Select one hit of 4 with ref. windows
};

#endif /* !HWALLONEHITF_H */
















