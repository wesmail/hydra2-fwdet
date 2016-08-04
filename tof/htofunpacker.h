#ifndef HTOFUNPACKER_H
#define HTOFUNPACKER_H

//
// Author: Walter Karig <W.Karig@gsi.de>
// Last update: 08/12/2000  I.Koenig
//
#include "hldunpack.h"
#include "hlocation.h"

class HTofLookup;

class HTofUnpacker: public HldUnpack {
public:
  HTofUnpacker(UInt_t id=411);
  ~HTofUnpacker(void) {}
  Int_t getSubEvtId() const { return subEvtId; }
  Int_t execute(); 
  Bool_t init(void);
  Int_t evCounter;
  Int_t errorCounter;
protected:
  UInt_t subEvtId;
  HLocation loc;
  HTofLookup* lookup;
public:
  ClassDef(HTofUnpacker,0)         // unpack TOF data
};          

#endif /* !HTOFUNPACKER_H */







