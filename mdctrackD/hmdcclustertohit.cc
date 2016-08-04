//*-- Author : V.Pechenov
//*--Modified : 29.10.2003 by V.Pechenov
//*--Modified : 12.05.2002 by V.Pechenov
//*--Modified : 17.01.2002 by V.Pechenov
//*--Modified : 31.05.2001
//*--Modified : 09.03.2001
//*--Modified : 10.08.2000

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
//  HMdcClusterToHit("","", Int_t flagSeg1, Int_t flagSeg2)
//
//  fills the data objects of HMdcHit and HMdcSeg
//  after track candidate search task (HMdcTrackFinder).
//
// flagSeg1=-1;0;1
//   -1 - don't fill HMdcSeg
//   0 (or 1) - at the typeClustFinder=1 (see hmdctrackfinder.cc)
//     HMdcSeg will filled by hits from MDC1 (or MDC2)
// If in segment 1 is only one MDC (1 or 2) and
// flagSeg1=0 or 1  HMdcSeg will filled by Hit in this MDC.
// flagSeg2= -1;2;3
//    -/- -/-
//
//  HMdcHit and HMdcSeg:
//  Chi2 - size of cluster (in proj. plot bins)
//  user flag - "number of wires in cluster"*100 + "number of merged clusters"
//
//  Using:
//  Insert this task in task list after HMdcTrackFinder.
//
///////////////////////////////////////////////////////////////////////////////

#include "hmdcclustertohit.h"
#include "hmdcdef.h"
#include "hades.h"
#include "hevent.h"
#include "hmdchitsim.h"
#include "hmdcsegsim.h"
#include "hmdctrkcand.h"
#include "hmdcdetector.h"
#include "hspectrometer.h"
#include "hmdcgetcontainers.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hmdcclussim.h"
#include "hmdcclusinfsim.h"
#include "hmdctrackdset.h"
#include "hmdcsizescells.h"
#include <stdlib.h>

HMdcClusterToHit::HMdcClusterToHit() {
  setParContainers();
}

HMdcClusterToHit::HMdcClusterToHit(const Text_t *name, const Text_t *title) :
    HReconstructor(name,title) {
  setParContainers();
}

HMdcClusterToHit::HMdcClusterToHit(Int_t fSeg1, Int_t fSeg2) {
  HMdcTrackDSet::setMdcClusterToHit(fSeg1,fSeg2);
  setParContainers();
}

HMdcClusterToHit::HMdcClusterToHit(const Text_t *name, const Text_t *title,
    Int_t fSeg1, Int_t fSeg2) : HReconstructor(name,title) {
  HMdcTrackDSet::setMdcClusterToHit(fSeg1,fSeg2);
  setParContainers();
}

HMdcClusterToHit::~HMdcClusterToHit(void) {
  HMdcTrackDSet::clearPClusterToHit();
}

void HMdcClusterToHit::setParContainers(void) {
  prntSt=kFALSE;
  HMdcTrackDSet::setClusterToHit(this);
  tFillSeg[0] = HMdcTrackDSet::getModForSeg1();
  tFillSeg[1] = HMdcTrackDSet::getModForSeg2();
}

Bool_t HMdcClusterToHit::init(void) {
  fGetCont  = HMdcGetContainers::getObject();
  if( !fGetCont ) return kFALSE;
  fMdcDet   = fGetCont->getMdcDetector();
  fClustCat = fGetCont->getCatMdcClus();
  if( !fMdcDet || !fClustCat) return kFALSE;
  isGeant = HMdcGetContainers::isGeant();

  fClusInfCat = fGetCont->getCatMdcClusInf(kTRUE);
  
  fHitCat = gHades->getCurrentEvent()->getCategory(catMdcHit);
  if (!fHitCat) {
    if(isGeant) fHitCat = fMdcDet->buildMatrixCategory("HMdcHitSim",0.5F);
    else fHitCat = fMdcDet->buildCategory(catMdcHit);
    if (!fHitCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catMdcHit,fHitCat,"Mdc");
  }

  if(tFillSeg[0]>=0 || tFillSeg[1]>=2) {
    fSegCat = gHades->getCurrentEvent()->getCategory(catMdcSeg);
    if (!fSegCat) {
      if(isGeant) fSegCat = fMdcDet->buildMatrixCategory("HMdcSegSim",0.5F);
      else fSegCat = fMdcDet->buildCategory(catMdcSeg);
      if (!fSegCat) return kFALSE;
      else gHades->getCurrentEvent()->addCategory(catMdcSeg,fSegCat,"Mdc");
    }
  } else fSegCat=0;
  iter=(HIterator *)fClustCat->MakeIterator("native");
  
  if(tFillSeg[0]>=0) {
    fTrkCandCat = fGetCont->getCatMdcTrkCand(kTRUE);
    if(!fTrkCandCat) return kFALSE;
  } else fTrkCandCat=0;
  
  fSizesCells=HMdcSizesCells::getObject();

  locHit.set(2,0,0);
  locSeg.set(2,0,0);
  loc.set(1,0);
  locTrkCand.set(1,0);
  if(!prntSt) printStatus();
  return kTRUE;
}

