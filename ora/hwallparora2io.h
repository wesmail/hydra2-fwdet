#ifndef HWALLPARORA2IO_H
#define HWALLPARORA2IO_H

#include "hdetparora2io.h"

class HWallTrb2Lookup;
class HWallCalPar;
class HWallGeomPar;

class HWallParOra2Io : public HDetParOra2Io {
private:
  HOra2GeomDetVersion* geomVers; // geometry version
public:
  HWallParOra2Io(HOra2Conn* p=0);
  ~HWallParOra2Io();
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HWallTrb2Lookup*);
  Bool_t read(HWallCalPar*);
  Bool_t read(HWallGeomPar*,Int_t*);
  Bool_t readModGeomNames(HWallGeomPar* pPar);
  Bool_t readCompGeomNames(HWallGeomPar* pPar);
  Int_t createVers(HParSet*);
  Int_t writePar(HWallTrb2Lookup*);
  Int_t writePar(HWallCalPar*);
  ClassDef(HWallParOra2Io,0) // Forward Wall parameter I/O from Oracle since 2010
};

#endif  /* !HWALLPARORA2IO_H */
