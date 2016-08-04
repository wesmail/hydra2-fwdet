#ifndef HSHOWERPARORA2IO_H
#define HSHOWERPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"
#include "TString.h"

class HOra2Conn;
class HShowerCalPar;
class HShowerGeomPar;

class HShowerParOra2Io : public HDetParOra2Io {
private:
  TArrayI* initModules;          // array of modules initialized from Oracle
  HOra2GeomDetVersion* geomVers; // geometry version
public:
  HShowerParOra2Io(HOra2Conn* p=0);
  ~HShowerParOra2Io();
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HShowerCalPar*,Int_t*);
  Bool_t read(HShowerGeomPar*,Int_t*);
  Bool_t readGeomNames(HShowerGeomPar*,Int_t*);
  Int_t createVers(HParSet*);
  Int_t writePar(HShowerCalPar*);
  void printInfo(const Char_t*);
  ClassDef(HShowerParOra2Io,0) // SHOWER parameter I/O from Oracle
};

#endif  /* !HSHOWERPARORA2IO_H */
