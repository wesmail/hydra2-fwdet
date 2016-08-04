#ifndef HMDCLAYERCORRPAR_H
#define HMDCLAYERCORRPAR_H

#include "hparcond.h"

class HMdcLayerCorrPar : public HParCond {
protected:
  Int_t   firstWire[72];      // first cell in shifted part of layer
  Float_t shift[72];          // shift value (in mkm!)
  Float_t wireOrientCorr[72]; // correction for wire orientation (in deg!)
  Int_t   nMdc;
  Int_t   nLay;
  Int_t   numCorrLayers;
  
public:
  HMdcLayerCorrPar(const Char_t* name    = "MdcLayerCorrPar",
                   const Char_t* title   = "parameters for Mdc layer correction",
                   const Char_t* context = "MdcLayerCorrProduction");
  ~HMdcLayerCorrPar(void) {}
  
  Bool_t  init(HParIo*,Int_t* set=0);
  Int_t   write(HParIo*);
  void    clear(void);
  void    putParams(HParamList*);
  Bool_t  getParams(HParamList*);
  
  Bool_t  addLayerShift(Int_t s,Int_t m,Int_t l,Int_t fstWr,Float_t sh,Float_t orCorr=0.);
  void    setDefaultPar(void);

  Bool_t  getLayerCorrPar(Int_t s,Int_t m,Int_t l,Int_t& fstWr,Float_t& sh,Float_t& orCorr) const;

private:
  Bool_t isSecOk(Int_t s) const                 {return s>=0 && s<6;}
  Bool_t isModOk(Int_t m) const                 {return m>=2 && m<4;}
  Bool_t isLayOk(Int_t l) const                 {return l>=0 && l<6;}
  Bool_t isAddOk(Int_t s,Int_t m,Int_t l) const {return isSecOk(s) && isModOk(m) && isLayOk(l);}

  ClassDef(HMdcLayerCorrPar,1) // Parameter container layer correction
};

#endif  /*!HMDCLAYERCORRPAR_H */









