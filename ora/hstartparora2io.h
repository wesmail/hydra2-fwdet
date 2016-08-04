#ifndef HSTARTPARORA2IO_H
#define HSTARTPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HStart2Calpar;
class HStart2Trb2Lookup;
class HStart2GeomPar;
class HStart2Trb3Calpar;
class HTrb3CalparTdc;
class HStart2Trb3Lookup;

class HStartParOra2Io : public HDetParOra2Io {
private:
  TArrayI* initModules;          // array of modules initialized from Oracle
  HOra2GeomDetVersion* geomVers; // geometry version
public:
  HStartParOra2Io(HOra2Conn* p=0);
  ~HStartParOra2Io(void);
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HStart2Calpar*,Int_t*);
  Bool_t read(HStart2Trb2Lookup*);
  Bool_t read(HStart2GeomPar*,Int_t*);
  Bool_t read(HStart2Trb3Calpar*);
  Bool_t readTrb3CalData(HTrb3CalparTdc*,Int_t,Int_t);
  Bool_t read(HStart2Trb3Lookup*);
  Int_t createVers(HParSet*);
  Int_t writePar(HStart2Calpar*);
  Int_t writePar(HStart2Trb2Lookup*);
  Int_t writePar(HStart2Trb3Calpar*);
  Int_t writePar(HStart2Trb3Lookup*);
  void printInfo(const Char_t*);
  ClassDef(HStartParOra2Io,0) // Parameter I/O from Oracle for Start detector
};

#endif  /* !HSTARTPARORA2IO_H */
