//*--- Author : V.Pechenov
//*--- Modified: 16.08.05 V.Pechenov

using namespace std;
#include <iostream>
#include <iomanip>

#include "hmdcwiredata.h"
#include "hmdccal1sim.h"
#include "hmdcdrifttimepar.h"
#include "hmdctrackparam.h"
#include "hmdcwirefitsim.h"
#include "hmdcclusfitsim.h"
#include "hmdctrackfitter.h"
#include "hmdcclussim.h"
#include "hmatrixcategory.h"
#include "hmdclistcells.h"
#include "hmdctrackdset.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hmdcsizescells.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
//  HMdcWireData
//
//    Class keep one wire data for track fitter.
//
//  HMdcWireArr
//
//    Array of HMdcWireData objects.
//
//////////////////////////////////////////////////////////////////////////////

ClassImp(HMdcWireData)
ClassImp(HMdcWiresArr)

HMdcDriftTimePar* HMdcWireData::pDrTimePar = NULL;

HMdcWireData::HMdcWireData(void) {
  drTimeDist = -1.;
}

void HMdcWireData::setCell(Int_t s, Int_t m, Int_t l, Int_t c, Int_t it, Float_t t, Int_t si) {
  sector      = s;
  module      = m;
  layer       = l;
  cell        = c;
  timeIndx    = it;
  tdcTimeCal1 = t;
  tdcTime     = t;
  fMdcCal1    = 0;
  secInd      = si;
  modInd      = module+4*si;
  setInitVal();
}

void HMdcWireData::setCell(HMdcCal1* cal1, Int_t it, Bool_t isGeant, Int_t si) {
  cal1->getAddress(sector, module,layer,cell);
  timeIndx    = it;
  if(timeIndx==1) tdcTimeCal1 = cal1->getTime1();
  else            tdcTimeCal1 = cal1->getTime2();
  tdcTime     = tdcTimeCal1;
  fMdcCal1    = cal1;
  secInd      = si;
  modInd      = module+4*si;
  setInitVal();
  if(isGeant) {
    HMdcCal1Sim* c=(HMdcCal1Sim*)cal1;
    if(timeIndx==1) {
      geantTrackNum = c->getNTrack1();
      tof           = c->getTof1();
    } else {
      geantTrackNum = c->getNTrack2();
      tof           = c->getTof2();
    }
  } else geantTrackNum = -1;
}

void HMdcWireData::setInitVal(void) {
  weight           = 1.;
  hitStatus        = 1;
  errTdcTime       = 1.;
  oneDErrTdcTime2  = 1.;
  wtNorm           = weight*oneDErrTdcTime2;
  signalTimeOffset = 0.;
  isWireUsed       = kTRUE;
  distanceSign     = 0;
  useInFit         = kTRUE;
}

void HMdcWireData::setUnitWeight(void) {
  // For target scan only!
  if(hitStatus==0) return;
  weight           = useInFit ? 1.:0.;
  errTdcTime       = oneDErrTdcTime2 = 1.;
  wtNorm           = weight*oneDErrTdcTime2;
  signalTimeOffset = 0.;
}

void HMdcWireData::reinitWtSt(void) {
  weight           = 1.;
  hitStatus        = 1;
  isWireUsed       = kTRUE;
  useInFit         = kTRUE;
}

void HMdcWireData::setNegTdcTimeTo0(void) {
  if(tdcTime<0.) tdcTime = 0.;
}

void HMdcWireData::setSignalTimeOffset(Double_t offset) {
  // must be called after setCell !
  signalTimeOffset = offset;
  tdcTime          = tdcTimeCal1-signalTimeOffset;
}

void HMdcWireData::calcTdcErrors(HMdcTrackParam& par) {
  if(hitStatus==0) return;
  Double_t drtime = tdcTime-par.getTimeOffset(modInd);
  if(drtime < 0.) drtime = 0.;
  Double_t dist   = pDrTimePar->calcDistance(sector,module,alpha,drtime);
  if(dist < 0.) dist = 0.;
  errTdcTime      = pDrTimePar->calcTimeErr(sector,module,alpha,dist);
  oneDErrTdcTime2 = 1./(errTdcTime*errTdcTime);
  wtNorm          = weight*oneDErrTdcTime2;
}

void HMdcWireData::calcTdcErrors(void) {
  // Function calculates drift time error by measure tdcTime and impact angle alpha
  // Time offset is not used.
  if(hitStatus==0) return;
  Double_t drtime   = tdcTime < 0. ? 0. : tdcTime;
  Double_t distance = pDrTimePar->calcDistance(sector,module,alpha,drtime);
  if(distance < 0.) distance = 0.;
  errTdcTime        = pDrTimePar->calcTimeErr(sector,module,alpha,distance);
  oneDErrTdcTime2   = 1./(errTdcTime*errTdcTime);
  wtNorm            = weight*oneDErrTdcTime2;
}

Bool_t HMdcWireData::setWeightTo1or0(Double_t maxChi2, Double_t minWeight) {
  isWireUsedPr = isWireUsed;
  if(chi2<maxChi2 || weight>minWeight) {
    setWeightEq1();
    if(hitStatus==1) return kTRUE;
  } else setWeightEq0();
  return kFALSE;
}

void HMdcWireData::setWeightEq1(void) {
  weight     = useInFit ? 1.:0.;
  wtNorm     = weight*oneDErrTdcTime2;
  isWireUsed = kTRUE;
}

void HMdcWireData::setWeightEq0(void) {
  weight     = 0.;
  wtNorm     = 0.;
  isWireUsed = kFALSE;
}

Bool_t HMdcWireData::testWeight1or0(void) {
  return hitStatus!=1 || isWireUsedPr!=isWireUsed;
}

void HMdcWireData::calcDistanceSign(HMdcTrackParam& par) {
  if(hitStatus != 0) distanceSign = pSCLayer->distanceSign(par,cell);
  else distanceSign = 0;
}

void HMdcWireData::calcDriftTime(HMdcTrackParam& par) {
  // calculation of distance to the wire, impact ang. and drift time
  if(hitStatus != 0) {
    distance      = pSCLayer->getImpact(par,cell,alpha,y,z,dirY,dirZ);
//!    driftTime     = pDrTimePar->calcTime(sector,module,alpha,distance);
if(TMath::IsNaN(alpha)) {printf("!!!!!!!!!!! 1. alpha=nan !!!!!!!!!!!!!!!!!\n"); par.printParam();}
    pDrTimeParBin = pDrTimePar->getBinDist(sector,module,alpha,distance);
    driftTime     = pDrTimeParBin->calcTime(alpha,distance);
    dev0          = driftTime - tdcTime;
    if(hitStatus==1) par.addToSumsDevWt(modInd,dev0,wtNorm);
  }
}

void HMdcWireData::calcDriftTimeAndErr(HMdcTrackParam& par) {
  // calculation of distance to the wire, impact ang., drift time
  // and drift time error
  if(hitStatus != 0) {
    distance        = pSCLayer->getImpact(par,cell,alpha,y,z,dirY,dirZ);
//!    driftTime       = pDrTimePar->calcTime(sector,module,alpha,distance);
if(TMath::IsNaN(alpha)) {printf("!!!!!!!!!!! 2. alpha=nan !!!!!!!!!!!!!!!!!\n"); par.printParam();}
    pDrTimeParBin   = pDrTimePar->getBinDist(sector,module,alpha,distance);
    driftTime       = pDrTimeParBin->calcTime(alpha,distance);
    dev0            = driftTime - tdcTime;
    errTdcTime      = pDrTimeParBin->calcTimeErr(alpha,distance);
    oneDErrTdcTime2 = 1./(errTdcTime*errTdcTime);
    wtNorm          = weight*oneDErrTdcTime2;
    if(hitStatus==1) par.addToSumsDevWt(modInd,dev0,wtNorm);
  }
}

void HMdcWireData::calcDriftTimeAndInCell(HMdcTrackParam& par) {
  // calculation of distance to the wire, impact ang. and drift time
  if(hitStatus != 0) {
    inCell        = pSCLayer->getImpact(par,cell,alpha,distance,y,z,dirY,dirZ);
//!    driftTime     = pDrTimePar->calcTime(sector,module,alpha,distance);
if(TMath::IsNaN(alpha)) {printf("!!!!!!!!!!! 3. alpha=nan !!!!!!!!!!!!!!!!!\n"); par.printParam();}
    pDrTimeParBin = pDrTimePar->getBinDist(sector,module,alpha,distance);
    driftTime     = pDrTimeParBin->calcTime(alpha,distance);
    dev0          = driftTime - tdcTime;
    if(hitStatus==1) par.addToSumsDevWt(modInd,dev0,wtNorm);
  }
}

void HMdcWireData::calcDriftTimeAndErrAndInCell(HMdcTrackParam& par) {
  // calculation of distance to the wire, impact ang., drift time
  // and drift time error
  if(hitStatus != 0) {
    inCell          = pSCLayer->getImpact(par,cell,alpha,distance,y,z,dirY,dirZ);
if(TMath::IsNaN(alpha)) {printf("!!!!!!!!!!! 4. alpha=nan !!!!!!!!!!!!!!!!!\n"); par.printParam();}
    pDrTimeParBin   = pDrTimePar->getBinDist(sector,module,alpha,distance);
    driftTime       = pDrTimeParBin->calcTime(alpha,distance);
    dev0            = driftTime - tdcTime;
    errTdcTime      = pDrTimeParBin->calcTimeErr(alpha,distance);
    oneDErrTdcTime2 = 1./(errTdcTime*errTdcTime);
    wtNorm          = weight*oneDErrTdcTime2;
    if(hitStatus==1) par.addToSumsDevWt(modInd,dev0,wtNorm);
  }
}

void HMdcWireData::calcDriftTimeForDeriv(HMdcTrackParam& par) {
  // calculation drift time for derivatives
  if(hitStatus==1) {
    Double_t alphaD;
    Double_t distD = pSCLayer->getImpact(par,cell,alphaD,y,z,dirY,dirZ);
//if(TMath::IsNaN(alphaD)) {printf("!!!!!!!!!!! 5. alphaD=nan !!!!!!!!!!!!!!!!!\n"); par.printParam();}
    driftTimeDer   = pDrTimeParBin->calcTime(alphaD,distD);
    devDer         = driftTimeDer - tdcTime;
    par.addToSumsDevWt(modInd,devDer,wtNorm);
  }
}

Double_t HMdcWireData::calcDriftTimeForAlign(const HGeomVector& p1,const HGeomVector& p2,
                                             Int_t& distSign) {
  // p1,p2 - in ROTATED LAYER COOR.SYS.!!!
  // For calculation of alignment parameters derivatives only
  if(hitStatus != 1) return 0;
  Double_t alphaD;
  Double_t distD  = pSCLayer->getImpactLSys(p1,p2,cell,alphaD,distSign);
if(TMath::IsNaN(alphaD)) {printf("!!!!!!!!!!! 6. alphaD=nan !!!!!!!!!!!!!!!!!\n");}
  return pDrTimeParBin->calcTime(alphaD,distD);
}

void HMdcWireData::fillLookupTableForDer(HMdcTrackParam& par) {
  Int_t parSec = par.getSec();
  if(parSec==sector || parSec<0) fillLookupTableForDer(pSCLayer->getRotLaySysRSec(cell),par);
  else {
    HGeomTransform trans;
    pSCLayer->getRotLSysForOtherSecSys(parSec,cell,trans);
    fillLookupTableForDer(trans,par);
  }
}

void HMdcWireData::fillLookupTableForDer(const HGeomTransform& trans,HMdcTrackParam& par) { 
  const HGeomRotation& rot=trans.getRotMatrix();
  dYdP[0] = rot(1)+rot(7)*par.dZ1dX1();
  dYdP[1] = rot(4)+rot(7)*par.dZ1dY1();
  dYdP[2] = dYdP[3] = 0.;
  dZdP[0] = rot(2)+rot(8)*par.dZ1dX1();
  dZdP[1] = rot(5)+rot(8)*par.dZ1dY1();
  dZdP[2] = dZdP[3] = 0.;
  
  dDirYdP[0] = rot(1)*par.dDirXdX1()+rot(7)*par.dDirZdX1();
  dDirYdP[1] = rot(4)*par.dDirYdY1()+rot(7)*par.dDirZdY1();
  dDirYdP[2] = rot(1)*par.dDirXdX2()+rot(7)*par.dDirZdX2();
  dDirYdP[3] = rot(4)*par.dDirYdY2()+rot(7)*par.dDirZdY2();
  
  dDirZdP[0] = rot(2)*par.dDirXdX1()+rot(8)*par.dDirZdX1();
  dDirZdP[1] = rot(5)*par.dDirYdY1()+rot(8)*par.dDirZdY1();
  dDirZdP[2] = rot(2)*par.dDirXdX2()+rot(8)*par.dDirZdX2();
  dDirZdP[3] = rot(5)*par.dDirYdY2()+rot(8)*par.dDirZdY2();

  for(Int_t k=0; k<4; k++) {
    cd2DsdP2[k]=dDirZdP[k]*dYdP[k]-dDirYdP[k]*dZdP[k];
    Int_t kn=(k<3) ? k-1:k;
    for(Int_t l=0; l<k; l++) {
      Int_t i=kn+l;
      cd2DsdPkdPl[i]=dDirZdP[l]*dYdP[k]-dDirYdP[l]*dZdP[k];
      cdVkdPl[i]=dDirYdP[l]*dDirZdP[k] - dDirZdP[l]*dDirYdP[k];
      cdUkdPl[i]=dDirZdP[l]*dDirZdP[k] + dDirYdP[l]*dDirYdP[k];
    }
  }
}

