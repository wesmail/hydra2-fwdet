#ifndef HEMCTRB3UNPACKER_H
#define HEMCTRB3UNPACKER_H

#include "hlocation.h"
#include "htrb3unpacker.h"

class HEmcTrb3Lookup;

class HEmcTrb3Unpacker: public HTrb3Unpacker {
protected:
   HLocation loc;           // location of raw cell object
   HEmcTrb3Lookup* lookup;  // TRB3 lookup table
   Bool_t timeRef;          // use reference time ?

public:
   HEmcTrb3Unpacker(UInt_t id = 0);
   ~HEmcTrb3Unpacker(void) {}

   Bool_t init(void);
   Int_t execute(void);

   void disableTimeRef(void) {
     timeRef = kFALSE ;
   }

   ClassDef(HEmcTrb3Unpacker, 0) // TRB unpacker for the EMC detector
};

#endif /* !HEMCTRB3UNPACKER_H */







