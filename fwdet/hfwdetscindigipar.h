#ifndef HFWDETSCINDIGIPAR_H
#define HFWDETSCINDIGIPAR_H

#include "hparcond.h"
#include "TArrayF.h"

class HFwDetScinDigiPar : public HParCond {
protected:
  // Parameters needed for digitizations of scintillator GEANT data
  // Geometry, layout ...
  Int_t   nScinCells;  // number of scintillator cells
  TArrayF scinSize;    // width and height of a single cell

public:
  HFwDetScinDigiPar(const Char_t* name="FwDetScinDigiPar",
               const Char_t* title="Digitization parameters for Forward Scintillator Detector",
               const Char_t* context="FwDetScinDigiProduction");
  ~HFwDetScinDigiPar() {;}
  Int_t getNScinCells() {return nScinCells;}
  void  getScinSize(Float_t& w, Float_t& h);

  void setNScinCells(Int_t n) {nScinCells = n;}
  void setScinSize(Float_t w, Float_t h);

  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);
  void   clear();

  ClassDef(HFwDetScinDigiPar,1) // Container for the Forward Scintillator Detector digitization

};

inline void HFwDetScinDigiPar::getScinSize(Float_t& w, Float_t& h) {
  w = scinSize.At(0);
  h = scinSize.At(1);
}

inline void HFwDetScinDigiPar::setScinSize(Float_t w, Float_t h) {
  scinSize.SetAt(w,0);
  scinSize.SetAt(h,1);
}

#endif  /* !HFWDETSCINDIGIPAR_H */