void HMdcWireData::calcAnalytDeriv(HMdcTrackParam& par,Int_t flag) {
  // calculation of the first and second derivatives
  // flag=0 - grad calculation only
  // flag=1 - calculation first derivatives only
  // flag=2 - calculation first and second derivatives
  // flag=3 - calculation first and second derivatives +  errors
  if(hitStatus!=1) return;
  Double_t dirZY2    = 1./(dirZ*dirZ+dirY*dirY);
  Double_t dirZY     = TMath::Sqrt(dirZY2); 
  if(y*dirZ < z*dirY) dirZY=-dirZY;        // +sign 
  Double_t yDrYzDrZ  = y*dirY+z*dirZ;
  Double_t c2wtNr    = 2.*wtNorm;
  Double_t c2wtNrDev = c2wtNr*dev;
  Double_t radToDeg  = TMath::RadToDeg();
  if(dirY*dirZ <0.) radToDeg=-radToDeg;    // +sign
  
  Double_t cdAl,cdDs,cdAldDs;
  pDrTimeParBin->coeffForDer(alpha,distance,cdAl,cdDs,cdAldDs);
  Double_t cDb=2*cdAldDs;
  
  Double_t u[4],w[4],dDsdP[4],dAldP[4];
  Double_t dirYn=dirY*dirZY2;
  Double_t dirZn=dirZ*dirZY2;

  for(Int_t k=0;k<4;k++) {
    Double_t& dDsdPk=dDsdP[k];
    Double_t& uk=u[k];
    Double_t& dAldPk=dAldP[k];
    Double_t& dTdPk=dTdP[k];
    
    Double_t vk     = dirYn*dDirZdP[k] - dirZn*dDirYdP[k];
    dDsdPk = (dirZ*dYdP[k]-dirY*dZdP[k]+yDrYzDrZ*vk)*dirZY;      // dDist/dPk^2
    dAldPk = vk*radToDeg;                                        // dAlpha/dPk^2
    dTdPk  = cdDs*dDsdPk + cdAl*dAldPk;                          // dTime/dPk
    if(flag!=3) {
      grad[k] += c2wtNrDev*dTdPk;
      if(flag==0) continue;
    } else (*matrH)(k,k) += 4.*wtNorm*dTdPk*dTdPk;
    par.addToTOffsetDer(modInd,k,dTdPk*wtNorm);
    uk=dirZn*dDirZdP[k]+dirYn*dDirYdP[k];
    Double_t dVdP=-2.*vk*uk;                                     // dV/dPk
    w[k]=dirY*dYdP[k]+dirZ*dZdP[k]+y*dDirYdP[k]+z*dDirZdP[k];    // d(yDrYzDrZ)/dPk
    Double_t d2DsdP=(cd2DsdP2[k]+vk*w[k]+yDrYzDrZ*dVdP)*dirZY - 
        dDsdPk*uk;                                               // d2Dist/dPk^2
    Double_t d2AldP=dVdP*radToDeg;                               // d2Alpha/dPk^2
    Double_t d2TdP2=cdDs*d2DsdP+cdAl*d2AldP+cDb*dAldPk*dDsdPk;   // d2Tm/dPk^2
    (*grad2)(k,k) += c2wtNr*(dev*d2TdP2 + dTdPk*dTdPk);
    
    if(flag<2) continue;
    
    Int_t kn=(k<3) ? k-1:k;
    for(Int_t l=0;l<k;l++) {
      Int_t i=kn+l;
      Double_t dVkdPl     = cdVkdPl[i]*dirZY2 - 2.*vk*u[l];    // dVk/dPl
      Double_t d2DsdPkdPl = (cd2DsdPkdPl[i] + w[l]*vk + 
          yDrYzDrZ*dVkdPl)*dirZY - dDsdPk*u[l];                // d2Dist/dPkdPl
      Double_t dUkdPl     = cdUkdPl[i]*dirZY2 - 2.*vk*u[l];    // dUk/dPl
      Double_t dVkdPkdPl  = -2.*(uk*dVkdPl+vk*dUkdPl);         // d2Vk/dPkdPl
      Double_t d2AlPkdPl  = dVkdPkdPl*radToDeg;                // d2Alpha/dPkdPl
      Double_t d2TdPkdPl  = cdDs*d2DsdPkdPl + cdAl*d2AlPkdPl + 
          cdAldDs*(dAldPk*dDsdP[l] + dDsdPk*dAldP[l]);         // d2Tm/dPkdPl
      (*grad2)(k,l) += c2wtNr*(dev*d2TdPkdPl + dTdPk*dTdP[l]);
      if(flag==3) (*matrH)(k,l) += 4.*wtNorm*dTdPk*dTdP[l];
    }
  }
}

Bool_t HMdcWireData::getAnalytDeriv(Float_t* der,HMdcTrackParam* par) {
  // This funct. is used in alignment only & must be called after track seg.fit.
  // "par" - track parameters after track seg.fit.
  // Return the derivatives dTcalc/dPk. k = 0,1,2,3
  // If par==0 - time offset is assumed constant,
  // else - dTof/dPk is added to dTcalc/dPk.
  // Size of "der" must be >=4 !
  if(hitStatus!=1 || der==0 ) return kFALSE;
  if(par) {
    Double_t dTofdP[4];
    par->getTimeOffsetDer(dTofdP);
    for(Int_t k=0;k<4;k++) der[k] = dTdP[k] + dTofdP[k];
  } else for(Int_t k=0;k<4;k++) der[k] = dTdP[k];
  return kTRUE;
}

void HMdcWireData::recalcFunctional(HMdcTrackParam& par) {
  // Recalculation of finctional without calculation of ditances
  // (for the same parameters as before)
  if(hitStatus!=0) {
    dev0 = driftTime - tdcTime;
    if(hitStatus==1) par.addToSumsDevWt(modInd,dev0,wtNorm);
  }
}

void HMdcWireData::calcFunctional(HMdcTrackParam& par) {
  if(hitStatus!=0) {
    dev          = dev0 + par.getTimeOffset(modInd);
    Double_t chi = dev/errTdcTime;
    chi2         = chi*chi;
    if(hitStatus==1) par.addToFunct(chi2*weight,weight);
  }
}

void HMdcWireData::calcFunctionalForDer(HMdcTrackParam& par) {
  if(hitStatus==1) {
    Double_t chi=(devDer+par.getTimeOffset(modInd))/errTdcTime;
    par.addToFunct(chi*chi*weight,weight);
  }
}

void HMdcWireData::calcDevAndFunct(HMdcTrackParam& par) {
  if(hitStatus!=0) {
    dev0         = driftTime - tdcTime;
    dev          = dev0 + par.getTimeOffset(modInd);
    Double_t chi = dev/errTdcTime;
    chi2         = chi*chi;
    if(hitStatus==1) par.addToFunct(chi2*weight,weight);
  }
}

void HMdcWireData::getAddress(Int_t& s,Int_t& m,Int_t& l,Int_t& c,Int_t& it) {
  s  = sector;
  m  = module;
  l  = layer;
  c  = cell;
  it = timeIndx;
}

Bool_t HMdcWireData::removeIfWeightLess(Double_t wtCut) {
  // function return kTRUE if hit is removed
  if(hitStatus==1 && weight<wtCut) {
    removeThisWire();
    return kTRUE;
  }
  return kFALSE;
}

Int_t HMdcWireData::unsetFittedHit(void) {
  if(hitStatus!=1) return 0;
  lInCl->delTime(module*6+layer,cell,timeIndx);
  return 1;
}

void HMdcWireData::getLocation(HLocation& loc) const {
  loc.set(4,sector,module,layer,cell);
}

void HMdcWireData::fillWireFitCont(HMdcWireFit* fWireFit) const {
  // Filling of HMdcWireFit container
  fWireFit->setAddress(sector,module,layer,cell);
  fWireFit->setTimeNum(timeIndx);
  fWireFit->setDev(dev);
  fWireFit->setDriftTime(driftTime);
  fWireFit->setFullTime(tdcTime+dev);
  fWireFit->setCal1Time(tdcTimeCal1);
  fWireFit->setTdcTime(tdcTimeCal1-signalTimeOffset);
  fWireFit->setMinDist(distanceSign==-1 ? -distance : distance);
  fWireFit->setAlpha(alpha);
  fWireFit->setIsInCell(inCell);
  fWireFit->setTdcTimeErr(errTdcTime);
  fWireFit->setWeight((hitStatus==1) ? weight:0.);
  fWireFit->setIsUsedFlag(useInFit);
  fWireFit->setToT(fMdcCal1->getNHits()==2 ? fMdcCal1->getTime2()-fMdcCal1->getTime1() : 0.);
}

void HMdcWireData::fillWireFitSimCont(HMdcWireFit* fWireFit,
    Int_t trackNum) const {
  // Filling of HMdcWireFit container
  if(!fWireFit->isGeant() || fMdcCal1==0) return;
  HMdcCal1Sim    *c = (HMdcCal1Sim*)fMdcCal1;
  HMdcWireFitSim *fWFSim = (HMdcWireFitSim*)fWireFit;
  Int_t track=(timeIndx==1) ? c->getNTrack1() : c->getNTrack2();
  fWFSim->setGeantTrackNum(track);
  fWFSim->setGeantMinDist (timeIndx==1 ? c->getMinDist1(): c->getMinDist2());
  fWFSim->setGeantAlpha   (timeIndx==1 ? c->getAngle1()  : c->getAngle2());
  fWFSim->setDigiTimeErr  (timeIndx==1 ? c->getError1()  : c->getError2());
  fWFSim->setGeantTof     (timeIndx==1 ? c->getTof1()    : c->getTof2());
  fWFSim->setClusFitTrFlag(track==trackNum);
}

void HMdcWireData::removeThisWire(void) {
  // Removing of wire from list of cells and setting hitStatus to -1
  lOutCl->delTime(layer+module*6,cell,timeIndx);
  hitStatus=-1;
}

Bool_t HMdcWireData::removeOneTimeIfItDoubleTime(HMdcWireData* time1) {
  // Removing time1 or time2 from one wire for double hit
  // "this" must be for time2 !
  if(hitStatus!=1 || timeIndx!=2 || lOutCl->getTime(module*6+layer,cell)!=3) return kFALSE;
  if(TMath::Abs(time1->dev) <= TMath::Abs(dev)) removeThisWire();
  else time1->removeThisWire();
  return kTRUE;
}

Bool_t HMdcWireData::removeIfOneDistOutCell(HMdcWireData* wire2) {
  // Removing "this" or wire2 from the same layer in "distance" out of cell
  // return kTRUE if one wire is removed
  if(abs(cell-wire2->cell)<=1 || (inCell && wire2->inCell)) return kFALSE;
  if(wire2->inCell) removeThisWire();
  else if(inCell) wire2->removeThisWire();
  else if(wire2->distance<distance) removeThisWire();
  else wire2->removeThisWire();
  return kTRUE;
}

void HMdcWireData::calcDriftTimeAndFunct(HMdcTrackParam& par) {
  // Calculation of functional without recalculation of time offset!
  // (for investigation of finctional)
  if(hitStatus != 0) {
    distance      = pSCLayer->getImpact(par,cell,alpha,y,z,dirY,dirZ);
//    driftTime      =pDrTimePar->calcTime(sector,module,alpha,distance);
if(TMath::IsNaN(alpha)) {printf("!!!!!!!!!!! 7. alpha=nan !!!!!!!!!!!!!!!!!\n"); par.printParam();}
    pDrTimeParBin = pDrTimePar->getBinDist(sector,module,alpha,distance);
    driftTime     = pDrTimeParBin->calcTime(alpha,distance);
    dev0          = driftTime - tdcTime;
    dev           = dev0 + par.getTimeOffset(modInd);
    Double_t chi  = dev/errTdcTime;
    chi2          = chi*chi;
    if(hitStatus==1) par.addToFunct(chi2*weight,weight);
  }
}

void HMdcWireData::calcDTdPar(Int_t par, Double_t oneDv2Step) {
  if(hitStatus!=1) return;
  dTdPar[par] -= driftTimeDer;
  dTdPar[par] *= oneDv2Step;
}

void HMdcWireData::printIfDeleted(void) const {
  if(weight>0.01 && hitStatus<0) printf("Warn.!  HIT %i is deleted\n",sequentialIndex);
}

void HMdcWireData::subtractWireOffset(HMdcTrackParam& par, Double_t sigSpeed) {
  // Subtraction of wire time offset
//! Function getSignPath(,,,,,,,Float_t outside) can be used for fake finding
  signalTimeOffset = pSCLayer->getSignPath(par,cell) * sigSpeed;
  tdcTime          = tdcTimeCal1-signalTimeOffset;
}

void HMdcWireData::addToTOffsetErr(HMdcTrackParam& par) {
  if(hitStatus==1) par.addToTOffsetErr(modInd,dTdPar,wtNorm);
}

void HMdcWireData::printTime(Int_t iflag,HMdcTrackParam& par,Bool_t isGeant) {
  calcDistanceSign(par);
  printf("%c%2i)%c %iS%iM%iL%3iC %+5.2fmm",(iflag==2) ? '!':' ',sequentialIndex,
      (hitStatus==1)?'+':'-',sector+1,module+1,layer+1,cell+1,distance*distanceSign);
  if(iflag==2) printf(" %c",inCell ? 'I':'O');
  printf(" dev=%5.1f%+4.1f%+6.1f=%5.1fns dT=%4.1f chi2=%6.1f WT=%-5.3g",
      driftTime,par.getTimeOffset(modInd),-tdcTime,dev,errTdcTime,chi2,weight);

  if(geantTrackNum>0) {
    printf("%5i trk. TOF=%.2f",geantTrackNum,tof);
    HMdcCal1Sim* cal1 = (HMdcCal1Sim*)fMdcCal1;
    printf(" MD=%5.2fmm",cal1->getMinDist1());
  }
  printf("\n");
}

Int_t HMdcWireData::setTukeyWeight(Double_t cwSig,Double_t c2Sig,Double_t c4Sig,
    Double_t tukeyScale, Double_t& sumWt, Double_t& funct) {
  if(hitStatus==0) return 0;
  Double_t sChi2 = chi2*tukeyScale;
  if(hitStatus!=1 || chi2 >= c2Sig || !useInFit) {
    weight = 0.;
    wtNorm = 0.;
    return 0;
  }
  if(sChi2 < cwSig) {
    weight = sChi2/c4Sig;
    weight = 1. - weight*weight;
  } else weight = 1. - sChi2/c2Sig;
  weight *= weight;
  wtNorm  = weight*oneDErrTdcTime2;
  sumWt  += weight;
  funct  += chi2*weight;
  return 1;
}

