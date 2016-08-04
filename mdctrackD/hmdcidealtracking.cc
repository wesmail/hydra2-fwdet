//*--- Author : V.Pechenov
//*--- Modified: 06.07.05 V.Pechenov

using namespace std;
#include <iostream>
#include <iomanip>
#include "hmdcidealtracking.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hmdcgetcontainers.h"
#include "hmdcsizescells.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hmdccal1sim.h"
#include "hmdcsegsim.h"
#include "hmdchitsim.h"
#include "hmdctrkcand.h"
#include "TRandom.h"


//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HMdcIdealTracking
//
// Filling of HMdcHitSim, HMdcSegSim and HMdcTrackCand containers
// by pure GEANT information. With fillParallelCategories() the
// reconstructor is forced to fill HMdcHitIdeal, HMdcSegIdeal and
// HMdcTrackCandIdeal (catMdcHitIdeal,catMdcSegIdeal,catMdcTrkCandIdeal).
// This catgeories are copies of the above. In this case the ideal tracking
// can be run in parallel to the normal tracking.
//
// HMdcHitSim is filled by corresponding HGeantMdc information. 
// HMdcSegSim - by two HGeantMdc hits (only x and y position is used)
//              or by HGeantMdc if only one Mdc is active in segment
//              The list of wires is filled if catMdcCal1Sim exists.
// HMdcTrackCand - for true combinations of inner and outer segments only
//
// HMdcHit::getTrackFinder() return 2 for containers filled by this
// code.
//
// Conditions of HGeantMdc hits selection:
// 1. Direction: target->meta.
// 2. Hits in the same sector only (after selection 1.
// 3. For inner segment >=5 HGeantMdc hits (or HMdcCal1Sim hist).
// 4. For outer segment must be inner one.
// 5. For outer segment >=4 HGeantMdc hits (or HMdcCal1Sim hist).
//
//////////////////////////////////////////////////////////////////////////////


ClassImp(HMdcIdealTracking)

HMdcIdealTracking::HMdcIdealTracking(void) {
  // Constructor
  clear();
}

HMdcIdealTracking::HMdcIdealTracking(const Text_t *name,const Text_t *title) : 
    HReconstructor(name,title) {
  // Constructor
  clear();
}

HMdcIdealTracking::~HMdcIdealTracking() {
  // Destructor
  HMdcSizesCells::deleteCont();
  if(iterGeantKine) delete iterGeantKine;
  iterGeantKine=0;
  if(lCells) delete lCells;
  lCells=0;
}

void HMdcIdealTracking::clear(void) {
  // Setting pointer to the categories to NULL
  pGeantKineCat       = NULL;
  pGeantMdcCat        = NULL;
  pMdcCal1Cat         = NULL;
  pMdcSegCat          = NULL;
  pMdcHitCat          = NULL;
  pMdcTrkCandCat      = NULL;
  iterGeantKine       = NULL;
  fillParallel        = kFALSE;
  lCells              = 0;
  nFiredLayersSeg1cut = 5;
  nFiredLayersSeg2cut = 4;
  setResolutionX(0.,0.,0.,0.);
  setResolutionY(0.,0.,0.,0.);
}

