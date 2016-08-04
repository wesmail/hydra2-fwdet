#ifndef HTOFGEOMPAR_H
#define HTOFGEOMPAR_H

#include "hdetgeompar.h"

class HGeomShapes;

class HTofGeomPar : public HDetGeomPar {
public:
  HTofGeomPar(const Char_t* name="TofGeomPar",
              const Char_t* title="Tof geometry parameters",
              const Char_t* context="GeomProduction");
  ~HTofGeomPar() {}
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  inline Int_t getSecNum(const TString&);
  inline Int_t getModNumInMod(const TString&);
  inline Int_t getModNumInComp(const TString&);
  inline Int_t getCompNum(const TString&);
  ClassDef(HTofGeomPar,1) // Container for the Tof geometry parameters
};

inline Int_t HTofGeomPar::getSecNum(const TString& name) {
  // returns the module index retrieved from the module name TxxFx
  return (Int_t)(name[4]-'0')-1;
}

inline Int_t HTofGeomPar::getModNumInMod(const TString& name) {
  // returns the module index retrieved from the module name TxxFx
  return (-((Int_t)(name[1]-'0')*10+(Int_t)(name[2]-'0')-maxModules));
}

inline Int_t HTofGeomPar::getModNumInComp(const TString& name) {
  // returns the module index retrieved from the  name TxxSx
  return (-((Int_t)(name[1]-'0')*10+(Int_t)(name[2]-'0')-maxModules));
}

inline Int_t HTofGeomPar::getCompNum(const TString& name) {
  // returns the layer index retrieved from the layer name TxxSx
  return (8-(Int_t)(name[4]-'0'));
}

#endif /* !HTOFGEOMPAR_H */
