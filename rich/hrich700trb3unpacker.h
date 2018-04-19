#ifndef HRich700Trb3Unpacker_H
#define HRich700Trb3Unpacker_H

#include "hlocation.h"
#include "htrb3tdcunpacker.h"



class HRich700Trb3Lookup;
class HRich700ThresholdPar;

class HRich700Trb3Unpacker: public HTrb3TdcUnpacker {
protected:
   HLocation fRawLoc;              //< current location of raw pixel object (pmt,pixel)
   HLocation fCalLoc;              //< current location of calibrated object (sector, row, column)

   HRich700Trb3Lookup* fLookup;  //< TRB3 lookup table
   HRich700ThresholdPar* fThresholds;  // TRB3 lookup table
   Bool_t fTimeRef;             //< use reference time ?


   HCategory*        fCalCat;  //< category for calibrated event data, output of unpacker


   /** add hit to the raw category at the current raw location cursor*/
   Int_t addRawHit(Double_t t_leading, Double_t t_trailing, Double_t tot, UInt_t flag=3);


   /** add hit of specified charge to cal category at current raw pixel location*/
   Int_t addCalHitCharge(Float_t charge);

   /** add hit of specified charge to cal category at specified location*/
   Int_t addCalHitCharge(Int_t sector, Int_t col, Int_t row, Float_t charge);


public:
   HRich700Trb3Unpacker(UInt_t id = 0);
   virtual ~HRich700Trb3Unpacker(void) {}

   virtual Bool_t init(void);
   virtual Bool_t reinit(void);
   virtual Int_t execute(void);

   void disableTimeRef(void) {
     fTimeRef = kFALSE ;
   }

   /** utility function to evaluate sector assignment from positoin on pmt plane
    * originally taken from HRich700DigiMapPar*/
   static Int_t getSector(Float_t x, Float_t y);

   ClassDef(HRich700Trb3Unpacker, 0) // TRB3 unpacker for the start detector
};

#endif /* !HRich700Trb3Unpacker_H */







