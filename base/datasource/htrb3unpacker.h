/////////////////////////////////////////////////////////////
//
//  HTrb3Unpacker
//
//  This is a base for decoding TRB data and filling arrays. 
//  Derived classes has to provide methods for filling 
//  Detector's Raw categories.
//
/////////////////////////////////////////////////////////////

#ifndef HTRB3UNPACKER_H
#define HTRB3UNPACKER_H

#include "hldunpack.h"
#include <vector>
#include "TString.h"

class HTrb3Unpacker : public HldUnpack {

protected:
  HTrb3Unpacker(UInt_t id=0);
  virtual ~HTrb3Unpacker();

  UInt_t subEvtId;                      //! subevent id - main identifier
  UInt_t uHUBId;                        //! number like 0x9000 which works as envelope for external TDCs
  UInt_t uCTSId;                        //! number like 0x8000 which indicates CTS block in data
  Int_t  debugFlag;                     //! allows to print subevent information to the STDOUT
  Bool_t quietMode;                     //! do not print errors!
  Bool_t reportCritical;                //! report critical errors!

public:
  Int_t getSubEvtId(void) const { return subEvtId; }

  void setHUBId(UInt_t id) { uHUBId = id; }
  void setCTSId(UInt_t id) { uCTSId = id; }

  void setDebugFlag(Int_t db) { debugFlag  = db; }
  Int_t getDebugFlag() { return debugFlag; }

  void setQuietMode(void)      { quietMode      = kTRUE; }
  void setReportCritical(void) { reportCritical = kTRUE; }

  Bool_t isQuietMode() const { return quietMode; }
  Bool_t isReportCritical() const { return reportCritical;}

  virtual Int_t  execute(void){ return 0;};
  virtual Bool_t init(void){ return kFALSE; };
  virtual Bool_t reinit(void) { return kTRUE; };

protected:
  virtual void   clearAll(void);
  Bool_t decode(void);
  virtual Bool_t decodeData(UInt_t trbaddr, UInt_t n, UInt_t * data) { return kFALSE; }

  ClassDef(HTrb3Unpacker,0); //Base class for TRB3 unpackers
};

#endif /* !HTRB3UNPACKER_H */


