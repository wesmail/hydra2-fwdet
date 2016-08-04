//*-- Author  : A.Ivashkin <ivashkin@inr.ru>
//*-- Modified: A.Sadovsky <sadovski@fz-rossendorf.de> (04.11.2004 Hydra implementation)
//*-- Last modified : 14/08/2005 by Ilse Koenig

using namespace std;
#include <iostream>
#include <fstream>
#include "hrungekutta.h"
#include "hmdcsizescells.h"
#include <math.h>
#include "TMatrixD.h"
#include "TDecompLU.h"
ClassImp(HRungeKutta)
		
HRungeKutta::HRungeKutta() {

  // constructor
  clear();

  // Eventually these parameters should be stored in a parameter container!
  initialStepSize=50.;
  stepSizeDec=0.75;
  stepSizeInc=1.25;
  maxStepSize=200.;
  minStepSize=10.;
  minPrecision=2.e-04;
  maxPrecision=2.e-05;
  maxNumSteps=1000;
  maxDist=2.5;
  dpar[0]=0.01; dpar[1]=0.001; dpar[2]=0.001; dpar[3]=0.01; dpar[4]=0.01;
  resolution[0]=0.140*2.00;  resolution[1]=0.140*1.00; // MDC1 intrinsic resolution
  resolution[2]=0.140*2.00;  resolution[3]=0.140*1.00; // MDC2
  resolution[4]=0.140*2.00;  resolution[5]=0.140*1.00; // MDC3
  resolution[6]=0.140*2.00;  resolution[7]=0.140*1.00; // MDC4
  for(Int_t i=0;i<8;i++) {sig[i]=resolution[i];}
  mTol=DBL_EPSILON;
}


void HRungeKutta::clear() {
  // clears magnetic field definition and vertex position and direction
  fieldMap=0;
  fieldScalFact=0.;
  for(Int_t m=0; m<4; m++) {
    for(Int_t s=0; s<6; s++) {
      mdcInstalled[m][s]=kFALSE;
      mdcPos[m][s][0]=0.;
      mdcPos[m][s][1]=0.;
      mdcPos[m][s][2]=0.;
      normVecMdc[m][s][0]=0.;
      normVecMdc[m][s][1]=0.;
      normVecMdc[m][s][2]=0.;
    }       
  }       
}


void HRungeKutta::setField(HMdcTrackGField* pField, Float_t fpol) {
  // sets the field map and the scaling factor
  fieldMap=pField;
  fieldScalFact=fpol;
}


void HRungeKutta::setMdcPosition(Int_t s, Int_t m, HGeomTransform& gtrMod2Sec) {
  // sets the MDC module position in the sector coordinate system
  // calculates the normal vector on the planes
  mdcInstalled[m][s]=kTRUE;
  HGeomVector aTransl=gtrMod2Sec.getTransVector();
  mdcPos[m][s][0] = aTransl.getX();
  mdcPos[m][s][1] = aTransl.getY();
  mdcPos[m][s][2] = aTransl.getZ();
  HGeomVector r0_mod(0.0, 0.0, 0.0);
  HGeomVector rz_mod(0.0, 0.0, 1.0);
  HGeomVector norm= gtrMod2Sec.transFrom(rz_mod) - gtrMod2Sec.transFrom(r0_mod);
  norm/=norm.length();  // to be shure it is normalized after transformation
  normVecMdc[m][s][0]=norm.getX();
  normVecMdc[m][s][1]=norm.getY();
  normVecMdc[m][s][2]=norm.getZ();
  if (m==0 || m==1) {
    dzfacMdc[m][s]=tan(acos(norm.getZ()));
    HGeomVector v(0.0, 0.0, -500.0);
    if (m==1 && mdcInstalled[0][s]==kFALSE) v.setZ(-650.);
    HGeomVector vs=gtrMod2Sec.transFrom(v);
    pointForVertexRec[s][0]=vs.getX();   
    pointForVertexRec[s][1]=vs.getY();   
    pointForVertexRec[s][2]=vs.getZ();
  }
}


