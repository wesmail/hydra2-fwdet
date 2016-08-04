// File: hldbase.h
//
// Author: Walter Karig <W.Karig@gsi.de>
// Modified: 08/11/02 by Peter Zumbruch <P.Zumbruch@gsi.de>
//

#ifndef HLDBASE_H
#define HLDBASE_H

#include "TObject.h"
typedef UInt_t UInt4;
typedef UChar_t UInt1;

class HldBase {
public:
  HldBase(): pData(0) {}
  virtual ~HldBase() {}
  // Get header informations (all header words are 32bit words):
  // type of getSize should be size_t
  size_t getSize() const { return ((BaseHdr*)pHdr)->size; }
  UInt4 getId() const { return ((BaseHdr*)pHdr)->id & 0x7fffffff; }
  UInt4 getDecoding() const { return ((BaseHdr*)pHdr)->decoding; }
  UInt_t getErrorBit(void) {return ((((BaseHdr*)pHdr)->id) >> 31) & 0x1;};
  // Get data body
  UInt4* getHeader() const { return pHdr; }
  UInt4* getData() const { return pData; }
  size_t getDataSize() const { return getSize() - getHdrSize(); }
  size_t getPaddedSize() const { return align8(getSize()); }
  UInt4* getEnd() const { return pData + getDataSize()/4; }
  UInt4* getPaddedEnd() const { return pData + getDataPaddedSize()/4; } 

protected:
  struct BaseHdr {
    UInt4 size;
    UInt4 decoding;
    UInt4 id;
    UInt4 :32;          // 64bit boundary
  };
  UInt4* pHdr;
  UInt4* pData;                              

  virtual size_t getHdrSize() const = 0;
  size_t getHdrLen() const { return getHdrSize()/4; }
  size_t getDataPaddedSize() const { return align8(getDataSize()); }
  Bool_t isSwapped() const { return byte(1, getDecoding()); }
  void swapHdr() { swap4(pHdr, getHdrLen()); }
  Int_t byte(Int_t b, UInt4 i) const { return (i >> ( 32 - 8*b)) & 0xff; }
  size_t align8(const size_t i) const { return 8*size_t( (i - 1)/8 + 1 ); }
  void swap4(UInt_t* p, const size_t len) const;
  void swap2(UShort_t* p, const size_t len) const;
public:
  //ClassDef(HldBase,1) //Base class for LMD events
};

#endif /* !HLDBASE_H */  
