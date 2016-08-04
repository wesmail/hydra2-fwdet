#ifndef HWALLCALPAR_H
#define HWALLCALPAR_H

using namespace std;
#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include <iostream>
#include <iomanip>

class HWallCalParCell : public TObject {
protected:
    Float_t TDC_slope;
    Float_t TDC_offset;
    Float_t TDC_WalkCorr1;
    Float_t TDC_WalkCorr2;
    Float_t ADC_slope;
    Float_t ADC_offset;

public:
    HWallCalParCell() { clear(); }
    ~HWallCalParCell() {;}
    Float_t getTDC_Slope()  { return TDC_slope; }
    Float_t getTDC_Offset() { return TDC_offset; }
    Float_t getTDC_WalkCorr1() { return TDC_WalkCorr1; }
    Float_t getTDC_WalkCorr2() { return TDC_WalkCorr2; }
    Float_t getADC_Slope()  { return ADC_slope; }
    Float_t getADC_Offset() { return ADC_offset; }
    void fill(Float_t Ts,Float_t To,Float_t Twc1,Float_t Twc2,Float_t As,Float_t Ao) {
      TDC_slope =Ts;
      TDC_offset=To;
      TDC_WalkCorr1=Twc1;
      TDC_WalkCorr2=Twc2;
      ADC_slope =As;
      ADC_offset=Ao;
    }
    void fill(HWallCalParCell& r) {
      TDC_slope =r.getTDC_Slope();
      TDC_offset=r.getTDC_Offset();
      TDC_WalkCorr1=r.getTDC_WalkCorr1();
      TDC_WalkCorr2=r.getTDC_WalkCorr2();
      ADC_slope =r.getADC_Slope();
      ADC_offset=r.getADC_Offset();
    }
    void setTDC_Slope(Float_t Ts)  { TDC_slope =Ts; }
    void setTDC_Offset(Float_t To) { TDC_offset=To; }
    void setTDC_WalkCorr1(Float_t Twc1) { TDC_WalkCorr1=Twc1; }
    void setTDC_WalkCorr2(Float_t Twc2) { TDC_WalkCorr2=Twc2; }
    void setADC_Slope(Float_t As)  { ADC_slope =As; }
    void setADC_Offset(Float_t Ao) { ADC_offset=Ao; }

    void clear() {
      TDC_slope =0.F;
      TDC_offset=0.F;
      TDC_WalkCorr1=0.F;
      TDC_WalkCorr2=0.F;
      ADC_slope =0.F;
      ADC_offset=0.F;
    }
    ClassDef(HWallCalParCell,1) // Chan level of the Forward Wall calibration parameters 
};

class HWallCalPar : public HParSet {
protected:
  TObjArray* array;     // array of pointers of type HWallCalParCell
public:
  HWallCalPar(const Char_t* name="WallCalPar",
               const Char_t* title="Calibration parameters for Forward Wall",
               const Char_t* context="WallCalProduction");
  ~HWallCalPar();
  HWallCalParCell& operator[](Int_t i) {
    return *static_cast<HWallCalParCell*>((*array)[i]);
  }
  Int_t getSize() { return array->GetEntries(); }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParam();
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t);
  ClassDef(HWallCalPar,1) // Container for the WALL calibration parameters
};

#endif  /*!HWALLCALPAR_H*/









