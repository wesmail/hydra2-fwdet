#ifndef HTOFPARORA2IO_H
#define HTOFPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HTofTrb2Lookup;
class HTofCalPar;
class HTofDigiPar;
class HTofGeomPar;

class HTofParOra2Io : public HDetParOra2Io {
private:
  TArrayI* initModules;          // array of modules initialized from Oracle
  HOra2GeomDetVersion* geomVers; // geometry version
public:
  HTofParOra2Io(HOra2Conn* p=0);
  ~HTofParOra2Io();
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HTofTrb2Lookup*);
  Bool_t read(HTofCalPar*,Int_t*);
  Bool_t read(HTofDigiPar*,Int_t*);
  Bool_t read(HTofGeomPar*,Int_t*);
  Bool_t readModGeomNames(HTofGeomPar*,Int_t*);
  Bool_t readCompGeomNames(HTofGeomPar*,Int_t*);
  Int_t createVers(HParSet*);
  Int_t writePar(HTofCalPar*);
  Int_t writePar(HTofDigiPar*);
  Int_t writePar(HTofTrb2Lookup*);
  void printInfo(const Char_t*);
  ClassDef(HTofParOra2Io,0) // TOF parameter I/O from Oracle
};

#endif  /* !HTOFPARORA2IO_H */
