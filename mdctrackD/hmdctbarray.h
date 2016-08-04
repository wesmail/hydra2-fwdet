#ifndef HMDCTBARRAY_H
#define HMDCTBARRAY_H

#include "Rtypes.h"

class HMdcTBArray {
  protected:
    static UChar_t nTBitLookUp[256];         
    static UChar_t pTBitLookUp[4][256];
    static UChar_t nextTBit[4];
    static UChar_t prevTBit[4];
  public: 
    inline static void    clear(UChar_t *pF, UChar_t *pL);
    inline static void    set(UChar_t *, Int_t pos, UChar_t cont);
    inline static UChar_t get(const UChar_t *, Int_t pos); 
    inline static void    unset(UChar_t *, Int_t pos, UChar_t cont);
    inline static Int_t   getNSet(const UChar_t *pF, const UChar_t *pL); 
    inline static Int_t   position(const UChar_t *pF,const UChar_t *pL,
                                   Int_t idx);
    inline static Int_t   first(const UChar_t *pF, const UChar_t *pL);
    inline static Int_t   last(const UChar_t *pF, const UChar_t *pL);
    inline static void    shiftRight(UChar_t *pF, UChar_t *pL, UChar_t *pS);
    inline static void    shiftLeft(UChar_t *pF, UChar_t *pL, UChar_t *pS);
    inline static Int_t   compare(const UChar_t *pF1, const UChar_t *pF2, 
                                  Int_t nBytes);
    inline static Int_t   andArr(const UChar_t *pF1, const UChar_t *pF2,
                                 Int_t nBytes, UChar_t *pOut);
    inline static Int_t   xorArr(const UChar_t *pF1, const UChar_t *pF2,
                                 Int_t nBytes,UChar_t *pOut);
    inline static Int_t   andArrAndUnset(UChar_t *pNoId, const UChar_t *pEx,
                                       Int_t nBytes,UChar_t *pIdnt);
    inline static Bool_t  isIdenBits(const UChar_t *pF1, const UChar_t *pF2,
                                     Int_t nBytes);
    static Int_t   next(const UChar_t *pF, const UChar_t *pL, Int_t prPos);
    static Int_t   previous(const UChar_t *pF, const UChar_t *pL, Int_t prPos);
    static UChar_t next2Bits(const UChar_t *pF, const UChar_t *pL, Int_t& pos);
  private:
    HMdcTBArray(void) {}
    virtual ~HMdcTBArray(void) {}

  ClassDef(HMdcTBArray,0) //A group of cells in layers
};

class HMdcBArray {
  protected:
    static UChar_t nBitLookUp[256];
    static UChar_t setBitLUp[8];
    static UChar_t unsetBitLUp[8];
    static UChar_t pBitLookUp[8][256];
    static UChar_t highBit[256];
    static UChar_t nextBit[8];
    static UChar_t prevBit[8];
    static UChar_t nLayOrientation[64];
    static UChar_t is40degCross[64];
  public: 
    inline static void    clear(UChar_t *pF, UChar_t *pL);
    inline static void    set(UChar_t *, Int_t pos);
    inline static UChar_t get(const UChar_t *, Int_t pos); 
    inline static void    unset(UChar_t *, Int_t pos);
    inline static Bool_t  testAndUnset(UChar_t *, Int_t pos); 
    inline static Int_t   getNSet(const UChar_t *pF, const UChar_t *pL);
    inline static UChar_t getNSet(const UChar_t *pF);    //One byte
    inline static UChar_t getNSet(const UChar_t byte);   //One byte
    inline static UChar_t getNLayOrientation(UChar_t list);
    inline static Bool_t  is40DegWireCross(UChar_t list);
    inline static Int_t   getNSet2B(const UChar_t *pF);  //Two bytes
    inline static Int_t   position(const UChar_t *pF,const UChar_t *pL,
                                   Int_t idx);
    inline static Int_t   first(const UChar_t *pF, const UChar_t *pL);
    inline static Int_t   last(const UChar_t *pF, const UChar_t *pL);
    inline static Int_t   next(const UChar_t *pF,const UChar_t *pL,Int_t prPos);
    inline static Int_t   nextAndUnset(UChar_t *pF, UChar_t *pL, Int_t prPos);
    inline static Int_t   previous(const UChar_t *pF, const UChar_t *pL, 
                                   Int_t prPos);
    inline static Int_t   prevAndUnset(UChar_t *pF, UChar_t *pL, Int_t prPos);
    inline static void    shiftRight(UChar_t *pF, UChar_t *pL, UChar_t *pS);
    inline static void    shiftLeft(UChar_t *pF, UChar_t *pL, UChar_t *pS);
    inline static Int_t   compare(const UChar_t *pF1, const UChar_t *pF2, 
                                  Int_t nBytes);
    inline static Int_t   andArr(const UChar_t *pF1, const UChar_t *pF2,
                                 Int_t nBytes, UChar_t *pOut);
    inline static Int_t   xorArr(const UChar_t *pF1, const UChar_t *pF2,
                                 Int_t nBytes,UChar_t *pOut);
    inline static Int_t   andArrAndUnset(UChar_t *pNoId, const UChar_t *pEx,
                                       Int_t nBytes,UChar_t *pIdnt);
  private:
    HMdcBArray(void) {}
    virtual ~HMdcBArray(void) {}