Bool_t HRungeKutta::fit4Hits(Double_t* u1pos, Double_t* u1dir,
                             Double_t* u2pos, Double_t* u2dir,
                             Float_t* sigMult, Int_t sec, Double_t pGuess) {
  // Input:  position and direction of the two segments u1 and u2,
  //         array with multiplication factors for errors
  //         sector number, initial guess for momentum
  // From the segments the hit positions are calculated.
  // Calls fitMdc for Runge-Kutta track propagation and fitting
  nMaxMod=4;
  ndf = 3; //{x,y}*4mdc=8 - {p,x,y,dx,dy}=5 == 3
  sector=sec;
  p0Guess = pGuess;
  findMdcIntersectionPoint(u1pos,u1dir,0,&hit[0][0]);  // hit = hit position in modules
  findMdcIntersectionPoint(u1pos,u1dir,1,&hit[1][0]);
  findMdcIntersectionPoint(u2pos,u2dir,2,&hit[2][0]);
  findMdcIntersectionPoint(u2pos,u2dir,3,&hit[3][0]);
  for(Int_t i=0;i<4;i++) {
    mdcModules[i]=i;
    mdcLookup[i]=i;
    sig[2*i]=sigMult[2*i]*resolution[2*i];
    sig[2*i+1]=sigMult[2*i+1]*resolution[2*i+1];
  }
  return fitMdc();
}


Bool_t HRungeKutta::fit3Hits(Double_t* u1pos, Double_t* u1dir,
                            Double_t* u2pos, Double_t* u2dir,
                            Float_t* sigMult, Int_t sec, Double_t pGuess) {
  // Input:  position and direction of the two segments u1 and u2
  //         array with multiplication factors for errors
  //         sector number, initial guess for momentum
  // From the segments the hit positions are calculated.
  // If the multiplication factors for errors of a hit -1, the hit is discarded.
  // Calls fitMdc for Runge-Kutta track propagation and fitting
  nMaxMod=3;
  ndf = 1; //{x,y}*3mdc=6 - {p,x,y,dx,dy}=5 == 1
  sector=sec;
  p0Guess = pGuess;
  Int_t m=0;
  // hit = hit position in modules
  for (Int_t i=0;i<2;i++) {
    if (sigMult[2*i]>0) {
      findMdcIntersectionPoint(u1pos,u1dir,i,&hit[m][0]);
      mdcModules[m]=i;
      mdcLookup[i]=m;
      sig[2*m]=sigMult[2*i]*resolution[2*i];
      sig[2*m+1]=sigMult[2*i+1]*resolution[2*i+1];
      m++;
    } else mdcLookup[i]=-1;      
  }
  for (Int_t i=2;i<4;i++) {
    if (sigMult[2*i]>0) {
      findMdcIntersectionPoint(u2pos,u2dir,i,&hit[m][0]);
      mdcModules[m]=i;
      mdcLookup[i]=m;
      sig[2*m]=sigMult[2*i]*resolution[2*i];
      sig[2*m+1]=sigMult[2*i+1]*resolution[2*i+1];
      m++;
    } else mdcLookup[i]=-1;      
  }
  mdcModules[3]=-1;
  return fitMdc();
}


