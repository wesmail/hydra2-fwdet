#ifndef HMDCTRK_H
#define HMDCTRK_H

#include "TObject.h"
#include "hsymmat.h"

class HMdcSeg;

class HMdcTrk : public TObject {
protected:
  Float_t z; 
  Float_t r;
  Float_t p;
  Float_t theta;
  Float_t phi;
  Int_t charge;
  Float_t chi2;
  HSymMat5 cov;
  Int_t flag;
  Int_t sector;
  Int_t segIndex[2];
public:
  inline HMdcTrk(void);
  ~HMdcTrk(void) {}
  void setZ(Float_t az,Float_t err) { z=az; cov.setErr(0,err); }
  void setR(Float_t ar,Float_t err) { r=ar; cov.setErr(1,err); }
  void setP(Float_t ap,Float_t err) { p=ap; cov.setErr(2,err); }
  void setTheta(Float_t atheta,Float_t err) {theta=atheta; cov.setErr(3,err); }
  void setPhi(Float_t aphi,Float_t err)     { phi=aphi; cov.setErr(4,err); }
  void setCharge(Int_t c)       { charge=c; }
  void setChi2(Float_t chi)     { chi2=chi; }
  void setFlag(Int_t aFlag)     { flag=aFlag; }
  void setSector(Int_t aSector) { sector=aSector; }
  void setSegmentIndex(Int_t index,Int_t segment) { segIndex[segment]=index; }
  void readFromSegment(HMdcSeg *seg);
  Float_t getZ(void)        { return z; }
  Float_t getErrZ(void)     { return cov.getErr(0); }
  Float_t getR(void)        { return r; }
  Float_t getErrR(void)     { return cov.getErr(1); }
  Float_t getP(void)        { return p; }
  Float_t getErrP(void)     { return cov.getErr(2); }
  Float_t getTheta(void)    { return theta; }
  Float_t getErrTheta(void) { return cov.getErr(3); }
  Float_t getPhi(void)      { return phi; }
  Float_t getErrPhi(void)   { return cov.getErr(4); }
  Int_t getCharge(void)     { return charge; }
  Float_t getChi2(void)     { return chi2; }
  Int_t getSector(void)     { return sector; }
  Int_t getSegmentIndex(Int_t segment) { return segIndex[segment]; }
  ClassDef(HMdcTrk,1)
};

////////////// Inline functions
inline HMdcTrk::HMdcTrk(void) {
  z=r=p=theta=phi=chi2=0.;
  flag=sector=charge=0;
  segIndex[0]=segIndex[1]=0;
}

#endif
