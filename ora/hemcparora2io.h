#ifndef HEMCPARORA2IO_H
#define HEMCPARORA2IO_H

#include "hdetparora2io.h"
#include "TArrayI.h"

class HEmcTrb3Lookup;
class HEmcCalPar;
class HEmcGeomPar;
class HGeomVolume;

class HEmcParOra2Io : public HDetParOra2Io {
private:
  TArrayI* initModules;          // array of modules initialized from Oracle
  HOra2GeomDetVersion* geomVers; // geometry version
  HGeomVolume* sensVolume;       // sensitive volume
public:
  HEmcParOra2Io(HOra2Conn* p=0);
  ~HEmcParOra2Io(void);
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
private:
  Bool_t getVersion(HParSet*,Int_t&);
  Bool_t read(HEmcTrb3Lookup*);
  Bool_t read(HEmcCalPar*,Int_t*);
  Bool_t read(HEmcGeomPar*,Int_t*);
  Bool_t readModGeomNames(HEmcGeomPar*,Int_t*);
  Bool_t readCompGeomNames(HEmcGeomPar*,Int_t*);
  void addGeomRefComponents(HDetGeomPar*,TList*);
  Bool_t transformGeomCompositeComponents(HDetGeomPar*);
  Int_t createVers(HParSet*);
  Int_t writePar(HEmcCalPar*);
  Int_t writePar(HEmcTrb3Lookup*);
  void printInfo(const Char_t*);
  ClassDef(HEmcParOra2Io,0) // EMC parameter I/O from Oracle
};

#endif /* !HEMCPARORA2IO_H */
