#ifndef HLATCHUNPACKER_H
#define HLATCHUNPACKER_H

#include "hldunpack.h"

class HLatchUnpacker: public HldUnpack {
protected:
   Int_t subEvtId;           //! subevent id
public:
   HLatchUnpacker(Int_t id = 865);
   ~HLatchUnpacker(void) {}
   Int_t getSubEvtId() const {
      return subEvtId;
   }
   Int_t execute();
   Bool_t init(void) {
      return kTRUE;
   }
protected:
   UInt_t* fillLatch(UInt_t *pData);

   ClassDef(HLatchUnpacker, 0) // unpack LATCH data
};

#endif /* !HLATCHUNPACKER_H */











