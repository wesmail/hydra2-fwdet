#ifndef HMDCGEOMPAR_H
#define HMDCGEOMPAR_H

#include "hdetgeompar.h"

class HGeomShapes;

class HMdcGeomPar : public HDetGeomPar {
public:
  HMdcGeomPar(const Char_t* name="MdcGeomPar",
              const Char_t* title="Mdc geometry parameters",
              const Char_t* context="GeomProduction");
  ~HMdcGeomPar() {}
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  inline Int_t getSecNum(const TString&);
  inline Int_t getModNumInMod(const TString&);
  inline Int_t getModNumInComp(const TString&);
  inline Int_t getCompNum(const TString&);
  void shiftLayerTransToCenter();
  ClassDef(HMdcGeomPar,1) // Container for the MDC geometry parameters
};

inline Int_t HMdcGeomPar::getSecNum(const TString& name) {
  // returns the module index retrieved from the module name DRxMx
  return (Int_t)(name[4]-'0')-1;
}

inline Int_t HMdcGeomPar::getModNumInMod(const TString& name) {
  // returns the module index retrieved from the module name DRxMx
  return (Int_t)(name[2]-'0')-1;
}

inline Int_t HMdcGeomPar::getModNumInComp(const TString& name) {
  // returns the module index retrieved from the layer name DxLx
  return (Int_t)(name[1]-'0')-1;
}

inline Int_t HMdcGeomPar::getCompNum(const TString& name) {
  // returns the layer index retrieved from the layer name DxLx
  return (Int_t)(name[3]-'0')-1;
}

#endif /* !HMDCGEOMPAR_H */