Bool_t HRungeKutta::fitMdc() {
  initMom(); // sets pfit
  if (fabs(pfit)<10.) return kFALSE;	

  Bool_t rc=kTRUE;
  Float_t chi2old=0.F;
  for(Int_t i=0;i<3;i++) fit[0][i]=hit[0][i];  // fit = fitted position in first module
  Float_t dist=distance(&hit[0][0],&hit[1][0]);
  if (dist==0.) { return kFALSE; }
  Float_t uh=(hit[1][0]-hit[0][0])/dist;       // cos(phi)
  Float_t uv=(hit[1][1]-hit[0][1])/dist;       // cos(theta)
  for(Int_t iter=0;iter<11;iter++) {
    polbending=fieldScalFact*pfit;
    chi2=0.;
    parSetNew0(uh,uv,iter);
    if (jstep>=maxNumSteps) { return kFALSE; }
    for(Int_t m=0;m<nMaxMod;m++) {
      for(Int_t i=0;i<2;i++) {
        Float_t chi=(hit[m][i]-fit[m][i])/sig[(m*2)+i];
        chi2+=chi*chi;
      }
    }
    if (finite(pfit)==0||finite(chi2)==0) {
      Warning("HRungeKuttta::fitMdc()","NaN or Inf value received for pFit or chi2...skipped!");
      return kFALSE;
    }
    if (chi2==0.) chi2=0.001;
    if ((TMath::Abs(chi2-chi2old)/chi2)<0.005||iter==10||(iter>5&&chi2old>1.e+06&&chi2>chi2old)) break;

    chi2old=chi2;
    parSetNewVar(uh,uv);
    if (jstep>=maxNumSteps) { return kFALSE; }

    derive(&fitdp[0][0],0);
    derive(&fitdh[0][0],1);	
    derive(&fitdv[0][0],2);
    derive(&fit1h[0][0],3);
    derive(&fit1v[0][0],4);

    for(Int_t i=0;i<nMaxMod;i++) {
      for(Int_t k=0;k<2;k++) {
        ydata[i*2+k]=fit[i][k]-hit[i][k];
      }
    }

    if (!(rc=linSys())) break;
    Double_t dpp=fitpar[0]/pfit;
    if ( dpp>=0) {
      pfit+=fitpar[0]*(1+dpp);
    } else {
      pfit+=fitpar[0]/(1-dpp);
    }
    uh+=fitpar[1];
    uv+=fitpar[2];
    fit[0][0]+=fitpar[3];
    fit[0][1]+=fitpar[4];
    fit[0][2]-=fitpar[4]*dzfacMdc[(mdcModules[0])][sector];
    if (pfit==0.) pfit=10.;
  }
  return rc;
}


void HRungeKutta::findMdcIntersectionPoint(Double_t* upos,Double_t* udir,Int_t mdcN,Double_t* xyz) {
  // calculates the hit points in the MDC module (index mdcN 0..3)
  // input:  posline == pos from track piece 
  // input:  dirline == pos from segment 
  //         mdcN    == module number 0..3
  // output: xyz     == hit position in module
  Double_t* normplane=&normVecMdc[mdcN][sector][0];
  Double_t* pplane=&mdcPos[mdcN][sector][0];
  findIntersectionPoint(upos,udir,pplane,normplane,xyz);
}


void HRungeKutta::initMom() {
  // Here we set initial approximation momentum value
  if (p0Guess!=999999999.) {
    // User supplied momentum guess, so we use it as 0-approximation
    pfit=p0Guess;
  } else {
    // User did not supply a momentum guess. We start from simple-minded kickplane,
    // which needs the tracklet directions on input.
    Double_t dir1[3],dir2[3];
    for(Int_t i=0;i<3;i++) {
      dir1[i]=hit[1][i]-hit[0][i];
      dir2[i]=hit[3][i]-hit[2][i];
    }
    Float_t temp=sqrt((dir1[1]*dir1[1]+dir1[2]*dir1[2])*(dir2[1]*dir2[1]+dir2[2]*dir2[2]));
    if (temp==0.) temp=1.;	
    Float_t cosxi=(dir1[1]*dir2[1]+dir1[2]*dir2[2])/temp;
    if (cosxi> 1.0) cosxi=1;
    if (cosxi<-1.0) cosxi=-1;
    Float_t xi=acos(cosxi);
    Float_t vprod=(dir1[1]*dir2[2]-dir1[2]*dir2[1]);
    temp=sin(xi/2.);
    if (temp==0.) temp=0.001;
    Float_t temp1=TMath::Abs(vprod);
    if (temp1==0.) temp1=0.001;
    pfit=75./(2*temp)*vprod/temp1;
  }
}
		

Float_t HRungeKutta::distance(Double_t* p1,Double_t* p2) {
  // calculates the distance between two points
  Float_t dist=sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+
                    (p1[1]-p2[1])*(p1[1]-p2[1])+
                    (p1[2]-p2[2])*(p1[2]-p2[2]));
  return dist;
}		


