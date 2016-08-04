//*--- AUTHOR : Vladimir Pechenov
//*--- Modified : 18/08/05 V.Pechenov

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcDriftTimePar
//
// Container class for the distance->time parameterization.
// Contains functions for calculating distance->time for track fitter.
//
///////////////////////////////////////////////////////////////////////////////

#include "hmdcdrifttimepar.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hmdccal2parsim.h"
#include "hmdccal2par.h"
#include "hmdcgetcontainers.h"

ClassImp(HMdcDriftTimeParBin)
ClassImp(HMdcDriftTimeParAngle)
ClassImp(HMdcDriftTimeParMod)
ClassImp(HMdcDriftTimeParSec)
ClassImp(HMdcDriftTimePar)



HMdcDriftTimeParBin::HMdcDriftTimeParBin(void) {
  c0 = 0.;
  c1 = 0.;
  c2 = 0.;
  c3 = 0.;
  d0 = 0.;
  d1 = 0.;
  d2 = 0.;
  d3 = 0.;
}

void HMdcDriftTimeParBin::print(void) const {
  printf("DriftTime = %.5g + %.5g*alpha + %.5g*dist + %.5g*alpha*dist\n",
      c0,c1,c2,c3);
  printf("ErrDriftTime = %.5g + %.5g*alpha + %.5g*dist + %.5g*alpha*dist\n",
      c0err,c1err,c2err,c3err);
}

void HMdcDriftTimeParBin::printTable(void) const {
  printf("| %10.4g | %10.4g | %10.4g | %10.4g || %10.4g | %10.4g | %10.4g | %10.4g |\n",
      c0,c1,c2,c3,c0err,c1err,c2err,c3err);
}

HMdcDriftTimeParMod::HMdcDriftTimeParMod(Int_t nAnBins, Int_t nDsBins) {
  // Constructor create array of HMdcDriftTimeParAngle objects.
  // One object per one impact angle bin.
//  for(Int_t i=0; i<nAnBins; ++i) AddAt(new HMdcDriftTimeParAngle(),i);
  nAngleBins      = nAnBins;
  lAngleBin       = nAngleBins-1;
  angleBinSize    = 90./nAngleBins;
  lastAngleBin    = lAngleBin*angleBinSize;
  invAngleBinSize = 1./angleBinSize;
  nDistBins       = nDsBins;
  lDistBin        = nDistBins-1;
  setDistBinSize(0.1); // default bin size
}

void HMdcDriftTimeParMod::setDistBinSize(Double_t dBSize) {
  distBinSize    = dBSize;
  lastBinDist    = lDistBin*distBinSize;
  invDistBinSize = 1./distBinSize;
}

void HMdcDriftTimeParMod::setTimeBinSize(Double_t tBSize) {
  timeBinSize = tBSize;
  lastBinTime = lDistBin*timeBinSize;
  invTimeBinSize = 1./timeBinSize;
}

Bool_t HMdcDriftTimeParMod::initContainer(HMdcCal2ParModSim& fC2PModS,HMdcCal2ParMod& fC2PMod,
                                          Double_t slOut,Double_t scaleErr) {
  slopeOutside = slOut;
  for(Int_t anBin=0; anBin<nAngleBins; anBin++) {
    for(Int_t dsBin=0; dsBin<nDistBins; dsBin++) {
      HMdcDriftTimeParBin* bin=at(anBin)->at(dsBin);
      if(bin==0) return kFALSE;
      HMdcCal2ParAngleSim& rAng1 = fC2PModS[anBin];// pointer to the first set
      HMdcCal2ParAngleSim& rAng2 = anBin<lAngleBin ? fC2PModS[anBin+1] : rAng1;
      
      fillDriftTime(rAng1,rAng2,bin,anBin,dsBin);
      fillDriftTimeErr(rAng1,rAng2,bin,anBin,dsBin,scaleErr);
      
      HMdcCal2ParAngle& tAng1 = fC2PMod[anBin];// pointer to the first set
      HMdcCal2ParAngle& tAng2 = anBin<lAngleBin ? fC2PMod[anBin+1] : tAng1;
      fillDriftDist(tAng1,tAng2,bin,anBin,dsBin);
    }
  }
  return kTRUE;
}

void HMdcDriftTimeParMod::print(void) {
  for(Int_t anBin=0; anBin<nAngleBins; anBin++) {
    for(Int_t dsBin=0; dsBin<nDistBins; dsBin++) {
      HMdcDriftTimeParBin* bin=at(anBin)->at(dsBin);
      if(bin==0) continue;
//      printf("ang.bin=%i dist.bin=%i: ",anBin,dsBin);
      printf("%3i   %3i ",anBin,dsBin);
      bin->printTable();
    }
  }
}

