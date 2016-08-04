#ifndef HSTART2CALPAR_H
#define HSTART2CALPAR_H

#include "TObjArray.h"
#include "TObject.h"

#include "hparset.h"

class HStart2CalparCell : public TObject {
protected:
   Float_t tdcSlope;  // TDC slope
   Float_t tdcOffset; // TDC offset
   Float_t adcSlope;  // ADC slope
   Float_t adcOffset; // ADC offset
public:
   HStart2CalparCell() {
      clear();
   }
   ~HStart2CalparCell() {
      ;
   }
   void clear() {
      tdcSlope = 1.F;
      tdcOffset = 0.F;
      adcSlope = 1.F;
      adcOffset = 0.F;
   }
   Float_t getTdcSlope(void)  {
      return tdcSlope;
   }
   Float_t getTdcOffset(void) {
      return tdcOffset;
   }
   Float_t getAdcSlope(void)  {
      return adcSlope;
   }
   Float_t getAdcOffset(void) {
      return adcOffset;
   }
   void getData(Float_t* data) {
      data[0] = tdcSlope;
      data[1] = tdcOffset;
      data[2] = adcSlope;
      data[3] = adcOffset;
   }
   void setTdcSlope(Float_t s)  {
      tdcSlope = s;
   }
   void setTdcOffset(Float_t o) {
      tdcOffset = o;
   }
   void setAdcSlope(Float_t s)  {
      adcSlope = s;
   }
   void setAdcOffset(Float_t o) {
      adcOffset = o;
   }
   void fill(Float_t ts, Float_t to, Float_t as, Float_t ao) {
      tdcSlope = ts;
      tdcOffset = to;
      adcSlope = as;
      adcOffset = ao;
   }
   void fill(Float_t* data) {
      tdcSlope = data[0];
      tdcOffset = data[1];
      adcSlope = data[2];
      adcOffset = data[3];
   }
   void fill(HStart2CalparCell& r) {
      tdcSlope  = r.getTdcSlope();
      tdcOffset = r.getTdcOffset();
      adcSlope  = r.getAdcSlope();
      adcOffset = r.getAdcOffset();
   }
   ClassDef(HStart2CalparCell, 1) // Chan level of the START2 calibration parameters
};


class HStart2CalparMod: public TObject {
protected:
   TObjArray* array;  // pointer array containing HStart2CalparCell objects
public:
   HStart2CalparMod(Int_t n = 128);
   ~HStart2CalparMod();
   HStart2CalparCell& operator[](Int_t i) {
      return *static_cast<HStart2CalparCell*>((*array)[i]);
   }
   Int_t getSize() {
      return array->GetEntries();
   }
   ClassDef(HStart2CalparMod, 1) // Module level of Start2 Calibration parameters
};


class HStart2Calpar : public HParSet {
protected:
   TObjArray* array;     // array of pointers of type HStart2CalparMod
public:
   HStart2Calpar(const Char_t* name = "Start2Calpar",
                 const Char_t* title = "Calibration parameters for Start2",
                 const Char_t* context = "Start2CalparProduction");
   ~HStart2Calpar();
   HStart2CalparMod& operator[](Int_t i) {
      return *static_cast<HStart2CalparMod*>((*array)[i]);
   }
   Int_t getSize() {
      return array->GetEntries();
   }
   Bool_t init(HParIo* input, Int_t* set);
   Int_t write(HParIo* output);
   void clear();
   void printParam();
   void readline(const Char_t*, Int_t*);
   void putAsciiHeader(TString&);
   Bool_t writeline(Char_t*, Int_t, Int_t);
   ClassDef(HStart2Calpar, 1) // Container for the START2 calibration parameters
};

#endif  /*!HSTART2CALPAR_H*/
