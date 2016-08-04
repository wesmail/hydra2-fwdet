/////////////////////////////////////////////////////////////////////////
//
//  HMdcHit this is the HIT class of MDC
//
//  R.Schicker 20-May-00
//
/////////////////////////////////////////////////////////////////////////

#ifndef HMDCHIT_H
#define HMDCHIT_H

#include "hcategory.h"
#include "hsymmat.h"
#include "hmdccellgroup.h"
#include "hmessagemgr.h"
#include "TMath.h"

class HMdcHit : public HMdcCellGroup6 {
protected:
  Float_t x;        // x-coordinate of hit in MDC coordinate sys [mm]
  Float_t y;        // y-coordinate of hit in MDC coordinate sys [mm]
  Float_t xDir;     // x-direction of hit
  Float_t yDir;     // y-direction of hit
  Float_t offset;   // time offset [ns]
  Float_t chi2;     // chi square per degree of freedom
  Int_t flag;       // user flag
  HSymMat5 cov;     // covariance matrix of parameters
  Int_t sector;     // sector [0-5]
  Int_t module;     // module [0-3]

  Short_t trFinder; // =1 - Dubna track finder (see hmdchit.cc)
                    //  Dubna:                    | Santiago:
  Short_t index;    // index of HMdcClusInf obj.  |
                    // in catMdcClusInf category  |

public:
  HMdcHit(void) {Clear();}
  ~HMdcHit(void) {;}

  void Clear(void);

  void setX(const Float_t xl,const Float_t err) {
    x=xl; cov.setErr(0,err);}

  void setY(const Float_t yl,const Float_t err) {
    y=yl; cov.setErr(1,err);}

  void setXDir(const Float_t xl,const Float_t err) {
    xDir=xl; cov.setErr(2,err);}

  void setYDir(const Float_t yl,const Float_t err) {
    yDir=yl; cov.setErr(3,err);}

  void setOff(const Float_t off,const Float_t err) {
    offset=off; cov.setErr(4,err);}

  void setXY(const Float_t xl,const Float_t errx,const Float_t yl,const Float_t erry) {
    x=xl; cov.setErr(0,errx); y=yl; cov.setErr(1,erry);}

  void setXYDir(const Float_t xl,const Float_t errx,const Float_t yl,const
   Float_t erry) {xDir=xl; cov.setErr(2,errx); yDir=yl; cov.setErr(3,erry);}

  void setPar(const Float_t xl,const Float_t erXl,const Float_t yl,const Float_t erYl,
	      const Float_t xDirl,const Float_t erXDirl,const Float_t yDirl,
	      const Float_t erYDirl,const Float_t offl,const Float_t erOffl ) {
    x=xl; y=yl; xDir=xDirl; yDir=yDirl; offset=offl;
    cov.setErr(0,erXl); cov.setErr(1,erYl); cov.setErr(2,erXDirl); cov.setErr(3,erYDirl);
    cov.setErr(4,erOffl);
  }

  void setPar(const Float_t xl,const Float_t yl,
	      const Float_t xDirl,const Float_t yDirl, const Float_t offl) {
    x=xl; y=yl; xDir=xDirl; yDir=yDirl; offset=offl;
  }
  
  void setChi2(const Float_t chl) {chi2=chl;}
  void setFlag(const Int_t fl) {flag = fl;}


  void setSecMod(const Int_t is,const Int_t im) {sector=is; module=im;}

  Float_t getX(void) { return x; }
  Float_t getY(void) { return y; }
  Float_t getXDir(void) { return xDir; }
  Float_t getYDir(void) { return yDir; }
  Float_t getOffset(void) { return offset; }
  Float_t getErrX(void) { return cov.getErr(0); }
  Float_t getErrY(void) { return cov.getErr(1); }
  Float_t getErrXDir(void) { return cov.getErr(2); }
  Float_t getErrYDir(void) { return cov.getErr(3); }
  Float_t getErrOffset(void) { return cov.getErr(4); }
  Float_t getTheta(void) {
    return TMath::ACos(TMath::Sqrt(1-xDir*xDir-yDir*yDir)); }
  Float_t getPhi(void) { return TMath::ATan2(yDir,xDir); }
  HSymMat &getCovariance(void) { return cov; }


  void getX(Float_t& xl,Float_t& errx) {xl=x; errx=cov.getErr(0);}
  void getY(Float_t& yl,Float_t& erry) {yl=y; erry=cov.getErr(1);}
  void getXDir(Float_t& xl,Float_t& errx) {xl=xDir; errx=cov.getErr(2);}
  void getYDir(Float_t& yl,Float_t& erry) {yl=yDir; erry=cov.getErr(3);}

  void getXY(Float_t& xl,Float_t& errx,Float_t& yl,Float_t& erry) {
    xl=x; errx=cov.getErr(0); yl=y; erry=cov.getErr(1);}

  void getXYDir(Float_t& xl,Float_t& errx,Float_t& yl,Float_t& erry) {
    xl=xDir; errx=cov.getErr(2); yl=yDir; erry=cov.getErr(3);}

  void getOff(Float_t& off,Float_t& err) { off=offset; err=cov.getErr(4);}

  void getPar(Float_t& p1,Float_t& p2,Float_t& p3,Float_t& p4,Float_t& p5) {
    p1=x; p2=y; p3=xDir; p4=yDir; p5=offset;}

  void getErrPar(Float_t& p1,Float_t& p2,Float_t& p3,Float_t& p4,Float_t& p5) {
    p1=cov.getErr(0); p2=cov.getErr(1); p3=cov.getErr(2); p4=cov.getErr(3);
    p5=cov.getErr(4);}

  Float_t getChi2(void) { return chi2;}
  Int_t getFlag(void) { return flag;}

  void getSecMod(Int_t& isec,Int_t& imod) {isec=sector; imod=module;}
  Int_t getSector(void) { return sector; }
  Int_t getModule(void) { return module; }
  void setSector(Int_t isec) {sector=isec;}
  void setModule(Int_t imod) {module=imod;}


  void print(void) {
      gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			     ,"x=%f y=%f xDir=%f yDir=%f offset=%f flag=%i \n"
			     ,x,y,xDir,yDir,offset,flag);}
  void setTrackFinder(Short_t trfn) {trFinder=trfn;}
  Short_t getTrackFinder(void) {return trFinder;}

  // Methods for Dubna track finder:
  void setClusInfIndex(const Short_t ind) {index=ind;}
  Short_t getClusInfIndex(void) {return index;}

  ClassDef(HMdcHit,1) // hit in a MDC chamber
};



#endif  /* HMdcHit_H */









