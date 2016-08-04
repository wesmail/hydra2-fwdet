// File: hldsubevt.h
//
// Author: Walter Karig <W.Karig@gsi.de>
// Last update: 97/12/05 18:36:05
//

#ifndef HLDSUBEVT_H
#define HLDSUBEVT_H

#include "hldbase.h"
#include <stdio.h>

class HldSubEvt: public HldBase {
public:
  HldSubEvt(): swapped(0) { pHdr = 0; }
  HldSubEvt(UInt4* p) {
    pHdr = p; pData = pHdr + getHdrLen();
       if ((swapped = isSwapped()) !=0){
            swapHdr();
        }
  }
  // Get header informations
  size_t getWordSize() const {

    return 1 <<  (((SubEvtHdr*)pHdr)->decoding >> 16 & 0xf);
  }
  Int_t getTrigNr() const { return (((SubEvtHdr*)pHdr)->trigNr)& 0xff; }
  Int_t getUnMaskedTrigNr() const { return ((SubEvtHdr*)pHdr)->trigNr; }
  

  size_t getDataLen() const { return getDataSize()/getWordSize(); }
  void swapData();
  void dumpIt(FILE *output=stderr) const;
  Bool_t scanIt(void);
private:
  Bool_t swapped;
  Bool_t wasSwapped() const { return swapped; }
  void resetSwapFlag() {swapped=0;}

  struct SubEvtHdr {
    UInt4 size;
    UInt4 decoding;
    UInt4 id;
    UInt4 trigNr;
  };
  size_t getHdrSize() const { return sizeof(SubEvtHdr); }
};

#endif /* !HLDSUBEVT_H */      