Bool_t HMdcClusterToHit::reinit(void) {
  if(!fSizesCells->initContainer()) return kFALSE;
  if(!fClusInfCat) return kFALSE;
  return kTRUE;
}

Int_t HMdcClusterToHit::execute(void) {
  slotNotAv = kFALSE;
  iter->Reset();
  while ((fClst=(HMdcClus*)iter->Next())!=0) {
    clusIndex     = fClst->getOwnIndex();
    Short_t seg   = fClst->getIOSeg();
    locTrkCand[0] = fClst->getSec();
    fTrkCand      = 0;
    if(seg==0) {
      fillContainers();
      if( slotNotAv ) return kSkipEvent;
      Int_t first,last;
      fClst->getIndexRegChilds(first,last);
      for(Int_t ind=first;ind<=last;ind++) {
        fClst=(HMdcClus*)fClustCat->getObject(ind);
        if(fClst==0) {
          Error("execute"," Mismatching in clusters category!");
          continue;
        }
        fillContainers();
        if( slotNotAv ) return kSkipEvent;
      }
    } else if(fClst->getIndexParent()<0) fillContainers();
  }
  return 0;
}

void HMdcClusterToHit::fillContainers(void) {
  Short_t sec   = fClst->getSec();
  Short_t modCl = fClst->getMod();
  Short_t seg   = fClst->getIOSeg();
  Short_t nMods = 0;
  if(fGetCont->isModActive(sec,seg*2))   nMods++;
  if(fGetCont->isModActive(sec,seg*2+1)) nMods++;
  clusIndex = fClst->getOwnIndex();
  locHit[0] = sec;
  Int_t typeClFn=fClst->getTypeClFinder();
  // (modCl&1)^1 = mod 0,2 => 1; mod 1,3 => 0    For modCl<0 nlays=6
  Int_t nlays=(typeClFn==2 && modCl>=0) ? fClst->getNLayersMod((modCl&1)^1):6;
  fSeg = 0;
  hitSegFiller.setClusParam(fClst,HMdcTrackDSet::isCoilOff());
  if(fSegCat && tFillSeg[seg]!=-1 && (typeClFn==0 || nMods==1 || 
      (typeClFn==1 && modCl==tFillSeg[seg]) || (typeClFn==2 && nlays>2))) {
    locSeg[0] = sec;
    locSeg[1] = seg;  
    Int_t indexSeg;
    fSeg = (HMdcSeg*)fSegCat->getNewSlot(locSeg,&indexSeg);
    if(!fSeg) {
      Warning("fillContainers","S.%i No HMdcSeg slot available",sec+1);
      slotNotAv = kTRUE;
      return;
    }
    fSeg = (isGeant) ? (HMdcSeg*)(new(fSeg) HMdcSegSim) : new(fSeg) HMdcSeg;
    fSeg->setSec(sec);
    fSeg->setIOSeg(seg);
    fSeg->setChi2(-1.);
    fSeg->setClusIndex(clusIndex);
    fillSeg();
    if(fTrkCandCat) {
      if(fTrkCand==0) {
        if(seg==0) fillTrkCandISeg(sec,indexSeg);
        else fillTrkCandOSeg(sec,indexSeg);
      } else fillTrkCandOSeg(indexSeg);
    }
    if( slotNotAv ) return;
  }
  for(Int_t imod=0; imod<2; imod++) {
    Int_t mod=imod+seg*2;
    if(modCl>=0 && modCl!=mod) continue;
    if(!fGetCont->isModActive(sec,mod)) continue;

    locHit[1]=mod;
    Int_t indexHit;
    fHit = (HMdcHit*)fHitCat->getNewSlot(locHit,&indexHit);
    if(!fHit) {
      Warning("fillContainers","S.%i No HMdcHit slot available",sec+1);
      slotNotAv = kTRUE;
      return; //continue;
    }
    if(isGeant) fHit=(HMdcHit*) new(fHit) HMdcHitSim;
    else fHit= new(fHit) HMdcHit;
    fHit->setSecMod(sec,mod);
    if(!fillHit(mod)) return;

    Int_t lc = imod*6;
    for(Int_t l=0; l<6; l++,lc++) {
      UShort_t list = fClst->getOneLayerGroup(lc);
      fHit->setLayerGroup(l,list);
      if(fSeg) fSeg->setLayerGroup(lc,list);
    }

    if(fSeg) fSeg->setHitInd(imod,indexHit);
    if(isGeant) ((HMdcHitSim*)fHit)->calcNTracks();
  }
  if(fSeg) {
    Int_t nMrCls=fClst->getNMergClust();
    if(nMrCls>99) nMrCls=99;
    Int_t flag=(fClst->getNBins()&32767) <<16;
    flag += (fClst->getNDrTimes()*100 + nMrCls) & 65535;
    fSeg->setFlag(flag);
    fHit->setChi2(-1.);
    if(isGeant) ((HMdcSegSim*)fSeg)->calcNTracks();
  }
}

