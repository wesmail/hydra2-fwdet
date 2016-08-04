#ifndef HMDCTRACKFITTERA_H
#define HMDCTRACKFITTERA_H

#include "hmdctrackfitter.h"

class HMdcTrackFitterA : public HMdcTrackFitter {
  protected:
    // Fit parameters for 1-st minim. method:
    Double_t initStepFit;       // init value of stepFit
    Int_t    limIter1forFilter; // max. number of iter. at the filtering
    Double_t limFunct1to2;      // limit for functional
    Double_t limDeltaF1to2;     // limit for functional
    Double_t limGrad1to2;       // limit for functional
    // DownhillOnGradient parameters:
    Double_t stepCh[6];
    Double_t dFunctMax;
    Double_t dFunctMin;

    // Fit parameters for 2-nd minim. method:
    Double_t limGrad2;          //
    Double_t limStep2[10];      // limit for diff. old-new param.
    Int_t    limIter2;          // max. number of iter.

    Double_t stepFit;           // current value of step for gradient dowhill
    Int_t    iterAfterFilter;   // counter of iter.number after filtering
    Int_t    return2to1;        // counter of method2->method1 switching
    HMdcTrackParam pari;        // track fit parameters for inner purpose
    HMdcTrackParam parMin;      // track fit parameters for inner purpose
    HMdcTrackParam tmpPar;      // track fit parameters for inner purpose

  public:
    HMdcTrackFitterA(HMdcTrackFitInOut* fIO);
    virtual ~HMdcTrackFitterA(void);
    virtual Int_t minimize(Int_t iter=0);
    void setDefaultParam(void);
    
    // Fit parameters for 1-st minim. method:
    void setInitStepFit(Double_t vl)       {initStepFit=vl;}
    void setLimIter1forFilter(Int_t vl)    {limIter1forFilter=vl;}
    void setLimFunct1to2(Double_t vl)      {limFunct1to2=vl;}
    void setLimDeltaF1to2(Double_t vl)     {limDeltaF1to2=vl;}
    void setLimGrad1to2(Double_t vl)       {limGrad1to2=vl;}
    // DownhillOnGradient parameters:
    void setStepCh(UInt_t i,Double_t vl)   {if(i<6) stepCh[i]=vl;}
    void setDFunctMax(Double_t vl)         {dFunctMax=vl;}
    void setDFunctMin(Double_t vl)         {dFunctMin=vl;}
    // Fit parameters for 2-nd minim. method:
    void setLimGrad2(Double_t vl)          {limGrad2=vl;}
    void setLimStep2(UInt_t i,Double_t vl) {if(i<10) limStep2[i]=vl;}
    void setLimIter2(Int_t vl)             {limIter2=vl;}

    virtual Double_t getFunctional(void);
    
  protected:
    void printResult(void);
    void printResult(const Char_t* status, HMdcTrackParam& par);
    void printStep(const Char_t* cond, Double_t fun, Double_t step);
    virtual void solutionOfLinearEquationsSystem(HMdcTrackParam& par);
    void downhillOnGradient(HMdcTrackParam& par);
    virtual Int_t firstMethod(void);
    virtual Int_t secondMethod(void);
    Double_t calcScaledAGrad(HMdcTrackParam& par);
    virtual Int_t doMinimization(void);

    ClassDef(HMdcTrackFitterA,0)  // Dubna track piece fitter. Version A.
};

#endif