void HRungeKutta::parSetNew0(Float_t uh,Float_t uv,Int_t iter) {
  // RK propagation with initial or last fit parameters
  // uv - vertical direction in MDC1
  // uh - horizontal direction
  Double_t upos[3], udir[3];
  for(Int_t i=0;i<3;i++) {
    upos[i]=fit[0][i];     // position of track on MDC1
  }
  cosines(uh,uv,udir);
  if (iter>0) {
    // start with point on plane 
    findMdcIntersectionPoint(upos,udir,mdcModules[0],&fit[0][0]);
    for(Int_t i=0;i<3;i++) { upos[i]=fit[0][i]; }
  }
  for(Int_t i=0;i<3;i++) {
    dirAtFirstMDC[i]=udir[i];  // direction of track on first MDC
  }
  trackLength=0.;
  gentrkNew(pfit,&upos[0],&udir[0],&fit[0][0],1);
}


void HRungeKutta::parSetNewVar(Float_t uh,Float_t uv) {
  // calculates the derivatives (RK propagation with parameter variations)
  // uv - vertical direction
  // uh - horizontal direction in MDC1
  Double_t upos[3], udir[3];
  upos[0]=fit[0][0];
  upos[1]=fit[0][1];
  upos[2]=fit[0][2];
  cosines(uh,uv,udir);
  gentrkNew(pfit+dpar[0],&upos[0],&udir[0],&fitdp[0][0],2); // momentum additive

  upos[0]=fit[0][0];
  upos[1]=fit[0][1];
  upos[2]=fit[0][2];
  cosines(uh+dpar[1],uv,udir);         // x direction additive
  gentrkNew(pfit,&upos[0],&udir[0],&fitdh[0][0],3);

  upos[0]=fit[0][0];
  upos[1]=fit[0][1];
  upos[2]=fit[0][2];
  cosines(uh,uv+dpar[2],udir);         // y direction additive
  gentrkNew(pfit,&upos[0],&udir[0],&fitdv[0][0],4);

  upos[0]=fit[0][0]+dpar[3];           // x position additive
  upos[1]=fit[0][1];
  upos[2]=fit[0][2];
  cosines(uh,uv,udir);
  gentrkNew(pfit,&upos[0],&udir[0],&fit1h[0][0],5);

  upos[0]=fit[0][0];
  upos[1]=fit[0][1]+dpar[4];                   // y position additive
  upos[2]=fit[0][2]-dpar[4]*dzfacMdc[(mdcModules[0])][sector]; // change in the first MDC plane
  cosines(uh,uv,udir);
  gentrkNew(pfit,&upos[0],&udir[0],&fit1v[0][0],6);
}


void HRungeKutta::cosines(Float_t uh, Float_t uv, Double_t* dir) {
  // calculates the direction
  Float_t prod=sqrt(uh*uh+uv*uv);
  if (prod==0.) prod=0.0001;
  if (prod<1.) { 
    dir[0]=uh;
    dir[1]=uv;
    dir[2]=sqrt(1.-prod*prod);
  } else {
    dir[0]=uh/prod;
    dir[1]=uv/prod;
    dir[2]=0.;
  }	
}


void HRungeKutta::derive(Double_t* hitpar, Int_t kind) {
  // calculates the derivative (hit-fit)/variation
  Double_t dp=dpar[kind];
  for(Int_t i=0;i<nMaxMod;i++) {
    Double_t* h=&hitpar[i*3];
    for(Int_t k=0;k<2;k++) {
      dydpar[kind][i*2+k]=(h[k]-fit[i][k])/dp;
    }
  }
}		


