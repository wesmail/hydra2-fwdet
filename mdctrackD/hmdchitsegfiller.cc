#include "hmdchitsegfiller.h"
#include "hmdcgeomobj.h"
#include "hmdchit.h"
#include "hmdcseg.h"
#include "hmdcclus.h"
#include "hmdcsizescells.h"
#include "hmdctrackparam.h"

//*-- Author : A.Ierusalimov & Vladimir Pechenov
//*-- Modified : 25/06/2004 by V. Pechenov

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////
//
// HMdcHitSegFiller
//
// class for error propagation from Dubna track fitter and cluster
// finder to HMdcSeg and HMdcHit straight line parametrization 
// and filling HMdcSeg and HMdcHit containers.
//
/////////////////////////////////////////////////////////////////////////

void HMdcHitSegFiller::setFitParam(HMdcTrackParam& par) {
  plane1  = par.getFirstPlane();
  A1      = plane1->A();
  B1      = plane1->B();
  x1      = par.x1();
  y1      = par.y1();
  z1      = par.z1();
  plane2  = par.getSecondPlane();
  A2      = plane2->A();
  B2      = plane2->B();
  x2      = par.x2();
  y2      = par.y2();
  z2      = par.z2();
  covMatr = &(par.getErrMatr());
  Double_t* tos      = par.getTimeOffset(); //!
  Double_t* errTOSet = par.getErrTimeOffset(); //!
  for(Int_t m=0;m<4;m++) {
    timeOffSet[m]    = tos[m];
    errTimeOffSet[m] = errTOSet[m];
  }
}

void HMdcHitSegFiller::setFitParamForSecondSec(HMdcTrackParam& par) {
  // !!! For cosmic 2 sectors data only !!!
  plane1  = par.getFirstPlane();
  A1      = plane1->A();
  B1      = plane1->B();
  x1      = par.x1();
  y1      = par.y1();
  z1      = par.z1();
  plane2  = par.getSecondPlane();
  A2      = plane2->A();
  B2      = plane2->B();
  x2      = par.x2();
  y2      = par.y2();
  z2      = par.z2();
  covMatr = &(par.getErrMatr());
  Double_t* tos      = par.getTimeOffset(); //!
  Double_t* errTOSet = par.getErrTimeOffset(); //!
  for(Int_t m=0;m<4;m++) {
    timeOffSet[m]    = tos[m+4];
    errTimeOffSet[m] = errTOSet[m+4];
  }
}

void HMdcHitSegFiller::setClusParam(HMdcClus* clus, Bool_t isCoilOff) {
  x1=clus->getXTarg();
  y1=clus->getYTarg();
  z1=clus->getZTarg();
  x2=clus->getX();
  y2=clus->getY();
  z2=clus->getZ();
  A2=clus->A();
  B2=clus->B();
  clus->fillErrMatClus(isCoilOff,clusErrMatr);
  covMatr=&clusErrMatr;
  if(isCoilOff || clus->getIOSeg()==0) typeClusVert=0;
  else {
    typeClusVert=1;
    A1=0.;
    B1=clus->getBParKickPl();
  }
  for(Int_t m=0;m<4;m++) timeOffSet[m]=errTimeOffSet[m]=0.;
}

void HMdcHitSegFiller::calcSegPar(const HGeomVector* targ) {
  Double_t dX=x2-x1;
  Double_t dY=y2-y1;
  Double_t dZ=z2-z1;
  Double_t rXY2=dX*dX+dY*dY;
  Double_t rXY=sqrt(rXY2);
  if(rXY<2.E-5) {
    x2+=1.E-5;
    y2+=1.E-5;
    calcSegPar();
    return;
  }
  theta=atan2(rXY,dZ);
  phi=atan2(dY,dX);
  zPrime=z=z1 - (x1*dX+y1*dY)/rXY2*dZ;
  rPrime=r=(y1*dX-x1*dY)/rXY;
  if(targ) {
    zPrime += (targ->getX()*dX+targ->getY()*dY)*dZ/rXY2;
    rPrime += (targ->getX()*dY-targ->getY()*dX)/rXY;
  }
}

