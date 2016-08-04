#ifndef HSTART2TRB2UNPACKER_H
#define HSTART2TRB2UNPACKER_H

#include "hlocation.h"
#include "htrb2unpacker.h"

class HStart2Trb2Lookup;

class HStart2Trb2Unpacker: public HTrb2Unpacker {
protected:
   HLocation loc;              // location of raw cell object
   HStart2Trb2Lookup* lookup;  // TRB lookup table
   Bool_t timeRef;             // use reference time ?

public:
   HStart2Trb2Unpacker(UInt_t id = 0);
   ~HStart2Trb2Unpacker(void) {
      ;
   }

   Bool_t init(void);
   Int_t execute(void);
   Bool_t finalize(void);

   void disableTimeRef(void) {
      timeRef = kFALSE ;
   }

   ClassDef(HStart2Trb2Unpacker, 0) // TRB unpacker for START detector
};

#endif /* !HSTART2TRB2UNPACKER_H */