Bool_t HRungeKutta::linSys() {
  // solves the system of linear equations and sets the new fitting parameters
  Double_t data[5][5], elements[5];
  Int_t kMax=nMaxMod*2;
  for(Int_t i=0;i<5;i++) {	  
    for(Int_t j=0;j<5;j++) {
      data[j][i]=0.;
      for(Int_t k=0;k<kMax;k++) {
        data[j][i]+=dydpar[j][k]*dydpar[i][k]/(sig[k]*sig[k]);
      } 	 
    }	  	
  }
  for(Int_t i=0;i<5;i++) {
    elements[i]=0.;
    for(Int_t k=0;k<kMax;k++) {
      elements[i]-= ydata[k]*dydpar[i][k]/(sig[k]*sig[k]);
    }
  }
		

  //-------------------------------------------
  // test if matrix is singular
  TMatrixD test(5,5,(Double_t*)data,NULL);
  Double_t sign;
  Int_t nrZeros;
  if(!decompose(test,sign,mTol,nrZeros)||nrZeros>0){return kFALSE;}
  //-------------------------------------------

  TMatrixD m(5,5,(Double_t*)data,NULL);

  TMatrixD b(5,1,(Double_t*)elements,NULL);
  TMatrixD m1(5,1);
  m.Invert(0);
  m1.Mult(m,b);
  
  for(Int_t i=0;i<5;i++) {
    fitpar[i]=m1(i,0);
  }

  return kTRUE;
}


void HRungeKutta::gentrkNew(Float_t p, Double_t* upos, Double_t* udir, Double_t* hitxyz, Int_t kind ) {
  // propages the track through the MDC system
  // p      == momentum
  // upos   == initial position
  // udir   == initial direction
  // hitxyz == hits in chambers
  // kind   == type of variation
  Double_t* pHit=&hitxyz[0];
  for(Int_t i=0;i<3;i++) pHit[i]=upos[i];
  jstep=0;
  Double_t d=0.;	
  Float_t step=initialStepSize;
  Float_t nextStep=step;
  Float_t stepFac=1.;
  for(Int_t nMod=1;nMod<nMaxMod;nMod++) {
    pHit=&hitxyz[nMod*3];
    do {	
      stepFac=rkgtrk(step,p,upos,udir,kind);
      findMdcIntersectionPoint(upos,udir,mdcModules[nMod],pHit);
      d=distance(upos,pHit);
      if (step<nextStep) {  // last step was rest step to chamber
        if (stepFac<1.) nextStep=step*stepFac;
      } else nextStep*=stepFac;
      if (d>nextStep||d<maxDist) step=nextStep;
      else step=d;
    } while (d>=maxDist&&((polbending>=0&&upos[2]<pHit[2])||(polbending<0&&upos[1]<pHit[1]))&&jstep<maxNumSteps);
  }
  if (kind==1) {
    // we store the track position after the last RK step for subsequent propagation
    // to Meta (this step ends near the chamber, while the position on the chamber is
    // stored in fit[3])
    for(Int_t i=0;i<3;i++) {
      posNearLastMDC[i] = upos[i];
      dirAtLastMDC[i] = udir[i]; // track direction
    }
    stepSizeAtLastMDC=nextStep;
  }
}