Int_t HMdcWireData::setTukeyWeight(Double_t cwSig,Double_t c2Sig,Double_t c4Sig,
    Double_t tukeyScale) {
  if(hitStatus==0) return 0;
  Double_t sChi2 = chi2*tukeyScale;
  if(hitStatus!=1 || chi2 >= c2Sig || !useInFit) {
    weight = 0.;
    wtNorm = 0.;
    return 0;
  }
  if(sChi2 < cwSig) {
    weight = sChi2/c4Sig;
    weight = 1. - weight*weight;
  } else weight = 1. - sChi2/c2Sig;
  weight *= weight;
  wtNorm  = weight*oneDErrTdcTime2;
  return 1;
}

// Bool_t HMdcWireData::testTukeyWeight(Double_t cwSig,Double_t c2Sig,
//     Double_t c4Sig, Double_t maxChi2, Double_t minWeight) {
//   if(hitStatus!=1 || weight>0.5) return kFALSE;
//   if(chi2 >= c2Sig || chi2>maxChi2) return kFALSE;
//   Double_t wt=0.;
//   if(chi2 < cwSig) {
//     wt=chi2/c4Sig;
//     wt = 1. - wt*wt;
//   } else wt = 1. - chi2/c2Sig;
//   wt*=wt;
//   if(wt>minWeight) return kFALSE;
//   weight=1.;
//   wtNorm=oneDErrTdcTime2;
//   return kTRUE;
// }

// Bool_t HMdcWireData::testTukeyWeight(Double_t cwSig,Double_t c2Sig,
//     Double_t c4Sig, Double_t maxChi2, Double_t minWeight) {
//   if(hitStatus==0) return kFALSE;
//   Double_t wtOld=weight;
// //  if(hitStatus!=1 || weight>0.5) return kFALSE;
//   if(chi2 >= c2Sig || chi2>maxChi2) return kFALSE;
//   Double_t wt=0.;
//   if(chi2 < cwSig) {
//     wt=chi2/c4Sig;
//     wt = 1. - wt*wt;
//   } else wt = 1. - chi2/c2Sig;
//   wt*=wt;
//   if(wt>minWeight && wtOld<0.5) {
//     wtNorm=oneDErrTdcTime2;
//     weight=1.;
// printf("add wire %iM %iL %iC\n",module+1,layer+1,cell+1);
//     return kFALSE;
//   } else if(wt<minWeight && wtOld>0.5) {
//     weight=wtNorm=0.;
// printf("remove wire %iM %iL %iC\n",module+1,layer+1,cell+1);
//     return kFALSE;
//   }
//   return kTRUE;
// }

Bool_t HMdcWireData::isAddressEq(Int_t s, Int_t m, Int_t l, Int_t c) {
  if(s>=0 && s!=sector) return kFALSE;
  if(m>=0 && m!=module) return kFALSE;
  if(l>=0 && l!=layer)  return kFALSE;
  if(c>=0 && c!=cell)   return kFALSE;
  return kTRUE;
}

void HMdcWireData::calcLeftRight(HMdcTrackParam& par) {
  // This method is used for calcInitialValue only.
  
   const HGeomVector* p1 = (*pSCLayer)[cell].getWirePoint(0);
   const HGeomVector* p2 = (*pSCLayer)[cell].getWirePoint(1);

   const HGeomVector& Vt = par.getDir();
   dir = *p2 - *p1;
   dir /= dir.length();
   HGeomVector V = Vt.vectorProduct(dir);
   V /= V.length();
   Double_t alpha    = pSCLayer->getAlpha(par,cell);
   Double_t drtime   = tdcTime < 0. ? 0. : tdcTime;
   Double_t distance = pDrTimePar->calcDistance(sector,module,alpha,drtime);
   if(distance < 0.) distance = 0.;
   else if(pSCLayer->getMaxDriftDist() < distance) distance = pSCLayer->getMaxDriftDist();
   V *= distance;
   pL = *p1 - V;
   pR = *p1 + V;
drTimeDist = distance;
}

Double_t HMdcWireData::dDist(const HMdcTrackParam& par) const {
  // This method is used for calcInitialValue only.
  return TMath::Abs(pSCLayer->getDist(par,cell) - drTimeDist);
}

//-----------------------------------------------------------

HMdcWiresArr::HMdcWiresArr(void) {
  arraySize = 0;
  wires     = NULL;
  fClst1    = NULL;
  fClst2    = NULL;
  fClst3    = NULL;
  fClst4    = NULL;
  fClst5    = NULL;
  fClst6    = NULL;
  fClst7    = NULL;
  fClst8    = NULL;
  setNumDriftTimes(500);
  fitInOut  = NULL;
  sector2   = -1;
  sector3   = -1;
  sector4   = -1;
  locCal1.set(4,0,0,0,0);
  dDistCut  = 1.8;
  meanDDist = -1.;
  event     = HMdcEvntListCells::getExObject();
}

HMdcWiresArr::~HMdcWiresArr(void) {
  if(wires) delete [] wires;
  wires = NULL;
  HMdcEvntListCells::deleteCont();
}

void HMdcWiresArr::setNumDriftTimes(Int_t nDrTm) {
  nDriftTimes = nDrTm;
  if(nDriftTimes>arraySize) {
    if(wires) delete [] wires;
    arraySize = arraySize<=0 ? nDriftTimes : (Int_t)(1.3*nDriftTimes);
    wires = new HMdcWireData [arraySize];
    for(Int_t i=0;i<arraySize;i++) {
      wires[i].setSequentialIndex(i);
      wires[i].setGradP(grad,&grad2,&matrH);
    }
  }
}

Bool_t HMdcWiresArr::fillListHits(HMdcClus* fCl1, HMdcClus* fCl2) {
  // Filling list of wires from cluster(s)
  setListCells(fCl1,fCl2);
  if(getNLayOrientInput()<3) return kFALSE;
  nClTimes      = 0;
  nDrTmFirstSec = 0;
  if(sector  >=0 && !fillListHits(0,sector, lInputCells, lOutputCells )) return kFALSE;
  if(nDriftTimes != nClTimes) return kFALSE;
  return kTRUE;
}

Bool_t HMdcWiresArr::fillListHits(HMdcClus* fCl1, HMdcClus* fCl2,HMdcClus* fCl3, HMdcClus* fCl4,
                                  HMdcClus* fCl5, HMdcClus* fCl6,HMdcClus* fCl7, HMdcClus* fCl8) {
  // Filling list of wires from cluster(s)
  setListCells(fCl1,fCl2,fCl3,fCl4,fCl5,fCl6,fCl7,fCl8);
  if(getNLayOrientInput()<3) return kFALSE;
  nClTimes      = 0;
  nDrTmFirstSec = 0;
    
  if(sector  >=0 && !fillListHits(0,sector, lInputCells, lOutputCells )) return kFALSE;
  if(sector2 >=0 && !fillListHits(1,sector2,lInputCells2,lOutputCells2)) return kFALSE;
  if(sector3 >=0 && !fillListHits(2,sector3,lInputCells3,lOutputCells3)) return kFALSE;
  if(sector4 >=0 && !fillListHits(3,sector4,lInputCells4,lOutputCells4)) return kFALSE;
 
  if(nDriftTimes != nClTimes) return kFALSE;
  return kTRUE;
}

Bool_t HMdcWiresArr::fillListHits(Int_t isec,Int_t sec,HMdcList24GroupCells& lInCells,
                                                       HMdcList24GroupCells& lOutCells) {
  if(sec<0) return kTRUE;
  if(isec == 0)  nDrTmFirstSec = 0;
  HCategory          *fCal1Cat = fitInOut->getMdcCal1Cat();
  HMdcSizesCellsMod **fSCModAr = fitInOut->getSCellsModArr(sec);
  locCal1[0] = sec;
  for(Int_t layer=0;layer<24;layer++) {
    Int_t ilay = layer + 24*isec;
    Int_t nDrTimes = lInCells.getNDrTimes(layer);
    if(nDrTimes<=0) {
      firstWireInLay[ilay] = NULL;
      lastWireInLay[ilay]  = NULL;
      continue;
    }
    firstWireInLay[ilay] = wires+nClTimes;
    lastWireInLay[ilay]  = wires+nClTimes+nDrTimes-1;
    Int_t modr = layer/6;
    Int_t lay  = layer%6;
    locCal1[1] = modr;
    locCal1[2] = lay;
    Int_t cell = -1;
    if(fSCModAr[modr]==0) return kFALSE;
    HMdcSizesCellsLayer& fSCLay=(*(fSCModAr[modr]))[lay];
    Bool_t excludeLay = fitInOut->isLayerExcluded(sec,modr,lay);
    while((cell=lInCells.next(layer,cell)) > -1) if(fSCLay[cell].getStatus()) {
      Int_t nTimes  = lInCells.getTime(layer,cell);
      locCal1[3]    = cell;
      HMdcCal1* cal = (HMdcCal1*)fCal1Cat->getObject(locCal1);
      if(!cal) {
        Error("fillListHits","S.%i M.%i L.%i  Cell %i is not fired",sec+1,modr+1,lay+1,cell+1);
        return kFALSE;
      }
      for(Int_t time=1; time<3; time++) {
        if((time&nTimes) == 0) continue;
        wires[nClTimes].setCell(cal,time,fitInOut->isGeant(),isec);
        wires[nClTimes].setSizesCellsLayer(&fSCLay);
        wires[nClTimes].setCellsLists(&lInCells,&lOutCells);
        if(excludeLay) wires[nClTimes].excludeWire(); // must be after setCell!!!
        nClTimes++;
        if(isec==0) nDrTmFirstSec++;
      }
    }
  }
  return kTRUE;
}

void HMdcWiresArr::setNegTdcTimeTo0(void) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->setNegTdcTimeTo0(); 
}

Bool_t HMdcWiresArr::fillListHits(HMdcEvntListCells* store, HMdcClus* fCl1, HMdcClus* fCl2) {
  // Filling list of wires from cluster(s),
  // drift times from HMdcEvntListCells
  setListCells(fCl1,fCl2);
  HMdcSecListCells& storeSec=(*store)[sector];
  HMdcSizesCellsMod** fSCModAr=fitInOut->getSCellsModArr(sector);
  Int_t nClTms=0;
  for(Int_t layer=0;layer<24;layer++) {
    Int_t nDrTimes = lInputCells.getNDrTimes(layer);
    if(nDrTimes<=0) {
      firstWireInLay[layer] = NULL;
      lastWireInLay[layer]  = NULL;
      continue;
    }
    firstWireInLay[layer] = wires+nClTms;
    lastWireInLay[layer]  = wires+nClTms+nDrTimes-1;
    Int_t modr = layer/6;
    Int_t lay  = layer%6;
    Int_t cell = -1;
    if(fSCModAr[modr]==0) return kFALSE;
    HMdcSizesCellsLayer &fSCLay   =(*(fSCModAr[modr]))[lay];
    HMdcLayListCells    &storeLay = storeSec[modr][lay];
    Bool_t excludeLay = fitInOut->isLayerExcluded(sector,modr,lay);
    while((cell=lInputCells.next(layer,cell)) > -1)  if(fSCLay[cell].getStatus()) {
      if(lInputCells.getTime(layer,cell) == 0) continue;
      wires[nClTms].setCell(sector,modr,lay,cell,1,storeLay.getTimeValue(cell)); // index of time =1 !!!!???
      wires[nClTms].setSizesCellsLayer(&fSCLay);
      wires[nClTms].setCellsLists(&lInputCells,&lOutputCells);
      if(excludeLay) wires[nClTms].excludeWire(); // must be after setCell!!!
      nClTms++;
    }
  }
  if(nDriftTimes != nClTms) return kFALSE;
  return kTRUE;
}

void HMdcWiresArr::setListCells(HMdcClus* fCl1, HMdcClus* fCl2, HMdcClus* fCl3, HMdcClus* fCl4,
                                HMdcClus* fCl5, HMdcClus* fCl6, HMdcClus* fCl7, HMdcClus* fCl8) {
  sigma2   = -1.;
  fClst1   = fCl1;
  fClst2   = fCl2;
  sector   = fClst1->getSec();
  segment  = fClst1->getIOSeg();
  dDistCut = HMdcTrackDSet::getCalcInitValueCut(segment);
  if(fClst2 != 0) segment = 3;
  if(segment==0)      lInputCells.set(fClst1,0);
  else if(segment==1) lInputCells.set(0,fClst1);
  else                lInputCells.set(fClst1,fClst2);
  xClst        = fClst1->getX();
  yClst        = fClst1->getY();
  zClst        = fClst1->getZ();
  lOutputCells = lInputCells;
  nDriftTimes  = 0;
  for(Int_t m=0;m<4;m++) nDriftTimes += nMdcTimes[m] = lInputCells.getNDrTimesMod(m);
  
  sector2 = -1;
  sector3 = -1;
  sector4 = -1;
  if(fCl3 != NULL) {
    // For cosmic:
    fClst3  = fCl3;
    fClst4  = fCl4;
    fClst5  = fCl5;
    fClst6  = fCl6;
    fClst7  = fCl7;
    fClst8  = fCl8;
    sector2  = fClst3->getSec();
    segment2 = fClst3->getIOSeg();
    if(fClst4 != NULL) segment2 = 3;
    if(segment2==0)      lInputCells2.set(fClst3,0);
    else if(segment2==1) lInputCells2.set(0,fClst3);
    else lInputCells2.set(fClst3,fClst4);
    lOutputCells2 = lInputCells2;
    for(Int_t m=0;m<4;m++) {
      nMdcTimes[m+4] = lInputCells2.getNDrTimesMod(m);
      nDriftTimes   += nMdcTimes[m+4];
    }
    //+++++++++++++:
    if(fClst5 != NULL) {
      sector3  = fClst5->getSec();
      segment3 = fClst5->getIOSeg();
      if(fClst6 != NULL) segment3 = 3;
      if(segment3==0)      lInputCells3.set(fClst5,0);
      else if(segment3==1) lInputCells3.set(0,fClst5);
      else lInputCells3.set(fClst5,fClst6);
      lOutputCells3 = lInputCells3;
      for(Int_t m=0;m<4;m++) {
        nMdcTimes[m+8] = lInputCells3.getNDrTimesMod(m);
        nDriftTimes   += nMdcTimes[m+8];
      }
      if(fClst7 != NULL) {
        sector4  = fClst7->getSec();
        segment4 = fClst7->getIOSeg();
        if(fClst8 != NULL) segment4 = 3;
        if(segment4==0)      lInputCells4.set(fClst7,0);
        else if(segment4==1) lInputCells4.set(0,fClst7);
        else lInputCells4.set(fClst7,fClst8);
        lOutputCells4 = lInputCells4;
        for(Int_t m=0;m<4;m++) {
          nMdcTimes[m+12] = lInputCells4.getNDrTimesMod(m);
          nDriftTimes    += nMdcTimes[m+12];
        }
      }
    }
      //+++++++++++++.
  }
  
  setNumDriftTimes(nDriftTimes);
  firstTime = 0;
  lastTime  = nDriftTimes;
  firstWire = wires;
  lastWire  = wires+nDriftTimes-1;
}

