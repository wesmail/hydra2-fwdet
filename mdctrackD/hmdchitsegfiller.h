#ifndef HMDCHITSEGFILLER_H
#define HMDCHITSEGFILLER_H

using namespace std;
#include "TObject.h"
#include "hsymmat.h"

class HMdcPlane;
class HMdcClus;
class HMdcHit;
class HMdcSeg;
class HMdcSizesCellsMod;
class HGeomVector;
class HMdcTrackParam;

class HMdcHitSegFiller : public TObject {
  protected:
    Float_t dP2dP1[4][4];
    // Fitter or cluster track paramet.
    HMdcPlane* plane1;
    Double_t A1;
    Double_t B1;
    Double_t x1;
    Double_t y1;
    HMdcPlane* plane2;
    Double_t A2;
    Double_t B2;
    Double_t x2;
    Double_t y2;
    HSymMat4* covMatr;
    Double_t timeOffSet[4];
    Double_t errTimeOffSet[4];

    Double_t z1;
    Double_t z2;
    
    Int_t typeClusVert;    // 0 - target, 1 - kick plane
    HSymMat4 clusErrMatr;

    // HMdcSeg paramet.
    Double_t z;
    Double_t r;
    Double_t theta;
    Double_t phi;
    Float_t dP1dP2[4][4];
    Double_t zPrime;  // z coordinate wrt axis centered on target position
    Double_t rPrime;  // r coordinate wrt axis centered on target position
    
    Double_t xTarget;
    Double_t yTarget;

    // HMdcHit paramet.
    Double_t x;
    Double_t y;
    Double_t xDir;
    Double_t yDir;
    HSymMat4 hitErr;
  public:
    HMdcHitSegFiller(void) {clear();}
    ~HMdcHitSegFiller(void) {}

    void clear(void) {plane1=plane2=0;}
    void setFitParam(HMdcTrackParam& par);
    void setFitParamForSecondSec(HMdcTrackParam& par);
    void calcSegPar(const HGeomVector* targ=0);

    HMdcPlane* getPlane1(void) {return plane1;}
    HMdcPlane* getPlane2(void) {return plane2;}
    Double_t getX1(void) {return x1;}
    Double_t getY1(void) {return y1;}
    Double_t getZ1(void) {return z1;}
    Double_t getX2(void) {return x2;}
    Double_t getZ2(void) {return z2;}

    void setSegParam(HMdcPlane* pl1, HMdcPlane* pl2,
        Double_t zm, Double_t r0, Double_t th, Double_t ph);
    void setClusParam(HMdcClus* clus, Bool_t isCoilOff);

    Double_t getZm(void) {return z;}
    Double_t getR(void) {return r;}
    Double_t getTheta(void) {return theta;}
    Double_t getPhi(void) {return phi;}

    void propogErrToSeg(HSymMat4& errMatOut);
    void fillMdcSeg(HMdcSeg* mdcSeg, const HGeomVector* targ);
    void fillMdcSegByClus(HMdcSeg* mdcSeg, const HGeomVector* targ,HMdcTrackParam* trPar=NULL);
    void fillMdcHit(HMdcSizesCellsMod* fSCMod,HMdcHit* mdcHit);
    void fillMdcHitByClus(HMdcSizesCellsMod* fSCMod,HMdcHit* mdcHit);
    
    void propogErrToFit(HSymMat4& errMatIn, HSymMat4& errMatOut);

  private:
    void calcDerMatrToSeg(void);
    void calcDerMatrClus1ToSeg(void);
    void calcDerMatrToFit(void);
    void calcFitPar(void);
    void fillSegPar(HMdcSeg* mdcSeg);

  ClassDef(HMdcHitSegFiller,0)
};

#endif  /* HMdcHitSegFiller_H */














