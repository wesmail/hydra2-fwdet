#ifndef HEMCDIGIPAR_H
#define HEMCDIGIPAR_H

#include "hparcond.h"

class HEmcDigiPar : public HParCond {
protected:
  Float_t sigmaT;       // time resolution [ns]
  Float_t phot2E;       // mean energy deposition per photon electron (1000./1306.) in MeV
  Float_t sigmaEIntern; // internal energy resolution
  Float_t sigmaEReal;   // measured energy resolution

public:
  HEmcDigiPar(const char* name="EmcDigiPar",
              const char* title="Emc digitization parameters",
              const char* context="EmcDigiProduction");
  ~HEmcDigiPar() {;}
  Float_t getSigmaT()       const {return sigmaT;}
  Float_t getPhot2E()       const {return phot2E;}
  Float_t getSigmaEIntern() const {return sigmaEIntern;}
  Float_t getSigmaEReal()   const {return sigmaEReal;}

  void   clear(void);
  void   fill(Float_t, Float_t, Float_t, Float_t);
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  ClassDef(HEmcDigiPar,1) // Container for the EMC digitization parameters
};

#endif /* !HEMCDIGIPAR_H */
  