    ClassDef(HMdcBArray,0) //A group of cells in layers
};

//----------------------- Inlines HMdcTBarray------------------------------
inline void HMdcTBArray::clear(UChar_t *pF, UChar_t *pL) {
  while( pF<=pL ) *(pF++)=0;
}

inline Int_t HMdcTBArray::getNSet(const UChar_t *pF, const UChar_t *pL) {
  Int_t nTBits=0;
  while(pF <= pL) nTBits+=nTBitLookUp[*(pF++)];
  return nTBits;
}

inline void HMdcTBArray::set(UChar_t *pF, Int_t pos, UChar_t cont) {
  pF[pos>>2] |= (cont & 0x3) << (pos&3)*2;
}

inline void HMdcTBArray::unset(UChar_t *pF, Int_t pos, UChar_t cont) {
  pF[pos>>2] &= ~(( cont & 0x3 ) << (pos&3)*2);
}

inline Int_t HMdcTBArray::position(const UChar_t *pF, const UChar_t *pL, Int_t idx) {
  // return: >=0 - ok.!;
  if(idx<0) return -1;
  for(const UChar_t *pB=pF; pB<=pL; pB++) {
    Int_t idn=idx-nTBitLookUp[*pB];
    if( idn < 0 ) return ((pB-pF)<<2)+pTBitLookUp[idx][*pB];
    idx=idn;
  }
  return -1;
}

inline Int_t HMdcTBArray::first(const UChar_t *pF, const UChar_t *pL) {
  // return: -1 - num.cells=0
  for(const UChar_t *pB=pF; pB<=pL; pB++) {
     if( *pB > 0 ) return ((pB-pF)<<2)+pTBitLookUp[0][*pB];
  }
  return -1;
}

inline Int_t HMdcTBArray::last(const UChar_t *pF, const UChar_t *pL) {
    // return: >=0 - ok.!; -2 - num. of cells =0
  for(const UChar_t *pB=pL; pB>=pF; pB--) {
    if(*pB > 0) return ((pB-pF)<<2)+pTBitLookUp[nTBitLookUp[*pB]-1][*pB];
  }
  return -1;
}

inline void HMdcTBArray::shiftRight(UChar_t *pF, UChar_t *pL, UChar_t *pS) {
  // pF - pointer to first element, pL - pointer to last el.
  // pS - pointer to element which must be moved to *pL ((pL-pS) - shift)
  while( pS>=pF ) *(pL--)=*(pS--);
  clear(pF, pL);
}

inline void HMdcTBArray::shiftLeft(UChar_t *pT, UChar_t *pL, UChar_t *pS) {
  // pT - pointer to the first elelement, pL - pointer to last el.
  // pS - pointer to element which must be moved to *pT
  while( pS<=pL ) *(pT++)=*(pS++);
  clear(pT, pL);
}