void HMdcHitSegFiller::calcDerMatrToSeg(void) {
  Double_t rXY=1./sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
  Double_t x1o=x1*rXY;
  Double_t x2o=x2*rXY;
  Double_t y1o=y1*rXY;
  Double_t y2o=y2*rXY;
  Double_t dZ=(z2-z1)*rXY;
  Double_t dX=x2o-x1o;
  Double_t dY=y2o-y1o;
  Double_t var1 = x1o*dX+y1o*dY;
  Double_t var2 = y1o*dX-x1o*dY;
  Double_t var3 = rXY/(dZ*dZ+1.);
  Double_t dXdZ=dX*dZ;
  Double_t dYdZ=dY*dZ;
  
  dP2dP1[0][0] = -A1 - dXdZ + x1o*dZ - var1*(A1+2.0*dXdZ);  // dZmdX1
  dP2dP1[0][1] = -B1 - dYdZ + y1o*dZ - var1*(B1+2.0*dYdZ);  // dZmdY1
  dP2dP1[0][2] = -x1o*dZ + var1*(A2+2.0*dXdZ);              // dZmdX2
  dP2dP1[0][3] = -y1o*dZ + var1*(B2+2.0*dYdZ);              // dZmdY2

  dP2dP1[1][0] =  var2*dX - y2o;                            // dR0dX1
  dP2dP1[1][1] =  var2*dY + x2o;                            // dR0dY1
  dP2dP1[1][2] = -var2*dX + y1o;                            // dR0dX2
  dP2dP1[1][3] = -var2*dY - x1o;                            // dR0dY2

  dP2dP1[2][0] = -(dXdZ + A1)*var3;                         // dThdX1
  dP2dP1[2][1] = -(dYdZ + B1)*var3;                         // dThdY1
  dP2dP1[2][2] =  (dXdZ + A2)*var3;                         // dThdX2
  dP2dP1[2][3] =  (dYdZ + B2)*var3;                         // dThdY2

  dP2dP1[3][0] =  dY*rXY;                                   // dPhdX1
  dP2dP1[3][1] = -dX*rXY;                                   // dPhdY1
  dP2dP1[3][2] = -dY*rXY;                                   // dPhdX2
  dP2dP1[3][3] =  dX*rXY;                                   // dPhdY2
}

void HMdcHitSegFiller::calcDerMatrClus1ToSeg(void) {
  // Calcul. error matrix of HMdcSeg from seg.1 cluster.  
  Double_t dX=x2-x1;
  Double_t dY=y2-y1;
  Double_t dZ=z2-z1;
  Double_t rXY=1./sqrt(dX*dX+dY*dY);
  dX *= rXY;
  dY *= rXY;
  dZ *= rXY;
  Double_t var2=x1*dX+y1*dY;
  Double_t var5=rXY/(1.+dZ*dZ);

  dP2dP1[0][0] = (x2*dX+y2*dY)*rXY;                // dZmdZ1
  dP2dP1[0][1] = 0.;                               // no param.
  dP2dP1[0][2] = (var2*A2+2*dX*(z1-z)-x1*dZ)*rXY;  // dZmdX2
  dP2dP1[0][3] = (var2*B2+2*dY*(z1-z)-y1*dZ)*rXY;  // dZmdY2

  dP2dP1[1][0] = 0.;                               // dR0dZ1 
  dP2dP1[1][1] = 0.;                               // no param.
  dP2dP1[1][2] = (y1-r*dX)*rXY;                    // dR0dX2
  dP2dP1[1][3] =-(x1+r*dY)*rXY;                    // dR0dY2
  
  dP2dP1[2][0] = var5;                             // dThdZ1
  dP2dP1[2][1] = 0.;                               // no param.
  dP2dP1[2][2] = (dZ*dX+A2)*var5;                  // dThdX2
  dP2dP1[2][3] = (dZ*dY+B2)*var5;                  // dThdY2

  dP2dP1[3][0] = 0.;                               // dPhdZ1
  dP2dP1[3][1] = 0.;                               // no param.
  dP2dP1[3][2] =-dY*rXY;                           // dPhdX2
  dP2dP1[3][3] = dX*rXY;                           // dPhdY2
}