Float_t HRungeKutta::rkgtrk(Float_t totStep, Float_t p, Double_t* upos, Double_t* udir, Int_t kind) {
  // one step of track tracing from the position upos in the direction of udir
  Float_t  k1[3],k2[3],k3[3],k4[3],hstep,qstep;
  Double_t u1pos[3], u1dir[3], u2pos[3], u2dir[3], ucm[3], b[3];
  Double_t est=0.;
  Float_t step=totStep;
  Float_t stepFac=1.;
  for(Int_t i=0;i<3;i++) ucm[i]=upos[i]/10.;		
  fieldMap->calcField(ucm,b,fieldScalFact);
  rkeqfw(b,p,udir,k1);

  do {
    jstep++;
    hstep=step*0.5;
    qstep=step*step*0.125;

    for(Int_t i=0;i<3;i++) {
      u1pos[i]=upos[i]+hstep*udir[i]+qstep*k1[i];
      u1dir[i]=udir[i]+hstep*k1[i];
    }
    for(Int_t i=0;i<3;i++) ucm[i]=u1pos[i]/10.;		
    fieldMap->calcField(ucm,b,fieldScalFact);
    rkeqfw(b,p,u1dir,k2);

    for(Int_t i=0;i<3;i++) {
      u1dir[i] = udir[i] + hstep*k2[i];
    }
    rkeqfw(b,p,u1dir,k3);	  

    for(Int_t i=0;i<3;i++) {
      u2pos[i]=upos[i]+step*udir[i]+2.*hstep*hstep*k3[i];
      u2dir[i]=udir[i]+step*k3[i];
    }
    for(Int_t i=0;i<3;i++) ucm[i]=u2pos[i]/10.;		
    fieldMap->calcField(ucm,b,fieldScalFact);
    rkeqfw(b,p,u2dir,k4);

    est=0.;
    for(Int_t i=0;i<3;i++) {
      est+=fabs(k1[i]+k4[i]-k2[i]-k3[i])*hstep;
    }
    if (est<minPrecision||step<=minStepSize) {
      break; 
    } else {
      stepFac*=stepSizeDec;
      step*=stepSizeDec;
    } 
  } while (jstep<maxNumSteps);
  for(Int_t i=0;i<3;i++) {
    upos[i]=upos[i]+step*udir[i]+step*step*(k1[i]+k2[i]+k3[i])/6.;
    udir[i]=udir[i]+step*(k1[i]+2.*k2[i]+2.*k3[i]+k4[i])/6.;
  }
  if (kind==1) {
    trackLength+=step;  // calculate track length
  }
  if (est<maxPrecision&&step<maxStepSize) {
    stepFac*=stepSizeInc;
  }
  return stepFac;
}


void HRungeKutta::rkeqfw(Double_t b[3], Float_t p, Double_t* udir, Float_t* duds) {
  // calculates 2nd derivative
  Double_t  con=0.2997925;
  duds[0]=con*(udir[1]*b[2]-udir[2]*b[1])/p;
  duds[1]=con*(udir[2]*b[0]-udir[0]*b[2])/p;
  duds[2]=con*(udir[0]*b[1]-udir[1]*b[0])/p;	
}	
				

void HRungeKutta::traceToVertex(HMdcSizesCells* pMSizesCells) {
  // propogates the track backwards from first MDC till Z=zVertex plane
  // should be called only after RK-minimisation is done
  Float_t step=initialStepSize;
  Float_t stepFac=1.;
  jstep=0;
  Double_t dist=0.;
  Double_t* normplane=&normVecMdc[(mdcModules[0])][sector][0];
  Double_t* pplane=&pointForVertexRec[sector][0];
  Double_t upos[3], udir[3], xyz[3];
  for(Int_t i=0;i<3;i++) {
    upos[i]=fit[0][i];     // track position at first MDC
    udir[i]=-dirAtFirstMDC[i]; // direction (reversed to go back from first MDC to target)
  }
  do {
    findIntersectionPoint(upos,udir,pplane,normplane,&xyz[0]);
    dist=distance(upos,xyz);
    if (dist<maxDist||upos[2]<=xyz[2]) {
      break;
    }
    if (dist<step) { step=dist; }
    stepFac=rkgtrk(step,-pfit,upos,udir,1); // with reversed momentum for correct track curvature
    step*=stepFac;
  } while (jstep<maxNumSteps);
  HMdcSizesCells::calcMdcSeg(upos[0],upos[1],upos[2],
        upos[0]-udir[0],upos[1]-udir[1],upos[2]-udir[2],
        zSeg1,rSeg1,thetaSeg1,phiSeg1);
  HMdcSizesCellsSec& fSCSec=(*pMSizesCells)[sector];
  const HGeomVector& target=fSCSec.getTargetMiddlePoint(); 
  xyz[0]=target.getX();
  xyz[1]=target.getY();
  xyz[2]=target.getZ();
  dist=distance(xyz,upos);
  trackLength+=dist;
  trackLengthAtLastMDC=trackLength;
}


