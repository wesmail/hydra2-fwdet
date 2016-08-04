#ifndef HEMCGEOMPAR_H
#define HEMCGEOMPAR_H

#include "hdetgeompar.h"

class HGeomShapes;

class HEmcGeomPar : public HDetGeomPar {
  Int_t maxCells;  
public:
  HEmcGeomPar(const Char_t* name="EmcGeomPar",
              const Char_t* title="Emc geometry parameters",
              const Char_t* context="GeomProduction");
  ~HEmcGeomPar() {}
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  inline Int_t getSecNum(const TString&);
  inline Int_t getModNumInMod(const TString&);
  inline Int_t getModNumInComp(const TString&);
  Int_t getCompNum(const TString&);
  ClassDef(HEmcGeomPar,1) // Container for the Emc geometry parameters
};

inline Int_t HEmcGeomPar::getSecNum(const TString& name) {
  // returns the module index retrieved from the module name GMOMx
  return (Int_t)(name[4]-'0')-1;
}

inline Int_t HEmcGeomPar::getModNumInMod(const TString& name) {
  // returns the module index (always 0)
  return 0;
}

inline Int_t HEmcGeomPar::getModNumInComp(const TString& name) {
  // returns the module index (always 0)
  return 0;
}

#endif /* !HEMCGEOMPAR_H */