// MdcSeg ==> fit param. ------------------------------------------

void HMdcHitSegFiller::setSegParam(HMdcPlane* pl1, HMdcPlane* pl2,
        Double_t zm, Double_t r0, Double_t th, Double_t ph) {
  z=zm;
  r=r0;
  theta=th;
  phi=ph;
  plane1=pl1;
  plane2=pl2;
  A1=pl1->A();
  B1=pl1->B();
  A2=pl2->A();
  B2=pl2->B();
  calcFitPar();
}

void HMdcHitSegFiller::calcFitPar() {
  plane1->calcSegIntersec(z,r,theta,phi,x1,y1,z1);
  plane2->calcSegIntersec(z,r,theta,phi,x2,y2,z2);
}

void HMdcHitSegFiller::calcDerMatrToFit(void) {
  // ne provereno ???????!!!!!!!!!!! i ne uproscheno
  Double_t cosTheta = cos(theta);
  Double_t sinTheta = sin(theta);
  Double_t cosPhi = cos(phi);
  Double_t sinPhi = sin(phi);
  Double_t cl = cosPhi*sinTheta;
  Double_t cm = sinPhi*sinTheta;
  Double_t cn = cosTheta;
  Double_t xM = -r*sinPhi;
  Double_t yM =  r*cosPhi;
  Double_t D1 = plane1->D();
  Double_t D2 = plane2->D();
  Double_t E0 = A1*cl+B1*cm+cn;
  Double_t F0 = D1-(A1*xM+B1*yM+z);
  Double_t EN = A2*cl+B2*cm+cn;
  Double_t FN = D2-(A2*xM+B2*yM+z);

  dP1dP2[0][0] = -cl/E0;
  dP1dP2[1][0] = -cm/E0;
  dP1dP2[2][0] = -cl/EN;
  dP1dP2[3][0] = -cm/EN;
  dP1dP2[0][1] =  -sinPhi + (A1*sinPhi - B1*cosPhi)*cl/E0;
  dP1dP2[1][1] =   cosPhi + (A1*sinPhi - B1*cosPhi)*cm/E0;
  dP1dP2[2][1] =  -sinPhi + (A2*sinPhi - B2*cosPhi)*cl/EN;
  dP1dP2[3][1] =   cosPhi + (A2*sinPhi - B2*cosPhi)*cm/EN;
  dP1dP2[0][2] = F0/(E0*E0)*cosPhi;
  dP1dP2[1][2] = F0/(E0*E0)*sinPhi;
  dP1dP2[2][2] = FN/(EN*EN)*cosPhi;
  dP1dP2[3][2] = FN/(EN*EN)*sinPhi;
  dP1dP2[0][3] = -yM - cm*F0/E0 + cl/E0*((A1*yM-B1*xM) + F0/E0*(A1*cm-B1*cl));
  dP1dP2[1][3] =  xM + cl*F0/E0 + cm/E0*((A1*yM-B1*xM) + F0/E0*(A1*cm-B1*cl));
  dP1dP2[2][3] = -yM - cm*FN/EN + cl/EN*((A2*yM-B2*xM) + FN/EN*(A2*cm-B2*cl));
  dP1dP2[3][3] =  xM + cl*FN/EN + cm/EN*((A2*yM-B2*xM) + FN/EN*(A2*cm-B2*cl));
}

// ----------------------------------------------------------------
void HMdcHitSegFiller::propogErrToSeg(HSymMat4& errMatOut) {
  // propogation of errMatS1 to errMatS2
  calcDerMatrToSeg();
  errMatOut.transform(*covMatr,dP2dP1[0]);
}

void HMdcHitSegFiller::fillMdcSeg(HMdcSeg* mdcSeg, const HGeomVector* targ) {
  calcSegPar(targ);
  calcDerMatrToSeg();
  fillSegPar(mdcSeg);
  mdcSeg->getCovariance().transform(*covMatr,dP2dP1[0]);
  mdcSeg->setXYPar(x1,y1,x2,y2);
}