Bool_t HMdcWiresArr::fillListHits(HMdcEvntListCells* store) {
  // Filling list of wires from cluster(s),
  // drift times from HMdcEvntListCells
  setListCells(store);
  HMdcSecListCells& storeSec=(*store)[sector];
  HMdcSizesCellsMod** fSCModAr=fitInOut->getSCellsModArr(sector);
  Int_t nClTms = 0;
  for(Int_t layer=0;layer<24;layer++) {
    Int_t nDrTimes = lInputCells.getNDrTimes(layer);
    if(nDrTimes<=0) {
      firstWireInLay[layer] = NULL;
      lastWireInLay[layer]  = NULL;
      continue;
    }
    firstWireInLay[layer] = wires+nClTms;
    lastWireInLay[layer]  = wires+nClTms+nDrTimes-1;
    Int_t modr = layer/6;
    Int_t lay  = layer%6;
    Int_t cell = -1;
    if(fSCModAr[modr]==0) return kFALSE;
    HMdcSizesCellsLayer& fSCLay=(*(fSCModAr[modr]))[lay];
    HMdcLayListCells& storeLay=storeSec[modr][lay];
    Bool_t excludeLay = fitInOut->isLayerExcluded(sector,modr,lay);
    while((cell=lInputCells.next(layer,cell)) > -1) if(fSCLay[cell].getStatus()) {
      if(lInputCells.getTime(layer,cell) == 0) continue;
      wires[nClTms].setCell(sector,modr,lay,cell,1,storeLay.getTimeValue(cell)); // index of time =1 !!!!???
      wires[nClTms].setSizesCellsLayer(&fSCLay);
      wires[nClTms].setCellsLists(&lInputCells,&lOutputCells);
      if(excludeLay) wires[nClTms].excludeWire(); // must be after setCell!!!
      nClTms++;
    }
  }
  if(nDriftTimes != nClTms) return kFALSE;
  return kTRUE;
}

void HMdcWiresArr::setListCells(HMdcEvntListCells* store) {
  Int_t s,m,l,c;
  s=m=l=c=-1;
  UChar_t t   = 0;
  nDriftTimes = 0;
  sector2     = -1;
  lInputCells.clear();
  while(store->nextCell(s, m, l, c, t)) {
    sector = s;
    lInputCells.setTime(m*6+l,c,t);
  }
  for(Int_t m=0;m<4;m++) nDriftTimes += nMdcTimes[m] = lInputCells.getNDrTimesMod(m);
  if(nMdcTimes[2]+nMdcTimes[3] == 0)      segment = 0;
  else if(nMdcTimes[0]+nMdcTimes[1] == 0) segment = 1;
  else                                    segment = 3;  
  setNumDriftTimes(nDriftTimes);
  nDrTmFirstSec = nDriftTimes;
  firstTime     = 0;
  lastTime      = nDriftTimes;
  firstWire     = wires;
  lastWire      = wires+nDriftTimes-1;
  lOutputCells  = lInputCells;
}

void HMdcWiresArr::fillLookupTableForDer(HMdcTrackParam& par) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->fillLookupTableForDer(par);
}
    
Double_t HMdcWiresArr::testFitResult(void) {
  // Removing cells which has small weight or
  // if min.distance outside of this cell for case when more than on wires
  // in this layer passed fit and so on
// Can be tuned and checked ???).
  Double_t sumDelWeight=0.;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
    // Test for Tukey weight:
    if(w->removeIfWeightLess(fitInOut->getWeightCut())) {
        sumDelWeight += w->getWeight();
        numOfGoodWires--;
      //Test on double time (time==2 - weight are tested already):
    } else if(w>firstWire && w->removeOneTimeIfItDoubleTime(w-1)) {
      sumDelWeight += 1.;  // +=weight[dHit] ???
      numOfGoodWires--;
    }
  }

  // Test for layers which have > 2 wires:
// !!!!!!!! Nado perepisat' i sdelat' proverku tol'ko na inCell !!!
  Int_t l1 = firstWire->getSequentialLayNum();
  Int_t l2 =  lastWire->getSequentialLayNum();
  for(Int_t lay=l1;lay<=l2;lay++) {
    HMdcWireData* fWire = firstWireInLay[lay];
    if(fWire==0) continue;
    HMdcWireData* lWire = lastWireInLay[lay];
    while(fWire<lWire) {
      if(fWire->getHitStatus()==1) {
        if(lWire->getHitStatus()==1) {
          if(fWire->removeIfOneDistOutCell(lWire)) {
            sumDelWeight+=1.;
            numOfGoodWires--;
          } else break;
        } else lWire--;
      } else fWire++;
    }
  }
  if(fprint) for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->printIfDeleted();
  return sumDelWeight;
}

void HMdcWiresArr::subtractWireOffset(HMdcTrackParam& par) {
  // Must be called after setCell !
  if(!fitInOut->useWireOffset()) return;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++)
      w->subtractWireOffset(par,fitInOut->getSignalSpeed());
}

void HMdcWiresArr::setHitStatM1toP1(void) {
  // if(hitStatus==-1) hitStatus=1
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->setHitStatM1toP1();
}

void HMdcWiresArr::switchOff(Int_t sec, Int_t mod, Int_t lay, Int_t cell) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++)
    if(w->isAddressEq(sec,mod,lay,cell)) {
      w->setHitStatus(-2);
      w->removeThisWire();
  }
}

Int_t HMdcWiresArr::unsetHits(void) {
  // removing cells passed fit
  Int_t nFitedTimes=0;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++)
      nFitedTimes +=w->unsetFittedHit();
  return nFitedTimes;
}

Int_t HMdcWiresArr::getNCellsInInput(Int_t mod,Int_t isec) {
  // Return number of cells in mdc module =mod in input list
  // if mod<0 return number of cells in all mdc's of segment
  // isec - sector index (0 or 1) for cosmic data only
  // isec<0 && mod<0 return number of cells in all mdc's of two sectors
  if(mod<0) {
    if(isec < 0) {
      Int_t            nCells  = lInputCells.getNCells();
      if(sector2 >= 0) nCells += lInputCells2.getNCells();
      if(sector3 >= 0) nCells += lInputCells3.getNCells();
      if(sector4 >= 0) nCells += lInputCells4.getNCells();
      return nCells;
    } else {
      if(isec==0)                 return lInputCells.getNCells();
      if(isec==1 && sector2 >= 0) return lInputCells2.getNCells();
      if(isec==2 && sector3 >= 0) return lInputCells3.getNCells();
      if(isec==3 && sector4 >= 0) return lInputCells4.getNCells(); 
    }
  }
  if(isec==0)                 return lInputCells.getNCellsMod(mod);
  if(isec==1 && sector2 >= 0) return lInputCells2.getNCellsMod(mod);
  if(isec==2 && sector3 >= 0) return lInputCells3.getNCellsMod(mod);
  if(isec==3 && sector4 >= 0) return lInputCells4.getNCellsMod(mod); 
  return 0;
}

UChar_t HMdcWiresArr::getNLayOrientInput(void) {
  // Return list of layer: 6 bits, one bit per layer
  UChar_t listLayers = 0;
  for(Int_t mod=0;mod<4;mod++) {
    listLayers                |= lInputCells.getListLayers(mod);
    if(sector2>=0) listLayers |= lInputCells2.getListLayers(mod);
    if(sector3>=0) listLayers |= lInputCells3.getListLayers(mod);
    if(sector4>=0) listLayers |= lInputCells4.getListLayers(mod);
  }
  return HMdcBArray::getNLayOrientation(listLayers);
}

UChar_t HMdcWiresArr::getNLayOrientOutput(void) {
  // Return list of layer: 6 bits, one bit per layer
  UChar_t listLayers = 0;
  for(Int_t mod=0;mod<4;mod++) {
    listLayers                |= lOutputCells.getListLayers(mod);
    if(sector2>=0) listLayers |= lOutputCells2.getListLayers(mod);
    if(sector3>=0) listLayers |= lOutputCells3.getListLayers(mod);
    if(sector4>=0) listLayers |= lOutputCells4.getListLayers(mod);
  }
  return HMdcBArray::getNLayOrientation(listLayers);
}

Int_t HMdcWiresArr::getNCellsInOutput(Int_t mod,Int_t isec) {
  // Return valid value after calling testFitResult() only !
  // Return number of cells in mdc module =mod passed fit
  // if mod<0 return number of cells in all mdc's of segment
  if(mod<0) {
    if(isec < 0) {
      Int_t            nCells = lOutputCells.getNCells();
      if(sector2 >= 0) nCells += lOutputCells2.getNCells();
      if(sector3 >= 0) nCells += lOutputCells3.getNCells();
      if(sector4 >= 0) nCells += lOutputCells4.getNCells();
      return nCells;
    } else {
      if(isec==0)                 return lOutputCells.getNCells();
      if(isec==1 && sector2 >= 0) return lOutputCells2.getNCells();
      if(isec==2 && sector3 >= 0) return lOutputCells3.getNCells();
      if(isec==3 && sector4 >= 0) return lOutputCells4.getNCells(); 
    }
  }
  if(isec==0)                 return lOutputCells.getNCellsMod(mod);
  if(isec==1 && sector2 >= 0) return lOutputCells2.getNCellsMod(mod);
  if(isec==2 && sector3 >= 0) return lOutputCells3.getNCellsMod(mod);
  if(isec==3 && sector4 >= 0) return lOutputCells4.getNCellsMod(mod); 
  return 0;
  
}

Bool_t HMdcWiresArr::calcNGoodWiresAndChi2(HMdcTrackParam& par) {
  numOfGoodWires = 0;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) if(w->isPassedFit()) numOfGoodWires++;
  return par.calcChi2PerDF(numOfGoodWires) >= 0.;
}

Double_t HMdcWiresArr::calcTrackChi2(Int_t trackNum) {
   // trackNum - geant track numer
  Int_t    nWr  = 0;
  Double_t chi2 = 0.;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) if(w->getGeantTrackNum() == trackNum &&
                                                    w->isPassedFit()) {
    chi2 += w->getChi2();
    nWr++;
  }
  if(nWr>1) chi2 /= nWr;
  return chi2;
}

Double_t HMdcWiresArr::calcChi2(HMdcList12GroupCells &listCells) {
   // calculate chi2/ndf for wires from listCells
  Int_t    nWr  = 0;
  Double_t chi2 = 0.;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) if(w->isPassedFit()) {
    Int_t s,m,l,c,it;
    w->getAddress(s,m,l,c,it);
    if((listCells.getTime((m&1)*6+l,c) & it) != 0) {
      chi2 += w->getChi2();
      nWr++;
    }
  }
  if(nWr>1) chi2 /= nWr;
  return chi2;
}

void HMdcWiresArr::fillClusFitAndWireFit(HMdcClusFit* fClusFit) {
  fClusFit->setSec(sector);
  fClusFit->setIOSeg(segment);
  fClusFit->setFitAuthor(HMdcTrackDSet::getFitAuthor());
  fClusFit->setDistTimeVer(fitInOut->getDrTimeCalcVer());
  fClusFit->setFitType(HMdcTrackDSet::getFitType());
  fClusFit->setClustCatIndex(fClst1->getOwnIndex()); // ???? for 2 segm. ???
  Int_t firstMod=firstWire->getModule();
  Int_t lastMod=lastWire->getModule();
  fClusFit->setMod((firstMod==lastMod) ? firstMod:-1);
  Int_t nLayers=0;
  for(Int_t m=firstMod;m<=lastMod;m++) nLayers += lOutputCells.getNLayersMod(m);
  fClusFit->setNumOfLayers(nLayers);
  Int_t   indFirstWireFit = -1;
  Int_t   indLastWireFit  = -1;
  
  Int_t   indLayer = -1;
  Int_t   firstCell,lastCell;
  Float_t firstCellPath,midCellPath,lastCellPath;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
    if(w->getHitStatus() == 0) continue;
    HMdcWireFit *fWireFit=fitInOut->getNewWireFitSlot(&indLastWireFit);
    if(fWireFit == NULL) break;
    Int_t indl = w->getSector()*100 + w->getModule()*10 + w->getLayer();
    if(indl != indLayer) {
      indLayer = indl;
      HMdcSizesCellsLayer *fSCLay = w->getSCLayer();
      if( !fSCLay->calcCrossedCells(fClusFit->getX1(),fClusFit->getY1(),fClusFit->getZ1(),
                                    fClusFit->getX2(),fClusFit->getY2(),fClusFit->getZ2(),
          firstCell,lastCell,firstCellPath,midCellPath,lastCellPath) ) firstCell = lastCell = -1;
    }
    Int_t c = w->getCell();
    Float_t path = -1.;
    if(c >= firstCell && c <= lastCell) {
      if     (c == firstCell) path = firstCellPath;
      else if(c == lastCell)  path = lastCellPath;
      else                    path = midCellPath;
    }
    fWireFit->setCellPath(path);
    if(indFirstWireFit<0) indFirstWireFit=indLastWireFit;
    w->fillWireFitCont(fWireFit);
    if(fClusFit->isGeant()) w->fillWireFitSimCont(fWireFit,
          ((HMdcClusFitSim*)fClusFit)->getGeantTrackNum());
  }
  fClusFit->setFirstWireFitInd(indFirstWireFit);
  fClusFit->setLastWireFitInd(indLastWireFit);
  fClusFit->setSigmaChi2(sigma2>=0. ? TMath::Sqrt(sigma2):-1.);
}

