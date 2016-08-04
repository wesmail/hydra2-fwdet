#ifndef HPIONTRACKERPARORA2IO_H
#define HPIONTRACKERPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HPionTrackerCalPar;
class HPionTrackerTrb3Lookup;

class HPionTrackerParOra2Io : public HDetParOra2Io {
private:
  TArrayI* initModules;       // array of modules initialized from Oracle
public:
  HPionTrackerParOra2Io(HOra2Conn* p=0);
  ~HPionTrackerParOra2Io(void);
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HPionTrackerCalPar*,Int_t*);
  Bool_t read(HPionTrackerTrb3Lookup*);
  Int_t createVers(HParSet*);
  Int_t writePar(HPionTrackerCalPar*);
  Int_t writePar(HPionTrackerTrb3Lookup*);
  void printInfo(const Char_t*);
  ClassDef(HPionTrackerParOra2Io,0) // Parameter I/O from Oracle for Pion Tracker
};

#endif  /* !HPIONTRACKERPARORA2IO_H */