void HMdcHitSegFiller::fillMdcSegByClus(HMdcSeg* mdcSeg, const HGeomVector* targ,HMdcTrackParam* trPar) {
  //  plane1=plane2=0;
  calcSegPar(targ);
  fillSegPar(mdcSeg);
  if(typeClusVert==0) calcDerMatrClus1ToSeg();
  else                calcDerMatrToSeg();
  mdcSeg->getCovariance().transform(*covMatr,dP2dP1[0]);
  if(trPar != NULL) mdcSeg->setXYPar(trPar->x1(),trPar->y1(),trPar->x2(),trPar->y2());
  else              mdcSeg->setXYPar(x1,y1,x2,y2);  // Cluster parametrisation
}

void HMdcHitSegFiller::fillSegPar(HMdcSeg* mdcSeg) {
  mdcSeg->setPar(z,r,theta,phi);
  mdcSeg->setZprime(zPrime);
  mdcSeg->setRprime(rPrime);
}

void HMdcHitSegFiller::propogErrToFit(HSymMat4& errMatIn, HSymMat4& errMatOut) {
  // propogation of errMatIn to errMatOut
  calcDerMatrToFit();
  errMatOut.transform(errMatIn,dP1dP2[0]);
}

// --- HMdcHit -----------------------------------------
void HMdcHitSegFiller::fillMdcHit(HMdcSizesCellsMod* fSCMod, HMdcHit* mdcHit) {
  // Calcul. a cross of the line with plane MDC (parA*x+parB*y+c*z=parD),
  // transform. this point to mdc coor.sys. (z=0) and calc.
  // hit direction in mdc coor.sys. and matrix errors
  const Double_t* tfSysRSec=fSCMod->getTfSysRSec();
  const Double_t* ct=fSCMod->getMdcHitLookupTb();
  Double_t parA=fSCMod->A();
  Double_t parB=fSCMod->B();
  Double_t parD=fSCMod->D();
  Double_t dX=x2-x1;
  Double_t dY=y2-y1;
  Double_t dZ=z2-z1;
  Double_t del=1/(parA*dX+parB*dY+dZ);
  Double_t cxz1=(parD-z1-parA*x1)*del;
  Double_t cxz2=(parD-z2-parA*x2)*del;
  Double_t cyz1=(parD-z1-parB*y1)*del;
  Double_t cyz2=(parD-z2-parB*y2)*del;
  Double_t xt0=x2*cyz1 - x1*cyz2;
  Double_t yt0=y2*cxz1 - y1*cxz2;
  
  Double_t xt=xt0-tfSysRSec[ 9];
  Double_t yt=yt0-tfSysRSec[10];
  Double_t rXYZ=1/sqrt(dX*dX+dY*dY+dZ*dZ);
  dX *= rXYZ; // unit vector
  dY *= rXYZ;
  dZ *= rXYZ;
  //---Hit parameters----------------------------------------------------
  x=ct[0]*xt+ct[1]*yt+ct[2];
  y=ct[3]*xt+ct[4]*yt+ct[5];
  xDir=tfSysRSec[0]*dX+tfSysRSec[3]*dY+tfSysRSec[6]*dZ;
  yDir=tfSysRSec[1]*dX+tfSysRSec[4]*dY+tfSysRSec[7]*dZ;
  Int_t mod=fSCMod->getMod();
  mdcHit->setPar(x,y,xDir,yDir,timeOffSet[mod]);
  //---------------------------------------------------------------------  
  Double_t x1t=(x1-xt0)*del;
  Double_t y1t=(y1-yt0)*del;
  Double_t xt2=(xt0-x2)*del;
  Double_t yt2=(yt0-y2)*del;
  Double_t xtA=xt0*parA*del;
  Double_t ytB=yt0*parB*del;
  Double_t dXtdX2=-x1t*A2+cyz1-xtA;
  Double_t dYtdY2=-y1t*B2+cxz1-ytB;
  Double_t dXtdX1=-xt2*A1-cyz2+xtA;
  Double_t dYtdY1=-yt2*B1-cxz2+ytB;
  Double_t dXtdY2=x1t*(parB-B2);
  Double_t dYtdX2=y1t*(parA-A2);
  Double_t dXtdY1=xt2*(parB-B1);
  Double_t dYtdX1=yt2*(parA-A1);
  
  dP2dP1[0][0]=ct[0]*dXtdX1+ct[1]*dYtdX1;   // dXdX1
  dP2dP1[0][1]=ct[0]*dXtdY1+ct[1]*dYtdY1;   // dXdY1
  dP2dP1[0][2]=ct[0]*dXtdX2+ct[1]*dYtdX2;   // dXdX2
  dP2dP1[0][3]=ct[0]*dXtdY2+ct[1]*dYtdY2;   // dXdY2
  
  dP2dP1[1][0]=ct[3]*dXtdX1+ct[4]*dYtdX1;   // dYdX1
  dP2dP1[1][1]=ct[3]*dXtdY1+ct[4]*dYtdY1;   // dYdY1
  dP2dP1[1][2]=ct[3]*dXtdX2+ct[4]*dYtdX2;   // dYdX2
  dP2dP1[1][3]=ct[3]*dXtdY2+ct[4]*dYtdY2;   // dYdY2
   
  Double_t c0xx=(tfSysRSec[0]-xDir*dX)*rXYZ;
  Double_t c3xy=(tfSysRSec[3]-xDir*dY)*rXYZ;
  Double_t c6xz=(tfSysRSec[6]-xDir*dZ)*rXYZ;
  Double_t c1yx=(tfSysRSec[1]-yDir*dX)*rXYZ;
  Double_t c4yy=(tfSysRSec[4]-yDir*dY)*rXYZ;
  Double_t c7yz=(tfSysRSec[7]-yDir*dZ)*rXYZ;
  
  dP2dP1[2][0] =-c0xx+c6xz*A1;   // dXDdX1
  dP2dP1[2][1] =-c3xy+c6xz*B1;   // dXDdY1
  dP2dP1[2][2] = c0xx-c6xz*A2;   // dXDdX2
  dP2dP1[2][3] = c3xy-c6xz*B2;   // dXDdY2
  
  dP2dP1[3][0] =-c1yx+c7yz*A1;   // dYDdX1
  dP2dP1[3][1] =-c4yy+c7yz*B1;   // dYDdY1
  dP2dP1[3][2] = c1yx-c7yz*A2;   // dYDdX2
  dP2dP1[3][3] = c4yy-c7yz*B2;   // dYDdY2

  hitErr.transform(*covMatr,dP2dP1[0]);
  HSymMat& cov = mdcHit->getCovariance();
  for(Int_t i=0;i<4;i++) for(Int_t j=i;j<4;j++) cov.setElement(i,j,hitErr(i,j));
  cov.setElement(4,4,errTimeOffSet[mod]*errTimeOffSet[mod]);
}