void HMdcWiresArr::countFittedWires(Double_t chi2cut) {
// Is not used and may be will not used
  // Mark fitted wires HMdcEvntListCells with chi2<chi2cut
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) if(w->getHitStatus()>0 && w->getChi2()<chi2cut) {
    event->increaseFittedCount(w->getSector(),w->getModule(),w->getLayer(),w->getCell());
  }  
}

void HMdcWiresArr::fillClusFitSim(HMdcClusFit* fClusFit,HMdcTrackParam& par) {
  if(!fClusFit->isGeant()) return;
  Int_t nTrcksClus=0;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
    if(w->getHitStatus() == 0) continue;
    Int_t track=w->getGeantTrackNum();
    Int_t indx=0;
    for(;indx<nTrcksClus;indx++) if(trackNum[indx]==track) break;
    if(indx==nTrcksClus) {
      if(indx==200) continue; //!!!!!!!!!
      numWiresClus[indx] = 0;
      numWiresFit[indx]  = 0;
      trackNum[indx]     = track;
      nTrcksClus++;
    }
    numWiresClus[indx]++;
    if(w->getHitStatus()==1) numWiresFit[indx]++;
  }
  Int_t indxMax=0;
  Int_t nTrcksFit=0;
  for(Int_t i=0;i<nTrcksClus;i++) {
    if(numWiresFit[i]==0) continue;
    nTrcksFit++;
    if(numWiresFit[indxMax]<numWiresFit[i]) indxMax=i;
  }
  Int_t geantTrack=trackNum[indxMax];
  HMdcClusFitSim* fClusFitSim=(HMdcClusFitSim*)fClusFit;
  fClusFitSim->setNumTracks(nTrcksFit);  
  fClusFitSim->setNumTracksClus(nTrcksClus);
  fClusFitSim->setGeantTrackNum(geantTrack);
  fClusFitSim->setNumWiresTrack(numWiresFit[indxMax]);
  fClusFitSim->setNumWiresTrClus(numWiresClus[indxMax]);
  if(fClusFitSim->isFake()) {
    fClusFitSim->setFakeTrack();
    return;
  }
  
  HMdcWireData* firstTrWire=NULL;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
    if(w->getGeantTrackNum()!=geantTrack || w->getHitStatus()!=1) continue;
    firstTrWire=w;
    break;
  }
  HMdcWireData* lastTrWire=NULL;
  for(HMdcWireData* w=lastWire;w>=firstWire;w--) {
    if(w->getGeantTrackNum()!=geantTrack || w->getHitStatus()!=1) continue;
    lastTrWire=w;
    break;
  }

  if(firstTrWire==0 || lastTrWire==0) {
    fClusFitSim->setXYZ1Geant(-1000.,-1000.,-1000.);
    fClusFitSim->setXYZ2Geant(-1000.,-1000.,-1000.);
    Error("fillClusFitSim","Sec.%i Seg.%i bad track parameters or weights!",
        sector+1,segment+1);
    par.printParam("Bad track:");
    return;
  }
  
  if(fitInOut->getGeantKineCat()==0 || trackNum[indxMax]<1) return;
  HGeantKine* fGeantKine=(HGeantKine*)fitInOut->getGeantKineCat()->getObject(trackNum[indxMax]-1);
  if(fGeantKine==0) return;
  fClusFitSim->setParticleID(fGeantKine->getID());
  fClusFitSim->setMomentum(fGeantKine->getTotalMomentum());
  fClusFitSim->setPrimaryFlag(fGeantKine->isPrimary());
  if(fitInOut->getGeantMdcCat()==0) return;

  fGeantKine->resetMdcIter();
  HGeantMdc* hit = NULL;
  HGeantMdc* firstHit = NULL;
  HGeantMdc* lastHit  = NULL;
  Int_t modF=firstTrWire->getModule();
  Int_t modL=lastTrWire ->getModule();
  Int_t layF=firstTrWire->getLayer();
  Int_t layL=lastTrWire ->getLayer();
  while((hit = (HGeantMdc*) fGeantKine->nextMdcHit()) != NULL) {
    if(sector!=hit->getSector()) break;
    if(firstHit==NULL && hit->getModule()==modF &&
        hit->getLayer()==layF) firstHit=hit;
    if(hit->getModule()<modL) continue;
    if(hit->getModule()==modL) {
      if(hit->getLayer()!=layL) continue;
      lastHit=hit;
    }
    break;
  }
  if(firstHit==0 || lastHit==0 || firstHit==lastHit) {
    fClusFitSim->setFakeTrack(kFALSE);
    return;
  }
  Double_t x1,y1,z1;
  getGeantHitPoint(firstTrWire,firstHit,x1,y1,z1);
  Double_t x2,y2,z2;
  getGeantHitPoint(lastTrWire,lastHit,x2,y2,z2);
  Double_t xc,yc,zc;
  par.getFirstPlane()->calcIntersection(x1,y1,z1,x2,y2,z2, xc,yc,zc);
  fClusFitSim->setXYZ1Geant(xc,yc,zc);
  par.getSecondPlane()->calcIntersection(x1,y1,z1,x2,y2,z2, xc,yc,zc);
  fClusFitSim->setXYZ2Geant(xc,yc,zc);
  if(fprint) fClusFitSim->printSimVsRec();
}

void HMdcWiresArr::getGeantHitPoint(HMdcWireData* w, HGeantMdc* hit,
    Double_t& x,Double_t& y,Double_t& z) {
  // Transformation of geant mdc hit in coor.sys. of sector.
  // x,y,z - [mm]
  Float_t ax,ay,atof,ptof;
  hit->getHit(ax,ay,atof,ptof);
  x = ax;
  y = ay;
  z = w->getSCLayer()->getZGeantHits();
  w->getSCLayer()->transFrom(x,y,z);
}

void HMdcWiresArr::calcTdcErrorsAndFunct(HMdcTrackParam& par, Int_t iflag) {
  // alphaDrDist and timeOffsets must be calculated befor this function!
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcTdcErrors(par);
  if(fprint) par.saveFunct();
  recalcFunctional(par,iflag);
  if(fprint && iflag>=0) par.printFunctChange(" TdcErr.Recalc.:");
  par.saveFunct();
}

void HMdcWiresArr::recalcFunct(HMdcTrackParam& par, Int_t iflag) {
  if(fprint) par.saveFunct();
  recalcFunctional(par,iflag);
  if(fprint && iflag>=0) par.printFunctChange(" TdcErr.Recalc.:");
  par.saveFunct();
}

void HMdcWiresArr::calcTdcErrorsTOff0AndFunct(HMdcTrackParam& par, Int_t iflag) {
  // alphaDrDist and timeOffsets must be calculated befor this function!
  // timeOffsets is not used for error calculation (see HMdcWireData::calcTdcErrors(void))
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcTdcErrors();
  if(fprint) par.saveFunct();
  recalcFunctional(par,iflag);
  if(fprint && iflag>=0) par.printFunctChange(" TdcErr.Recalc.:");
  par.saveFunct();
}

Double_t HMdcWiresArr::valueOfFunctional(HMdcTrackParam& par, Int_t iflag) {
  // iflag:
  //   0 - setting minimal val. of time offset    and no print (default value)
  //   1 - setting minimal val. of time offset    and print if fprint=kTRUE
  //   2 - no setting minimal val. of time offset, calculation of inCell[time]
  //                                              and print if fprint=kTRUE
  calcDriftTime(par,iflag);
  par.calcTimeOffsets(fitInOut->getTofFlag());
  calcFunctional(par,iflag);
  return par.functional();
}

Double_t HMdcWiresArr::valueOfFunctAndErr(HMdcTrackParam& par, Int_t iflag) {
  // iflag:
  //   0 - setting minimal val. of time offset    and no print (default value)
  //   1 - setting minimal val. of time offset    and print if fprint=kTRUE
  //   2 - no setting minimal val. of time offset, calculation of inCell[time]
  //                                              and print if fprint=kTRUE
  calcDriftTimeAndErr(par,iflag);
  par.calcTimeOffsets(fitInOut->getTofFlag());
  calcFunctional(par,iflag);
  return par.functional();
}

Bool_t HMdcWiresArr::setRegionOfWires(Int_t mod,Int_t isec) {
  if(mod<0 || mod>3) {
    Int_t mInd1 = 0;
    Int_t mInd2 = 4;
    if(isec < 0) {
      if(sector2>=0) mInd2 += 4;
      if(sector3>=0) mInd2 += 4;
      if(sector4>=0) mInd2 += 4;
    } else {
      mInd1 += isec*4;
      mInd2 += isec*4;
    }
    firstWire  = wires;
    lastWire   = wires-1;
    nModsInFit = 0;
    for(Int_t m=0;m<mInd2;m++) if(nMdcTimes[m]>0) {
      if(m < mInd1) firstWire += nMdcTimes[m];
      else          nModsInFit++;
      lastWire += nMdcTimes[m];
    }
  } else {
    Int_t mInd = mod;
    if(isec>0) mInd += isec*4;
    firstWire = wires;
    for(Int_t m=0;m<mInd;m++) firstWire += nMdcTimes[m];
    lastWire   = firstWire + nMdcTimes[mInd]-1;
    nModsInFit = 1;
  }
  if(firstWire==0 || lastWire==0) {
    Warning("setFittingTimesList","No fired wires in MDC %i",mod+1);
    return kFALSE;
  }
  firstTime = firstWire->getSequentialIndex();
  lastTime  = lastWire->getSequentialIndex()+1;
  return kTRUE;
}

void HMdcWiresArr::setWeightsTo1or0(Double_t maxChi2, Double_t minWeight) {
  Int_t   nDrTimes = 0;
  UChar_t nLayList= 0;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
    if(w->setWeightTo1or0(maxChi2,minWeight)) {
      nDrTimes++;
      nLayList |= 1<<w->getLayer();
    }
  }
  if(nDrTimes<5) {
    Int_t ntm=5-nDrTimes;
    for(Int_t nw=0;nw<ntm;nw++) {
      HMdcWireData* wmin=findMinChi2();
      if(wmin==0) {
        Error("setWeightsTo1or0","Number of wires for fit=%i",nDrTimes);
        return;
      }
      wmin->setWeightEq1();
      nLayList |= 1<<wmin->getLayer();
      nDrTimes++;
    }
  }
  
  while(HMdcBArray::getNLayOrientation(nLayList) < 3) {
    HMdcWireData* wmin = findMinChi2();
    if(wmin==0) {
      Error("setWeightsTo1or0","Number of wires orientations = %i (<3)",
          HMdcBArray::getNLayOrientation(nLayList));
      return;
    }
    wmin->setWeightEq1();
    nLayList |= 1<<wmin->getLayer();
  }
}

Bool_t HMdcWiresArr::testTukeyWeights(HMdcTrackParam& par) {
  filterOfHitsV2(par); 
  setWeightsTo1or0(fitInOut->getMaxChi2(),fitInOut->getMinWeight());
  Bool_t exitFlag=kFALSE;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) if(w->testWeight1or0()) {
    if(fprint) printf("Reset: %is%im%il%3ic %s\n",w->getSector()+1,w->getModule()+1,
        w->getLayer()+1,w->getCell()+1,w->getIsWireUsed() ? "ON":"OFF");
    exitFlag=kTRUE;
  }
  recalcFunctional(par);
  par.saveFunct();
  return exitFlag;
}

HMdcWireData* HMdcWiresArr::findMinChi2(void) {
  // finding wire with minimal chi2
  HMdcWireData* wmin = 0; 
  Double_t chi2min   = 1.e+200;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
    if(w->getIsWireUsed() || w->getHitStatus()!=1) continue;
    Double_t chi2=w->getChi2();
    if(chi2>chi2min) continue;
    wmin    = w;
    chi2min = chi2;
  }
  return wmin;
}

void HMdcWiresArr::initDTdPar(Int_t k) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->initDTdPar(k);
}

void HMdcWiresArr::calcDTdPar(Int_t k, Double_t oneDv2StepD) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcDTdPar(k,oneDv2StepD);
}

void HMdcWiresArr::calcDriftTime(HMdcTrackParam& par, Int_t iflag) {
  par.clearFunct();
  if(iflag != 2) for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcDriftTime(par);
  else for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcDriftTimeAndInCell(par);
}

void HMdcWiresArr::calcDriftTimeAndErr(HMdcTrackParam& par, Int_t iflag) {
  par.clearFunct();
  if(iflag != 2) for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcDriftTimeAndErr(par);
  else for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcDriftTimeAndErrAndInCell(par);
}

void HMdcWiresArr::calcFunctional(HMdcTrackParam& par, Int_t iflag) {
  if(iflag!=2) par.correctMinTimeOffsets(fitInOut->getMinTOffsetIter());
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcFunctional(par);
  if(fprint && (iflag==1 || iflag==2)) printTimes(iflag,par);
}

