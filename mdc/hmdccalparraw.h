#ifndef HMDCCALPARRAW_H
#define HMDCCALPARRAW_H

#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
#include "hparset.h"

class HMdcCalParTdc : public TObject {
protected:
    Float_t slope;       // gain
    Float_t offset;      // offset
    Float_t slopeErr;    // error of gain
    Float_t offsetErr;   // error of offset
    Int_t slopeMethod;   // number of method used to determine the slope
    Int_t offsetMethod;  // number of method used to determine the offset
public:
    HMdcCalParTdc() : slope(1.F), offset(0.F), slopeErr(0.F), offsetErr(0.F),
                      slopeMethod(0), offsetMethod(0) {;}
    ~HMdcCalParTdc() {;}
    Float_t getSlope()  {return slope;}
    Float_t getOffset() {return offset;}
    Float_t getSlopeErr() {return slopeErr;}
    Float_t getOffsetErr() {return offsetErr;}
    Int_t getSlopeMethod() {return slopeMethod;}
    Int_t getOffsetMethod() {return offsetMethod;}
    void fill(Float_t s,Float_t o, Float_t sE,Float_t oE,Int_t sM,Int_t oM)
    {
      slope=s;
      offset=o;
      slopeErr=sE;
      offsetErr=oE;
      slopeMethod=sM;
      offsetMethod=oM;
    }
    void fill(HMdcCalParTdc&);
    void setSlope(Float_t s)  {slope=s;}
    void setOffset(Float_t o) {offset=o;}
    void setSlopeErr(Float_t sE){slopeErr=sE;}
    void setOffsetErr(Float_t oE){offsetErr=oE;}
    void setSlopeMethod(Int_t sM) {slopeMethod=sM;}
    void setOffsetMethod(Int_t oM) {offsetMethod=oM;}
    void clear() {
      slope       =1.F;
      offset      =0.F;
      slopeErr    =0.F;
      offsetErr   =0.F;
      slopeMethod =0;
      offsetMethod=0;
    }
    ClassDef(HMdcCalParTdc,1) // Tdc level of the MDC calibration parameters
};


class HMdcCalParMbo : public TNamed {
protected:
    TObjArray *array; // array of pointers of type HMdcCalParTdc
public:
    HMdcCalParMbo(Int_t tdc = 96, const Text_t* name="");
    ~HMdcCalParMbo();
    HMdcCalParTdc& operator[](Int_t i) {
      return *static_cast<HMdcCalParTdc*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcCalParMbo,1) // Mbo level of the MDC calibration parameters
};


class HMdcCalParRawMod : public TObject {
protected:
    TObjArray *array;   // array of pointers of type HMdcCalParMbo
public:
    HMdcCalParRawMod(Int_t mbo = 16);
    ~HMdcCalParRawMod();
    HMdcCalParMbo& operator[](Int_t i) {
      return *static_cast<HMdcCalParMbo*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    void createMbo(Int_t, Int_t, const Char_t*);
    ClassDef(HMdcCalParRawMod,1) // Module level of the MDC calibration parameters
};


class HMdcCalParRawSec : public TObject {
protected:
    TObjArray* array;   // array of pointers of type HMdcCalParRawMod

public:
    HMdcCalParRawSec(Int_t mod = 4);
    ~HMdcCalParRawSec();
    HMdcCalParRawMod& operator[](Int_t i) {
      return *static_cast<HMdcCalParRawMod*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcCalParRawSec,1) // Sector level of the MDC calibration parameters
};


class HMdcCalParRaw : public HParSet {
protected:
    TObjArray* array;      // array of pointers of type HMdcCalParRawSec
public:
    HMdcCalParRaw(const Char_t* name="MdcCalParRaw",
                  const Char_t* title="raw calibration parameters for Mdc",
                  const Char_t* context="MdcCalParRawProduction",
                  Int_t n=6);
    ~HMdcCalParRaw();
    HMdcCalParRawSec& operator[](Int_t i) {
        return *static_cast<HMdcCalParRawSec*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    Bool_t init(HParIo*, Int_t*);
    Int_t write(HParIo*);
    void readline(const Char_t*, Int_t*);
    void putAsciiHeader(TString&);
    Bool_t writeline(Char_t*, Int_t, Int_t, Int_t, Int_t);
    void clear();
    void printParam();
    ClassDef(HMdcCalParRaw,2) // Container for the MDC calibration parameters on raw level
};

#endif  /*!HMDCCALPARRAW_H*/

