#ifndef HEMCCALPAR_H
#define HEMCCALPAR_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"

class HEmcCalParCell : public TObject {
protected:
  Float_t tdcSlope;  // TDC slope
  Float_t tdcOffset; // TDC offset
  Float_t adcSlope;  // ADC slope
  Float_t adcOffset; // ADC offset
  Float_t twcPar1;   // TWC par 1
  Float_t twcPar2;   // TWC par 2
  Float_t twcPar3;   // TWC par 3
public:
  HEmcCalParCell()  {clear();}
  virtual ~HEmcCalParCell() {}
  void clear();
  Float_t getTdcSlope(void)  {return tdcSlope;}
  Float_t getTdcOffset(void) {return tdcOffset;}
  Float_t getAdcSlope(void)  {return adcSlope;}
  Float_t getAdcOffset(void) {return adcOffset;}
  Float_t getTwcPar1(void)   {return twcPar1;}
  Float_t getTwcPar2(void)   {return twcPar2;}
  Float_t getTwcPar3(void)   {return twcPar3;}
  void getData(Float_t* data) {
    data[0] = tdcSlope;
    data[1] = tdcOffset;
    data[2] = adcSlope;
    data[3] = adcOffset;
    data[4] = twcPar1;
    data[5] = twcPar2;
    data[6] = twcPar3;
  }
  void setTdcSlope(Float_t s)  {tdcSlope = s;}
  void setTdcOffset(Float_t o) {tdcOffset = o;}
  void setAdcSlope(Float_t s)  {adcSlope = s;}
  void setAdcOffset(Float_t o) {adcOffset = o;}
  void setTwcPar1(Float_t t)   {twcPar1 = t;}
  void setTwcPar2(Float_t t)   {twcPar2 = t;}
  void setTwcPar3(Float_t t)   {twcPar3 = t;}
  void fill(Float_t, Float_t, Float_t, Float_t, Float_t, Float_t, Float_t);
  void fill(Float_t*);
  void fill(HEmcCalParCell&);
  ClassDef(HEmcCalParCell, 1) // Chan level of the Emc calibration parameters
};


class HEmcCalParSec: public TObject {
protected:
  TObjArray* array;  // pointer array containing HEmcCalParCell objects
public:
  HEmcCalParSec(Int_t n = 255);
  virtual ~HEmcCalParSec();
  HEmcCalParCell& operator[](Int_t i) {
    return *static_cast<HEmcCalParCell*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HEmcCalParSec, 1) // Sector level of Emc calibration parameters
};


class HEmcCalPar : public HParSet {
protected:
  TObjArray* array;     // array of pointers of type HEmcCalParSec
public:
  HEmcCalPar(const Char_t* name,// = "EmcCalPar",
             const Char_t* title,// = "Calibration parameters of the Emc",
             const Char_t* context) ;// = "EmcCalParProduction");

  HEmcCalPar() : array(0) {;} // for streamer? see segv in root file io JAM
  virtual ~HEmcCalPar();

  HEmcCalParSec& operator[](Int_t i) {
    return *static_cast<HEmcCalParSec*>((*array)[i]);
  }

  Int_t getSize() {return array->GetEntries();}
  Bool_t init(HParIo* input, Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParams();
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  void write(fstream&);
  ClassDef(HEmcCalPar, 1) // Container for the calibration parameters of the Emc
};

#endif  /*!HEMCCALPAR_H*/