void HRungeKutta::traceToMETA(HGeomVector& metaHit,HGeomVector& metaNormVec,
                              HGeomVector* pathCorrPos,HGeomVector* pathCorrNorm) {
  // propagates the track from the last fitted MDC position till track intersection
  // with META plane (is defined by META-hit and normal vector to rod's plane
  // should be called only after RK-minimisation is done
  trackLength=trackLengthAtLastMDC;
  Double_t  pointOnMETA[3], normVectorOfMETA[3];
  pointOnMETA[0]=metaHit.getX();
  pointOnMETA[1]=metaHit.getY();
  pointOnMETA[2]=metaHit.getZ();
  normVectorOfMETA[0] = metaNormVec.getX();
  normVectorOfMETA[1] = metaNormVec.getY();
  normVectorOfMETA[2] = metaNormVec.getZ();
   
  Float_t step = stepSizeAtLastMDC;
  Float_t stepFac=1.;
  jstep = 0;
  Double_t upos[3], udir[3];
  for(Int_t i=0;i<3;i++) {
    upos[i]=posNearLastMDC[i];   // tracking position near last MDC
    udir[i]=dirAtLastMDC[i];     // and direction
  }
  Double_t dist=0.;
  Bool_t rc=kTRUE;
  do {
    rc=findIntersectionPoint(upos,udir,pointOnMETA,normVectorOfMETA,&trackPosOnMETA[0]);
    if (!rc) break;
    dist=distance(upos,trackPosOnMETA);
    if ((polbending>=0&&upos[2]>trackPosOnMETA[2])||(polbending<0&&upos[1]>trackPosOnMETA[1])) {
      trackLength-=dist;
      break;
    } else if (dist<maxDist) {
      trackLength+=dist;
      break;
    } else if (upos[1]>=2475.||upos[1]<=0.) {  
      trackLength=0.;
      break;
    }
    if (dist<step) { step=dist; }
    stepFac=rkgtrk(step,pfit,upos,udir,1); // we use pfit as value for momentum to propagate
    step*=stepFac;
  } while (jstep<maxNumSteps);
  if (!rc&&jstep>=maxNumSteps) {
    trackLength=0.;
  }
  HMdcSizesCells::calcMdcSeg(upos[0],upos[1],upos[2],
        upos[0]+udir[0],upos[1]+udir[1],upos[2]+udir[2],
        zSeg2,rSeg2,thetaSeg2,phiSeg2);
  if (pathCorrPos!=0&&pathCorrNorm!=0&&trackLength>0.) {
    Double_t p1[3], pn[3], p2[3];
    p1[0]=pathCorrPos->getX();
    p1[1]=pathCorrPos->getY();
    p1[2]=pathCorrPos->getZ();
    pn[0]=pathCorrNorm->getX();
    pn[1]=pathCorrNorm->getY();
    pn[2]=pathCorrNorm->getZ();
    findIntersectionPoint(trackPosOnMETA,udir,p1,pn,&p2[0]);
    trackLength-=distance(trackPosOnMETA,p2);
  }
}


Bool_t HRungeKutta::findIntersectionPoint(Double_t* upos, Double_t* udir,
                                          Double_t* planeR0, Double_t* planeNorm,
                                          Double_t* pointIntersect) {
  // upos, udir - "moving point" current position and movement direction vector
  // planeR0[3] some point on the plane to which intersection is supposed to happen
  // planeNorm[3] perpendicular vector to the plane
  // pointIntersect[3] - output - the intersection point of "moving point" with plane
  Double_t denom = (planeNorm[0]*udir[0] + planeNorm[1]*udir[1] + planeNorm[2]*udir[2]);
  if (denom!=0.0) {
    Double_t t = ((planeR0[0]-upos[0])*planeNorm[0] + 
                  (planeR0[1]-upos[1])*planeNorm[1] + 
                  (planeR0[2]-upos[2])*planeNorm[2]) / denom;
    for(Int_t i=0;i<3;i++) pointIntersect[i] = upos[i] + udir[i]*t;  
    return kTRUE;
  } else {
    Warning("findIntersectionPoint","No intersection point found : (plane || track)");
    return kFALSE;
  }
}