void HMdcClusterToHit::fillTrkCandISeg(Short_t sec,Int_t indexSeg) {
  Int_t index;
  fTrkCand = (HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
  if(fTrkCand) fTrkCand = new(fTrkCand) HMdcTrkCand(sec,indexSeg,index);
  else {
    Warning("fillTrkCandISeg"," No slot available in catMdcTrkCand");
    slotNotAv = kTRUE;
  }
}

void HMdcClusterToHit::fillTrkCandOSeg(Short_t sec,Int_t indexSeg) {
  Int_t index;
  fTrkCand = (HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
  if(fTrkCand) {
    fTrkCand = new(fTrkCand) HMdcTrkCand(sec,indexSeg,index);
    fTrkCand->setSeg2Ind(index);
    fTrkCand->setSeg1Ind(-1);
  } else {
    Warning("fillTrkCandOSeg"," No slot available in catMdcTrkCand");
    slotNotAv = kTRUE;
  }
}

void HMdcClusterToHit::fillTrkCandOSeg(Int_t indexSeg) {
//   if(fTrkCand->getSeg2Ind() <0) {
//     fTrkCand->addSeg2Ind(indexSeg);
//   } else {
    Int_t index;
    HMdcTrkCand* newTr=(HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
    if(newTr) fTrkCand=new(newTr) HMdcTrkCand(fTrkCand,indexSeg,index);
    else {
      Warning("fillTrkCandOSeg"," No slot available in catMdcTrkCand");
      slotNotAv = kTRUE;
    }
//   }
}

void HMdcClusterToHit::fillSeg(void) {
  HMdcSizesCellsSec& fSCSec=(*fSizesCells)[fSeg->getSec()];
  const HGeomVector* targ=(&fSCSec) ? &(fSCSec.getTargetMiddlePoint()) : 0;
  hitSegFiller.fillMdcSegByClus(fSeg,targ);
}

Bool_t HMdcClusterToHit::fillHit(Short_t mod) {
  HMdcSizesCellsMod& fSCMod=(*fSizesCells)[fClst->getSec()][mod];
  if(!&fSCMod) return kFALSE;
  HMdcClusInf* fClusInf = fillClusInf(mod&1);
  hitSegFiller.fillMdcHitByClus(&fSCMod,fHit);
  if(fClusInf) {
    fClusInf->setXYHit(fHit->getX(),fHit->getY());
    Double_t xm,ym,zm;
    fSCMod.calcIntersection(
        fClst->getXTarg(),fClst->getYTarg(),fClst->getZTarg(),
        fClst->getX(),    fClst->getY(),    fClst->getZ(),    xm,ym,zm);
    fSCMod.transTo(xm,ym,zm);
    fClusInf->setXYClus(xm,ym);
  }
  return kTRUE;
}

HMdcClusInf* HMdcClusterToHit::fillClusInf(Int_t iMod) {
  Int_t index;
  HMdcClusInf* fClusInf=(HMdcClusInf*)fClusInfCat->getNewSlot(loc,&index);
  if(!fClusInf) {
    Warning("fillFitCat","No slot HMdcClusInf available");
    slotNotAv = kTRUE;
    return 0;
  }
  fClusInf = (isGeant) ? (HMdcClusInf*)(new(fClusInf) HMdcClusInfSim) : 
                                        new(fClusInf) HMdcClusInf;
  fHit->setTrackFinder(1);
  fHit->setClusInfIndex(index);
  fHit->setChi2(-1.);
  fClusInf->setIsNotFitted();
  Int_t nmods=fClst->getMod();
  fClusInf->setNModInCl( (nmods<0) ? -nmods:1 );
  fClusInf->setClusIndex(clusIndex);
  fClusInf->setNModInFit(0);
  if(iMod==0) {
    fClusInf->setLevelClFinding(fClst->getClFnLevelM1());
    fClusInf->setClusSize(fClst->getClusSizeM1());
    fClusInf->setNMergClust(fClst->getNMergClustM1());
    fClusInf->setNDrTimes(fClst->getNDrTimesM1());
    fClusInf->setSigma1(fClst->getSigma1M1());
    fClusInf->setSigma2(fClst->getSigma2M1());
    fClusInf->setAlpha(fClst->getAlphaM1());
  } else {
    fClusInf->setLevelClFinding(fClst->getClFnLevelM2());
    fClusInf->setClusSize(fClst->getClusSizeM2());
    fClusInf->setNMergClust(fClst->getNMergClustM2());
    fClusInf->setNDrTimes(fClst->getNDrTimesM2());
    fClusInf->setSigma1(fClst->getSigma1M2());
    fClusInf->setSigma2(fClst->getSigma2M2());
    fClusInf->setAlpha(fClst->getAlphaM2());
    
  }
  if(isGeant) {
    HMdcClusSim* fClSim=(HMdcClusSim*)fClst;
    ((HMdcClusInfSim*)fClusInf)->setTracksList(fClSim->getNTracksMod(iMod),
        fClSim->getListTrM(iMod),fClSim->getNTimesM(iMod));
  }
  return fClusInf;
}

void HMdcClusterToHit::printStatus(void) {
  // prints the parameters to the screen
  const Char_t* hFSeg1a ="-1 = don't fill HMdcSeg for segment 1,";
  const Char_t* hFSeg1b =" 0 = fill HMdcSeg by cluster in MDC1 if typeClFinder=1";
  const Char_t* hFSeg1c ="     and sector has mdc1&2, else by cluster in existing MDC,";
  const Char_t* hFSeg1d =" 1 =  -/- in mdc2";
  const Char_t* hFSeg2a ="-1 = don't fill HMdcSeg for segment 2,";
  const Char_t* hFSeg2b =" 2 = fill HMdcSeg by cluster in MDC3 if typeClFinder=1";
  const Char_t* hFSeg2c ="     and sector has mdc3&4 and MagnetOff,";
  const Char_t* hFSeg2d ="     else by cluster in existing MDC,";
  const Char_t* hFSeg2e =" 3 =  -/- in MDC4";
  printf("--------------------------------------------------------------------------------------------\n");
  printf("HMdcClusterToHitSetup:\n");
  printf("ModForSeg1      = %2i :  %s\n",tFillSeg[0],hFSeg1a);
  printf("                        %s\n",hFSeg1b);
  printf("                        %s\n",hFSeg1c);
  printf("                        %s\n",hFSeg1d);
  printf("ModForSeg2      = %2i :  %s\n",tFillSeg[1],hFSeg2a);
  printf("                        %s\n",hFSeg2b);
  printf("                        %s\n",hFSeg2c);
  printf("                        %s\n",hFSeg2d);
  printf("                        %s\n",hFSeg2e);
  printf ("--------------------------------------------------------------------------------------------\n");
  prntSt=kTRUE;
}

ClassImp(HMdcClusterToHit)
