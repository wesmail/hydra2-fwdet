#ifndef HWALLDIGIPAR_H
#define HWALLDIGIPAR_H

#include "hparcond.h"

#define WALL_MAXCELLS 302

class HWallDigiPar : public HParCond {
protected:
  Float_t TDC_slope[WALL_MAXCELLS];
  Float_t TDC_offset[WALL_MAXCELLS];
  Float_t ADC_slope[WALL_MAXCELLS];
  Float_t ADC_offset[WALL_MAXCELLS];
  Float_t CFD_threshold[WALL_MAXCELLS];
  Float_t ADC_threshold[WALL_MAXCELLS];
public:
  HWallDigiPar(const Char_t* name="WallDigiPar",
               const Char_t* title="Digitization parameters for Forward Wall",
               const Char_t* context="WallDigiProduction");
  ~HWallDigiPar() {;}
  Float_t getTDC_Slope(Int_t cell)     { return TDC_slope[cell]; }
  Float_t getTDC_Offset(Int_t cell)    { return TDC_offset[cell]; }
  Float_t getADC_Slope(Int_t cell)     { return ADC_slope[cell]; }
  Float_t getADC_Offset(Int_t cell)    { return ADC_offset[cell]; }
  Float_t getCFD_Threshold(Int_t cell) { return CFD_threshold[cell]; }
  Float_t getADC_Threshold(Int_t cell) { return ADC_threshold[cell]; }

  void fill(Int_t cell,Float_t Ts,Float_t To,Float_t As,Float_t Ao,Float_t Ct,Float_t At) {
    TDC_slope[cell]     = Ts;
    TDC_offset[cell]    = To;
    ADC_slope[cell]     = As;
    ADC_offset[cell]    = Ao;
    CFD_threshold[cell] = Ct;
    ADC_threshold[cell] = At;
  }
  Int_t  getSize() { return WALL_MAXCELLS; }
  void   clear();
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  ClassDef(HWallDigiPar,1) // Container for the WALL digitisation parameters

};

#endif  /*!HWALLDIGIPAR_H*/