void HMdcWiresArr::recalcFunctional(HMdcTrackParam& par, Int_t iflag) {
  // Recalculation of finctional without calculation of ditances
  // (for the same parameters as before)
  // iflag:
  //   0 - setting minimal val. of time offset    and no print (default value)
  //   1 - setting minimal val. of time offset    and print if fprint=kTRUE
  //   2 - no setting minimal val. of time offset and print if fprint=kTRUE
  par.clearFunct();
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->recalcFunctional(par);
  par.calcTimeOffsets(fitInOut->getTofFlag());
  calcFunctional(par,iflag);
}

Double_t HMdcWiresArr::functForDeriv(HMdcTrackParam& par, Int_t iflag) {
  // Calculation of finctional for derivatives.
  // functional for par must be calc. before this !!!
  // iflag:
  //   !=2 - setting minimal val. of time offset
  //    =2 - no setting minimal val. of time offset
  par.clearFunct();
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcDriftTimeForDeriv(par);
  par.calcTimeOffsets(fitInOut->getTofFlag());
  if(iflag!=2) par.correctMinTimeOffsets(fitInOut->getMinTOffsetIter());
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcFunctionalForDer(par);
  return par.functional();
}

void HMdcWiresArr::calcDistanceSign(HMdcTrackParam& par) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcDistanceSign(par);
}

void HMdcWiresArr::reinitWtSt(void) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->reinitWtSt();
}

Bool_t HMdcWiresArr::calcErrorsAnalyt(HMdcTrackParam& par) {
  // calculation of fit parameters errors analyticaly
  matrH.Zero();
  grad2.Zero();
  par.clearTOffsetDer();
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcAnalytDeriv(par,3);
  par.addTimeOffsetDer2(grad2);
  if(!calcErrs(par)) return kFALSE;
  par.calcTimeOffsetsErr();
  if(fprint) par.printErrors();
  return kTRUE;
}

Bool_t HMdcWiresArr::calculateErrors(HMdcTrackParam& par) {
  // calculation of fit parameters errors numer.
  calcSecondDeriv(par);
  if(!calcErrs(par)) return kFALSE;
  calcTimeOffsetsErr(par);
  if(fprint) par.printErrors();
  return kTRUE;
}

Bool_t HMdcWiresArr::calcErrs(HMdcTrackParam& par) {
  Int_t numOfParam=par.getNumParam();
  for(Int_t k=0; k<numOfParam; k++) {
    if(TMath::Abs(grad2(k,k))<3.e-16) {
      grad2(k,k)=3.e-16;
      if(matrH(k,k)==0.) matrH(k,k)=1.;
    }
    for(Int_t l=0; l<k; l++) {
      grad2(l,k) = grad2(k,l);
      matrH(l,k) = matrH(k,l);
    }
  }
  grad2.InvertFast();
  if(!grad2.IsValid()) {
    Error("calcErrs","Inverted matrix is not valid!");
    grad2.MakeValid();
    return kFALSE;
  }
  matrH=grad2*matrH*grad2.T(); //!!!  matrH.Similarity(grad2);
  if(!matrH.IsValid()) {
    Error("calcErrs","Errors matrix is not valid!");
    matrH.MakeValid();
    return kFALSE;
  }
  par.fillErrorsMatr(matrH);
  return kTRUE;
}

void HMdcWiresArr::calcTimeOffsetsErr(HMdcTrackParam& par) {
  // Calculation of time offset error
  par.clearTOffsetDer();
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->addToTOffsetErr(par);
  par.calcTimeOffsetsErr();
  if(fprint) par.printErrors();
}

void HMdcWiresArr::printTimes(Int_t iflag, HMdcTrackParam& par) {
  for(HMdcWireData* w=firstWire;w<=lastWire;w++)
      w->printTime(iflag,par,fitInOut->isGeant());
}

Double_t HMdcWiresArr::getSumOfWeights(void) {
  Double_t sum=0.;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++)
      if(w->getHitStatus()==1) sum += w->getWeight();
  return sum;
}

Double_t HMdcWiresArr::findNewSigm2(Int_t ntm) {
  Double_t chi2buf[ntm];
  Int_t nwr=0;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
    if(w->getWeight()>0. || w->getHitStatus()!=1) continue;
    Double_t chi2 = w->getChi2();
    if(nwr==0 || chi2>=chi2buf[nwr-1]) {
      if(nwr<ntm) {
        chi2buf[nwr++] = chi2;
      }
    } else {
      for(Int_t i=0;i<nwr;i++) if(chi2<chi2buf[i]) {
        Int_t j=(nwr<ntm) ? nwr:ntm-1;
        for(;j>i;j--) chi2buf[j]=chi2buf[j-1];
        chi2buf[i]=chi2;
        if(nwr<ntm) nwr++;
        break;
      }
    }
  }
  Double_t sigma2 = (nwr==ntm) ? chi2buf[nwr-1]*1.1 : 1000000; // +10%
  return sigma2/fitInOut->getTukeyConst2s();
}

Double_t HMdcWiresArr::setTukeyWeightsAndCalcSigma2(Double_t sigma2) {
  Double_t cwSig  = sigma2*fitInOut->getTukeyConstWs();
  Double_t c2Sig  = sigma2*fitInOut->getTukeyConst2s();
  Double_t c4Sig  = sigma2*fitInOut->getTukeyConst4s();
  Double_t tukeyScale = fitInOut->getTukeyScale();
  Double_t sumWt  = 0.;
  Double_t funct  = 0.;
  Int_t    nwires = 0;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++)
      nwires += w->setTukeyWeight(cwSig,c2Sig,c4Sig,tukeyScale,sumWt,funct);
  
  if(nwires>=5) return funct/sumWt;
  return setTukeyWeightsAndCalcSigma2(findNewSigm2(5-nwires)); //???
}

Bool_t HMdcWiresArr::setTukeyWeights(Double_t sigma2) {
  // return kTRUE if sigma2 was increased due to small amount of wires after filtering (<5)
  Double_t cwSig  = sigma2*fitInOut->getTukeyConstWs();
  Double_t c2Sig  = sigma2*fitInOut->getTukeyConst2s();
  Double_t c4Sig  = sigma2*fitInOut->getTukeyConst4s();
  Double_t tukeyScale = fitInOut->getTukeyScale();
  Int_t    nwires = 0;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++)
      nwires += w->setTukeyWeight(cwSig,c2Sig,c4Sig,tukeyScale);
  
  if(nwires >= 5 || cwSig > 50000) return kFALSE;
  setTukeyWeights(findNewSigm2(5-nwires)); //???
  return kTRUE;
}

Bool_t HMdcWiresArr::filterOfHits(HMdcTrackParam& par, Int_t iflag) {
  // It was used in HTrackFitterA only.
  // Filtering of hits using Tukey weights
  // return kTRUE new weights was calculated
  if(getNWiresInFit()<=5) {
    if(fprint) printf(" num.wires=%i => No filtering!\n",getNWiresInFit());
    return kFALSE;
  }
  Double_t sigma2n = par.getNormFunctional();
  if(sigma2n <= fitInOut->getTukeyWtMinSigma2()) {
    if(fprint) printf(" sigma=%f => Not filtering!\n",TMath::Sqrt(sigma2n));
    return kFALSE;
  }
  Bool_t exitFlag = kTRUE;
  for(Int_t j=0; j<fitInOut->maxNumFilterIter(); j++) {
//    A esli sigma2n <fitInOut->getTukeyWtMinSigma2() pri pervom zhe prohode ???
    if(sigma2n < fitInOut->getTukeyWtMinSigma2()) {
      exitFlag = kFALSE;
//????            =kFALSE - eto bug??? //????? Kak chasto nado fil'trovat' ?????
      break;
    }
//    Mozhet nado delat' tak???
//    if(sigma2n < fitInOut->getTukeyWtMinSigma2()) {
//      sigma2n=fitInOut->getTukeyWtMinSigma2();
//      j=4;
//    }
    sigma2  = sigma2n;
    sigma2n = setTukeyWeightsAndCalcSigma2(sigma2);
    if(sigma2n>sigma2) break;
//Pochemu ne pereschitiyvaetsya funcional dlya kazhdogo j ??? Proverit' !!!
  }
  par.saveFunct();
  recalcFunctional(par,iflag);
  if(fprint) {
    printf(" sigma=%f => FILTER! ",TMath::Sqrt(sigma2n));
    par.printFunctChange();
  }
  par.saveFunct();
  return exitFlag;
}

Bool_t HMdcWiresArr::filterOfHitsV2(HMdcTrackParam& par, Int_t iflag) {
  // Filtering of hits using Tukey weights
  // return kTRUE new weights was calculated
// seychas eto ne tak!!!
  if(getNWiresInFit() <= 5) {
    if(fprint) printf(" num.wires=%i => No filtering!\n",getNWiresInFit());
    return kFALSE;
  }
  Double_t sigma2n = par.getNormFunctional();
  if(sigma2n <= fitInOut->getTukeyWtMinSigma2()) {
    if(fprint) printf(" sigma=%f => No filtering!\n",TMath::Sqrt(sigma2n));
    return kFALSE;
  }
  Int_t maxIter = fitInOut->maxNumFilterIter()-1;
  Int_t iter    = 0;
  par.saveFunct();
  for(; iter<=maxIter; iter++) {
    sigma2      = sigma2n;
    Bool_t exit = setTukeyWeights(sigma2);
    recalcFunctional(par);
    sigma2n     = par.getNormFunctional();
    if(sigma2n < fitInOut->getTukeyWtMinSigma2() || exit) break;
  }
  if(fprint) {
    if(iflag>0) printTimes(iflag,par);
    printf(" sigma=%f => FILTER! ",TMath::Sqrt(sigma2n));
    par.printFunctChange();
  }
  return iter >= maxIter;
}

void HMdcWiresArr::filterOfHitsConstSig(HMdcTrackParam& par, Double_t sig) {
  // Filtering of hits using Tukey weights
  // return kTRUE new weights was calculated
  if(getNWiresInFit()<=5) {
    if(fprint) printf(" num.wires=%i => No filtering!\n",getNWiresInFit());
    return;
  }
  setTukeyWeightsAndCalcSigma2(sig*sig);
  recalcFunctional(par);
  par.saveFunct();
}

void HMdcWiresArr::setWeightsTo1or0(HMdcTrackParam& par, Int_t iflag) {
  setWeightsTo1or0(fitInOut->getMaxChi2(),fitInOut->getMinWeight());

  if(fprint && iflag>=0) par.saveFunct();
  recalcFunctional(par);
  if(fprint && iflag>=0) par.printFunctChange(" Weights=>0/1:");
  par.saveFunct();

//? poka rabotaet huzhe ???
// recalcFunctional(par);
// calcTdcErrorsAndFunct(par);
// ....
//-----------------------
}

void HMdcWiresArr::setUnitWeights(void) {
  // For target scan only!
  // Setting all weights to 1.
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->setUnitWeight();
}

void HMdcWiresArr::setSizeGrad2Matr(HMdcTrackParam& par) {
  // Seting of sizes of gradient matrix.
  grad2.ResizeTo(par.getNumParam(),par.getNumParam());
  matrH.ResizeTo(par.getNumParam(),par.getNumParam());
}

void HMdcWiresArr::calcDerivatives(HMdcTrackParam& par,Int_t iflag) {
  // Numerical calculation of the fit parameters derivatives
  // iflag<2 - calculate the diagonal elements of the grad2 matrix only
  // else    - calculate the full grad2 matrix
  Double_t funMin=par.functional();
  Double_t stepD=fitInOut->getStepDer(funMin);
  Double_t oneDv2StepD=1./(2.*stepD);
  Double_t oneDvStepD2=1./(stepD*stepD);
  HMdcTrackParam tPar;
  tPar.copyPlanes(par);
  Int_t numOfParam=par.getNumParam();
  agrad=0;
  for(Int_t k = 0; k < numOfParam; k++) {
    Double_t func0 = functForDeriv(tPar(par,k, stepD));
    Double_t func1 = functForDeriv(tPar(par,k,-stepD));
    grad2(k,k)=(func0 + func1 - 2.0*funMin)*oneDvStepD2;
    grad[k] = (func0 - func1)*oneDv2StepD;
    agrad += grad[k]*grad[k];
    if(iflag < 2) continue;
    for(Int_t l = k+1; l < numOfParam; l++) {
      Double_t func2 = functForDeriv(tPar(par,k,stepD,l,stepD));
      Double_t func3 = functForDeriv(tPar(par,l,stepD));
      grad2(k,l)=(funMin - func0 + func2 - func3) * oneDvStepD2;
    }
  }
  agrad=TMath::Sqrt(agrad);
}

void HMdcWiresArr::calcAnalyticDerivatives1(HMdcTrackParam& par) {
  // Analytical calculation of the fit parameters derivatives 
  // for the first min.method:
  // calculate the first der. and the diagonal elements of the grad2 matrix
  Int_t numOfParam=par.getNumParam();
  for(Int_t n=0; n<numOfParam; n++) grad[n]=0.;
  grad2.Zero();
  par.clearTOffsetDer();
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcAnalytDeriv(par,1);
  par.addTimeOffsetDer1(grad2);
  // dTof/dP == 0 - no need to correct grad!
  agrad=0;
  for(Int_t k=0; k<numOfParam; k++) agrad += grad[k]*grad[k];
  agrad=TMath::Sqrt(agrad);
}

Double_t HMdcWiresArr::calcAGradAnalyt(HMdcTrackParam& par) {
  for(Int_t k=0; k<4; k++) grad[k]=0.;
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcAnalytDeriv(par,0);
  Double_t agrd=0;
  for(Int_t k=0; k<4; k++) agrd += grad[k]*grad[k];
  return TMath::Sqrt(agrd);
}

void HMdcWiresArr::calcAnalyticDerivatives2(HMdcTrackParam& par) {
  // Analytical calculation of the fit parameters derivatives 
  // for the second min.method.
  // Calculate the first der. and all elements of the grad2 matrix
  Int_t numOfParam=par.getNumParam();
  for(Int_t n=0; n<numOfParam; n++) grad[n]=0.;
  grad2.Zero();
  par.clearTOffsetDer();
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcAnalytDeriv(par,2);
  par.addTimeOffsetDer2(grad2);
  agrad=0;
  for(Int_t k=0; k<numOfParam; k++) {
    agrad += grad[k]*grad[k];
    for(Int_t l=0; l<k; l++) grad2(l,k)=grad2(k,l);
  }
  agrad=TMath::Sqrt(agrad);
}

