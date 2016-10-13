#ifndef HWALLTRB2UNPACKER_H
#define HWALLTRB2UNPACKER_H

#include "htrb2unpacker.h"
#include "hlocation.h"
#include "TH2F.h"

class HWallTrb2Lookup;

class HWallTrb2Unpacker: public HTrb2Unpacker {

protected:
  HLocation loc;
  HWallTrb2Lookup* lookup;  // TRB lookup table
  Bool_t timeRef;             // use reference time ?
  Bool_t timeRefRemove;       // remove reference times from data?
  Float_t timeShift;          //shift all times by a constant (to positive)


public:
  HWallTrb2Unpacker(UInt_t id=0);
  ~HWallTrb2Unpacker(void) { }

  Int_t execute(void);
  Bool_t init(void);
  //  Bool_t reinit(void);
  Bool_t finalize(void);

  void disableTimeRef(void) { timeRef = kFALSE ; }
  void removeTimeRef(void) {timeRefRemove = kTRUE ; } 
  void shiftTimes(Float_t ts) {timeShift = ts;};

public:
  ClassDef(HWallTrb2Unpacker,0) // Unpacker for Wall data
};

#endif /* !HWALLTRB2UNPACKER_H */