void HMdcDriftTimeParMod::fillDriftTime(HMdcCal2ParAngleSim& rAng1,
    HMdcCal2ParAngleSim& rAng2, HMdcDriftTimeParBin* bin,
    Int_t anBin, Int_t dsBin) {
  Double_t k0=90./angleBinSize-anBin;
  if(dsBin==lDistBin) {  // distance outside matrix
    Double_t y1=rAng1.getDriftTime1(lDistBin);
    Double_t y2=rAng2.getDriftTime1(lDistBin);
    Double_t k1=y1-y2;
    bin->setC0(y1-lastBinDist*slopeOutside - k0*k1);
    bin->setC1(k1/angleBinSize);
    bin->setC2(slopeOutside);
    bin->setC3(0.);
  } else {
    Double_t y1=rAng1.getDriftTime1(dsBin);
    Double_t y2=rAng2.getDriftTime1(dsBin);
    Double_t y3=rAng2.getDriftTime1(dsBin+1);
    Double_t y4=rAng1.getDriftTime1(dsBin+1);
    Double_t k1=y1-y2;
    Double_t k2=y1-y4;
    Double_t k3=k1+y3-y4;
    k1+=dsBin*k3;
    bin->setC0(y1 + dsBin*k2 - k0*k1);
    bin->setC1(k1/angleBinSize);
    bin->setC2((k0*k3-k2)/distBinSize);
    bin->setC3(-k3/distBinSize/angleBinSize);
  }
}

void HMdcDriftTimeParMod::fillDriftTimeErr(HMdcCal2ParAngleSim& rAng1,
    HMdcCal2ParAngleSim& rAng2, HMdcDriftTimeParBin* bin,Int_t anBin, Int_t dsBin,Double_t scaleErr) {
  Double_t k0=90./angleBinSize-anBin;
  if(dsBin==lDistBin) {  // distance outside matrix
    Double_t y1=rAng1.getDriftTime1Error(lDistBin) * scaleErr;
    Double_t y2=rAng2.getDriftTime1Error(lDistBin) * scaleErr;
    Double_t k1=y1-y2;
    bin->setC0Err(y1 - k0*k1);
    bin->setC1Err(k1/angleBinSize);
    bin->setC2Err(0.);
    bin->setC3Err(0.);
  } else {
    Double_t y1=rAng1.getDriftTime1Error(dsBin)   * scaleErr;
    Double_t y2=rAng2.getDriftTime1Error(dsBin)   * scaleErr;
    Double_t y3=rAng2.getDriftTime1Error(dsBin+1) * scaleErr;
    Double_t y4=rAng1.getDriftTime1Error(dsBin+1) * scaleErr;
    Double_t k1=y1-y2;
    Double_t k2=y1-y4;
    Double_t k3=k1+y3-y4;
    k1+=dsBin*k3;
    bin->setC0Err(y1 + dsBin*k2 - k0*k1);
    bin->setC1Err(k1/angleBinSize);
    bin->setC2Err((k0*k3-k2)/distBinSize);
    bin->setC3Err(-k3/distBinSize/angleBinSize);
  }
}

void HMdcDriftTimeParMod::fillDriftDist(HMdcCal2ParAngle& tAng1,HMdcCal2ParAngle& tAng2, 
    HMdcDriftTimeParBin* bin,Int_t anBin, Int_t tmBin) {
  Double_t k0=90./angleBinSize-anBin;
  if(tmBin==lDistBin) {  // distance outside matrix
    Double_t y1 = tAng1.getDistance(lDistBin/10,lDistBin%10);
    Double_t y2 = tAng2.getDistance(lDistBin/10,lDistBin%10);
    Double_t k1 = y1-y2;
    bin->setD0(y1-lastBinTime/slopeOutside - k0*k1);
    bin->setD1(k1/angleBinSize);
    bin->setD2(1./slopeOutside);
    bin->setD3(0.);
  } else {
    Double_t y1 = tAng1.getDistance(tmBin/10,tmBin%10);
    Double_t y2 = tAng2.getDistance(tmBin/10,tmBin%10);
    Double_t y3 = tAng2.getDistance((tmBin+1)/10,(tmBin+1)%10);
    Double_t y4 = tAng1.getDistance((tmBin+1)/10,(tmBin+1)%10);
    Double_t k1 = y1-y2;
    Double_t k2 = y1-y4;
    Double_t k3 = k1+y3-y4;
    k1 += tmBin*k3;
    bin->setD0(y1 + tmBin*k2 - k0*k1);
    bin->setD1(k1/angleBinSize);
    bin->setD2((k0*k3-k2)/timeBinSize);
    bin->setD3(-k3/timeBinSize/angleBinSize);
  }
}

Bool_t HMdcDriftTimeParMod::testSizes(Int_t anSz, Int_t dsSz) const {
  return anSz==nAngleBins && dsSz==nDistBins;
}

HMdcDriftTimeParSec::HMdcDriftTimeParSec(void) : TObjArray(4) {
  // Constructor create TObjArray for HMdcDriftTimeParMod objects.
}