Bool_t HMdcIdealTracking::init(void) {
  // Categories and parameters initialization
  HMdcGetContainers* fGetCont = HMdcGetContainers::getObject();
  pGeantKineCat = fGetCont->getCatGeantKine();
  if(pGeantKineCat == 0) {
    Error("init","No catGeantKine in tree!");
    return kFALSE;
  }
  iterGeantKine=(HIterator*)pGeantKineCat->MakeIterator();
  pGeantMdcCat  = fGetCont->getCatGeantMdc();
  if(pGeantMdcCat == 0) {
    Error("init","No catGeantMdc in tree!");
    return kFALSE;
  }
  HMdcDetector* pMdcDetector = fGetCont->getMdcDetector();
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) isMdcActive[s][m] = fGetCont->isModActive(s,m);

  if(!fillParallel)
  {
      pMdcSegCat = gHades->getCurrentEvent()->getCategory(catMdcSeg);
      if(!pMdcSegCat) {
	  pMdcSegCat = pMdcDetector->buildMatrixCategory("HMdcSegSim",0.5F);
	  if (!pMdcSegCat) return kFALSE;
	  gHades->getCurrentEvent()->addCategory(catMdcSeg,pMdcSegCat,"Mdc");
      } else {
	  const Text_t* clName=pMdcSegCat->getClassName();
	  if(strcmp(clName,"HMdcSegSim")!=0) {
	      Error("init","Category catMdcSeg exist but class name is not HMdcSegSim!");
	      return kFALSE;
	  } else Warning("init","Category catMdcSeg exist already!");
      }
  }
  if(fillParallel)
  {
      pMdcSegCat = gHades->getCurrentEvent()->getCategory(catMdcSegIdeal);
      if(!pMdcSegCat) {
	  pMdcSegCat = pMdcDetector->buildMatrixCategory("HMdcSegIdeal",0.5F);
	  if (!pMdcSegCat) return kFALSE;
	  gHades->getCurrentEvent()->addCategory(catMdcSegIdeal,pMdcSegCat,"Mdc");
      } else {
	  const Text_t* clName=pMdcSegCat->getClassName();
	  if(strcmp(clName,"HMdcSegIdeal")!=0) {
	      Error("init","Category catMdcSegIdeal exist but class name is not HMdcSegIdeal!");
	      return kFALSE;
	  } else Warning("init","Category catMdcSegIdeal exist already!");
      }
  }
  if(!fillParallel)
  {
      pMdcHitCat = gHades->getCurrentEvent()->getCategory(catMdcHit);
      if (!pMdcHitCat) {
	  pMdcHitCat = pMdcDetector->buildMatrixCategory("HMdcHitSim",0.5F);
	  if (!pMdcHitCat) return kFALSE;
	  gHades->getCurrentEvent()->addCategory(catMdcHit,pMdcHitCat,"Mdc");
      } else {
	  const Text_t* clName=pMdcHitCat->getClassName();
	  if(strcmp(clName,"HMdcHitSim")!=0) {
	      Error("init","Category catMdcHit exist but class name is not HMdcHitSim!");
	      return kFALSE;
	  } else Warning("init","Category catMdcHit exist already!");
      }
  }

  if(fillParallel)
  {
      pMdcHitCat = gHades->getCurrentEvent()->getCategory(catMdcHitIdeal);
      if (!pMdcHitCat) {
	  pMdcHitCat = pMdcDetector->buildMatrixCategory("HMdcHitIdeal",0.5F);
	  if (!pMdcHitCat) return kFALSE;
	  gHades->getCurrentEvent()->addCategory(catMdcHitIdeal,pMdcHitCat,"Mdc");
      } else {
	  const Text_t* clName=pMdcHitCat->getClassName();
	  if(strcmp(clName,"HMdcHitIdeal")!=0) {
	      Error("init","Category catMdcHitIdeal exist but class name is not HMdcHitIdeal!");
	      return kFALSE;
	  } else Warning("init","Category catMdcHitIdeal exist already!");
      }
  }

  if(!fillParallel)
  {
      pMdcTrkCandCat = gHades->getCurrentEvent()->getCategory(catMdcTrkCand);
      if (!pMdcTrkCandCat) {
	  pMdcTrkCandCat = pMdcDetector->buildMatrixCategory("HMdcTrkCand",0.5F);
	  if (!pMdcTrkCandCat) return kFALSE;
	  gHades->getCurrentEvent()->addCategory(catMdcTrkCand,pMdcTrkCandCat,"Mdc");
      } else {
	  const Text_t* clName=pMdcTrkCandCat->getClassName();
	  if(strcmp(clName,"HMdcTrkCand")!=0) {
	      Error("init","Category catMdcTrkCand exist but class name is not HMdcTrkCand!");
	      return kFALSE;
	  } else Warning("init","Category catMdcTrkCand exist already!");
      }
  }
  if(fillParallel)
  {
      pMdcTrkCandCat = gHades->getCurrentEvent()->getCategory(catMdcTrkCandIdeal);
      if (!pMdcTrkCandCat) {
	  pMdcTrkCandCat = pMdcDetector->buildMatrixCategory("HMdcTrkCandIdeal",0.5F);
	  if (!pMdcTrkCandCat) return kFALSE;
	  gHades->getCurrentEvent()->addCategory(catMdcTrkCandIdeal,pMdcTrkCandCat,"Mdc");
      } else {
	  const Text_t* clName=pMdcTrkCandCat->getClassName();
	  if(strcmp(clName,"HMdcTrkCandIdeal")!=0) {
	      Error("init","Category catMdcTrkCandIdeal exist but class name is not HMdcTrkCandIdeal!");
	      return kFALSE;
	  } else Warning("init","Category catMdcTrkCandIdeal exist already!");
      }
  }

  pMdcCal1Cat = fGetCont->getCatMdcCal1();
  if(pMdcCal1Cat) {
    const Text_t* clName=pMdcCal1Cat->getClassName();
    if(strcmp(clName,"HMdcCal1Sim")!=0) {
      Error("init","Category catMdcCal1 exist but class name is not HMdcCal1Sim!");
      return kFALSE;
    }
    lCells=new HMdcList24GroupCells();
  }
  pMSizesCells = HMdcSizesCells::getObject();
  locSeg.set(2,0,0);
  locHit.set(2,0,0);
  locTrkCand.set(1,0);

  return kTRUE;
}

