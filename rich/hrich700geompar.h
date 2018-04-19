#ifndef HRICH700GEOMPAR_H
#define HRICH700GEOMPAR_H

#include "hdetgeompar.h"

class HGeomShapes;

class HRich700GeomPar : public HDetGeomPar {
public:
  HRich700GeomPar(const Char_t* name="Rich700GeomPar",
               const Char_t* title="Rich 700 geometry parameters",
               const Char_t* context="GeomProduction");
  ~HRich700GeomPar() {}
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  inline Int_t getModNumInMod(const TString&);
  inline Int_t getModNumInComp(const TString&);
  Int_t getCompNum(const TString&);
  TString getCellName(Int_t);    
  ClassDef(HRich700GeomPar,1) // Container for the Wall geometry parameters
};

inline Int_t HRich700GeomPar::getModNumInMod(const TString& name) {
  // returns the module index 0
  return 0;
}

inline Int_t HRich700GeomPar::getModNumInComp(const TString& name) {
  // returns the module index 0
  return 0;
}

#endif /* !HWALLGEOMPAR_H */
