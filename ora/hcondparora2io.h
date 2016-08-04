#ifndef HCONDPARORA2IO_H
#define HCONDPARORA2IO_H

#include "hdetparora2io.h"

class HMagnetPar;
class HParCond;
class HParamObj;

class HCondParOra2Io : public HDetParOra2Io {
public:
  HCondParOra2Io(HOra2Conn* p=0);
  ~HCondParOra2Io() {}
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  Bool_t read(HMagnetPar*);
  Bool_t readCond(HParCond*);
  Int_t writeCond(HParCond*);
  Bool_t readFromLoadingTable(HParCond*,Int_t);
private:
  Bool_t readBlob(HParamObj*,Int_t,Bool_t);
  Int_t createParamVers(HParCond*);
  Int_t storeBlob(UChar_t*,Int_t,Bool_t);
  Bool_t readLoadBlob(HParamObj*,Int_t,Bool_t);
  ClassDef(HCondParOra2Io,0) // I/O from Oracle for parameter containers derived from HParCond
};

#endif  /* !HCONDPARORA2IO_H */

