//*-- Author : V.Pechenov (18/04/2010)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
//  HMdcKickPlane
//
//  Descriptor of the magnetic kick plane of HADES.
//  Contains a parametrization of the kick plane.
//  Parametrization and parameters of the kick plane is the copy 
//  of HSurfPlaneTriV parameters from HKickPlane2 (detPartName: MDC3)
////////////////////////////////////////////////////////////////////////////

#include "hmdckickplane.h"
#include "hmdcseg.h"

ClassImp(HMdcKickPlane)

void HMdcKickPlane::setDefaultParam(void) {
  // Plane equation: y=fDx*x + fDz[i]*z + fC[i]
  // This parameters is the copy of HSurfPlaneTriV parameters
  // from HKickPlane2 (detPartName: MDC3)
  fDzLimit[1] =  400.;        // =fDzLimit[1] in HSurfPlaneTriV 
  fDzLimit[0] =  1100.;       // =fDzLimit[1] in HSurfPlaneTriV 
  fDx         =  0.124811;  
  fDz[2]      = -0.6546;      // =fDz[0] in HSurfPlaneTriV 
  fDz[1]      = -0.82008;     // =fDz[1] in HSurfPlaneTriV 
  fDz[0]      = -1.1989;      // =fDz[2] in HSurfPlaneTriV 
  fC[2]       =  1295.53;     // =fDc[0] in HSurfPlaneTriV 
  
  fC[1]  = fC[2] + (fDz[2]-fDz[1])*fDzLimit[1]; // =fDc[1] in HSurfPlaneTriV
  fC[0]  = fC[1] + (fDz[1]-fDz[0])*fDzLimit[0]; // =fDc[2] in HSurfPlaneTriV
  
  //Plane equation: parA[i]*x + parB[i]*y + z = padD[i]
  for(Int_t i=0;i<3;i++) {
    parA[i] = fDx/fDz[i];
    parB[i] = -1./fDz[i];
    parD[i] = -fC[i]/fDz[i];
  }
}

void HMdcKickPlane::calcIntersection(Double_t x1,Double_t y1,Double_t z1,
                                     Double_t x2, Double_t y2, Double_t z2,
                                     Double_t& x, Double_t& y, Double_t& z) const { 
  // Calcul. a cross of the line with plane       y=fDx*x+fDz*z+fC
  Double_t dX = x2-x1;
  Double_t dY = y2-y1;
  Double_t dZ = z2-z1;
  
  for(Int_t i=0;i<3;i++) {
    Double_t c2  = fDz[i]*dZ;
    Double_t c3  = y1-fC[i];
    
    Double_t cDx = fDx;
    Double_t c1  = cDx*dX-dY;
    Double_t div = 1/(c1+c2);
    Double_t cx  = dX*(c3-fDz[i]*z1)+ x1*(c2-dY);
    x = cx*div;
    if(x<0) {
      cDx = -fDx;
      c1  = cDx*dX-dY;
      div = 1/(c1+c2);
      x   = cx*div;
    }   
    z = (dZ*(c3-cDx*x1) + z1*c1)*div;
    if(i<2 && z<fDzLimit[i]) continue;
    y = cDx*x+fDz[i]*z+fC[i];
    break;
  }
}

void HMdcKickPlane::calcIntersection(Double_t x1,Double_t y1,Double_t z1,
                                     Double_t x2,Double_t y2,Double_t z2, HGeomVector &out) const { 
  // Calcul. a cross of the line with plane       y=fDx*x+fDz*z+fC
  Double_t dX = x2-x1;
  Double_t dY = y2-y1;
  Double_t dZ = z2-z1;
  
  for(Int_t i=0;i<3;i++) {
    Double_t c2  = fDz[i]*dZ;
    Double_t c3  = y1-fC[i];
    
    Double_t cDx = fDx;
    Double_t c1  = cDx*dX-dY;
    Double_t div = 1/(c1+c2);
    Double_t cx  = dX*(c3-fDz[i]*z1)+ x1*(c2-dY);
    Double_t x   = cx*div;
    if(x<0) {
      cDx = -fDx;
      c1  = cDx*dX-dY;
      div = 1/(c1+c2);
      x   = cx*div;
    }   
    Double_t z = (dZ*(c3-cDx*x1) + z1*c1)*div;
    if(i<2 && z<fDzLimit[i]) continue;
    Double_t y = cDx*x+fDz[i]*z+fC[i];
    out.setXYZ(x,y,z);
    break;
  }
}

