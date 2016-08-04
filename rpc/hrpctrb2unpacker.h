#ifndef HRPCTRB2UNPACKER_H
#define HRPCTRB2UNPACKER_H

#include "htrb2unpacker.h"
#include "hlocation.h"

class HRpcTrb2Lookup;

class HRpcTrb2Unpacker: public HTrb2Unpacker {

protected:
  HLocation loc;

	HRpcTrb2Lookup* lookup;        // TRB lookup table
  
  Bool_t bNoTimeRefCorr;         // Switch of Reference Time substraction (debugging)
  Bool_t bStoreSpareChanData;    // flag for storing data from spare channels (cells over no. 31). By default, it is not.

public:
  HRpcTrb2Unpacker(UInt_t id);
  ~HRpcTrb2Unpacker(void) { }

  Int_t execute(void); 
  Bool_t init(void);
  Bool_t finalize(void);
  
  void disableTimeRef(void) { bNoTimeRefCorr=true; };              // Switch off Reference Time substraction (debugging)
  void storeSpareChannelsData()  { bStoreSpareChanData = true; }   // Switch on storing spare channels data


public:
  ClassDef(HRpcTrb2Unpacker,0) // Unpacker for Rpc data
};          

#endif /* !HRPCTRB2UNPACKER_H */
