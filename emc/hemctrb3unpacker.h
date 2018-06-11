#ifndef HEMCTRB3UNPACKER_H
#define HEMCTRB3UNPACKER_H

#include "hlocation.h"
#include "htrb3tdcunpacker.h"

class HEmcTrb3Lookup;

class HEmcTrb3Unpacker: public HTrb3TdcUnpacker {
protected:
   HLocation fLoc;           // location of raw cell object
   HEmcTrb3Lookup* fLookup;  // TRB3 lookup table
   Bool_t fTimeRef;          // use reference time ?
   Double_t fTimeShift;      //shift all times by a constant (to positive). Pendant to trb2 unpacker timeShift JAM
public:
   HEmcTrb3Unpacker(UInt_t id = 0);
   virtual ~HEmcTrb3Unpacker(void) {}

   Bool_t init(void);
   Bool_t reinit(void);
   Int_t execute(void);

   void disableTimeRef(void) {
     fTimeRef = kFALSE ;
   }
   void shiftTimes(Double_t ts) {fTimeShift = ts;};


   Int_t addRawHit(Double_t t_leading, Double_t t_trailing, Bool_t isfastchannel);

   ClassDef(HEmcTrb3Unpacker, 0) // TRB unpacker for the EMC detector
};

#endif /* !HEMCTRB3UNPACKER_H */







