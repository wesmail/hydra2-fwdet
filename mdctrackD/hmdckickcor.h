#ifndef HMDCKICKCOR_H
#define HMDCKICKCOR_H

#include "TObject.h"
#include <TMath.h>
// version 3, aug 2013, W.Koenig
class HMdcKickCor : public TObject {
protected:
    const static Int_t nThetaMax=10,iPhiMax=8,parMax=6,layerMax=6,mdcSetMax=4;
    Double_t theta0Mdc[4], y0Mdc[4], z0Mdc[4]; //position of mdc's in sector coordinate system
    Float_t thetaPar[nThetaMax], phiPar[iPhiMax];
    Float_t drParPos[mdcSetMax][layerMax][parMax][iPhiMax][nThetaMax];
    Float_t drParNeg[mdcSetMax][layerMax][parMax][iPhiMax][nThetaMax];
    Float_t rMaxPar[2][nThetaMax-3][4]; // max deflection distance pos. charges, phi, theta mdc &layer bins
    Float_t tmpKickPar[2], tmpPhiPar[2], par[parMax];
    Float_t dThetaPar, dThetaLow, dThetaHigh;
    Float_t dPhiPar, dPhiLow, dPhiHigh;
    Float_t rad2deg;
    Int_t indexOffset, nTheta, iPhi;

  
public:
    HMdcKickCor(void) {initParam();};
    ~HMdcKickCor(void) {};
    void interpolateAngles(const Float_t theta, const Float_t phi);

    Float_t calcCorrection(Float_t rKick, const Int_t mdcNo, const Int_t layerNo);
    void initParam(void);

    Double_t getYMdc(Int_t mdcNo) {
        if(mdcNo>=0 && mdcNo<4) return y0Mdc[mdcNo];
	else return 0.0;
    }
    Double_t getZMdc(Int_t mdcNo) {
        if(mdcNo>=0 && mdcNo<4) return z0Mdc[mdcNo];
	else return 0.0;
    }
    Double_t getThetaMdc(Int_t mdcNo) {
        if(mdcNo>=0 && mdcNo<4) return theta0Mdc[mdcNo];
	else return 0.0;
    }
  
    ClassDef(HMdcKickCor,0) //Calculate correction to outgoing kickplane hits in MDC plane III and IV
};
#endif //HMDCKICKCOR_H
