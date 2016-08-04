#ifndef HTOFUNPACKER99_H
#define HTOFUNPACKER99_H

// File: tofunpacker99.h
//
// Author: Walter Karig <W.Karig@gsi.de>
// Last update: 28/5/2000 by R. Holzmann 
//
#include "hldsubevt.h"
#include "hldunpack.h"
#include "tofdef.h"

class HTofUnpacker99: public HldUnpack {
public:
  HTofUnpacker99(void);
  ~HTofUnpacker99(void);
  Int_t getSubEvtId() const { return 400; }
  Int_t byte(Int_t b, UInt4 i) { return (i >> (32 - 8*b)) & 0xff; }
  Int_t execute(); 
  Bool_t init(void);
protected:
  Int_t fill();
  UInt_t *pTofData;

public:
  ClassDef(HTofUnpacker99,0)         // unpack TOF November 1999 data
};          

#endif /* !HTOFUNPACKER99_H */
