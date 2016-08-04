#ifndef HPIONTRACKERCALPAR_H
#define HPIONTRACKERCALPAR_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"

class HPionTrackerCalParCell : public TObject {
protected:
  Float_t tdcSlope;  // TDC slope
  Float_t tdcOffset; // TDC offset
  Float_t adcSlope;  // ADC slope
  Float_t adcOffset; // ADC offset
public:
  HPionTrackerCalParCell()  {clear();}
  ~HPionTrackerCalParCell() {}
  void clear();
  Float_t getTdcSlope(void)  {return tdcSlope;}
  Float_t getTdcOffset(void) {return tdcOffset;}
  Float_t getAdcSlope(void)  {return adcSlope;}
  Float_t getAdcOffset(void) {return adcOffset;}
  void getData(Float_t* data) {
    data[0] = tdcSlope;
    data[1] = tdcOffset;
    data[2] = adcSlope;
    data[3] = adcOffset;
  }
  void setTdcSlope(Float_t s)  {tdcSlope = s;}
  void setTdcOffset(Float_t o) {tdcOffset = o;}
  void setAdcSlope(Float_t s)  {adcSlope = s;}
  void setAdcOffset(Float_t o) {adcOffset = o;}
  void fill(Float_t, Float_t, Float_t, Float_t);
  void fill(Float_t*);
  void fill(HPionTrackerCalParCell&);
   ClassDef(HPionTrackerCalParCell, 1) // Chan level of the PionTracker calibration parameters
};


class HPionTrackerCalParMod: public TObject {
protected:
  TObjArray* array;  // pointer array containing HPionTrackerCalParCell objects
public:
  HPionTrackerCalParMod(Int_t n = 128);
  ~HPionTrackerCalParMod();
  HPionTrackerCalParCell& operator[](Int_t i) {
     return *static_cast<HPionTrackerCalParCell*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HPionTrackerCalParMod, 1) // Module level of PionTracker Calibration parameters
};


class HPionTrackerCalPar : public HParSet {
protected:
  TObjArray* array;     // array of pointers of type HPionTrackerCalParMod
public:
  HPionTrackerCalPar(const Char_t* name = "PionTrackerCalPar",
                     const Char_t* title = "Calibration parameters of the PionTracker",
                     const Char_t* context = "PionTrackerCalParProduction");
  ~HPionTrackerCalPar();

  HPionTrackerCalParMod& operator[](Int_t i) {
    return *static_cast<HPionTrackerCalParMod*>((*array)[i]);
  }

  Int_t getSize() {return array->GetEntries();}
  Bool_t init(HParIo* input, Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParams();
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  void write(fstream&);
  ClassDef(HPionTrackerCalPar, 1) // Container for the calibration parameters of the PionTracker
};

#endif  /*!HPIONTRACKERCALPAR_H*/
