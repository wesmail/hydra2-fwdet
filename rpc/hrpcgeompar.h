#ifndef HRPCGEOMPAR_H
#define HRPCGEOMPAR_H

#include "hdetgeompar.h"

class HGeomShapes;

class HRpcGeomPar : public HDetGeomPar {
  Int_t maxCells;  
public:
  HRpcGeomPar(const Char_t* name="RpcGeomPar",
              const Char_t* title="Rpc geometry parameters",
              const Char_t* context="GeomProduction");
  ~HRpcGeomPar() {}
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  inline Int_t getSecNum(const TString&);
  inline Int_t getModNumInMod(const TString&);
  inline Int_t getModNumInComp(const TString&);
  inline Int_t getCompNum(const Int_t, const Int_t);
  Int_t getCompNum(const TString&);
  ClassDef(HRpcGeomPar,1) // Container for the Rpc geometry parameters
};

inline Int_t HRpcGeomPar::getSecNum(const TString& name) {
  // returns the module index retrieved from the module name EBOXx
  return (Int_t)(name[4]-'0')-1;
}

inline Int_t HRpcGeomPar::getModNumInMod(const TString& name) {
  // returns the module index (always 0)
  return 0;
}

inline Int_t HRpcGeomPar::getModNumInComp(const TString& name) {
  // returns the module index (always 0)
  return 0;
}

inline Int_t HRpcGeomPar::getCompNum(const Int_t col, const Int_t cell) {
  // returns the cell index calculated by column and cell number
  return col*maxCells+cell;
}

#endif /* !HRPCGEOMPAR_H */