void HMdcWiresArr::calcSecondDeriv(HMdcTrackParam& par) {
  // For errors calculation
  Double_t funMin      = par.functional();
  Double_t stepD       = fitInOut->getStepDer(funMin);
  Double_t oneDv2StepD = 1./(2.*stepD);
  Double_t oneDvStepD2 = 1./(stepD*stepD);
  Int_t numOfParam=par.getNumParam();
  HMdcTrackParam tPar;
  tPar.copyPlanes(par);
  for(Int_t k = 0; k < numOfParam; k++) {
    Double_t func0 = functForDeriv(tPar(par,k, stepD),2);//functForDeriv(tPar,2);
    initDTdPar(k);
    Double_t func1  = functForDeriv(tPar(par,k,-stepD),2);//functForDeriv(tPar,2);
    calcDTdPar(k,oneDv2StepD);
    grad2(k,k)= (func0 + func1 - 2.0*funMin)*oneDvStepD2;
    for(Int_t l=k+1; l<numOfParam; l++) {
      Double_t func2 = functForDeriv(tPar(par,k,stepD,l,stepD),2);
      Double_t func3 = functForDeriv(tPar(par,l,stepD),2);
      grad2(k,l) = grad2(l,k) = (funMin - func0 + func2 - func3) * oneDvStepD2;
    }
  }
}

void HMdcWiresArr::setInitWeghts(HMdcTrackParam& par) {
  Int_t layMax = 24;
  if(sector2 >= 0) {
    layMax += 24;
    if(sector3 >= 0) {
      layMax += 24;
      if(sector4 >= 0) layMax += 24;
    }
  }
  for(Int_t layer=0;layer<layMax;layer++) {
    if(firstWireInLay[layer]==NULL || lastWireInLay[layer]==NULL) continue;
    if(firstWireInLay[layer] == lastWireInLay[layer]) continue;
    Int_t nWires=0;
    HMdcWireData* fWr = firstWireInLay[layer];
    HMdcWireData* lWr = lastWireInLay[layer];
    for(HMdcWireData* w=fWr;w<=lWr;w++) if(w->getHitStatus() == 1) nWires++;
    if(nWires<=2) continue;
    HMdcSizesCellsLayer* pSCLayer = fWr->getSCLayer();
    Float_t cell1,cell2;
    pSCLayer->calcCrossedCells(par,cell1,cell2);
    Double_t cell=(cell1+cell2)/2.;
    for(HMdcWireData* w=fWr;w<=lWr;w++) {
      if(w->getHitStatus() != 1) continue;
      Double_t dCell=TMath::Abs(w->getCell()-cell);
      if(dCell>1.) w->setWeightAndWtNorm(w->getWeight()/dCell);
    }
  }
}

Int_t HMdcWiresArr::getOutputNLayers(void) const {
  Int_t nLays = lOutputCells.getNLayers();
  if(sector2>=0) {
    nLays += lOutputCells2.getNLayers();
    if(sector3>=0) {
      nLays += lOutputCells3.getNLayers();
      if(sector4>=0) nLays += lOutputCells4.getNLayers();
    }
  }  
  return nLays;
}

Bool_t HMdcWiresArr::calcInitialValue(HMdcTrackParam& par) {
  if(getNWiresInFit() > 30) return kFALSE;
  useNewParam   = kFALSE;
  meanDDist     = 0.;
  maxNGoodWires = 0;
  newListCells.clear();
  maxAddWr = 0;
 
  maxNGoodWires = collectAllWires(par,maxAddWr,meanDDist);

  if(maxNGoodWires > 7) newListCells = tmpList;
  else {
    newListCells = lInputCells;
    maxNGoodWires = 0;
    meanDDist = 0.;
    for(HMdcWireData* w=firstWire;w<=lastWire;w++) {
      Double_t dDist = w->dDist(par);
      if(dDist < dDistCut) {
        meanDDist += dDist;
        maxNGoodWires++;
      }
    }
  }

  if(maxNGoodWires == 0) meanDDist = 10000000.;
  else                   meanDDist /= maxNGoodWires; 

  HMdcTrackParam minPar = par;
  findGoodComb(par,minPar);

  if(!useNewParam || meanDDist > 9000000.) return kFALSE;
  if(maxNGoodWires <= 7) newListCells.add(&lInputCells);
  par = minPar;
  reinitWireArr();
  subtractWireOffset(par);
  return kTRUE;
}

Bool_t HMdcWiresArr::reinitWireArr(void) {
  if(lInputCells.getNCells() == newListCells.getNCells()) {
    if(lInputCells.isIncluded(newListCells)) return kFALSE;  // lInputCells == newListCells
  }

  lInputCells  = newListCells;
  lOutputCells = newListCells;

  nDriftTimes  = 0;
  nClTimes     = 0;
  for(Int_t m=0;m<4;m++) nDriftTimes += nMdcTimes[m] = lInputCells.getNDrTimesMod(m);

  setNumDriftTimes(nDriftTimes);
  lastTime  = nDriftTimes;
  firstWire = wires;
  lastWire  = wires+nDriftTimes-1;
  
  nDrTmFirstSec = 0;
  if(sector  >=0 && fillListHits(0,sector, lInputCells, lOutputCells )) return kTRUE;
  return kFALSE;
}

Int_t HMdcWiresArr::collectAllWires(HMdcTrackParam &par,Int_t &nAddWires,Double_t &dDistSum) {
  HMdcSizesCellsMod **fSCModAr = fitInOut->getSCellsModArr(sector);
  HMdcEvntListCells  *store    = HMdcEvntListCells::getExObject();
  HMdcSecListCells   &storeSec = (*store)[sector];
  HMdcDriftTimeParSec* drTimeParSec = HMdcWireData::getDriftTimePar()->at(sector);
  Int_t iLayer  = segment != 1 ?  0 : 12;
  Int_t lastLay = segment == 0 ? 12 : 24;
  Int_t nWires  = 0;
  nAddWires = 0;
  tmpList.clear();
  dDistSum  = 0.;

  Double_t vo[6] = {0.,0.,0.,0.,0.,0.};
  Double_t v[6];
  par.getLinePar(v);

  for(;iLayer<lastLay;iLayer++) {
    Int_t mod = iLayer/6;
    if(fSCModAr[mod]==0) continue;
    Int_t lay = iLayer%6;
    if(lay == 0) {
      fSCModAr[mod]->transTo(v,vo);
      vo[3] /= vo[5];
      vo[4] /= vo[5];
    }
    HMdcSizesCellsLayer &fSCLay       = (*(fSCModAr[mod]))[lay];
    HMdcLayListCells    &storeLay     = storeSec[mod][lay];
    HMdcDriftTimeParMod *drTimeParMod = drTimeParSec->at(mod);
    Int_t cell1,cell2,cell3;
    Double_t al[2],md[2],st[2];
    if( !fSCLay.calcCrossedCellsPar(vo,0.49999,cell1,cell2,cell3,al,md,st) ) continue;
   
    for(Int_t pt=0;pt<2;pt++) {
      Int_t c1 = pt == 0 ? cell1 : cell2;
      Int_t c2 = pt == 0 ? cell2 : cell3;
      if(c1 < 0 || c2 < 0) continue;
      for(Int_t c=c1;c<c2;c++) if(storeLay.isCell(c) && fSCLay[c].getStatus()) {
        Double_t  tdcTime    = storeLay.getTimeValue(c);
        if(tdcTime < -100.) continue;

if(mod<2) tdcTime -= 7; // inner mdc
else      tdcTime -= 8; // outer mdc

        if(tdcTime < 0.) tdcTime = 0.;

        Double_t drTmDist = drTimeParMod->calcDistance(al[pt],tdcTime);
        if(drTmDist < 0.) drTmDist = 0.;
        else if(fSCLay.getMaxDriftDist() < drTmDist) drTmDist = fSCLay.getMaxDriftDist();
        
        Double_t minDist = TMath::Abs(md[pt] + st[pt]*(c-c1));
        Double_t dDist   = TMath::Abs(minDist - drTmDist);

        Double_t corCut = 2.5;  // mod < 2 ? 2.5 : 2.5;
        if( dDist < dDistCut*corCut ) {
          tmpList.setTime(iLayer,c,1);
          if(dDist < dDistCut) {
            dDistSum += dDist;
            nWires++;
            if(lInputCells.getTime(iLayer,c)==0) nAddWires++;
          }
        } 
      }
    }
  }
  return nWires;
}

Bool_t HMdcWiresArr::getComb4layers(UInt_t c,Int_t add12,Int_t &l1,Int_t &l2,
                                             Int_t add34,Int_t &l3,Int_t &l4) {
  if(c >= 20) return kFALSE;
  static Int_t comb[20][4] = {{0,1,10,11},{0,1, 9,11},{0,1, 8,11},{0,2,10,11},{0,2, 7,10},
                              {0,3,10,11},{0,3, 7,10},{0,5, 7,10},{1,4, 9,11},{1,4, 8,11},
                              {1,4, 6,11},{1,4, 6, 9},{1,4, 6, 8},{2,5, 7,10},{2,5, 6, 7},
                              {3,5, 7,10},{3,5, 6, 7},{4,5, 6, 9},{4,5, 6, 8},{4,5, 6, 7}};
  l1 = comb[c][0] + add12;
  l2 = comb[c][1] + add12;
  l3 = comb[c][2] + add34;
  l4 = comb[c][3] + add34;
  return kTRUE;
}

void HMdcWiresArr::findGoodComb(HMdcTrackParam& par,HMdcTrackParam& minPar) {
  if(segment > 1) return; // "findGoodComb" is not implemented for field off data 
  Int_t nWiresTot = getNWiresInFit();
  calcLeftRight(par);
  Int_t nFirstWrDone = 0;
  Int_t add = segment!=1 ? 0:12;
 
  Int_t layerIndx1,layerIndx2,layerIndx3,layerIndx4;
  UInt_t icomb = 0;
  while( getComb4layers(icomb,add,layerIndx1,layerIndx2,add,layerIndx3,layerIndx4) ) {
    icomb++;
    if(firstWireInLay[layerIndx1] == NULL) continue;
    if(firstWireInLay[layerIndx2] == NULL) continue;
    if(firstWireInLay[layerIndx3] == NULL) continue;
    if(firstWireInLay[layerIndx4] == NULL) continue;
    HMdcWireData* fWr1 = firstWireInLay[layerIndx1];
    HMdcWireData* fWr2 = firstWireInLay[layerIndx2];
    HMdcWireData* fWr3 = firstWireInLay[layerIndx3];
    HMdcWireData* fWr4 = firstWireInLay[layerIndx4];
    HMdcWireData* lWr1 = lastWireInLay[layerIndx1];
    HMdcWireData* lWr2 = lastWireInLay[layerIndx2];
    HMdcWireData* lWr3 = lastWireInLay[layerIndx3];
    HMdcWireData* lWr4 = lastWireInLay[layerIndx4];
    for(HMdcWireData* w1=fWr1;w1<=lWr1;w1++) {
      if( nWiresTot-nFirstWrDone+3 < maxNGoodWires-maxAddWr) return;
      nFirstWrDone++;
      for(HMdcWireData* w2=fWr2;w2<=lWr2;w2++)
          for(HMdcWireData* w3=fWr3;w3<=lWr3;w3++)
              for(HMdcWireData* w4=fWr4;w4<=lWr4;w4++) {
                getTrack(par,w1,w2,w3,w4,minPar);
              }
    }
  }
}

void HMdcWiresArr::getTrack(HMdcTrackParam& par,
                            HMdcWireData* w1, HMdcWireData* w2,
                            HMdcWireData* w3, HMdcWireData* w4,
			    HMdcTrackParam& minPar) {
   Int_t    nWiresTot = getNWiresInFit();
   Int_t    nWiresCut = nWiresTot/2 + 1;   // At list 1/2 wires

   HMdcTrackParam tmpPar(par);
   HGeomVector& v1 = w1->getDirection();
   HGeomVector& v2 = w2->getDirection();
   HGeomVector& v3 = w3->getDirection();
   HGeomVector& v4 = w4->getDirection();
   HGeomVector V13(v1.vectorProduct(v3));
   HGeomVector V14(v1.vectorProduct(v4));
   HGeomVector V23(v2.vectorProduct(v3));
   HGeomVector V24(v2.vectorProduct(v4));
   Double_t B1 = v2.scalarProduct(V13);
   Double_t B2 = v2.scalarProduct(V14);
   for(Int_t side1=0;side1<2;side1++) {
      HGeomVector& p1 = w1->getPoint(side1);
      for(Int_t side2=0;side2<2;side2++) {     
 	 HGeomVector& p2 = w2->getPoint(side2);
         HGeomVector  P21(p2 - p1);
	 for(Int_t side3=0;side3<2;side3++) {     
 	    HGeomVector& p3 = w3->getPoint(side3);
            HGeomVector  P31(p3 - p1);
            Double_t D1 = (p2 - p3).scalarProduct(V13);
            Double_t F1 = v3.scalarProduct(P31.vectorProduct(P21));
            Double_t E1 = P31.scalarProduct(V23);
  
	    for(Int_t side4=0;side4<2;side4++) {     
 	       HGeomVector& p4 = w4->getPoint(side4);
               HGeomVector  P41(p4 - p1);
               Double_t D2    = (p2 - p4).scalarProduct(V14);
               Double_t F2    = v4.scalarProduct(P41.vectorProduct(P21));
               Double_t E2    = P41.scalarProduct(V24);
               
               Double_t BE    = B1*E2 - B2*E1;
               Double_t BC    = 0.5/BE;
               Double_t DEpBF = D1*E2 - D2*E1 + B1*F2 - B2*F1;
               Double_t SQ    = TMath::Sqrt(DEpBF*DEpBF - 4*BE*(D1*F2 - D2*F1));
               Double_t t2    = (SQ - DEpBF)*BC;  
               Double_t t1    = -(E2*t2 + F2)/(B2*t2 + D2);
               Double_t t2b   = (-SQ - DEpBF)*BC;  
               Double_t t1b   = -(E2*t2b + F2)/(B2*t2b + D2);
               if(TMath::Abs(t1b) < TMath::Abs(t1)) {
                 t2=t2b;
                 t1=t1b;
               }
               HGeomVector P5(v1);
               P5 *= t1;
               P5 += p1;
               HGeomVector Q(v2);
               Q *= t2;
               Q += p2;
               Q -= P5;
               tmpPar.setParam(P5,Q);

//calcTrackN(p1,v1,p2,v2,p3,v3,p4,v4,tmpPar);
               
	       Double_t sumMod     = 0;
	       Int_t    nGoodWires = 4; // For wires is on the track already
               
               //++++++++++++++++++++++++++++++++++++++++++++++
               Int_t nAddWr;
               nGoodWires = collectAllWires(tmpPar,nAddWr,sumMod);
               //++++++++++++++++++++++++++++++++++++++++++++++
               
               if(nGoodWires < maxNGoodWires) continue;
	       if(nGoodWires > nWiresCut) {
                  Double_t sMod = sumMod/nGoodWires;
		  if(sMod < meanDDist) {
                     useNewParam   = kTRUE;
		     meanDDist     = sMod;
		     minPar        = tmpPar;
		     maxNGoodWires = nGoodWires;
                     newListCells = tmpList;
                     maxAddWr = nAddWr;
		  }
	       }
	    }
	 }
      }  
   }
}

