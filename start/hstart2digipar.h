#ifndef HSTART2DIGIPAR_H
#define HSTART2DIGIPAR_H

#include "hparcond.h"

class HStart2DigiPar : public HParCond {
protected:
  Float_t sigmaT;       // time resolution [ns]
  Float_t sigmaE;       // energy resolution [%]
  Float_t deltaTimeE;   // time window to integrate energy
  Float_t minEloss;     // minimum energy loss
  Float_t timeNoHit;    // default time set in HStart2Hit if no hit is detecteted
public:
  HStart2DigiPar(const char* name="Start2DigiPar",
                 const char* title="Start2 digitization parameters",
                 const char* context="Start2DigiProduction");
  ~HStart2DigiPar() {;}

  Float_t getSigmaT()       const {return sigmaT;}
  Float_t getSigmaE()       const {return sigmaE;}
  Float_t getDeltaTimeE()   const {return deltaTimeE;}
  Float_t getMinEloss()     const {return minEloss;}
  Float_t getTimeNoHit()    const {return timeNoHit;}

  void   clear(void);
  void   fill(Float_t, Float_t, Float_t, Float_t, Float_t);
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  ClassDef(HStart2DigiPar,1) // Container for the START digitization parameters
};

#endif /* !HSTART2DIGIPAR_H */
  
