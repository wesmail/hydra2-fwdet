#ifndef HRPCPARORA2IO_H
#define HRPCPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HRpcCalPar;
class HRpcGeomPar;
class HRpcTrb2Lookup;

class HRpcParOra2Io : public HDetParOra2Io {
private:
  TArrayI* initModules;          // array of modules initialized from Oracle
  HOra2GeomDetVersion* geomVers; // geometry version
public:
  HRpcParOra2Io(HOra2Conn* p=0);
  ~HRpcParOra2Io(void);
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HRpcCalPar*,Int_t*);
  Bool_t read(HRpcGeomPar*,Int_t*);
  Bool_t readModGeomNames(HRpcGeomPar*,Int_t*);
  Bool_t readCompGeomNames(HRpcGeomPar*,Int_t*);
  Bool_t read(HRpcTrb2Lookup*);
  Int_t createVers(HParSet*);
  Int_t writePar(HRpcCalPar*);
  Int_t writePar(HRpcTrb2Lookup*);
  void printInfo(const Char_t*);
  ClassDef(HRpcParOra2Io,0) // RPC parameter I/O from Oracle
};

#endif  /* !HRPCPARORA2IO_H */