void HMdcKickPlane::calcSegIntersec(Float_t z0,Float_t r0,Float_t theta,Float_t phi,
				    HGeomVector &p, HGeomVector &dir, HGeomVector &out) const {
  // Calcul. line param. a cross of this line with plane       y=fDx*x+fDz*z+fC
  Double_t cosPhi = TMath::Cos((Double_t)phi);
  Double_t sinPhi = TMath::Sin((Double_t)phi);
  Double_t x0     = -r0*sinPhi;   
  Double_t y0     =  r0*cosPhi; 
  Double_t dZ     = TMath::Cos((Double_t)theta);
  Double_t dxy    = TMath::Sqrt(1.-dZ*dZ);
  Double_t dX     = dxy*cosPhi;
  Double_t dY     = dxy*sinPhi;
  p.setXYZ(x0,y0,z0);
  dir.setXYZ(dX,dY,dZ);

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

void HMdcKickPlane::calcSegIntersec(Float_t z0,Float_t r0,Float_t theta,Float_t phi,HSymMat &m, 
                         HGeomVector &p, HGeomVector &dir, HGeomVector &out,Double_t &errX,Double_t &errY) const {
  // Calcul. a cross of the line with plane       y=fDx*x+fDz*z+fC
  // and errors dx,dy on the kick plane
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t x0     = -r0*sinPhi;
  Double_t y0     =  r0*cosPhi;
  Double_t dZ     = TMath::Cos(theta);
  Double_t dxy    = TMath::Sqrt(1.-dZ*dZ);  // == sin(theta)
  Double_t dX     = dxy*cosPhi;             // == sin(theta) * cos(phi)
  Double_t dY     = dxy*sinPhi;             // == sin(theta) * sin(phi)
  p.setXYZ(x0,y0,z0);
  dir.setXYZ(dX,dY,dZ);

  for(Int_t i = 0;i<3;i++) {
    Double_t c2  = fDz[i]*dZ;
    Double_t c3  = y0-fC[i];
    
    Double_t cDx = fDx;
    Double_t c1  = cDx*dX-dY;
    Double_t div = 1./(c1+c2);
    Double_t cx  = dX*(c3-fDz[i]*z0)+ x0*(c2-dY);
    Double_t x   = cx*div;
    Double_t pA  = parA[i];
    if(x<0) {
      cDx = -fDx;
      c1  = cDx*dX-dY;
      div = 1./(c1+c2);
      x   = cx*div;
      pA = -pA;
    }   
    Double_t z = (dZ*(c3-cDx*x0) + z0*c1)*div;
    if(i<2 && z<fDzLimit[i]) continue;
    out.setXYZ(x,cDx*x+fDz[i]*z+fC[i],z);
    
    // Error propogation:
    Double_t pB = parB[i];
    Double_t EN = 1./(pA*dX+pB*dY+dZ);     // dZ==cos(theta)
    Double_t FN = parD[i]-pA*x0-pB*y0-z;
    Double_t k0 = FN*EN*EN;
    Double_t k1 = pA*sinPhi - pB*cosPhi;
    Double_t k2 = pA*y0 - pB*x0 + FN*EN*(pA*dY-pB*dX);
    Double_t k3 = dX*EN;
    Double_t k4 = dY*EN;
    Double_t XdP1dP2[4];
    XdP1dP2[0] = -k3;
    XdP1dP2[1] = -sinPhi + k1*k3;  
    XdP1dP2[2] =  k0*cosPhi;
    XdP1dP2[3] = -y0 - k4*FN + k3*k2;
    Double_t YdP1dP2[4];
    YdP1dP2[0] = -k4;
    YdP1dP2[1] =  cosPhi + k1*k4;
    YdP1dP2[2] =  k0*sinPhi;
    YdP1dP2[3] =  x0 + k3*FN + k4*k2;
    
    errX = 0.;
    errY = 0.;
    for (Int_t k=0;k<4;k++) for (Int_t l=0;l<4;l++) {
      errX += m.getElement(k,l)*XdP1dP2[k]*XdP1dP2[l];
      errY += m.getElement(k,l)*YdP1dP2[k]*YdP1dP2[l];
    }
    errX = TMath::Sqrt(errX);
    errY = TMath::Sqrt(errY);
    break;
  }
}

void HMdcKickPlane::calcSegIntersec(HMdcSeg *seg1,HGeomVector &p, HGeomVector &dir, HGeomVector &out,
                                    HGeomVector *sOnKick) const {
  // Calcul. a cross of the line with plane       y=fDx*x+fDz*z+fC
  // and errors dx,dy on the kick plane
  Double_t z0     = seg1->getZ();  
  Double_t r0     = seg1->getR();   
  Double_t theta  = seg1->getTheta();
  Double_t phi    = seg1->getPhi();  
  HSymMat &m      = seg1->getCovariance();
  
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t x0     = -r0*sinPhi;   
  Double_t y0     =  r0*cosPhi; 
  Double_t dZ     = TMath::Cos(theta);
  Double_t dxy    = TMath::Sqrt(1.-dZ*dZ);  // == sin(theta)
  Double_t dX     = dxy*cosPhi;             // == sin(theta) * cos(phi)
  Double_t dY     = dxy*sinPhi;             // == sin(theta) * sin(phi)
  p.setXYZ(x0,y0,z0);
  dir.setXYZ(dX,dY,dZ);

  Int_t i = 0;
  for(;i<3;i++) {
    Double_t c2  = fDz[i]*dZ;
    Double_t c3  = y0-fC[i];
    
    Double_t cDx = fDx;
    Double_t c1  = cDx*dX-dY;
    Double_t div = 1./(c1+c2);
    Double_t cx  = dX*(c3-fDz[i]*z0)+ x0*(c2-dY);
    Double_t x   = cx*div;
    Double_t pA  = parA[i];
    if(x<0) {
      cDx = -fDx;
      c1  = cDx*dX-dY;
      div = 1./(c1+c2);
      x   = cx*div;
      pA = -pA;
    }   
    Double_t z = (dZ*(c3-cDx*x0) + z0*c1)*div;
    if(i<2 && z<fDzLimit[i]) continue;
    Double_t y =cDx*x+fDz[i]*z+fC[i];
    out.setXYZ(x,y,z);
    
    // Error propogation:
    Double_t pB = parB[i];
    Double_t EN = 1./(pA*dX+pB*dY+dZ);     // dZ==cos(theta)
    Double_t FN = parD[i]-pA*x0-pB*y0-z;
    Double_t k0 = FN*EN*EN;
    Double_t k1 = pA*sinPhi - pB*cosPhi;
    Double_t k2 = pA*y0 - pB*x0 + FN*EN*(pA*dY-pB*dX);
    Double_t k3 = dX*EN;
    Double_t k4 = dY*EN;
    Double_t XdP1dP2[4];
    XdP1dP2[0] = -k3;
    XdP1dP2[1] = -sinPhi + k1*k3;  
    XdP1dP2[2] =  k0*cosPhi;
    XdP1dP2[3] = -y0 - k4*FN + k3*k2;
    Double_t YdP1dP2[4];
    YdP1dP2[0] = -k4;
    YdP1dP2[1] =  cosPhi + k1*k4;
    YdP1dP2[2] =  k0*sinPhi;
    YdP1dP2[3] =  x0 + k3*FN + k4*k2;
    
    Double_t errX = 0.;
    Double_t errY = 0.;
    for (Int_t k=0;k<4;k++) for (Int_t l=0;l<4;l++) {
      errX += m.getElement(k,l)*XdP1dP2[k]*XdP1dP2[l];
      errY += m.getElement(k,l)*YdP1dP2[k]*YdP1dP2[l];
    }
    errX = TMath::Sqrt(errX);
    errY = TMath::Sqrt(errY); 
    sOnKick[0].setXYZ(x+errX,y+errY,z-pA*errX-pB*errY);
    sOnKick[1].setXYZ(x-errX,y+errY,z+pA*errX-pB*errY);
    sOnKick[2].setXYZ(x+errX,y-errY,z-pA*errX+pB*errY);
    sOnKick[3].setXYZ(x-errX,y-errY,z+pA*errX+pB*errY);
    break;
  }
}

void HMdcKickPlane::calcSegIntersec(HMdcSeg *seg, HGeomVector &out) const {
  // Calcul. a cross of the segment with plane       y=fDx*x+fDz*z+fC
  Double_t z0     = seg->getZ();
  Double_t r0     = seg->getR();
  Double_t theta  = seg->getTheta();
  Double_t phi    = seg->getPhi();
  
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t x0     = -r0*sinPhi;   
  Double_t y0     =  r0*cosPhi; 
  Double_t dZ     = TMath::Cos(theta);
  Double_t dxy    = TMath::Sqrt(1.-dZ*dZ);  // == sin(theta)
  Double_t dX     = dxy*cosPhi;             // == sin(theta) * cos(phi)
  Double_t dY     = dxy*sinPhi;             // == sin(theta) * sin(phi)

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
