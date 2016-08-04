#ifndef HRICHPARORA2IO_H
#define HRICHPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HRichCalPar;
class HRichMappingPar;
class HRichThresholdPar;

class HRichParOra2Io : public HDetParOra2Io {
private: 
  TArrayI* initModules; // array of modules initialized from Oracle
public:
  HRichParOra2Io(HOra2Conn* p=0);
  ~HRichParOra2Io();
  Bool_t init(HParSet*,Int_t*); 
  Int_t write(HParSet*); 
private:
  void printInfo(const Char_t*);
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HRichCalPar*,Int_t*); 
  Bool_t read(HRichMappingPar*,Int_t*); 
  Bool_t read(HRichThresholdPar*,Int_t*);
  Int_t createVers(HParSet*);
  Int_t createThresholdVers(HRichThresholdPar*);
  Int_t writePar(HRichCalPar*); 
  Int_t writePar(HRichMappingPar*);
  Int_t writePar(HRichThresholdPar*); 
  ClassDef(HRichParOra2Io,0) // RICH parameter I/O from Oracle for data since 2010
};

#endif  /* !HRICHPARORA2IO_H */
