#ifndef HRICHPARORA2IO_H
#define HRICHPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HRichCalPar;
class HRichMappingPar;
class HRichThresholdPar;
class HRich700Trb3Lookup;
class HRich700ThresholdPar;
class HRich700GeomPar;

class HRichParOra2Io : public HDetParOra2Io {
private: 
  TArrayI* initModules; // array of modules initialized from Oracle
  HOra2GeomDetVersion* geomVers; // geometry version
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
  Bool_t read(HRich700Trb3Lookup*,Int_t*);
  Bool_t read(HRich700ThresholdPar*,Int_t*);
  Bool_t read(HRich700GeomPar*,Int_t*);
  Bool_t readModGeomNames(HRich700GeomPar* pPar);
  Bool_t readCompGeomNames(HRich700GeomPar* pPar);


  Int_t createVers(HParSet*);
  Int_t createThresholdVers(HRichThresholdPar*);
  Int_t writePar(HRichCalPar*); 
  Int_t writePar(HRichMappingPar*);
  Int_t writePar(HRichThresholdPar*); 
  Int_t writePar(HRich700Trb3Lookup*);
  Int_t writePar(HRich700ThresholdPar*);
  ClassDef(HRichParOra2Io,0) // RICH parameter I/O from Oracle for data since 2010
};

#endif  /* !HRICHPARORA2IO_H */