Bool_t HMdcIdealTracking::reinit(void) {
  // Parameters reinitialization
  if(!pMSizesCells->initContainer()) return kFALSE;
  return kTRUE;
}

Int_t HMdcIdealTracking::execute(void) {
  // Filling of HMdcHitSim, HMdcSegSim and HMdcTrackCand containers
  // by pure GEANT information.
  // HMdcHitSim is filled by corresponding HGeantMdc information. 
  // HMdcSegSim - by two HGeantMdc hits (only x and y position is used)
  //              or by HGeantMdc if only one Mdc is active in segment
  // HMdcTrackCand - for true combinations of inner and outer segments only
  iterGeantKine->Reset();
  HGeantKine* pGeantKine;
  while((pGeantKine=(HGeantKine*)iterGeantKine->Next())) {
    trackNumber = pGeantKine->getTrack();
    if(!testTrack(pGeantKine)) continue;
    Int_t indxSeg1=fillHitsSeg(0);
    if(indxSeg1<0) continue;
    Int_t indxSeg2=fillHitsSeg(1);
    HMdcTrkCand* pTrkCand=fillTrkCandISeg(indxSeg1);
    if(indxSeg2>=0) pTrkCand=fillTrkCandOSeg(pTrkCand,indxSeg2);
  }
  return 0;
}

Bool_t HMdcIdealTracking::testTrack(HGeantKine* pGeantKine) {
  // Selection tracks.
  // Conditions of HGeantMdc hits selection:
  // 1. Direction: target->meta 
  // 2. Hits in the same sector only (after selection 1.
  // 3. For inner segment >=nFiredLayersSeg1cut HGeantMdc hits (or HMdcCal1Sim hits)
  // 4. For outer segment must be inner one.
  // 5. For outer segment >=nFiredLayersSeg2cut HGeantMdc hits (or HMdcCal1Sim hits)
  if(lCells) lCells->clear();
  pGeantKine->resetMdcIter();
  HGeantMdc* pGeantMdc;
  Int_t sec=-1;
  Int_t mod=-1;
  Int_t lay=-1;
  trackSector=-1;
  for(Int_t m=0;m<4;m++) {
    nGeantMdcLay[m]=0;
    geantHitMod[m]=0;
    for(Int_t l=0;l<6;l++) geantHitLay[m][l]=0;
  }

  while((pGeantMdc=(HGeantMdc*)pGeantKine->nextMdcHit()) != NULL) {
    Int_t sector = pGeantMdc->getSector();
    Int_t module = pGeantMdc->getModule();
    Int_t layer  = pGeantMdc->getLayer();
    
    Float_t ath,aph;
    pGeantMdc->getIncidence(ath,aph);
    if(ath>=90.)         { break;} // track must not go back!
    if(sec<0) sec=sector;
    else if(sec!=sector) { break;} // track must be in one sector only!
    if(module<mod)       { break;} // track must not go back!
    else if(module>mod) {
      mod=module;
      lay=-1;
    }
    if(layer!=6) {
	if(layer<=lay)   { break;} // track must not go back!
	if(geantHitLay[module][layer] != 0) { break; } // must be one hit in layer
      lay=layer;
      if(isMdcActive[sector][module]) {
	geantHitLay[module][layer]=pGeantMdc;
        nGeantMdcLay[module]++;
        if(pMdcCal1Cat) collectWires(sector,module,layer,pGeantMdc);
      }
    } else {
	if(geantHitMod[module] != 0) { break;} // must be one hit in mdc
      if(isMdcActive[sector][module]) geantHitMod[module]=pGeantMdc;
    }
  }
  if(lCells!=NULL && pMdcCal1Cat!=NULL) for(Int_t m=0;m<4;m++) nGeantMdcLay[m]=lCells->getNLayersMod(m);
  // Test inner segment:
  if(geantHitMod[0]==NULL && geantHitMod[1]==NULL) { return kFALSE; }
  if(nGeantMdcLay[0]+nGeantMdcLay[1] < nFiredLayersSeg1cut)    { return kFALSE; }
  // Test outer segment:
  if(nGeantMdcLay[2]+nGeantMdcLay[3] < nFiredLayersSeg2cut) {
    geantHitMod[2]  = geantHitMod[3]  = NULL;
    nGeantMdcLay[2] = nGeantMdcLay[3] = 0;
  }
  trackSector=sec;
  return kTRUE;
}

