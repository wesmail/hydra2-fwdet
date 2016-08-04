#ifndef HWALLGEOMPAR_H
#define HWALLGEOMPAR_H

#include "hdetgeompar.h"

class HGeomShapes;

class HWallGeomPar : public HDetGeomPar {
public:
  HWallGeomPar(const Char_t* name="WallGeomPar",
               const Char_t* title="Wall geometry parameters",
               const Char_t* context="GeomProduction");
  ~HWallGeomPar() {}
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  inline Int_t getModNumInMod(const TString&);
  inline Int_t getModNumInComp(const TString&);
  Int_t getCompNum(const TString&);
  TString getCellName(Int_t);    
  ClassDef(HWallGeomPar,1) // Container for the Wall geometry parameters
};

inline Int_t HWallGeomPar::getModNumInMod(const TString& name) {
  // returns the module index 0
  return 0;
}

inline Int_t HWallGeomPar::getModNumInComp(const TString& name) {
  // returns the module index 0
  return 0;
}

#endif /* !HWALLGEOMPAR_H */
