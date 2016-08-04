#ifndef HTOFCALPAR_H
#define HTOFCALPAR_H

#include "TObject.h"
#include "TObjArray.h"
#include "hlocation.h"
#include "hparset.h"

class HTofCalParCell : public TObject {
private:
  Float_t leftK;      // left side TDC width channel
  Float_t rightK;     // right side TDC width channel
  Float_t vGroup;     // group velocity
  Float_t posK;       // position offset
  Float_t timK;       // timing offset
  Float_t pedestalL;  // ADC left pedestal
  Float_t pedestalR;  // ADC right pedestal
  Float_t attLen;     // Attenuation length
  Float_t gainAsym;   // Gain asymmetry
  Float_t edepK;      // amplitude to ADC channel conversion factor
  Float_t twC1;       // time walk correction (left)
  Float_t twC2;       // time walk correction (right)
  Float_t twC3;       // time walk correction of time offset
public:
  HTofCalParCell(void) { setDefaults(); }
  ~HTofCalParCell(void) {}
  Float_t getLeftK() { return leftK; }
  Float_t getRightK() { return rightK; }
  Float_t getVGroup() { return vGroup; }
  Float_t getPosK() { return posK; }
  Float_t getTimK() { return timK; }
  Float_t getPedestalL() { return pedestalL; }
  Float_t getPedestalR() { return pedestalR; }
  Float_t getAttLen() { return attLen; }
  Float_t getGainAsym() { return gainAsym; }
  Float_t getEdepK() { return edepK; }
  Float_t getTimeWalkC1() { return twC1; }
  Float_t getTimeWalkC2() { return twC2; }
  Float_t getTimeWalkC3() { return twC3; }
  void fill(HTofCalParCell& c) {
    leftK=c.getLeftK();
    rightK=c.getRightK();
    vGroup=c.getVGroup();
    posK=c.getPosK();
    timK=c.getTimK();
    pedestalL=c.getPedestalL();
    pedestalR=c.getPedestalR();
    attLen=c.getAttLen();
    gainAsym=c.getGainAsym();
    edepK=c.getEdepK();
    twC1=c.getTimeWalkC1();
    twC2=c.getTimeWalkC2();
    twC3=c.getTimeWalkC3();
  }
  void setLeftK(Float_t f) { leftK = f; }
  void setRightK(Float_t f) { rightK = f; }
  void setVGroup(Float_t f) { vGroup = f; }
  void setPosK(Float_t f) { posK = f; }
  void setTimK(Float_t f) { timK = f; }
  void setPedestalL(Float_t f) { pedestalL=f; }
  void setPedestalR(Float_t f) { pedestalR=f; }
  void setAttLen(Float_t f) { attLen=f; }
  void setGainAsym(Float_t f) { gainAsym=f; }
  void setEdepK(Float_t f) { edepK=f; }
  void setTimeWalkC1(Float_t f) {twC1=f; }
  void setTimeWalkC2(Float_t f) {twC2=f; }
  void setTimeWalkC3(Float_t f) {twC3=f; }
  void setDefaults(void);
  ClassDef(HTofCalParCell,3)  // Cell level of the TOF calibration parameters
}; 

class HTofCalParMod: public TObject {
private:
  TObjArray* array;  // pointer array containing HStartCalParChan objects
public:
  HTofCalParMod(Int_t s=0, Int_t m=0);
  ~HTofCalParMod();
  HTofCalParCell& operator[](Int_t i) {
      return *static_cast<HTofCalParCell*>((*array)[i]);
  }
  Int_t getSize() { return array->GetEntries(); }
  ClassDef(HTofCalParMod,1) // Module level of the Tof Calibration parameters
};

class HTofCalParSec: public TObject {
private:
  TObjArray* array;  // pointer array containing HStartCalParMod objects
public:
  HTofCalParSec(Int_t s=0, Int_t n=22);
  ~HTofCalParSec();
  HTofCalParMod& operator[](Int_t i) {
      return *static_cast<HTofCalParMod*>((*array)[i]);
  }
  Int_t getSize() { return array->GetEntries(); }
  ClassDef(HTofCalParSec,1) // Sector level of the Tof Calibration parameters
};

class HTofCalPar : public HParSet {
private:
  TObjArray* array;     // array of pointers of type HTofCalParMod
  void printPause(void);
public:
  HTofCalPar(const Char_t* name="TofCalPar",
             const Char_t* title="Calibration parameters for Tof",
             const Char_t* context="TofCalProduction");
  ~HTofCalPar(void);
  HTofCalParSec& operator[](Int_t i) {
    return *static_cast<HTofCalParSec*>((*array)[i]);
  }
  Int_t getSize() { return array->GetEntries(); }
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear();
  void printParam();
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t, Int_t, Int_t);
  ClassDef(HTofCalPar,1)  // Container for the TOF calibration parameters
};

#endif /* !HTOFCALPAR_H */