inline UChar_t HMdcTBArray::get(const UChar_t *pF, Int_t pos) {
  // Returns 1 for time1, 2 for time2 and 3 for time1 and time2 in cell
  return (pF[pos>>2] >> (pos&3)*2) &  0x3;
}

inline Int_t HMdcTBArray::compare(const UChar_t *pF1, const UChar_t *pF2,
    Int_t nBytes) {
  Int_t nTBits=0;
  const UChar_t *pF1End = pF1+nBytes;
  for(;pF1<pF1End;pF1++,pF2++) nTBits += nTBitLookUp[*pF1 & *pF2];
  return nTBits;
}

inline Bool_t HMdcTBArray::isIdenBits(const UChar_t *pF1, const UChar_t *pF2,
    Int_t nBytes) {
  //return kTRUE if at list one the same bit is setted on both arrays
  while((nBytes--)>0) if( *(pF1++) & *(pF2++) ) return kTRUE;
  return kFALSE;
}

inline Int_t HMdcTBArray::andArr(const UChar_t *pF1, const UChar_t *pF2,
    Int_t nBytes,UChar_t *pOut) {
  // Function produce list of identical for pF1 & ppF2 bits - pOut
  // return number of the same cells(!)
  Int_t nTBits=0;
  while((nBytes--)>0) {
    *pOut=*(pF1++) & *(pF2++);
    nTBits+=nTBitLookUp[*pOut];
    pOut++;
  }
  return nTBits;
}

inline Int_t HMdcTBArray::andArrAndUnset(UChar_t *pNoId, const UChar_t *pEx,
    Int_t nBytes,UChar_t *pIdnt) {
  // Function produce list of identical for pNoId & ppEx bits - pIdnt
  // and unset identical bits in pNoId
  // return number of the same cells(!)
  Int_t nIBits=0;
  const UChar_t *pExEnd = pEx + nBytes;
  for(;pEx<pExEnd;pEx++,pIdnt++,pNoId++) {
    *pIdnt  = *pNoId & *pEx;
    *pNoId ^= *pIdnt;
    nIBits += nTBitLookUp[*pIdnt];
  }
  return nIBits;
}

inline Int_t HMdcTBArray::xorArr(const UChar_t *pF1, const UChar_t *pF2,
    Int_t nBytes,UChar_t *pOut) {
  // Function produce list of nonidentical for pF1 & ppF2 bits - pOut
  // return number of the same cells(!)
  Int_t nTBits=0;
  while((nBytes--)>0) {
    *pOut=*(pF1++) ^ *(pF2++);
    nTBits+=nTBitLookUp[*pOut];
    pOut++;
  }
  return nTBits;
}

//----------------------- Inlines HMdcBArray------------------------------

inline void HMdcBArray::clear(UChar_t *pF, UChar_t *pL) {
  while( pF<=pL ) *(pF++)=0;
}

inline Int_t HMdcBArray::getNSet(const UChar_t *pF, const UChar_t *pL) {
  Int_t nBits=0;
  while(pF <= pL) nBits+=nBitLookUp[*(pF++)];
  return nBits;
}

inline UChar_t HMdcBArray::getNSet(const UChar_t *pF) {
  return nBitLookUp[*pF];
}

inline UChar_t HMdcBArray::getNSet(const UChar_t byte) {
  return nBitLookUp[byte];
}

inline Int_t HMdcBArray::getNSet2B(const UChar_t *pF) {
  return nBitLookUp[*pF]+nBitLookUp[*(pF+1)];
}

inline void HMdcBArray::set(UChar_t *pF, Int_t pos) {
  pF[pos>>3] |= setBitLUp[pos&7];
}

inline UChar_t HMdcBArray::getNLayOrientation(UChar_t list) {
  // list - list of layers: 6 bits, one bit per layer
  return nLayOrientation[list&63];
}

inline Bool_t HMdcBArray::is40DegWireCross(UChar_t list) {
  // list - list of layers: 6 bits, one bit per layer
  return is40degCross[list&63]!=0;
}

inline void HMdcBArray::unset(UChar_t *pF, Int_t pos) {
  pF[pos>>3] &= unsetBitLUp[pos&7];
}

