#ifndef HEMCDIGIPAR_H
#define HEMCDIGIPAR_H

#include "hparcond.h"

class HEmcDigiPar : public HParCond {
protected:
  Float_t sigmaT;       // time resolution [ns]
  Float_t phot2E;       // mean energy deposition in 1.5" PMT per photon electron (1000./1306.) in MeV
  Float_t phot2E2;      // mean energy deposition in 3" PMT per photon electron (1000./1306.) in MeV
  Float_t sigmaEIntern; // internal energy resolution
  Float_t sigmaEReal;   // measured energy resolution in 1.5" PMT
  Float_t sigmaEReal2;  // measured energy resolution in 3" PMT

public:
  HEmcDigiPar(const char* name="EmcDigiPar",
              const char* title="Emc digitization parameters",
              const char* context="EmcDigiProduction");
  ~HEmcDigiPar() {;}
  Float_t getSigmaT()       const {return sigmaT;}
  Float_t getPhot2E()       const {return phot2E;}
  Float_t getPhot2E2()      const {return phot2E2;}
  Float_t getSigmaEIntern() const {return sigmaEIntern;}
  Float_t getSigmaEReal()   const {return sigmaEReal;}
  Float_t getSigmaEReal2()  const {return sigmaEReal2;}

  void   clear(void);
//  void   fill(Float_t, Float_t, Float_t, Float_t, Float_t, Float_t);
  void   fill(Float_t sT, Float_t p2e, Float_t p2e2, Float_t seI, Float_t seR, Float_t seR2);
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  ClassDef(HEmcDigiPar,1) // Container for the EMC digitization parameters
};

#endif /* !HEMCDIGIPAR_H */
  
