#ifndef HENERGYLOSSCORRPAR_H
#define HENERGYLOSSCORRPAR_H

#include "hparcond.h"

class HEnergyLossCorrPar : public HParCond {
protected:
// for protons
  Int_t    nThetaReg;            // number of theta regions 
  Double_t thetaStep;            // size of one theta region, degrees
  Double_t thetaMiddle0;         // middle of first theta region, degrees
  Double_t momInt1;              // end of first momentum interval, MeV
  Double_t momInt2;              // end of second momentum interval, MeV
  Int_t    nParams;              // number of parameters for one theta region
  Double_t parMomCorrH[264];     // parameters. Size=nThetaReg*nParams
  Short_t  typePar;              // type of parametrisation 

//for electrons  
  Int_t    nParamsElect;         // number of parameters
  Double_t parMomCorrElect[7];   // parameters
  
  static   HEnergyLossCorrPar* gEnergyLossCorrPar;
  
public:
  HEnergyLossCorrPar(const Char_t* name    = "EnergyLossCorrPar",
                     const Char_t* title   = "Energy losses correction parameters",
                     const Char_t* context = "EnergyLossCorrProduction");
  ~HEnergyLossCorrPar(void) {}
  static HEnergyLossCorrPar* getObject(void) {return gEnergyLossCorrPar;}
  
  void     clear(void);
  void     putParams(HParamList*);
  Bool_t   getParams(HParamList*);

  Bool_t   setDefaultPar(TString run);
  Double_t getDeltaMom(Int_t pId, Double_t mom, Double_t theta) const; // units: MeV for mom, degrees for theta
  Double_t getCorrMom(Int_t pId, Double_t mom, Double_t theta) const {return mom+getDeltaMom(pId,mom,theta);}

private:
  void     fillParMomCorrH(Int_t size,Double_t *par);
  Double_t getDeltaMomT1(Int_t pId, Double_t mom, Double_t theta) const;
  Double_t getDeltaMomT3(Int_t pId, Double_t mom, Double_t theta) const;
  Double_t deltaMomT2(Double_t b,Double_t rs, const Double_t *par) const;
    
  ClassDef(HEnergyLossCorrPar,1) // Parameter container for energy loss correction
};

#endif  /*!HENERGYLOSSCORRPAR_H */









