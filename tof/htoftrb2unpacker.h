#ifndef HTOFTRB2UNPACKER_H
#define HTOFTRB2UNPACKER_H

#include "htrb2unpacker.h"
#include "hlocation.h"
#include "TH2F.h"

class HTofTrb2Lookup;

class HTofTrb2Unpacker: public HTrb2Unpacker {

protected:
  HLocation loc;
  HTofTrb2Lookup* lookup;  // TRB lookup table
  Bool_t timeRef;             // use reference time ?
  Bool_t timeRefRemove;       // remove reference times from data?
  Float_t timeShift;          //shift all times by a constant (to positive)


public:
  HTofTrb2Unpacker(UInt_t id=0);
  ~HTofTrb2Unpacker(void) { }

  Int_t execute(void);
  Bool_t init(void);
  //  Bool_t reinit(void);
  Bool_t finalize(void);

  void disableTimeRef(void) { timeRef = kFALSE ; }
  void removeTimeRef(void) {timeRefRemove = kTRUE ; } 
  void shiftTimes(Float_t ts) {timeShift = ts;};
  
public:
  ClassDef(HTofTrb2Unpacker,0) // Unpacker for TOF data
};

#endif /* !HTOFTRB2UNPACKER_H */







