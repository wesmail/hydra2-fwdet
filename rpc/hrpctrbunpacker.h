#ifndef HRPCTRBUNPACKER_H
#define HRPCTRBUNPACKER_H

#include "htrbbaseunpacker.h"
#include "hlocation.h"

#include "TH2F.h"


class HTrbLookup;

class HRpcTrbUnpacker: public HTrbBaseUnpacker {

protected:
  HLocation loc;
  
  Int_t debugRpcFlag; 
  Bool_t fNoTimeRefCorr;         // Switch of Reference Time substraction (debugging)
  Bool_t fControlHistograms;     // flag for control histograms
  TH2F *histdbos, *histdbombo, *histmbotrb, *histtrbchan;  // control histos

public:
  HRpcTrbUnpacker(UInt_t id);
  ~HRpcTrbUnpacker(void) { }

  Int_t execute(void); 
  Bool_t init(void);
  Bool_t finalize(void);
  
  void setDebugRpcFlag(Int_t db) { debugRpcFlag = db;}
  void disableTimeRef(void) { fNoTimeRefCorr=true; };             // Switch off Reference Time substraction (debugging)
  void enableControlHistograms() { fControlHistograms = kTRUE; }  // Switch on control histos


public:
  ClassDef(HRpcTrbUnpacker,0) // Unpacker for Rpc data
};          

#endif /* !HRPCTRBUNPACKER_H */