void HMdcHitSegFiller::fillMdcHitByClus(HMdcSizesCellsMod* fSCMod, 
    HMdcHit* mdcHit) {
  // Calcul. a cross of the line with plane MDC (parA*x+parB*y+c*z=parD),
  // transform. this point to mdc coor.sys. (z=0) and calc.
  // hit direction in mdc coor.sys. and matrix errors
  if(typeClusVert) {
    fillMdcHit(fSCMod, mdcHit);
    return;
  }
  const Double_t* tfSysRSec=fSCMod->getTfSysRSec();
  const Double_t* ct=fSCMod->getMdcHitLookupTb();
  Double_t parA=fSCMod->A();
  Double_t parB=fSCMod->B();
  Double_t parD=fSCMod->D();
  Double_t dX=x2-x1;
  Double_t dY=y2-y1;
  Double_t dZ=z2-z1;
  Double_t del=1/(parA*dX+parB*dY+dZ);
  Double_t cxz1=(parD-z1-parA*x1)*del;
  Double_t cxz2=(parD-z2-parA*x2)*del;
  Double_t cyz1=(parD-z1-parB*y1)*del;
  Double_t cyz2=(parD-z2-parB*y2)*del;
  Double_t xt0=x2*cyz1 - x1*cyz2;
  Double_t yt0=y2*cxz1 - y1*cxz2;
  Double_t xt=xt0-tfSysRSec[ 9];
  Double_t yt=yt0-tfSysRSec[10];
  Double_t rXYZ=1/sqrt(dX*dX+dY*dY+dZ*dZ);
  dX *= rXYZ; // unit vector
  dY *= rXYZ;
  dZ *= rXYZ;
  //---Hit parameters----------------------------------------------------
  x=ct[0]*xt+ct[1]*yt+ct[2];
  y=ct[3]*xt+ct[4]*yt+ct[5];
  xDir=tfSysRSec[0]*dX+tfSysRSec[3]*dY+tfSysRSec[6]*dZ;
  yDir=tfSysRSec[1]*dX+tfSysRSec[4]*dY+tfSysRSec[7]*dZ;
  mdcHit->setPar(x,y,xDir,yDir,timeOffSet[fSCMod->getMod()]);
  //---------------------------------------------------------------------  
  Double_t x1t=(x1-xt0)*del;
  Double_t y1t=(y1-yt0)*del;
  Double_t xtA=xt0*parA*del;
  Double_t ytB=yt0*parB*del;
  Double_t dXtdZ1=(xt0-x2)*del;
  Double_t dYtdZ1=(yt0-y2)*del;
  Double_t dXtdX2=-x1t*A2+cyz1-xtA;
  Double_t dYtdY2=-y1t*B2+cxz1-ytB;
  Double_t dXtdY2=x1t*(parB-B2);
  Double_t dYtdX2=y1t*(parA-A2);
  
  dP2dP1[0][0]=ct[0]*dXtdZ1+ct[1]*dYtdZ1;   // dXdZ1 
  dP2dP1[0][1]=0;                           //
  dP2dP1[0][2]=ct[0]*dXtdX2+ct[1]*dYtdX2;   // dXdX2
  dP2dP1[0][3]=ct[0]*dXtdY2+ct[1]*dYtdY2;   // dXdY2
  
  dP2dP1[1][0]=ct[3]*dXtdZ1+ct[4]*dYtdZ1;   // dYdZ1
  dP2dP1[1][1]=0;                           //
  dP2dP1[1][2]=ct[3]*dXtdX2+ct[4]*dYtdX2;   // dYdX2
  dP2dP1[1][3]=ct[3]*dXtdY2+ct[4]*dYtdY2;   // dYdY2
  
  Double_t c0xx=(tfSysRSec[0]-xDir*dX)*rXYZ;
  Double_t c3xy=(tfSysRSec[3]-xDir*dY)*rXYZ;
  Double_t c6xz=(tfSysRSec[6]-xDir*dZ)*rXYZ;
  Double_t c1yx=(tfSysRSec[1]-yDir*dX)*rXYZ;
  Double_t c4yy=(tfSysRSec[4]-yDir*dY)*rXYZ;
  Double_t c7yz=(tfSysRSec[7]-yDir*dZ)*rXYZ;
  
  dP2dP1[2][0] =-c6xz;           // dXDdZ1
  dP2dP1[2][1] =0;               //
  dP2dP1[2][2] = c0xx-c6xz*A2;   // dXDdX2
  dP2dP1[2][3] = c3xy-c6xz*B2;   // dXDdY2
  
  dP2dP1[3][0] =-c7yz;           // dYDdZ1
  dP2dP1[3][1] =0;               //
  dP2dP1[3][2] = c1yx-c7yz*A2;   // dYDdX2
  dP2dP1[3][3] = c4yy-c7yz*B2;   // dYDdY2

  hitErr.transform(*covMatr,dP2dP1[0]);
  HSymMat& cov = mdcHit->getCovariance();
  for(Int_t i=0;i<4;i++) for(Int_t j=i;j<4;j++) cov.setElement(i,j,hitErr(i,j));
}

ClassImp(HMdcHitSegFiller) // HMdcHitSegFiller
