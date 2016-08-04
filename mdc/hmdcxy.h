#ifndef HMDCXY_H
#define HMDCXY_H

#include "TObject.h"

class HMdcXY : public TObject {
protected:
  // xHit,yHit,zHit is point on the midplane MDC
  // in coordinate system of sector
  Int_t sector;      // sector number [0-5]
  Int_t module;      // module number [0-3]
  Float_t xHit;      // x in sector frame [mm]
  Float_t yHit;      // y in sector frame [mm]
  Float_t zHit;      // z in sector frame [mm]
  Float_t errX;      // error on x-coordinate of hit [mm]
  Float_t errY;      // error on y-coordinate of hit [mm]
  Float_t xDir;      // x-direction of hit
  Float_t yDir;      // y-direction of hit 
  Float_t errXdir;   // error on x-direction of hit
  Float_t errYdir;   // error on y-direction of hit 
  
  
  Int_t nCluster;    // cluster number cont. HMdc12Cluster

public:
  HMdcXY(void) {clear();}
  ~HMdcXY(void) {}
  void clear() {
    sector=module=-1;
    xHit = yHit = zHit = 0.0;
    nCluster = -1;
    xDir = yDir = -1.;
  }
  void setSector(const Int_t s) { sector = s; }
  void setModule(const Int_t m) { module = m; }
  void setAddress(const Int_t s,const Int_t m) {
    sector=s;
    module=m;
  }
  void setX(const Float_t x) {xHit=x;}
  void setY(const Float_t y) {yHit=y;}
  void setZ(const Float_t z) {zHit=z;}
  void setXdir(const Float_t xdir) {xDir=xdir;}
  void setYdir(const Float_t ydir) {yDir=ydir;}
  void setErrX(const Float_t errx) {errX=errx;}
  void setErrY(const Float_t erry) {errY=erry;}
  void setErrXdir(const Float_t errxd) {errXdir=errxd;}
  void setErrYdir(const Float_t erryd) {errYdir=erryd;}
  void setNClust(const Int_t n) {nCluster=n;}

  Int_t getSector(void) const { return sector; }
  Int_t getModule(void) const { return module; }
  void getAddress(Int_t& s,Int_t& m) {
    s=sector;
    m=module;
  }
  Float_t getX(void) {return xHit;}
  Float_t getY(void) {return yHit;}
  Float_t getZ(void) {return zHit;}
  Float_t getXdir(void) {return xDir;}
  Float_t getYdir(void) {return yDir;}
  Float_t getErrX(void) {return errX;}
  Float_t getErrY(void) {return errY;}
  Float_t getErrXdir(void) {return errXdir;}
  Float_t getErrYdir(void) {return errYdir;}
  Int_t getNClust(void) {return nCluster;}

  ClassDef(HMdcXY,1) // temporary MDC hit class (May 2000 test beam)
};

#endif  /* HMDCXY_H */





