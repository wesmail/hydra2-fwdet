#ifndef HMDCPARORA2IO_H
#define HMDCPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HMdcRawStruct;
class HMdcGeomStruct;
class HMdcCalParRaw;
class HMdcCal2ParSim;
class HMdcLookupGeom;
class HMdcLayerGeomPar;
class HMdcGeomPar;
class HMdcCellEff;
class HMdcTimeCut;
class HMdcTdcThreshold;
class HMdcTdcChannel;

class HMdcParOra2Io : public HDetParOra2Io {
private:
  TArrayI* initModules;          // array of modules initialized from Oracle
  HOra2GeomDetVersion* geomVers; // geometry version
public:
  HMdcParOra2Io(HOra2Conn* p=0);
  ~HMdcParOra2Io();
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet* pPar,Int_t&);
  Int_t  getLookupVersion(HMdcLookupGeom*);
  Bool_t read(HMdcRawStruct*,Int_t*);
  Bool_t read(HMdcGeomStruct*,Int_t*);
  Bool_t read(HMdcLookupGeom*,Int_t*);
  Bool_t read(HMdcGeomPar*,Int_t*);
  Bool_t readModGeomNames(HMdcGeomPar*,Int_t*);
  Bool_t readLayerGeomNames(HMdcGeomPar*,Int_t*);
  Bool_t read(HMdcCalParRaw*,Int_t*);
  Bool_t read(HMdcCal2ParSim*,Int_t*);
  Bool_t read(HMdcLayerGeomPar*,Int_t*);
  Bool_t read(HMdcCellEff*,Int_t*);
  Bool_t read(HMdcTimeCut*,Int_t*);
  Bool_t read(HMdcTdcThreshold*,Int_t*);
  Bool_t read(HMdcTdcChannel* pPar, Int_t* set);
  Int_t createVersion(HParSet*);
  Int_t writePar(HMdcCalParRaw*);
  Int_t writePar(HMdcCal2ParSim*);
  Int_t writePar(HMdcCellEff*);
  Int_t writePar(HMdcTimeCut*);
  Int_t writePar(HMdcLayerGeomPar*);
  Int_t writePar(HMdcLookupGeom*);
  Int_t writePar(HMdcTdcThreshold*);
  Int_t writePar(HMdcTdcChannel*);
  Int_t countTdcChannels(Int_t,Int_t);
  void printInfo(const Char_t*);
  ClassDef(HMdcParOra2Io,0) // MDC parameter I/O from Oracle since 2010
};

#endif  /* !HMDCPARORA2IO_H */
