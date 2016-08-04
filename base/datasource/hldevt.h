// File: hldevt.h
//
// Author: Walter Karig <W.Karig@gsi.de>
// Last update: 10/12/2001  R. Holzmann
//

#ifndef HLDEVT_H
#define HLDEVT_H

#include "hldsubevt.h"
#include "hldunpack.h"

class HldEvt: public HldBase {
public:
  HldEvt() { isWritable=kFALSE; } 
  ~HldEvt() {;}
  // Header informations:
  
  size_t getHdrSize() const { return sizeof(EvtHdr); }
  UInt4 getSize() const { return ((EvtHdr*)pHdr)->size; }
  UInt4 getDecoding() const { return ((EvtHdr*)pHdr)->decoding; }
  UInt4 getId() const { return ((EvtHdr*)pHdr)->id; } 
  UInt4 getSeqNr() const { return ((EvtHdr*)pHdr)->seqNr; }
  UInt4 getDate() const { return ((EvtHdr*)pHdr)->date; }
  UInt4 getTime() const { return ((EvtHdr*)pHdr)->time; }
  Int_t getYear() const { return byte(2, ((EvtHdr*)pHdr)->date); }  
  Int_t getMonth() const { return byte(3, ((EvtHdr*)pHdr)->date) + 1; }
  Int_t getDay() const { return byte(4, ((EvtHdr*)pHdr)->date); }
  Int_t getHour() const { return byte(2, ((EvtHdr*)pHdr)->time); }
  Int_t getMinute() const { return byte(3, ((EvtHdr*)pHdr)->time); }
  Int_t getSecond() const { return byte(4, ((EvtHdr*)pHdr)->time); }
  UInt4 getRunNr() const { return ((EvtHdr*)pHdr)->runNr; }
  UInt4 getPad() const { return ((EvtHdr*)pHdr)->pad; }
  
  void dumpHdr() const;
  Bool_t scanHdr(void);
  Int_t appendSubEvtIdx(HldUnpack& unpack);
  virtual Bool_t execute() {return kFALSE;}
  virtual Bool_t swap(void){return kFALSE;}
  void setWritable(Bool_t f=kTRUE) { isWritable=f; }
  
protected:
  // input file
  istream* file;
  Bool_t isWritable; // flag used for HLD output
 
  // Event header
  struct EvtHdr {
    UInt4 size;
    UInt4 decoding;
    UInt4 id;
    UInt4 seqNr;
    UInt4 date;
    UInt4 time;
    UInt4 runNr;
    UInt4 pad;
  } hdr;

  size_t getDataLen() const { return (getDataSize()+3)/4; }
  // void    setEvtSize(UInt4  i) const {hdr.size=i;}
  // preliminary table for unpack
  size_t lastSubEvtIdx;

  static const size_t  maxSubEvtIdx = 100;
  struct SubEvtTable {
    UInt4 id;
    HldSubEvt** p;
  };

  SubEvtTable subEvtTable[maxSubEvtIdx];

  // subevent list (preliminary)
  static const size_t maxSubEvts = 100;
  HldSubEvt subEvt[maxSubEvts];

  virtual Bool_t read(){return kFALSE;}
  virtual Bool_t readSubEvt(size_t ){return kFALSE;}

public:
  //ClassDef(HldEvt, 0)           // HADES LMD Event
};

#endif /* !HLDEVT_H */ 