inline Int_t HMdcBArray::position(const UChar_t *pF, const UChar_t *pL, Int_t idx) {
  // return: >=0 - ok.!;
  if(idx<0) return -1;
  for(const UChar_t *pB=pF; pB<=pL; pB++) {
    Int_t idn=idx-nBitLookUp[*pB];
    if( idn < 0 ) return ((pB-pF)<<3)+pBitLookUp[idx][*pB];
    idx=idn;
  }
  return -1;
}

inline Int_t HMdcBArray::first(const UChar_t *pF, const UChar_t *pL) {
  // return: -1 - num.cells=0
  for(const UChar_t *pB=pF; pB<=pL; pB++) {
     if( *pB > 0 ) return ((pB-pF)<<3)+pBitLookUp[0][*pB];
  }
  return -1;
}

inline Int_t HMdcBArray::last(const UChar_t *pF, const UChar_t *pL) {
    // return: >=0 - ok.!; -2 - num. of cells =0
  for(const UChar_t *pB=pL; pB>=pF; pB--) {
    if(*pB > 0) return ((pB-pF)<<3)+pBitLookUp[nBitLookUp[*pB]-1][*pB];
  }
  return -1;
}

inline Int_t HMdcBArray::next(const UChar_t *pF, const UChar_t *pL, Int_t prPos) {
  // prPos <0 eq. first
  // return: -1 next is absent
  const UChar_t *pB=pF;
  UChar_t next=*pB;
  if(prPos >=0 ) {
    pB+=prPos>>3;
    if(pB>pL) return -1;
    next=*pB & nextBit[prPos&7];
  }
  for(; pB<=pL; next=*(++pB)) {
     if( next ) return ((pB-pF)<<3)+pBitLookUp[0][next];
  }
  return -1;
}

inline Int_t HMdcBArray::nextAndUnset(UChar_t *pF, UChar_t *pL, Int_t prPos) {
  // The function search next seted bit after prPos in array, unset this
  // bin and return a position of this one.
  // The bit prPos must be unseted (=0), if not - function will return 
  // the same position!
  // prPos <0 = seach first seted bin
  // return: -1 next is absent
  UChar_t* pB=pF;
  if(prPos>7) pB += prPos>>3;
  for(; pB<=pL; pB++) {
    if(*pB) {
      UChar_t bitN=pBitLookUp[0][*pB];
      *pB &= unsetBitLUp[bitN];   // Bin unsetting
      return ((pB-pF)<<3)+bitN;
    }
  }
  return -1;
}

inline Int_t HMdcBArray::previous(const UChar_t *pF, const UChar_t *pL, Int_t prPos) {
  // prPos >size = start from latest bit
  // return: -1 previous is absent
  if(prPos<0) return -1;
  const UChar_t *pB=pL;
  UChar_t next=*pB;
  if((prPos>>3)+pF <= pL) {
    pB=pF+(prPos>>3);
    next=*pB & prevBit[prPos&7];
  }
  for(; pB>=pF; next=*(--pB)) {
    if(next) return ((pB-pF)<<3)+pBitLookUp[nBitLookUp[next]-1][next];
  }
  return -1;
}

inline Int_t HMdcBArray::prevAndUnset(UChar_t *pF, UChar_t *pL, Int_t prPos) {
  // The same as nextAndUnset but do it back order.
  // prPos >= (pL-pF+1)*8 = start from latest bit
  // return: -1 next is absent
  if(prPos<0) return -1;
  UChar_t* pB=pF + (prPos>>3);  // = prPos/8
  if(pB>pL) pB=pL;
  for(; pB>=pF; pB-- ) {
    if(*pB) {
      UChar_t bitN=highBit[*pB];
      *pB &= unsetBitLUp[bitN];   // Bin unsetting
      return ((pB-pF)<<3)+bitN;
    }
  }
  return -1;
}

inline void HMdcBArray::shiftRight(UChar_t *pF, UChar_t *pL, UChar_t *pS) {
  // pF - pointer to first el., pL - pointer to last el.
  // pS - pointer to el. which must be moved to *pL ((pL-pS) - shift)
  while( pS>=pF ) *(pL--)=*(pS--);
  clear(pF, pL);
}

