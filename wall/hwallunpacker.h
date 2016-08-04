#ifndef HWALLUNPACKER_H
#define HWALLUNPACKER_H

#include "hldunpack.h"
#include "hlocation.h"

class HWallLookup;

class HWallUnpacker: public HldUnpack {
protected:
  UInt_t subEvtId;
  HLocation loc;
  HWallLookup* lookup;
public:
  HWallUnpacker(UInt_t id=416);
  ~HWallUnpacker(void) {}
  Int_t getSubEvtId(void) const { return subEvtId; }
  Int_t execute(void); 
  Bool_t init(void);
public:
  ClassDef(HWallUnpacker,0) // Unpacker for Foward Wall data
};          

#endif /* !HWALLUNPACKER_H */







