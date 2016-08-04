/////////////////////////////////////////////////////////////////////////
//
//  HMdcSeg this is the SEGment class of MDC
//
//  Added data members, R.Holzmann 23-June-04
//  R.Schicker 20-May-00
//
/////////////////////////////////////////////////////////////////////////
#ifndef HMDCSEG_H
#define HMDCSEG_H

#include "hsymmat.h"
#include "hmdccellgroup.h"

class HMdcSeg : public HMdcCellGroup12 {

protected: 
  Float_t z;       // z cordinate in sector coordinate sys. [mm]
  Float_t r;       // r coordinate in sector coordinate sys. [mm]
  Float_t theta;   // polar angle theta in sector coordinate sys. [radian]
  Float_t phi;     // azimuthal angle phi in sector coordinate sys. [radian]
  Float_t chi2;    // chi square per degree of freedom
  Int_t   flag;    // Cluster info: number of cells, bins and merged clusters 
  HSymMat4 cov;    // covariance matrix 4x4 in sector system		      
  UInt_t  ind;     // index for hit1/sector
  UInt_t  ind2;    // index for hit2/inner-outer segment
  Float_t zPrime;  // z cordinate wrt axis centered on target position
  Float_t rPrime;  // r coordinate wrt axis centered on target position
  Int_t   clusInd; // index of MDC cluster
  Short_t x1;      // x1,y1 - cross point of the segment line with first mdc layer (in sec.coor.sys.)
  Short_t y1;      // Convert to mm: Float_t(x1)*0.1; Float_t(y1)*0.1;
  Short_t x2;      // x2,y2 - cross point of the segment line with last mdc layer (in sec.coor.sys.)
  Short_t y2;      // Convert to mm: Float_t(x2)*0.1; Float_t(y2)*0.1;

  enum {kIsFake         = BIT(14),   // bit in TObject::fBits for "fake" segment flag
        kOffVertexClust = BIT(20)};  // bit in TObject::fBits for "fake" segment flag
 public:
  HMdcSeg(void); 
  ~HMdcSeg(void);
  
  void Clear(void);
  
  void setZ(const Float_t zl,const Float_t err) {z=zl; cov.setErr(0,err);}
  void setR(const Float_t rl,const Float_t err) {r=rl; cov.setErr(1,err);}
  void setTheta(const Float_t th,const Float_t err) 
    {theta=th; cov.setErr(2,err);}
  void setPhi(const Float_t ph,const Float_t err) {phi=ph; cov.setErr(3,err);}

  void setZR(const Float_t zl,const Float_t erz,const Float_t rl, const Float_t err) { z=zl; cov.setErr(0,erz); r=rl; cov.setErr(1,err);}
  void setThetaPhi(const Float_t th,const Float_t ert,
		 const Float_t ph, const Float_t erp) { 
    theta=th; cov.setErr(2,ert); phi=ph; cov.setErr(3,erp);
  }

  void setPar(const Float_t zl, const Float_t ezl, const Float_t rl, 
	      const Float_t erl, const Float_t th, const Float_t eth, 
	      const Float_t ph, const Float_t eph) {
    z=zl; cov.setErr(0,ezl); r=rl; cov.setErr(1,erl); theta=th; 
    cov.setErr(2,eth); phi=ph; cov.setErr(3,eph);
  }
  
  void setZ(const Float_t zl) {z=zl;}
  void setR(const Float_t rl) {r=rl;}
  void setTheta(const Float_t th) {theta=th;}
  void setPhi(const Float_t ph) {phi=ph;}
  void setPar(const Float_t zl, const Float_t rl, 
	      const Float_t th, const Float_t ph) {
    z=zl; r=rl; theta=th; phi=ph;
  }
  void setZprime(const Float_t zpr) {zPrime=zpr;}
  void setRprime(const Float_t rpr) {rPrime=rpr;}

  void setChi2(const Float_t chl) {chi2=chl;}  
  void setFlag(const Int_t fl) {flag=fl;}

  void setSec(Int_t sec)   { ind  = (sec & 0x7) | (ind & 0x7FFFFF00); }
  void setIOSeg(Int_t seg) { ind2 = (seg & 0x1) | (ind & 0x7FFFFF00); }
  void setHitInd(UInt_t i,UInt_t idx) {
    if(idx > 8388606 || i > 1){
    Warning("setHitInd","max hit= 8388606, ind= %i mdc=0,1 ind= %i",idx,i);
    }
    else {
        UInt_t &indc = i==0 ? ind : ind2;
        idx++;
	indc = (idx << 8) | (indc & 0xFF);
    }
  }
  void setClusIndex(Int_t ind) {clusInd = ind;}