void HMdcIdealTracking::collectWires(Int_t s,Int_t m, Int_t l,
    HGeantMdc* pGeantMdc) {
  // Collecting list of fired wires (if category catMdcCal1 exist)
  HMdcSizesCellsLayer& pSCLay=(*pMSizesCells)[s][m][l];
  if(&pSCLay==0) return;
  Float_t ax,ay,atof,ptof;
  pGeantMdc->getHit(ax,ay,atof,ptof);
  Int_t cell=pSCLay.calcCellNum(ax,ay); // Not precise, but well enough!
  Int_t c=(cell<3) ? 0:cell-3;
  Int_t cmax=cell+3;
  for(;c<=cmax;c++) {
    locCal1.set(4,s,m,l,c);
    HMdcCal1Sim* pMdcCal1=(HMdcCal1Sim*)pMdcCal1Cat->getObject(locCal1);
    if(pMdcCal1==0) continue;
    Int_t nHits=pMdcCal1->getNHits();
    if(nHits > 2 || nHits==0) continue;
    UChar_t times = (nHits==-2) ? 3:1;
    if((times&1) && (pMdcCal1->getStatus1()<=0           || 
                     pMdcCal1->getNTrack1()!=trackNumber ||
                     TMath::Abs(atof-pMdcCal1->getTof1())>0.05) ) times &= 2;
    if((times&2) && (pMdcCal1->getStatus2()<=0           || 
                     pMdcCal1->getNTrack2()!=trackNumber ||
                     TMath::Abs(atof-pMdcCal1->getTof2())>0.05) ) times &= 1;
    if(times==0) continue;
    lCells->setTime(l+m*6,c,times);
  }
}