Bool_t HMdcDriftTimeParSec::initContainer(HMdcCal2ParSecSim& fC2PSecS,HMdcCal2ParSec& fC2PSec,
                                          Int_t sec, Double_t slOut,Double_t* scaleError) {
  sector=  sec;
  for(Int_t m=0; m<4; m++) if(HMdcGetContainers::getObject()->isModActive(sec,m)) {
    HMdcCal2ParModSim& fC2PModS = fC2PSecS[m];
    if(&fC2PModS == 0) return kFALSE;
    HMdcCal2ParMod& fC2PMod = fC2PSec[m];
    if(&fC2PModS == 0) return kFALSE;
    Int_t    nAngleBins  = fC2PModS.getSize();
    Int_t    nDistBins   = 100;    //!!!!!
    Double_t distBinSize = 0.1;    //!!!!!
    Double_t timeBinSize = 4.0;    //!!!!!
    if(At(m)==0) AddAt(new HMdcDriftTimeParMod(nAngleBins,nDistBins),m);
    else if(!at(m)->testSizes(nAngleBins,nDistBins)) {
      delete at(m);
      AddAt(new HMdcDriftTimeParMod(nAngleBins,nDistBins),m);
    }
    at(m)->setDistBinSize(distBinSize);
    at(m)->setTimeBinSize(timeBinSize);
    if(!at(m)->initContainer(fC2PModS,fC2PMod,slOut,scaleError[m])) return kFALSE;
  }
  return kTRUE;
}

void HMdcDriftTimeParSec::print(void) {
  for(Int_t m=0; m<4; m++) if(HMdcGetContainers::getObject()->isModActive(sector,m)) {
    printf("\n HMdcDriftTimePar: %i sector  %i module \n",sector+1,m+1);
    printf("---bin---------------------------------------------------------");
    printf(          "-----------------------------------------------------\n");
    printf("ang. dis. |     c0     |     c1     |     c2     |     c3     |");
    printf(          "|    c0err   |    c1err   |    c2err   |    c3err   |\n");
    printf("----------|------------|------------|------------|------------|");
    printf(          "|------------|------------|------------|------------|\n");
    at(m)->print();
  }
}

HMdcDriftTimeParSec::~HMdcDriftTimeParSec(void) {
  // destructor
  Delete();
}

HMdcDriftTimePar* HMdcDriftTimePar::fMdcDriftTimePar  = NULL;

HMdcDriftTimePar::HMdcDriftTimePar(void) : TObjArray(6) { // 6 sectors
  // Constructor create TObjArray for HMdcDriftTimeParSec objects.
  fMdcDriftTimePar = this;
  pCal2ParSim      = (HMdcCal2ParSim*)gHades->getRuntimeDb()->getContainer("MdcCal2ParSim");
  pCal2Par         = (HMdcCal2Par*)gHades->getRuntimeDb()->getContainer("MdcCal2Par");
  isInited         = kFALSE;
  setScalerTime1Err(1.,1.,1.,1.);
}

HMdcDriftTimePar* HMdcDriftTimePar::getObject(void) {
  if(fMdcDriftTimePar == NULL) fMdcDriftTimePar = new HMdcDriftTimePar();
  return fMdcDriftTimePar;
}

void HMdcDriftTimePar::deleteCont(void) {
  if(fMdcDriftTimePar == NULL) return;
  delete fMdcDriftTimePar;
  fMdcDriftTimePar = NULL;
}

void HMdcDriftTimePar::setScalerTime1Err(Double_t m0,Double_t m1,Double_t m2,Double_t m3) {
  scaleError[0] = m0;
  scaleError[1] = m1;
  scaleError[2] = m2;
  scaleError[3] = m3;
}

Bool_t HMdcDriftTimePar::initContainer(void) {
  if( isInited ) return kTRUE;
  if(pCal2ParSim == NULL || !HMdcGetContainers::isInited(pCal2ParSim)) return kFALSE;
  if(pCal2Par    == NULL || !HMdcGetContainers::isInited(pCal2Par))    return kFALSE;
  
  Double_t slopeOutside = pCal2ParSim->getSlopeOutside();
  for(Int_t s=0; s<6; s++) if( HMdcGetContainers::getObject()->isSecActive(s) ) {
    HMdcCal2ParSecSim& fC2PSecS = (*pCal2ParSim)[s];
    if(&fC2PSecS == NULL) return kFALSE;
    HMdcCal2ParSec& fC2PSec = (*pCal2Par)[s];
    if(&fC2PSec == NULL) return kFALSE;
    if(At(s)==0) AddAt(new HMdcDriftTimeParSec(),s);
    if( !at(s)->initContainer(fC2PSecS,fC2PSec,s,slopeOutside,scaleError) ) return kFALSE;
  }
  isInited = kTRUE;
  return kTRUE;
}

void HMdcDriftTimePar::print(void) {
  for(Int_t s=0; s<6; s++) if(HMdcGetContainers::getObject()->isSecActive(s)) at(s)->print();
}

HMdcDriftTimePar::~HMdcDriftTimePar(void) {
  // destructor
  Delete();
}
