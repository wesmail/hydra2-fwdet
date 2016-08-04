#ifndef HMDCTRACKFITTERB_H
#define HMDCTRACKFITTERB_H

#include "hmdctrackfittera.h"

class HMdcTrackFitterB : public HMdcTrackFitterA {
  private:
     Bool_t recalcTdcError;  // kTRUE - TDC errors will not change during minimization
  public:
    HMdcTrackFitterB(HMdcTrackFitInOut* fIO);
    virtual ~HMdcTrackFitterB(void);
    virtual Int_t minimize(Int_t iter=0);

  protected:
    void targetScan(Bool_t useTukeyInScan=kFALSE);
    void solutionOfLinearEquationsSystem(HMdcTrackParam& par);
    Int_t firstMethod(void);
    Int_t secondMethod(void);
    virtual Int_t doMinimization(void);

    ClassDef(HMdcTrackFitterB,0)  // Dubna track piece fitter
};

#endif
