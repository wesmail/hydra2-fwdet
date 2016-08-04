#ifndef HSTART2CALIBRATER_H
#define HSTART2CALIBRATER_H

#include "hlocation.h"
#include "hreconstructor.h"
#include "hstart2calrunpar.h"

class HCategory;
class HIterator;
class HStart2Calpar;

class HStart2Calibrater : public HReconstructor {
protected:
   HCategory *rawCat;          //! pointer to the raw data
   HCategory *calCat;          //! pointer to the cal data
   HIterator *iter;            //! iterator on raw data.
   HLocation loc;              //! location for new cal object
   HStart2Calpar* calpar;      //! calibration parameters
   HStart2CalRunPar* runpar;   //! corrections per strip per run
   HStart2CalRunPars correction;   //! corrections per strip per
   Double_t corrs[128];         //!
   static Bool_t doCorrection;  //! kTRUE= use HStart2CalRunPar (default)
public:
   HStart2Calibrater(void);
   HStart2Calibrater(const Text_t* name, const Text_t* title);
   ~HStart2Calibrater(void);
   Bool_t init(void);
   Bool_t reinit(void);
   Bool_t finalize(void) {
      return kTRUE;
   }
   Int_t execute(void);
   static void   setCorrection(Bool_t corr);
   static Bool_t getCorrection() { return doCorrection; }
   ClassDef(HStart2Calibrater, 0) // Calibrater raw->cal for START data
};

#endif /* !HSTART2CALIBRATER_H */
