Bool_t HRungeKutta::decompose(TMatrixD &lu,Double_t &sign,Double_t tol,Int_t &nrZeros)
{
    // THis part is copid from ROOT TDecompLU::Decompose() to get
    // rid of error message!
    // Crout/Doolittle algorithm of LU decomposing a square matrix, with implicit partial
    // pivoting.  The decomposition is stored in fLU: U is explicit in the upper triag
    // and L is in multiplier form in the subdiagionals .
    // Row permutations are mapped out in fIndex. fSign, used for calculating the
    // determinant, is +/- 1 for even/odd row permutations. .

  // copied part from  TDecompLU constructor-------

  Int_t  fNIndex = lu.GetNcols();
  Int_t* index = new Int_t[fNIndex];
  memset(index,0,fNIndex*sizeof(Int_t));

  const Int_t kWorkMax = 100; // size of work array's in several routines
  //-----------------------------------------------

  const Int_t     n     = lu.GetNcols();
        Double_t *pLU   = lu.GetMatrixArray();

  Double_t work[kWorkMax];
  Bool_t isAllocated = kFALSE;
  Double_t *scale = work;
  if (n > kWorkMax) {
    isAllocated = kTRUE;
    scale = new Double_t[n];
  }

  sign    = 1.0;
  nrZeros = 0;
  // Find implicit scaling factors for each row
  for (Int_t i = 0; i < n ; i++) {
    const Int_t off_i = i*n;
    Double_t max = 0.0;
    for (Int_t j = 0; j < n; j++) {
      const Double_t tmp = TMath::Abs(pLU[off_i+j]);
      if (tmp > max)
        max = tmp;
    }
    scale[i] = (max == 0.0 ? 0.0 : 1.0/max);
  }

  for (Int_t j = 0; j < n; j++) {
    const Int_t off_j = j*n;
    Int_t i;
    // Run down jth column from top to diag, to form the elements of U.
    for (i = 0; i < j; i++) {
      const Int_t off_i = i*n;
      Double_t r = pLU[off_i+j];
      for (Int_t k = 0; k < i; k++) {
        const Int_t off_k = k*n;
        r -= pLU[off_i+k]*pLU[off_k+j];
      }
      pLU[off_i+j] = r;
    }

    // Run down jth subdiag to form the residuals after the elimination of
    // the first j-1 subdiags.  These residuals divided by the appropriate
    // diagonal term will become the multipliers in the elimination of the jth.
    // subdiag. Find fIndex of largest scaled term in imax.

    Double_t max = 0.0;
    Int_t imax = 0;
    for (i = j; i < n; i++) {
      const Int_t off_i = i*n;
      Double_t r = pLU[off_i+j];
      for (Int_t k = 0; k < j; k++) {
        const Int_t off_k = k*n;
        r -= pLU[off_i+k]*pLU[off_k+j];
      }
      pLU[off_i+j] = r;
      const Double_t tmp = scale[i]*TMath::Abs(r);
      if (tmp >= max) {
        max = tmp;
        imax = i;
      }
    }

    // Permute current row with imax
    if (j != imax) {
      const Int_t off_imax = imax*n;
      for (Int_t k = 0; k < n; k++ ) {
        const Double_t tmp = pLU[off_imax+k];
        pLU[off_imax+k] = pLU[off_j+k];
        pLU[off_j+k]    = tmp;
      }
      sign = -sign;
      scale[imax] = scale[j];
    }
    index[j] = imax;

    // If diag term is not zero divide subdiag to form multipliers.
    if (pLU[off_j+j] != 0.0) {
      if (TMath::Abs(pLU[off_j+j]) < tol)
        nrZeros++;
      if (j != n-1) {
        const Double_t tmp = 1.0/pLU[off_j+j];
        for (Int_t i = j+1; i < n; i++) {
          const Int_t off_i = i*n;
          pLU[off_i+j] *= tmp;
        }
      }
    } else {
      if (isAllocated) delete [] scale;
      delete [] index;
      return kFALSE;
    }
  }

  if (isAllocated)
    delete [] scale;

  delete [] index;
  return kTRUE;
}

