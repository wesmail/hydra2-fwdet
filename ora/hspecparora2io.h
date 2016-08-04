#ifndef HSPECPARORA2IO_H
#define HSPECPARORA2IO_H

#include "hdetparora2io.h"
#include "TList.h"

class HSpecGeomPar;
class HOra2GeomDetVersion;
class HTrbnetAddressMapping;
class HTrb2Correction;
class HSlowPar;

class HSpecParOra2Io : public HDetParOra2Io {
private:
  HOra2GeomDetVersion* geomCaveVers;    // Geometry version and range for the cave
  HOra2GeomDetVersion* geomSectorVers;  // Geometry version and range for the sectors
  HOra2GeomDetVersion* geomTargetVers;  // Geometry version and range for the targets
  HParOra2Set*         alignTargetVers; // Alignment version and range for the targets
  HParOra2Set*         slowVers;        // Slow control parameter version and range
  TString              slowPartition;   // Slow control partition
public:
  HSpecParOra2Io(HOra2Conn* p=0);
  ~HSpecParOra2Io();
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  Int_t writeTrb2Corrections(HTrbnetAddressMapping*);
private:
  Bool_t read(HSpecGeomPar*,Int_t*);
  Bool_t readTargetVolumes(HSpecGeomPar*,TList&);
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HTrbnetAddressMapping*);
  Bool_t readTrb2CorrData(HTrb2Correction*,Int_t);
  void findTrb2CorrDataTimeRange(HTrb2Correction*,Double_t*);
  Bool_t read(HSlowPar* pPar);
  Bool_t readSlowParRuns(HSlowPar*,Int_t&,Int_t&);
  void readSlowSummaries(HSlowPar* pPar);
  Int_t writeAlignment(HSpecGeomPar*);
  Int_t createTrbnetAddressMappingVers(HTrbnetAddressMapping*);
  Int_t writeTrbnetAddressMapping(HTrbnetAddressMapping*);
  Int_t compareForWrite(HTrb2Correction&,Float_t&);
  ClassDef(HSpecParOra2Io,0) // Parameter I/O from Oracle for spectrometer
};

#endif  /* !HSPECPARORA2IO_H */