  void setX1(Double_t v)       {x1 = convertToShort(v);}
  void setY1(Double_t v)       {y1 = convertToShort(v);}
  void setX2(Double_t v)       {x2 = convertToShort(v);}
  void setY2(Double_t v)       {y2 = convertToShort(v);}
  void setXYPar(Double_t vx1,Double_t vy1,Double_t vx2,Double_t vy2) {
    x1 = convertToShort(vx1);
    y1 = convertToShort(vy1);
    x2 = convertToShort(vx2);
    y2 = convertToShort(vy2);
  }

  Float_t getZ(void) const         {return z;}
  Float_t getR(void) const         {return r;}
  Float_t getTheta(void) const     {return theta;}
  Float_t getPhi(void) const       {return phi;}
  Float_t getErrZ(void)            {return cov.getErr(0);}
  Float_t getErrR(void)            {return cov.getErr(1);}
  Float_t getErrTheta(void)        {return cov.getErr(2);}
  Float_t getErrPhi(void)          {return cov.getErr(3);} 
  Float_t getZprime(void) const    {return zPrime;}
  Float_t getRprime(void) const    {return rPrime;}
  Int_t   getClusIndex(void) const {return clusInd;}
  
  Float_t getX1(void) const        {return Float_t(x1)*0.1;}  // in [mm]
  Float_t getY1(void) const        {return Float_t(y1)*0.1;}  // in [mm]
  Float_t getX2(void) const        {return Float_t(x2)*0.1;}  // in [mm]
  Float_t getY2(void) const        {return Float_t(y2)*0.1;}  // in [mm]
  Short_t getX1Sh(void) const      {return x1;}               // in hundreds mkm
  Short_t getY1Sh(void) const      {return y1;}               // in hundreds mkm
  Short_t getX2Sh(void) const      {return x2;}               // in hundreds mkm
  Short_t getY2Sh(void) const      {return y2;}               // in hundreds mkm

  void getZ(Float_t& zl,Float_t& err)           { zl=z; err=cov.getErr(0);}
  void getR(Float_t& rl,Float_t& err)           { rl=r; err=cov.getErr(1);}
  void getTheta(Float_t& th,Float_t& err)       { th=theta; err=cov.getErr(2);}
  void getPhi(Float_t& ph,Float_t& err)         { ph=phi; err=cov.getErr(3);}
  void getZR(Float_t& zl,Float_t& erz,Float_t& rl,Float_t& err) { 
    zl=z; erz=cov.getErr(0); rl=r; err=cov.getErr(1);}
  void getThetaPhi(Float_t& th,Float_t& ert,Float_t& ph,Float_t& erp) { 
    th=theta; ert=cov.getErr(2); ph=phi; erp=cov.getErr(3);}
  void getPar(Float_t& p1,Float_t& p2,Float_t& p3,Float_t& p4) const {
    p1=z; p2=r; p3=theta; p4=phi;}
  void getErrPar(Float_t& p1,Float_t& p2,Float_t& p3,Float_t& p4) {
    p1=cov.getErr(0); p2=cov.getErr(1); p3=cov.getErr(2); p4=cov.getErr(3);}
  Float_t getChi2(void) const      {return chi2;}
  HSymMat &getCovariance(void)     {return cov;}
  Int_t getFlag(void) const        {return flag;}

  Int_t getSec(void)   const { return ind  & 0xFF; } // sector number
  Int_t getIOSeg(void) const { return ind2 & 0xFF; } // inner seg=0, outer=1 
  inline Int_t getHitInd(Int_t i) const; // Hit index (-1 --> No hit)
  
  void   setFakeFlag(void)      { SetBit(kIsFake); }
  void   resetFakeFlag(void)    { ResetBit(kIsFake); }
  Bool_t isFake(void) const     { return TestBit(kIsFake); }
  void   setClusInfo(Int_t nBins,Int_t nCells,Int_t nMerClus) {
    if(nMerClus > 99) nMerClus = 99;
    flag = ((nBins & 0x7FFF) << 16) | ((nCells*100 & 0xFFFF) + nMerClus);
  }
  void    setOffVertexClus(void)      { SetBit(kOffVertexClust); }
  Bool_t  isOffVertexClus(void) const { return TestBit(kOffVertexClust); }
  
  Int_t   getNBinsClus(void) const   {return flag >> 16;}
  Int_t   getNCellsClus(void) const  {return (flag & 0xFFFF)/100;}
  Int_t   getNMergedClus(void) const {return (flag & 0xFFFF)%100;}
  void    print(void);
 
protected:
  Short_t convertToShort(Double_t v) const {
    if(v>=0.) return v< 3200. ? Short_t(v*10.+0.5) :  32000;
    else      return v>-3200. ? Short_t(v*10.-0.5) : -32000;
  }

  ClassDef(HMdcSeg,3) // MDC segment
};

inline Int_t HMdcSeg::getHitInd(Int_t i) const {
  Int_t r = i==0 ? (ind >> 8) : (ind2 >> 8);
  return r-1;
}

#endif  /* HMdcSeg_H */


