Int_t HMdcIdealTracking::fillHitsSeg(Int_t segment) {
  // Filling HMdcSegSim container
  Int_t m1=segment*2;
  Int_t m2=m1+1;
  if(trackSector<0) return -1;
  if(geantHitMod[m1]==0 && geantHitMod[m2]==0) return -1;
  Int_t indx1=fillHit(m1);
  Int_t indx2=fillHit(m2);
  if(indx1<0 && indx2<0) return -1;
  Int_t index=-1;
  HMdcSegSim* pMdcSeg = getSegSlot(segment,index);
  if(!pMdcSeg) return -1;
  HMdcSizesCellsSec& fSCSec=(*pMSizesCells)[trackSector];
  if(&fSCSec==0) return -1;
  Double_t zm,r0,theta,phi,zPrime,rPrime;
  if(indx1>=0 && indx2>=0) {
    HMdcSizesCells::calcMdcSeg(x1[m1],y1[m1],z1[m1],x1[m2],y1[m2],z1[m2],zm,r0,theta,phi);
    fSCSec.calcRZToTargLine(x1[m1],y1[m1],z1[m1],x1[m2],y1[m2],z1[m2],zPrime,rPrime);
    pMdcSeg->setXYPar(x1[m1],y1[m1],x1[m2],y1[m2]);
  } else {
    Int_t m=(indx2<0) ? m1:m2;
    HMdcSizesCells::calcMdcSeg(x1[m],y1[m],z1[m],x2[m],y2[m],z2[m],zm,r0,theta,phi);
    fSCSec.calcRZToTargLine(x1[m],y1[m],z1[m],x2[m],y2[m],z2[m],zPrime,rPrime);
    pMdcSeg->setXYPar(x1[m],y1[m],x2[m],y2[m]);
  }
  pMdcSeg->setPar(zm,r0,theta,phi);
  pMdcSeg->setZprime(zPrime);
  pMdcSeg->setRprime(rPrime);
  if(indx1>=0) pMdcSeg->setHitInd(0,indx1);
  if(indx2>=0) pMdcSeg->setHitInd(1,indx2);
  UChar_t nHitsSeg = nGeantMdcLay[m1]+nGeantMdcLay[m2];
  pMdcSeg->setNTracks(1,&trackNumber, &nHitsSeg);
  pMdcSeg->setNTracks(1,1,0);
  pMdcSeg->setChi2(1.); //!!!???
  setWires(pMdcSeg,segment);
  return index;
}

HMdcSegSim* HMdcIdealTracking::getSegSlot(Int_t segment, Int_t& index) {
  // Geting a slot for HMdcSegSim obj. and setting address
  locSeg[0]=trackSector;
  locSeg[1]=segment;
  HMdcSegSim* pMdcSeg = (HMdcSegSim*)pMdcSegCat->getNewSlot(locSeg,&index);
  if(!pMdcSeg) {
    Warning("getSegSlot"," No slot in catMdcSeg available");
    return 0;
  }
  pMdcSeg = new(pMdcSeg) HMdcSegSim;
  pMdcSeg->setSec(trackSector);
  pMdcSeg->setIOSeg(segment);
  return pMdcSeg;
}

Int_t HMdcIdealTracking::fillHit(Int_t module) {
  // Filling HMdcHitSim container
  HGeantMdc* pGeantMdc=geantHitMod[module];
  if(pGeantMdc==0) return -1;
  Int_t index=-1;
  HMdcHitSim* pMdcHit = getHitSlot(module,index);
  if(!pMdcHit) return -1;
  HMdcSizesCellsMod& pSCMod=(*pMSizesCells)[trackSector][module];
  if(&pSCMod==0) return -1;
  Float_t ax,ay,atof,ptof,ath,aph;
  pGeantMdc->getHit(ax,ay,atof,ptof);


  //------------------------------------------
  // resolution smearing in x,y (neglect phi,theta)
  // default sigmas = 0
  if(sigX[module]){
      ax += gRandom->Gaus() * sigX[module];
  }
  if(sigY[module]){
      ay += gRandom->Gaus() * sigY[module];
  }
  //------------------------------------------

  pGeantMdc->getIncidence(ath,aph);
  Double_t theta=ath*TMath::DegToRad();
  Double_t phi  =aph*TMath::DegToRad();
  Double_t sinTh=sin(theta);
  Double_t xDir=sinTh*cos(phi);
  Double_t yDir=sinTh*sin(phi);
  Double_t zDir=sqrt(1.-sinTh*sinTh);
  pMdcHit->setPar(ax,ay,xDir,yDir, atof);
  pMdcHit->setX(ax,sigX[module]);
  pMdcHit->setY(ay,sigY[module]);
  pMdcHit->setChi2(0.); //!!!???
  pMdcHit->setTrackFinder(2);  // !!!
  pMdcHit->setNTracks(1,&trackNumber, nGeantMdcLay+module);
  x2[module]=x1[module]=ax;
  y2[module]=y1[module]=ay;
  z2[module]=z1[module]=0.;
  x2[module]+=xDir*1000.;
  y2[module]+=yDir*1000.;
  z2[module]+=zDir*1000.;
  pSCMod.transFromZ0(x1[module],y1[module],z1[module]);
  pSCMod.transFrom(x2[module],y2[module],z2[module]);
  setWires(pMdcHit,module);
  return index;
}