inline void HMdcBArray::shiftLeft(UChar_t *pT, UChar_t *pL, UChar_t *pS) {
  // pT - pointer to first el., pL - pointer to last el.
  // pS - pointer to el. which must be moved to *pT
  while( pS<=pL ) *(pT++)=*(pS++);
  clear(pT, pL);
}

inline UChar_t HMdcBArray::get(const UChar_t *pF, Int_t pos) {
  //Returns 1 if bit is seted, 0 if not
  return (pF[pos>>3] >> (pos&7)) & 1;
}

inline Bool_t HMdcBArray::testAndUnset(UChar_t *pF, Int_t pos) {
  // Bit n=pos will unseted. 
  // Returns kTRUE if bit was seted, kFALSE if not
  UChar_t mask=1<<(pos&7);
  pF+=(pos>>3);             // = pos/8
  if(*pF & mask) {
    *pF &= ~mask;
    return kTRUE;
  }
  return kFALSE;
}

inline Int_t HMdcBArray::compare(const UChar_t *pF1, const UChar_t *pF2,
    Int_t nBytes) {
  Int_t nBits=0;
  while((nBytes--)>0) nBits+=nBitLookUp[*(pF1++) & *(pF2++)];
  return nBits;
}

inline Int_t HMdcBArray::andArr(const UChar_t *pF1, const UChar_t *pF2,
    Int_t nBytes,UChar_t *pOut) {
  // Function produce list of identical for pF1 & ppF2 bits - pOut
  // return number of identical bits
  Int_t nTBits=0;
  while((nBytes--)>0) {
    *pOut=*(pF1++) & *(pF2++);
    nTBits+=nBitLookUp[*pOut];
    pOut++;
  }
  return nTBits;
}

inline Int_t HMdcBArray::andArrAndUnset(UChar_t *pNoId, const UChar_t *pEx,
    Int_t nBytes,UChar_t *pIdnt) {
  // Function produce list of identical for pNoId & ppEx bits - pIdnt
  // and unset identical bits in pNoId
  // return number of identical bits
//   Int_t nIBits=0;
//   const UChar_t *pExEnd = pEx + nBytes;
//   for(;pEx<pExEnd;pEx++,pIdnt++,pNoId++) {
//     *pIdnt  = *pNoId & *pEx;
//     *pNoId ^= *pIdnt;
//     nIBits += nBitLookUp[*pIdnt];
//   }
  
  UChar_t *pId    = pIdnt;
  UChar_t *pIdEnd = pIdnt + nBytes;
  Int_t nInt = nBytes>>2;
  if(nInt>0) {
    const UInt_t* pEx4    = (UInt_t*)pEx;
    const UInt_t* pEx4End = pEx4+nInt;
    UInt_t* pIdnt4  = (UInt_t*)pIdnt;
    UInt_t* pNoId4  = (UInt_t*)pNoId;
    for(;pEx4<pEx4End;pEx4++,pIdnt4++,pNoId4++) {
      *pIdnt4  = *pNoId4 & *pEx4;
      *pNoId4 ^= *pIdnt4;
    }
    pEx   = (UChar_t*)pEx4;
    pIdnt = (UChar_t*)pIdnt4;
    pNoId = (UChar_t*)pNoId4;
  }
  for(;pIdnt<pIdEnd;pEx++,pIdnt++,pNoId++) {
    *pIdnt  = *pNoId & *pEx;
    *pNoId ^= *pIdnt;
  }
  
  Int_t nIBits=0;
  for(;pId<pIdEnd;pId++) nIBits += nBitLookUp[*pId];
  
  return nIBits;
}

inline Int_t HMdcBArray::xorArr(const UChar_t *pF1, const UChar_t *pF2,
    Int_t nBytes,UChar_t *pOut) {
  // Function produce list of nonidentical for pF1 & ppF2 bits - pOut
  // return number of not identical bits in pF1 and pF2
  Int_t nTBits=0;
  while((nBytes--)>0) {
    *pOut   = *(pF1++) ^ *(pF2++);
    nTBits += nBitLookUp[*pOut];
    pOut++;
  }
  return nTBits;
}

#endif
