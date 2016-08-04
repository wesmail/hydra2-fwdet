#ifndef HTOFDIGIPAR_H
#define HTOFDIGIPAR_H

#include "TObject.h"
#include "TObjArray.h"
#include "hlocation.h"
#include "hparset.h"

class HTofDigiParCell : public TObject {

private:
  Float_t halfLen;       // half length of the strip (mm)
  Float_t angleRef;      // total internal reflection angle (deg)
  Float_t attenLen;      // attenuation length (mm)
  Float_t groupVel;      // group velocity (mm/ns)
  Float_t slopeLeftTDC;  // left TDC slope (ns/ch)
  Float_t slopeRightTDC; // right TDC slope (ns/ch)
  Int_t thrLeftCFD;      // left CFD threshold (ch)
  Int_t thrRightCFD;     // right CFD threshold (ch)
  Int_t thrLeftADC;      // left ADC threshold (ch)
  Int_t thrRightADC;     // right ADC threshold (ch)

public:
  HTofDigiParCell(void) {clear();}
  ~HTofDigiParCell(void) {}
  Float_t getHalfLen() { return halfLen; }
  Float_t getAngleRef() { return angleRef; }
  Float_t getAttenLen() { return attenLen; }
  Float_t getGroupVel() { return groupVel; }
  Float_t getLeftTDCSlope() {return slopeLeftTDC; }
  Float_t getRightTDCSlope() {return slopeRightTDC; }
  Int_t getLeftCFDThreshold() { return thrLeftCFD; }
  Int_t getRightCFDThreshold() { return thrRightCFD; }
  Int_t getLeftADCThreshold() { return thrLeftADC; }
  Int_t getRightADCThreshold() { return thrRightADC; }
  void fill(HTofDigiParCell& c) {
    halfLen=c.getHalfLen();
    angleRef=c.getAngleRef();
    attenLen=c.getAttenLen();
    groupVel=c.getGroupVel();
    slopeLeftTDC=c.getLeftTDCSlope();
    slopeRightTDC=c.getRightTDCSlope();
    thrLeftCFD=c.getLeftCFDThreshold();
    thrRightCFD=c.getRightCFDThreshold();
    thrLeftADC=c.getLeftADCThreshold();
    thrRightADC=c.getRightADCThreshold();
  }
  void setHalfLen(Float_t f) { halfLen = f; }
  void setAngleRef(Float_t f) { angleRef = f; }
  void setAttenLen(Float_t f) { attenLen = f; }
  void setGroupVel(Float_t f) { groupVel = f; }
  void setLeftTDCSlope(Float_t f) {slopeLeftTDC = f; }
  void setRightTDCSlope(Float_t f) {slopeRightTDC = f; }
  void setLeftCFDThreshold(Int_t f) { thrLeftCFD = f; }
  void setRightCFDThreshold(Int_t f) { thrRightCFD = f; }
  void setLeftADCThreshold(Int_t f) { thrLeftADC = f; }
  void setRightADCThreshold(Int_t f) { thrRightADC = f; }
  void setDefaults(Int_t f);
  void clear();
  ClassDef(HTofDigiParCell,1)  // Cell level of the TOF digitization parameters
};

class HTofDigiParMod: public TObject {
private:
  TObjArray* array;  // pointer array containing HTofDigiParCell objects
public:
  HTofDigiParMod(Int_t s=0, Int_t m=0);
  ~HTofDigiParMod();
  HTofDigiParCell& operator[](Int_t i) {
      return *static_cast<HTofDigiParCell*>((*array)[i]);
  }
  Int_t getSize() { return array->GetEntries(); }
  ClassDef(HTofDigiParMod,1) // Module level of the Tof digitization parameters
};

class HTofDigiParSec: public TObject {
private:
  TObjArray* array;  // pointer array containing HTofDigiParMod objects
public:
  HTofDigiParSec(Int_t s=0, Int_t n=22);
  ~HTofDigiParSec();
  HTofDigiParMod& operator[](Int_t i) {
      return *static_cast<HTofDigiParMod*>((*array)[i]);
  }
  Int_t getSize() { return array->GetEntries(); }
  ClassDef(HTofDigiParSec,1) // Sector level of the Tof digitizaton parameters
};

class HTofDigiPar : public HParSet {
private:
  TObjArray* array;     // array of pointers of type HTofDigiParMod
  void printPause(void);
public:
  HTofDigiPar(const Char_t* name="TofDigiPar",
              const Char_t* title="TOF digitization parameters",
              const Char_t* context="TofDigiProduction");
  ~HTofDigiPar(void);
  HTofDigiParSec& operator[](Int_t i) {
    return *static_cast<HTofDigiParSec*>((*array)[i]);
  }
  Int_t getSize() { return array->GetEntries(); }
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear();
  void printParam();
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t, Int_t, Int_t);
  ClassDef(HTofDigiPar,1)  // Container for the TOF digitization parameters
};

#endif /* !HTOFDIGIPAR_H */
