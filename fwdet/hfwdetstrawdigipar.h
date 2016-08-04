#ifndef HFWDETSTRAWDIGIPAR_H
#define HFWDETSTRAWDIGIPAR_H

#include "hparcond.h"

class HFwDetStrawDigiPar : public HParCond {
protected:
    // Parameters needed for digitizations of straws
    // Geometry, layout ...
    Int_t   nStrawCells;
    Float_t strawRadius;

public:
  HFwDetStrawDigiPar(const Char_t* name="FwDetStrawDigiPar",
               const Char_t* title="Digitization parameters for Forward Straw Detector",
               const Char_t* context="FwDetStrawDigiProduction");
  ~HFwDetStrawDigiPar() {;}
  Float_t getStrawRadius() {return strawRadius;}
  Int_t   getNStrawCells() {return nStrawCells;}

  void setStrawRadius(Float_t r) {strawRadius = r;}
  void setNStrawCells(Int_t c) {nStrawCells = c;}

  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);
  void   clear();

  ClassDef(HFwDetStrawDigiPar,1) // Container for the Forward Straw Detector digitization

};

#endif  /* !HFWDETSTRAWDIGIPAR_H */
