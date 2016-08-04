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
#include "htrb3tdcunpacker.h"
#include <vector>
#include "TString.h"

class HTrb3Calpar;
class HTrbnetAddressMapping;

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

  HTrb3Calpar* calpar;                   //! TDC calibration parameters 
  std::vector<HTrb3TdcUnpacker*> fTDCs;  //! vector of TDC unpackers

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
  virtual Bool_t reinit(void);

  void createTDC(UInt_t id1, HTrb3CalparTdc*);
  void addTDC(HTrb3TdcUnpacker* tdc);

  UInt_t numTDC() const { return fTDCs.size(); }
  HTrb3TdcUnpacker* getTDC(UInt_t indx) const { return fTDCs[indx]; }

protected:
  void   clearAll(void);
  Bool_t decode(void);

  ClassDef(HTrb3Unpacker,0); //Base class for TRB3 unpackers
};

#endif /* !HTRB3UNPACKER_H */


