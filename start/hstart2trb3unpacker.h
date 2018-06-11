#ifndef HSTART2TRB3UNPACKER_H
#define HSTART2TRB3UNPACKER_H

#include "hlocation.h"
#include "htrb3tdcunpacker.h"

class HStart2Trb3Lookup;

class HStart2Trb3Unpacker: public HTrb3TdcUnpacker {
protected:
   HLocation loc;              // location of raw cell object
   HStart2Trb3Lookup* lookup;  // TRB3 lookup table
   Bool_t timeRef;             // use reference time ?

public:
   HStart2Trb3Unpacker(UInt_t id = 0);
   ~HStart2Trb3Unpacker(void) {}

   Bool_t init(void);
   Int_t execute(void);
   Bool_t reinit(void);

   void disableTimeRef(void) {
     timeRef = kFALSE ;
   }

   ClassDef(HStart2Trb3Unpacker, 0) // TRB3 unpacker for the start detector
};

#endif /* !HSTART2TRB3UNPACKER_H */







