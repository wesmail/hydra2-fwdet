#ifndef HMDCKICKPLANE_H
#define HMDCKICKPLANE_H

#include "TObject.h"
#include "hgeomvector.h"
#include "hsymmat.h"

class HSymMat;
class HMdcSeg;

class HMdcKickPlane : public TObject {
protected:
  //Plane equation: y=fDx*x + fDz[i]*z + fC[i]
  Double_t fDzLimit[2];
  Double_t fDx;
  Double_t fDz[3];
  Double_t fC[3];
  //Plane equation: parA[i]*x + parB[i]*y + z = padD[i]
  Double_t parA[3];
  Double_t parB[3];
  Double_t parD[3];
  
  void setDefaultParam(void);
public:
  HMdcKickPlane(void) {setDefaultParam();}
  ~HMdcKickPlane(void) {}
  inline void calcIntersection(const HGeomVector &p,const HGeomVector &dir,HGeomVector &out) const;
  inline void calcSegIntersec(Double_t z0,Double_t r,Double_t theta,Double_t phi,HGeomVector &out) const;
  
  void calcIntersection(Double_t x1,Double_t y1,Double_t z1,
                        Double_t x2, Double_t y2, Double_t z2,
                        Double_t& x, Double_t& y, Double_t& z) const;
  void calcIntersection(Double_t x1,Double_t y1,Double_t z1,
                        Double_t x2,Double_t y2,Double_t z2, HGeomVector &out) const;
  
  void calcSegIntersec(Float_t z0,Float_t r0,Float_t theta,Float_t phi,
		       HGeomVector &p, HGeomVector &dir, HGeomVector &out) const;
  
  void calcSegIntersec(Float_t z0,Float_t r0,Float_t theta,Float_t phi, HSymMat &m, 
                        HGeomVector &p, HGeomVector &dir,HGeomVector &out,
                        Double_t &errX,Double_t &errY) const;
  void calcSegIntersec(HMdcSeg *seg1,HGeomVector &p, HGeomVector &dir, HGeomVector &out,
                       HGeomVector *sOnKick) const;
  void calcSegIntersec(HMdcSeg *seg, HGeomVector &out) const;
  
  ClassDef(HMdcKickPlane,0) //Defines a plane surface
};

inline void HMdcKickPlane::calcIntersection(const HGeomVector &p,const HGeomVector &dir,
				            HGeomVector &out) const {
  // Calcul. a cross of the line with plane       y=fDx*x+fDz*z+fC
  for(Int_t i=0;i<3;i++) {
    Double_t c2  = fDz[i]*dir.getZ();
    Double_t c3  = p.getY()-fC[i];
    
    Double_t cDx = fDx;
    Double_t c1  = cDx*dir.getX()-dir.getY();
    Double_t div = 1/(c1+c2);
    Double_t cx  = dir.getX()*(c3-fDz[i]*p.getZ())+ p.getX()*(c2-dir.getY());
    Double_t x   = cx*div;
    if(x<0) {
      cDx = -fDx;
      c1  = cDx*dir.getX()-dir.getY();
      div = 1/(c1+c2);
      x   = cx*div;
    }   
    Double_t z = (dir.getZ()*(c3-cDx*p.getX()) + p.getZ()*c1)*div;
    if(i<2 && z<fDzLimit[i]) continue;
    out.setXYZ(x,cDx*x+fDz[i]*z+fC[i],z);
    break;
  }
}

inline void HMdcKickPlane::calcSegIntersec(Double_t z0,Double_t r0,Double_t theta,Double_t phi,
				       HGeomVector &out) const {
  // Calcul. a cross of the line with plane       y=fDx*x+fDz*z+fC
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t x0     = -r0*sinPhi;   
  Double_t y0     =  r0*cosPhi; 
  Double_t dZ     = TMath::Cos(theta);
  Double_t dxy    = TMath::Sqrt(1.-dZ*dZ);
  Double_t dX     = dxy*cosPhi;
  Double_t dY     = dxy*sinPhi;

  for(Int_t i=0;i<3;i++) {
    Double_t c2  = fDz[i]*dZ;
    Double_t c3  = y0-fC[i];
    
    Double_t cDx = fDx;
    Double_t c1  = cDx*dX-dY;
    Double_t div = 1./(c1+c2);
    Double_t cx  = dX*(c3-fDz[i]*z0)+ x0*(c2-dY);
    Double_t x   = cx*div;
    if(x<0) {
      cDx = -fDx;
      c1  = cDx*dX-dY;
      div = 1./(c1+c2);
      x   = cx*div;
    }   
    Double_t z = (dZ*(c3-cDx*x0) + z0*c1)*div;
    if(i<2 && z<fDzLimit[i]) continue;
    out.setXYZ(x,cDx*x+fDz[i]*z+fC[i],z);
    break;
  }
}

#endif   /*!HMDCKICKPLANE_H*/
