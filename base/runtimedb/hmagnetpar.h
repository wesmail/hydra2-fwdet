#ifndef HMAGNETPAR_H
#define HMAGNETPAR_H

#include "hparcond.h"
#include "TNamed.h"

class HMagnetPar :  public HParCond {
protected:
  Int_t current;    // mean current[A]
  Int_t maxCurrent; // maximum current (full field)
public:
  HMagnetPar(const Char_t* name="MagnetPar",
             const Char_t* title="Magnet field and polarity",
             const Char_t* context="MagnetCurrentSetValues");
  ~HMagnetPar(void) {}
  Int_t getCurrent(void) {return current;}
  Int_t getPolarity(void) {return ((current>=0) ? 1 : -1);}
  Int_t getMaximumCurrent(void) {return maxCurrent;}
  Float_t getScalingFactor(void);
  void setCurrent(Int_t c) {current=c;}
  Bool_t init(HParIo*,Int_t* set=0);
  Int_t write(HParIo*);
  void clear(void);
  void putParams(HParamList*);
  Bool_t getParams(HParamList*);
  ClassDef(HMagnetPar,1) // parameter container for magnet current
};

#endif /* !HMAGNETPAR_H */
