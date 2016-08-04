#ifndef HMDCDIGITPAR_H
#define HMDCDIGITPAR_H
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;

class HMdcDigitPar : public HParCond {
protected:
    TArrayF layEff;        // efficiency per module and layer
    TArrayF layEffScale;       // efficiency per module and layer scaling with dedx
    TArrayF layEffThickness;   // efficiency per module and layer scaling with 1/cos(theta)
    TArrayF scaleTime1Err;     // input scaling of t1err for MAPS
    TArrayF scaleTime1ErrMIPS; // input scaling of t1err for MIPS
    Float_t cellEffScale;// cell efficiency scaling with dedx
    Float_t signalspeed; // speed of signal on the wire (ns/mm)
public:
    HMdcDigitPar(const Char_t* name ="MdcDigitPar",
	      const Char_t* title  ="digitisation parameters for Mdc",
	      const Char_t* context="MdcDigitParProduction");
    ~HMdcDigitPar();
    Float_t getLayerEfficiency(Int_t s,Int_t m,Int_t l)             {return layEff[(s*24)+(m*6)+l];}
    void    setLayerEfficiency(Int_t s,Int_t m,Int_t l,Float_t eff) {layEff[(s*24)+(m*6)+l]=eff;}
    Float_t getLayerEfficiencyScale(Int_t s,Int_t m,Int_t l)             {return layEffScale[(s*24)+(m*6)+l];}
    void    setLayerEfficiencyThickness(Int_t s,Int_t m,Int_t l,Float_t eff) {layEffThickness[(s*24)+(m*6)+l]=eff;}
    Float_t getLayerEfficiencyThickness(Int_t s,Int_t m,Int_t l)             {return layEffThickness[(s*24)+(m*6)+l];}
    void    setLayerEfficiencyScale(Int_t s,Int_t m,Int_t l,Float_t eff) {layEffScale[(s*24)+(m*6)+l]=eff;}
    Float_t getTime1ErrScale(Int_t m)                                    {return scaleTime1Err[m];}
    void    setTime1ErrScale(Int_t m,Float_t scale)                      {scaleTime1Err[m] = scale;}
    Float_t getTime1ErrScaleMIPS(Int_t m)                                {return scaleTime1ErrMIPS[m];}
    void    setTime1ErrScaleMIPS(Int_t m,Float_t scale)                  {scaleTime1ErrMIPS[m] = scale;}
    Float_t getSignalSpeed()                                             {return signalspeed;}
    void    setSignalSpeed(Float_t speed)                                {signalspeed=speed;}
    Float_t getCellScale()                                               {return cellEffScale;}
    void    setCellEffScale(Float_t scale)                               {cellEffScale=scale;}
    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();
    void    printParam(void);
    ClassDef(HMdcDigitPar,3) // Container for the MDC Digitizer parameters
};
#endif  /*!HMDCDIGITPAR_H*/