void HMdcWiresArr::calcLeftRight (HMdcTrackParam& par) {
   
  for(HMdcWireData* w=firstWire;w<=lastWire;w++) w->calcLeftRight(par);

}

void HMdcWiresArr::calcTrack(HGeomVector& P1, HGeomVector& V1,
                             HGeomVector& P2, HGeomVector& V2,
			     HGeomVector& P3, HGeomVector& V3,
                             HGeomVector& P4, HGeomVector& V4,
			     HMdcTrackParam& par) {
  HGeomVector V13(V1.vectorProduct(V3));
  HGeomVector V14(V1.vectorProduct(V4));
  HGeomVector V23(V2.vectorProduct(V3));
  HGeomVector V24(V2.vectorProduct(V4));
  Double_t B1 = V2.scalarProduct(V13);
  Double_t B2 = V2.scalarProduct(V14);

//--------- After loop 2
  HGeomVector P21(P2 - P1);

//--------- After loop 3
    
  Double_t D1 = (P2 - P3).scalarProduct(V13);
  HGeomVector P31(P3 - P1);
  Double_t F1 = V3.scalarProduct(P31.vectorProduct(P21));
  Double_t E1 = P31.scalarProduct(V23);
  
//--------- After loop 4
  Double_t D2 = (P2 - P4).scalarProduct(V14);
  HGeomVector P41(P4 - P1);
  Double_t F2 = V4.scalarProduct(P41.vectorProduct(P21));
  Double_t E2 = P41.scalarProduct(V24);

  Double_t BE = B1*E2 - B2*E1;
  Double_t BC = 0.5/BE;

  Double_t DEpBF = D1*E2 - D2*E1 + B1*F2 - B2*F1;
  Double_t SQ =TMath::Sqrt(DEpBF*DEpBF - 4*BE*(D1*F2 - D2*F1));
  
    
  Double_t t2 = (SQ - DEpBF)*BC;  
  Double_t t1 = -(E2*t2 + F2)/(B2*t2 + D2);
  
  Double_t t2b = (-SQ - DEpBF)*BC;  
  Double_t t1b = -(E2*t2b + F2)/(B2*t2b + D2);
  if(TMath::Abs(t1b) < TMath::Abs(t1)) {
    t2=t2b;
    t1=t1b;
  }
  
  HGeomVector P5(V1);
  P5 *= t1;
  P5 += P1;
  
  HGeomVector Q(V2);
  Q *= t2;
  Q += P2;
  Q -= P5;

  par.setParam(P5,Q);
}

Int_t HMdcWiresArr::testRestForTrack(Int_t sec,HMdcLineParam &lineParam,
    HMdcList12GroupCells &listCells, Int_t nLayersMax,Int_t nWiresMax) {
  
  listCells.clear();
  Int_t nLaysBest  = 0; 
  Int_t nCellsBest = 0;
  
  HMdcSizesCellsSec &pSCSec   = (*HMdcSizesCells::getExObject())[sec];
//  const HGeomVector &targMPnt = pSCSec.getTargetMiddlePoint();
  HMdcSecListCells &pSListCells = (*event)[sec];
//  HMdcSizesCellsMod **fSCModAr = fitInOut->getSCellsModArr(sec);
  Double_t zTarget = pSCSec.getTargetMiddlePoint().getZ();
   
  Int_t mod1 = 0;
  Int_t mod2 = 0;
  Int_t mod3 = 1;
  Int_t mod4 = 1;
  
  Int_t wind = HMdcTrackDSet::getNCellsCutOVT();
        
  HMdcLineParam tmpLineParam;
  tmpLineParam.setFirstPlane (&(pSCSec[mod1][0]));
  tmpLineParam.setSecondPlane(&(pSCSec[mod4][5]));
//   tmpLineParam.setFirstPlane (&((*(fSCModAr[0]))[0]));
//   tmpLineParam.setSecondPlane(&((*(fSCModAr[1]))[5]));
  tmpLineParam.setCoorSys(sec);
  HMdcList12GroupCells tmpListCells;
    
  Int_t lay1,lay2,lay3,lay4;
  Int_t icomb = 0;
  while( getComb4layers(icomb,0,lay1,lay2,-6,lay3,lay4) ) {
    icomb++;
    HMdcLayListCells &pLay1list   = pSListCells[mod1][lay1]; 
    HMdcLayListCells &pLay2list   = pSListCells[mod2][lay2]; 
    HMdcLayListCells &pLay3list   = pSListCells[mod3][lay3]; 
    HMdcLayListCells &pLay4list   = pSListCells[mod4][lay4];

    if(pLay1list.getNumNotFitted() == 0) continue;
    if(pLay2list.getNumNotFitted() == 0) continue;
    if(pLay3list.getNumNotFitted() == 0) continue;
    if(pLay4list.getNumNotFitted() == 0) continue;

    Double_t tY2[4],tY3[4],tY4[4];
    pSCSec[mod2][lay2].getPntToCell(tY2);
    pSCSec[mod3][lay3].getPntToCell(tY3);
    pSCSec[mod4][lay4].getPntToCell(tY4);
    
    Double_t parA = pSCSec[mod3][lay3].A();  // Parmeters of a layer plane in MDCII
    Double_t parB = pSCSec[mod3][lay3].B();
    Double_t parD = pSCSec[mod3][lay3].D();
    
    Int_t cell1 = 1000;
    while( pLay1list.previousNonFittedCell(cell1) ) {
      HMdcSizesCellsCell &scell1 = pSCSec[mod1][lay1][cell1];
      HGeomVector &p1  = scell1.getWirePnt1();
      HGeomVector &pe1 = scell1.getWirePnt2();
      HGeomVector  v1  = pe1 - p1;

      // Calculate cells region in [mod2][lay2]:
      Int_t c1 = tY2[0]*p1.X()  + tY2[1]*p1.Y()  + tY2[2]*p1.Z()  + tY2[3];
      Int_t c2 = tY2[0]*pe1.X() + tY2[1]*pe1.Y() + tY2[2]*pe1.Z() + tY2[3];
      if(c1 > c2) std::swap(c1,c2);
      
      Int_t cell2 = c2+1;
      while( pLay2list.previousNonFittedCell(cell2) && cell2>=c1 ) {
        HMdcSizesCellsCell &scell2 = pSCSec[mod2][lay2][cell2];
        HGeomVector &p2  = scell2.getWirePnt1();
        HGeomVector &pe2 = scell2.getWirePnt2();
        HGeomVector  v2 = pe2 - p2;

        HGeomVector P21(p2 - p1);
        
        // Calc. of 2 wires cross on X-Y plane
        Double_t a1  = (p1.Y() - pe1.Y())/(p1.X() - pe1.X());
        Double_t b1  =  p1.Y() - a1*p1.X();
        Double_t a2  = (p2.Y() - pe2.Y())/(p2.X() - pe2.X());
        Double_t b2  =  p2.Y() - a2*p2.X();
        
        
        Double_t x   = -(b1-b2)/(a1-a2);
        Double_t y   = a1*x+b1;  //= -a1*(b1-b2)/(a1-a2)+b1 = (a1*b2 - b1*a2)/(a1-a2);
        Double_t z   = pSCSec[mod2][lay2].getZOnPlane(x,y);
        
        Double_t del = 1/(parA*x+parB*y+z-zTarget);
        Double_t xc  = x*(parD-zTarget)*del;
        Double_t yc  = y*(parD-zTarget)*del;
        Double_t zc  = parD-parA*xc-parB*yc;

        Int_t crC3   = tY3[0]*xc  + tY3[1]*yc  + tY3[2]*zc  + tY3[3];
        Int_t cell3e = crC3+wind;
        Int_t cell3m = crC3-wind;
        
        Int_t cell3  = cell3e+1;
        while( pLay3list.previousNonFittedCell(cell3) && cell3>=cell3m ) {
          HMdcSizesCellsCell &scell3 = pSCSec[mod3][lay3][cell3];
          HGeomVector &p3  = scell3.getWirePnt1();
          HGeomVector &pe3 = scell3.getWirePnt2();
          HGeomVector v3 = pe3 - p3;
          HGeomVector P31(p3 - p1);
          HGeomVector V13(v1.vectorProduct(v3));
          HGeomVector V23(v2.vectorProduct(v3));
          Double_t B1 = v2.scalarProduct(V13);
          Double_t D1 = (p2 - p3).scalarProduct(V13);
          Double_t F1 = v3.scalarProduct(P31.vectorProduct(P21));
          Double_t E1 = P31.scalarProduct(V23);
          
          // Calculate cells region in [mod4][lay4]:
          Int_t c3 = tY4[0]*p3.X()  + tY4[1]*p3.Y()  + tY4[2]*p3.Z()  + tY4[3];
          Int_t c4 = tY4[0]*pe3.X() + tY4[1]*pe3.Y() + tY4[2]*pe3.Z() + tY4[3];
          if(c3 > c4) std::swap(c3,c4);

          Int_t cell4 = c4+1;
          while( pLay4list.previousNonFittedCell(cell4) && cell4>=c3 ) {
            HMdcSizesCellsCell &scell4 = pSCSec[mod4][lay4][cell4];
            HGeomVector &p4  = scell4.getWirePnt1();
            HGeomVector &pe4 = scell4.getWirePnt2();
            HGeomVector v4 = pe4 - p4;
            HGeomVector P41(p4 - p1);
            HGeomVector V14(v1.vectorProduct(v4));
            HGeomVector V24(v2.vectorProduct(v4));
            Double_t B2 = v2.scalarProduct(V14);
            Double_t D2    = (p2 - p4).scalarProduct(V14);
            Double_t F2    = v4.scalarProduct(P41.vectorProduct(P21));
            Double_t E2    = P41.scalarProduct(V24);

            Double_t BE    = B1*E2 - B2*E1;
            Double_t BC    = 0.5/BE;
            Double_t DEpBF = D1*E2 - D2*E1 + B1*F2 - B2*F1;
            Double_t SQ    = TMath::Sqrt(DEpBF*DEpBF - 4*BE*(D1*F2 - D2*F1));
            Double_t t2    = (SQ - DEpBF)*BC;  
            Double_t t1    = -(E2*t2 + F2)/(B2*t2 + D2);
            Double_t t2b   = (-SQ - DEpBF)*BC;  
            Double_t t1b   = -(E2*t2b + F2)/(B2*t2b + D2);
            if(TMath::Abs(t1b) < TMath::Abs(t1)) {
              t2=t2b;
              t1=t1b;
            }
            HGeomVector P5(v1);
            P5 *= t1;
            P5 += p1;
            HGeomVector Q(v2);
            Q *= t2;
            Q += p2;
            Q -= P5;
            tmpLineParam.setParam(P5,Q);


            // Collecting wires:
            Int_t iLayer  = 0; //seg != 1 ?  0 : 12;
            Int_t lastLay = 12; //seg == 0 ? 12 : 24;

            tmpListCells.clear();
            for(;iLayer<lastLay;iLayer++) {
              Int_t mod = iLayer/6;
// ????              if(fSCModAr[mod]==0) continue;
              Int_t lay = iLayer%6;
              HMdcSizesCellsLayer &fSCLay   = pSCSec[mod][lay];
              HMdcLayListCells    &pLaylist = pSListCells[mod][lay]; 
              Float_t fcell1,fcell2;
              if( !fSCLay.calcCrossedCells(tmpLineParam,fcell1,fcell2) ) continue;
              Int_t c1 = fcell1 - 0.5;
              Int_t c2 = fcell2 + 0.5;
              for(Int_t c=c1;c<=c2;c++) {
                if(pLaylist.isCell(c) && pLaylist.getNFitted(c)==0 && fSCLay[c].getStatus()) {
                  tmpListCells.setTime(iLayer,c,1);
                }
              }
            }
            Int_t nLays = tmpListCells.getNLayers();

            if(nLays <  nLaysBest) continue;
            if(nLays == nLaysBest && nCellsBest >= tmpListCells.getNCells()) continue;  //?????????????

            listCells  = tmpListCells;
            nLaysBest  = nLays; 
            nCellsBest = listCells.getNCells();
            lineParam  = tmpLineParam;
            //  if(nLayersMax==nLaysBest && nCellsBest+1 >= nWiresMax) return nLaysBest; // Old version.
            if(nLayersMax==nLaysBest) return nLaysBest; // ????????????
          }
        }
      }
    }
  }
  return nLaysBest;
}