HMdcHitSim* HMdcIdealTracking::getHitSlot(Int_t module, Int_t& index) {
  // Geting a slot for HMdcHitSim obj. and setting address
  locHit[0]=trackSector;
  locHit[1]=module;
  HMdcHitSim* pMdcHit = (HMdcHitSim*)pMdcHitCat->getNewSlot(locHit,&index);
  if(!pMdcHit) {
    Warning("getHitSlot"," No slot in catMdcHit available");
    index=-1;
    return 0;
  }
  pMdcHit=new(pMdcHit) HMdcHitSim;
  pMdcHit->setSecMod(trackSector,module);
  return pMdcHit;
}

HMdcTrkCand* HMdcIdealTracking::fillTrkCandISeg(Int_t segIndex) {
  // Filling HMdcTrkCand container by inner segment
  locTrkCand[0]=trackSector;
  Int_t index;
  HMdcTrkCand* pTrkCand = 
      (HMdcTrkCand*)pMdcTrkCandCat->getNewSlot(locTrkCand,&index);
  if(!pTrkCand) {
    Warning("fillTrkCandISeg"," No slot available in catMdcTrkCand");
    return 0;
  }
  return new(pTrkCand) HMdcTrkCand(trackSector,segIndex,index);
}

HMdcTrkCand* HMdcIdealTracking::fillTrkCandOSeg(HMdcTrkCand* fTCand,
    Int_t segIndex) {
  // Filling HMdcTrkCand container by outer segment
  Int_t index;
  HMdcTrkCand* pTrkCand =
      (HMdcTrkCand*)pMdcTrkCandCat->getNewSlot(locTrkCand,&index);
  if(!pTrkCand) {
    Warning("fillTrkCandOSeg"," No slot available in catMdcTrkCand");
    return 0;
  }
  return new(pTrkCand) HMdcTrkCand(fTCand,segIndex,index);
}

Bool_t HMdcIdealTracking::finalize(void) {
  return kTRUE;
}
 
void HMdcIdealTracking::setWires(HMdcSegSim* pMdcSeg, Int_t seg) {
  // Filling list of cells in HMdcSeg
  if(!lCells) return;
  Int_t lc = seg*12;
  for(Int_t l=0; l<12; l++,lc++) pMdcSeg->setLayerGroup(l,lCells->getOneLayerGroup(lc));
  UChar_t  nHitsSeg = pMdcSeg->getSumWires();
  pMdcSeg->setNTracks(1,&trackNumber, &nHitsSeg);
}

void HMdcIdealTracking::setWires(HMdcHitSim* pMdcHit, Int_t mod) {
  // Filling list of cells in HMdcHit
  if(!lCells) return;
  Int_t lc = mod*6;
  for(Int_t l=0; l<6; l++,lc++) pMdcHit->setLayerGroup(l,lCells->getOneLayerGroup(lc));
  UChar_t  nHitsHit=pMdcHit->getSumWires();
  pMdcHit->setNTracks(1,&trackNumber, &nHitsHit);
}

void HMdcIdealTracking::printStatus(void) {
  // prints the parameters to the screen

  printf("--------------------------------------------------------------------------------------------\n");
  printf("HMdcIdealTrackingSetup:\n");
  printf("Minimal number of fired layers in inner segment = %i\n",nFiredLayersSeg1cut);
  printf("Minimal number of fired layers in outer segment = %i\n",nFiredLayersSeg2cut);
  printf("Sigma for gaussian resolution for MDC in x [mm] = %5.2g %5.2g %5.2g %5.2g\n",sigX[0],sigX[1],sigX[2],sigX[3]);
  printf("Sigma for gaussian resolution for MDC in y [mm] = %5.2g %5.2g %5.2g %5.2g\n",sigY[0],sigY[1],sigY[2],sigY[3]);
  if(fillParallel) {
    printf("Fill HMdcHitSim,HMdcSegSim and HMdcTrkCand to ideal categories\n");
    printf("     catMdcSegIdeal, catMdcHitIdeal, catMdcTrkCandIdeal\n");
  }
  printf ("--------------------------------------------------------------------------------------------\n");
}
