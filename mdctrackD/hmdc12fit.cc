//*--- Author : V.Pechenov
//*--- Modified: 26.06.07 V.Pechenov
//*--- $Id: hmdc12fit.cc,v 1.68 2009-10-16 09:07:50 pechenov Exp $
//*--- Modified: 23.01.07 V.Pechenov
//*--- Modified: 17.06.05 V.Pechenov
//*--- Modified: 29.10.03 V.Pechenov
//*--- Modified: 21.07.03 V.Pechenov
//*--- Modified: 04.04.03 V.Pechenov
//*--- Modified: 17.07.02 V.Pechenov
//*--- Modified: 16.08.01 V.Pechenov
//*--- Modified: 17.10.00 A.Nekhaev

using namespace std;
#include "hmdc12fit.h"
#include "hdebug.h"
#include "hevent.h"
#include "hiterator.h"
#include "hspectrometer.h"
#include "hmatrixcategory.h"
#include <iostream>
#include <iomanip>
#include "hmdcsizescells.h"
#include "hgeomvector.h"
#include "hmdcsegsim.h"
#include "hmdchitsim.h"
#include "hmdcgetcontainers.h"
#include "hmdcclussim.h"
#include "hmdcdetector.h"
#include "hmdctrackfittera.h"
#include "hmdctrackfitterb.h"
#include "hmdctrackdset.h"
#include "hmdcclusinfsim.h"
#include "hruntimedb.h"
#include "hmdc34clfinder.h"
#include "hmdctrkcand.h"
#include "hmdcgeanttrack.h"
#include "hmdcclusfitsim.h"
#include "hmdclistcells.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// 
// HMdcTrackFittesrArray
//
// Array of HMdcTrackFitterA (B,...) objects
//
//
// HMdc12Fit
//
// Manager class for Dubna track straight piece fitter.
//
//////////////////////////////////////////////////////////////////////////////


ClassImp(HMdc12Fit)
ClassImp(HMdcFittersArray)

HMdcFittersArray::HMdcFittersArray(void) {
  nObj      = 0;
  fitAuthor = 1;
  fitpar    = 0;
  size      = 100;
  fitterArr = new HMdcTrackFitter* [size];
  for(Int_t i=0;i<size;i++) fitterArr[i] = 0;
}

HMdcFittersArray::~HMdcFittersArray(void) {
  if(fitterArr) {
    for(Int_t i=0;i<size;i++) if(fitterArr[i]!=0) delete fitterArr[i];
    delete [] fitterArr;
    fitterArr = NULL;
  }
}

void HMdcFittersArray::init(Int_t fitAut, HMdcTrackFitInOut* fpar) {
  fitpar    = fpar;
  fitAuthor = fitAut;
}

void HMdcFittersArray::expand(void) {
  Int_t newSize=size+50;
  HMdcTrackFitter** fitterArrOld=fitterArr;
  fitterArr=new HMdcTrackFitter* [newSize];
  for(Int_t i=0;i<size;i++) fitterArr[i]=fitterArrOld[i];
  for(Int_t i=size;i<newSize;i++) fitterArr[i]=0;
  size=newSize;
  delete [] fitterArrOld;
}

HMdcTrackFitter* HMdcFittersArray::getNewFitter(void) {
  if(nObj == size) expand();
  HMdcTrackFitter* fitter = fitterArr[nObj];
  if(fitter == NULL) {
    if(fitAuthor == 2) fitter = (HMdcTrackFitter*) new HMdcTrackFitterB(fitpar);
    else               fitter = (HMdcTrackFitter*) new HMdcTrackFitterA(fitpar);
    fitterArr[nObj]=fitter;
  }
  fitter->getInitParam()->setOneSecData();
  fitter->getFinalParam()->setOneSecData();
  nObj++;
  return fitter;
}

//--------------------------------------------------------------------

HMdc12Fit::HMdc12Fit(Int_t type,Int_t ver,Int_t dp,Bool_t hst,Bool_t prnt) {
  HMdcTrackDSet::setMdc12Fit(type,ver,dp,hst,prnt);
  setParContainers();
}

HMdc12Fit::HMdc12Fit(const Text_t *name,const Text_t *title,Int_t type,
    Int_t ver,Int_t dp,Bool_t hst, Bool_t prnt) : HReconstructor(name,title) {
  HMdcTrackDSet::setMdc12Fit(type,ver,dp,hst,prnt);
  setParContainers();
}

HMdc12Fit::HMdc12Fit() {
  setParContainers();
}

HMdc12Fit::HMdc12Fit(const Text_t *name,const Text_t *title)
    : HReconstructor(name,title) {
  setParContainers();
}

HMdc12Fit::~HMdc12Fit() {
  HMdcSizesCells::deleteCont();
  HMdcTrackDSet::clearP12Fit();
  if(iterClus){delete iterClus;iterClus=0;}
}

void HMdc12Fit::setParContainers(void) {
  HMdcTrackDSet::set12Fit(this);
  prntSt             = kFALSE;
  fSegCat            = NULL; 
  iterClus           = NULL;
  fHitCat            = NULL;
  fClusFitCat        = NULL;
  modForSeg[0]       = 1;
  modForSeg[1]       = 3;
  fitpar.setWeightCut(0.01);
  fitpar.setSignalSpeed(0.0);
  notFillByClus      = kFALSE;
  isCosmicData       = kFALSE;
  fillHitSeg         = kTRUE;
  pTrackInfSim       = NULL;
  useFitted          = kFALSE;
  useFittedTrPar     = kFALSE;
  findOffVertTrk     = kFALSE;
  nLayersCutOVT      = 0;
  nWiresCutOVT       = 0;

  // Fake suppression parameters:
  setFakeSupprStep1Par(0, 4,3,4, 8);
  setFakeSupprStep1Par(1, 4,3,5,10);

  Float_t cutRealS0[8] = { 0.0, 0.1, 0.5 , 0.7, 1.0, 1.1, 1.3, 1.6};  //Seg.0
  Float_t cutFakeS0[8] = { 0.1, 0.1, 0.5 , 0.8, 1.1, 1.2, 1.5, 1.7};  //Seg.0
  setFakeSupprStep2Par(0, cutRealS0,cutFakeS0);

  Float_t cutRealS1[8] = { 0.0, 0.1, 0.5 , 0.8, 1.1, 1.2, 1.3, 1.5};  //Seg.1
  Float_t cutFakeS1[8] = { 0.1, 0.1, 0.65, 1.1, 1.2, 1.4, 1.5, 1.6};  //Seg.1
  setFakeSupprStep2Par(1, cutRealS1,cutFakeS1);
}

void HMdc12Fit::setFakeSupprStep1Par(UInt_t seg,Int_t n1,Int_t n2,Int_t n3,Int_t n4) {
  if(seg>1) return;
  nSharedLayersCut[seg] = n1;
  nLayerOrientCut[seg]  = n2;
  nDCellsMinCut[seg]    = n3;
  nDCellsCut[seg]       = n4;
}

void HMdc12Fit::setFakeSupprStep2Par(UInt_t seg,Float_t *cutReal,Float_t *cutFake) {
  // cutFake[i] must be >= cutReal[i]
  if(seg>1) return;
  for(Int_t i=0;i<8;i++) {
    cutForReal[seg][i] = cutReal[i];
    cutForFake[seg][i] = cutFake[i];
  }
}

Bool_t HMdc12Fit::init(void) {
  HMdcGetContainers* fGetCont = HMdcGetContainers::getObject();
  fClusCat = fGetCont->getCatMdcClus();
  if( !fClusCat ) return kFALSE;
  iterClus=(HIterator *)fClusCat->MakeIterator();
  
  if(!fitpar.init()) return kFALSE;
  isGeant = fitpar.isGeant();
  HMdcDetector* fMdcDet = fGetCont->getMdcDetector();
  modForSeg[0] = HMdcTrackDSet::getModForSeg1();
  modForSeg[1] = HMdcTrackDSet::getModForSeg2();
  if(fillHitSeg) {
    if(modForSeg[0]>=0 && modForSeg[1]>0) {
      fSegCat = gHades->getCurrentEvent()->getCategory(catMdcSeg);
      if(!fSegCat) {
        if(isGeant) fSegCat = fMdcDet->buildMatrixCategory("HMdcSegSim",0.5F);
        else        fSegCat = fMdcDet->buildCategory(catMdcSeg);
        if (!fSegCat) return kFALSE;
        gHades->getCurrentEvent()->addCategory(catMdcSeg,fSegCat,"Mdc");
      }
    }

    fHitCat = gHades->getCurrentEvent()->getCategory(catMdcHit);
    if (!fHitCat) {
      if(isGeant) fHitCat = fMdcDet->buildMatrixCategory("HMdcHitSim",0.5F);
      else        fHitCat = fMdcDet->buildCategory(catMdcHit);
      if (!fHitCat) return kFALSE;
      gHades->getCurrentEvent()->addCategory(catMdcHit,fHitCat,"Mdc");
    }
  }
  if(isCosmicData) {
    fTrkCandCat = 0;
    fClusInfCat = 0;
  } else {
    fTrkCandCat = fGetCont->getCatMdcTrkCand(kTRUE);
    if(!fTrkCandCat) return kFALSE;
    fClusInfCat = fGetCont->getCatMdcClusInf(kTRUE);
    if(!fClusInfCat) return kFALSE;
    findOffVertTrk = HMdcTrackDSet::getFindOffVertTrkFlag();
  }


  locClInf.set(1,0);
  locSeg.set(2,0,0);
  locHit.set(2,0,0);
  locTrkCand.set(1,0);
  locClus.set(2,0,0);

  if(HMdcTrackDSet::getFitAuthor()==0) {
    Warning("init",
        "Hejdar's tack fitter not suported more, Alexander's tack fitter will used!");
    HMdcTrackDSet::setFitAuthor(1);
  }
      
  fittersArr[0].init(HMdcTrackDSet::getFitAuthor(),&fitpar);
  fittersArr[1].init(HMdcTrackDSet::getFitAuthor(),&fitpar);
  f34ClFinder  = HMdc34ClFinder::getExObject();
  if(isGeant) pTrackInfSim = HMdcGeantEvent::getMdcTrackInfSim();
  
  findOffVertTrk = HMdcTrackDSet::getFindOffVertTrkFlag();
  nLayersCutOVT  = HMdcTrackDSet::getNLayersCutOVT();
  nWiresCutOVT   = HMdcTrackDSet::getNWiresCutOVT();
  
  return kTRUE;
}

Bool_t HMdc12Fit::reinit(void) {
  if(!fitpar.reinit()) return kFALSE;
  if(isCosmicData) {
    clusFitAlg = 4;
    fitpar.setTargetScanFlag(kFALSE);
  } else {
    clusFitAlg = 1;
    if(HMdcTrackDSet::isCoilOff()) {
      if(HMdcTrackDSet::getClFinderType()==1) clusFitAlg = 2;
      else clusFitAlg = 3;
    } else useFittedTrPar = HMdcTrackDSet::useFittedSeg1ParFlag();
  }
  if(!prntSt) printStatusM();
  if(f34ClFinder != NULL) {
    printf("--------------------------------------------------------------------------------------------\n");
    printf("Outer cluster finder (HMdc34ClFinder):\n");
    printf("  Use track parameter of inner %s.\n",useFittedTrPar ? "segment (HMdcTrackParam)" :
                                                                   "cluster (HMdcClus)");
    if(useFitted) printf("  Skip not fitted inner segments.\n");
    f34ClFinder->printClFinderParam();
    printf("--------------------------------------------------------------------------------------------\n");
  }
  return kTRUE;
}

Int_t HMdc12Fit::execute(void) {
  if(HMdcTrackDSet::fPrint()) {
    fitpar.setPrintFlag(gHades->getEventCounter());
    if(fitpar.getPrintFlag()) printf(
      "============================== Event %i =============================\n",
      gHades->getEventCounter());
  }
  iterClus->Reset();
  for(Int_t s=0;s<6;s++) {
    indFirstSeg[s]  = -1;
    indLastSeg[s]   = -2;
    indFstTrCand[s] = -1;
    indLstTrCand[s] = -2;
  }
  if     (clusFitAlg == 1) fitAlgorithm1();   // magnet on                       
  else if(clusFitAlg == 2) fitAlgorithm2();   // magnet off + chanmer clusters   
  else if(clusFitAlg == 3) fitAlgorithm3b();  // magnet off + sector clusters    
  else if(clusFitAlg == 4) fitAlgorithm4();   // cosmic data
  return 0;
}

Bool_t HMdc12Fit::testRestOfWires(Int_t sec) {
  HMdcEvntListCells *pEvListCells = HMdcEvntListCells::getExObject();
  locClus[0]    = sec;
  locTrkCand[0] = sec;
  Int_t nMdcsSeg2 = fitpar.getMdcSizesCells()->nModInSeg(sec,1);
  pSecListCells = &((*pEvListCells)[sec]);
  Int_t nNFCellsM1,nNFCellsM2;
  Int_t nLay1 = (*pSecListCells)[0].genNumNotFittedLayers(nNFCellsM1);
  Int_t nLay2 = (*pSecListCells)[1].genNumNotFittedLayers(nNFCellsM2);
  if(nLay1 + nLay2 < nLayersCutOVT) return kFALSE;  // 7 ???
  //  if(nNFCellsM1 > nWiresCutOVT || nNFCellsM2 > nWiresCutOVT) return kFALSE;
  if(nNFCellsM1*nNFCellsM2 > nWiresCutOVT*nWiresCutOVT) return kFALSE;
  fitter = fittersArr[0].getFitter(0);
  if(fitter == NULL) fitter = fittersArr[0].getNewFitter();

  HMdcLineParam lineParam;
  HMdcList12GroupCells listCells;
  Int_t nlays = fitter->getWiresArr().testRestForTrack(sec,lineParam,listCells, 
                                                       nLay1+nLay2,nNFCellsM1+nNFCellsM2);
                         
  if(nlays < nLayersCutOVT) return kFALSE;
  locClus[0] = sec;
  locClus[1] = 0;    // Inner segment
  Int_t index;
  HMdcClus *pClusSeg1 = (HMdcClus*)fClusCat->getNewSlot(locClus,&index);
  if(!pClusSeg1) {
    Warning("execute","Sec.%i Segment %i No slot HMdcClus available",sec+1,1);
    return kFALSE;
  }
  if(isGeant) pClusSeg1 = (HMdcClus*)(new(pClusSeg1) HMdcClusSim(listCells));
  else        pClusSeg1 =             new(pClusSeg1) HMdcClus(listCells);
  pClusSeg1->setTarget(lineParam.x1(),0.,lineParam.y1(),0.,lineParam.z1(),0.);
  HMdcPlane *pl = lineParam.getSecondPlane();
  pClusSeg1->setPrPlane(pl->A(),pl->B(),pl->D());
  pClusSeg1->setXY(lineParam.x2(),0.,lineParam.y2(),0.);
  pClusSeg1->setSecSegInd(sec,0,index);  // ,0, - Inner segment
  pClusSeg1->setMod(-1);
  pClusSeg1->setSumWt(0.);
  pClusSeg1->setRealLevel(nlays);
  pClusSeg1->setNBins(0);
  pClusSeg1->setTypeClFinder(256 + 0);  // Ampl.cut + comb.cl.finder
  pClusSeg1->setMinCl(nLayersCutOVT-nLayersCutOVT/2,nLayersCutOVT/2);
                                                                                                                                   
  if(pTrackInfSim != NULL) pTrackInfSim->fillClusTrackInf((HMdcClusSim*)pClusSeg1);

  fittersArr[0].reset();
  Bool_t flag = fitSeg(pClusSeg1);

  if(!flag) return kFALSE;
  // HMdcTrkCand filling:
  if(fitter->getSegIndex()<0) return kFALSE;
  HMdcTrkCand* fTrkCand = fillTrkCandISeg();
  if(fTrkCand == NULL) return kFALSE;
  //------------------------------------- Cluster finder in segment 2:
  if( !isFitted ) return kFALSE;
  
  if(nMdcsSeg2 <= 0) return kTRUE;
  if(findSeg2Clusters(fitter) <= 0) return kTRUE;

  Int_t firstCh,lastCh;
  pClusSeg1->getIndexRegChilds(firstCh,lastCh);
  fittersArr[1].reset();
  for(Int_t ind=firstCh;ind<=lastCh;ind++) {
    HMdcClus* fClstSeg2 = (HMdcClus*)fClusCat->getObject(ind);
    if(fClstSeg2 == NULL) return kTRUE;
    flag = nMdcsSeg2==1 ? fitMod(fClstSeg2,1) : fitSeg(fClstSeg2,1);
    if(!fillHitSeg || !flag || fitter->getSegIndex()<0) return kTRUE;
    // HMdcTrkCand filling:
    fTrkCand = fillTrkCandOSeg(fTrkCand);
    if(fTrkCand == NULL) break;
  }
  return kTRUE;
}

void HMdc12Fit::sortGeantInf(void) {
  // Sorting geant track information in clusters and segments:
  iterClus->Reset();
  HMdcClusSim* pClusSim;
  while ((pClusSim=(HMdcClusSim*)iterClus->Next())!=0) pClusSim->sortTrListByContr();
  
  Int_t nsegs = fSegCat->getEntries();
  for(Int_t i=0; i<nsegs; i++) ((HMdcSegSim*)fSegCat->getObject(i))->sortTrListByContr();
  
  if(HMdcTrackDSet::fNTuple()) {
    if(fClusFitCat == NULL) fClusFitCat = HMdcGetContainers::getObject()->getCatMdcClusFit();
    if(fClusFitCat != NULL) {
      Int_t nent = fClusFitCat->getEntries();
      for(Int_t n=0;n<nent;n++) {
        HMdcClusFitSim *pClusFitSim = (HMdcClusFitSim*)(fClusFitCat->getObject(n));
        if(pClusFitSim == NULL) continue;
        Int_t clInd = pClusFitSim->getClustCatIndex();
        HMdcClusSim* pClusSim = (HMdcClusSim*)(fClusCat->getObject(clInd));
        if(pClusSim == NULL) continue;
        Int_t segInd = pClusSim->getSegIndex();
        if(segInd<0) continue;
        HMdcSegSim* pSegSim = (HMdcSegSim*)(fSegCat->getObject(segInd));
        if(pSegSim == NULL) continue;
        Int_t track = pClusFitSim->getGeantTrackNum();
        if(track <= 0) continue;
        Int_t ind = pSegSim->findTrack(track);
        if(pSegSim->isFakeContribution(ind) ) pClusFitSim->setFakeFlag();
      }
    } 
  }
}

void HMdc12Fit::fitAlgorithm1(void) {
  Int_t maxNumSectors = ((HMatrixCategory*)fClusCat)->getSize(0);
  locClus[1]          = 0;
  Int_t fitType       = HMdcTrackDSet::getFitType();
  for(Int_t sec=0; sec<maxNumSectors; sec++) {
     //      iterClus->Reset();
    locClus[0]    = sec;
    locTrkCand[0] = sec;
    iterClus->gotoLocation(locClus);
    Int_t nMdcsSeg1 = fitpar.getMdcSizesCells()->nModInSeg(sec,0);
    Int_t nMdcsSeg2 = fitpar.getMdcSizesCells()->nModInSeg(sec,1);
    HMdcClus * fClstSeg1;
    while((fClstSeg1=(HMdcClus*)iterClus->Next())) {
      // ------------------------------------ Segment 1:
      Bool_t flag = kFALSE;
      Int_t typeClFnSeg1=fClstSeg1->getTypeClFinder();
      fittersArr[0].reset();
      if(nMdcsSeg1==1)           flag = fitMod(fClstSeg1); // One mdc per seg.
      else if(typeClFnSeg1==0) {                           // Two mdc per seg.
        if(fitType==0)           flag = fitSeg(fClstSeg1);
        else if(fitType==1)      flag = fitCombClusIndFit(fClstSeg1);
      } else if(typeClFnSeg1==1) flag = fitChambClus(fClstSeg1);
      else if(typeClFnSeg1==2)   flag = fitMixedClus(fClstSeg1);
      if(!flag) continue;
      // HMdcTrkCand filling:
      fitter = fittersArr[0].getFitter(0); // ???
      if(fitter->getSegIndex()<0) continue;
      HMdcTrkCand* fTrkCand = fillTrkCandISeg();
      if(fTrkCand == NULL) continue;
      //------------------------------------- Cluster finder in segment 2:
      if(useFitted && !isFitted) continue;
      if(nMdcsSeg2 <= 0) continue;
      if(findSeg2Clusters(fitter) <= 0) continue;

      Int_t firstCh,lastCh;
      fClstSeg1->getIndexRegChilds(firstCh,lastCh);
      fittersArr[1].reset();
      for(Int_t ind=firstCh;ind<=lastCh;ind++) {
        HMdcClus* fClstSeg2 = (HMdcClus*)fClusCat->getObject(ind);
        if(fClstSeg2==0) {
          Error("fitAlgorithm1","Absent cluster with ind=%i!",ind);
          continue;
        }
        Int_t typeClFnSeg2 = fClstSeg2->getTypeClFinder();
        if(nMdcsSeg2==1)           flag = fitMod(fClstSeg2,1); // One mdc per seg.
        else if(typeClFnSeg2==0) {                             // Two mdc per seg.
          if(fitType==0)           flag = fitSeg(fClstSeg2,1);
          else if(fitType==1)      flag = fitCombClusIndFit(fClstSeg2,1);
        } else if(typeClFnSeg2==1) flag = fitChambClus(fClstSeg2,1);
        else if(typeClFnSeg2==2)   flag = fitMixedClus(fClstSeg2,1);
        if(!fillHitSeg) continue;
        if(!flag || fitter->getSegIndex()<0) continue;
        // HMdcTrkCand filling:
        fTrkCand = fillTrkCandOSeg(fTrkCand);
        if(!fTrkCand) break;
      }
    }
    
    if(isGeant) sortGeantInf(); // Must be before suppressFakes()
    
    suppressFakes(sec,0);
    if(findOffVertTrk) while (testRestOfWires(sec));
    suppressFakes(sec,1); 
    
    // Remove identical track candidates:
    indFirstTrCand = indFstTrCand[sec];
    indLastTrCand  = indLstTrCand[sec];
    if(indFirstTrCand < 0 || indFirstTrCand >= indLastTrCand) continue;
    for(Int_t itr1=indFirstTrCand; itr1<indLastTrCand; itr1++) {
      HMdcTrkCand* trkCand1 = (HMdcTrkCand*)fTrkCandCat->getObject(itr1);
      Int_t ind1 = trkCand1->getSeg1Ind();
      Int_t ind2 = trkCand1->getSeg2Ind();
      if(ind1 < 0) continue;
      for(Int_t itr2=itr1+1; itr2<=indLastTrCand; itr2++) {
        HMdcTrkCand* trkCand2 = (HMdcTrkCand*)fTrkCandCat->getObject(itr2);
        if(ind1 != trkCand2->getSeg1Ind() || ind2 != trkCand2->getSeg2Ind()) continue;
        if(ind2 >= 0) {
          trkCand1->setSeg2Ind(-1);
          trkCand1->setFakeFlagSeg2();
        } 
        trkCand1->setFakeFlagSeg1();
        break;
      }
    }
  }  
  if(isGeant && fClusFitCat != NULL) {  // Sinchronize GEANT info (HMdcSeg => HMdcClusFit) 
    Int_t nent = fClusFitCat->getEntries();
    for(Int_t n=0;n<nent;n++) {
      HMdcClusFitSim *pClusFitSim = (HMdcClusFitSim*)(fClusFitCat->getObject(n));
      HMdcSegSim     *pSegSim     = (HMdcSegSim*)fSegCat->getObject(n);
      if(pSegSim == NULL) continue;
      Int_t track = pClusFitSim->getGeantTrackNum();
      if(track <= 0) continue;
      Int_t ind = pSegSim->findTrack(track);
      if( pSegSim->isFakeContribution(ind) ) pClusFitSim->setFakeFlag();
    }
  }
}

Int_t HMdc12Fit::findSeg2Clusters(HMdcTrackFitter* fitter) {
  // Cluster finder in segment 2.
  // flag=kTRUE - use seg1 cluster; =kFALSE - use fitted cluster (if fit ok.)
  if(f34ClFinder == NULL) return 0;
  HMdcWiresArr& wiresArr =  fitter->getWiresArr();
  HMdc34ClFinderSec& f34CFSc=(*f34ClFinder)[wiresArr.getSector()];
  if(!&f34CFSc || f34CFSc.notEnoughWires()) return 0;

  if(isFitted && useFittedTrPar) {
    HMdcSeg* fSeg = (HMdcSeg*)fSegCat->getObject(fitter->getSegIndex());
    if(fSeg==0) return 0;
//    return      f34CFSc.findClustersSeg2(fSeg,wiresArr.getClust1());
    return      f34CFSc.findClustersSeg2(fitter->getFinalParam(),wiresArr.getClust1());
  } else return f34CFSc.findClustersSeg2(wiresArr.getClust1());
}

void HMdc12Fit::fitAlgorithm2(void) {
  // Magnet off + chamber clusters
  Int_t maxNumSectors  = ((HMatrixCategory*)fClusCat)->getSize(0);
  Int_t maxNumSegments = ((HMatrixCategory*)fClusCat)->getSize(1);
  for(locClus[0]=0; locClus[0]<maxNumSectors; locClus[0]++){
    for(locClus[1]=0; locClus[1]<maxNumSegments; locClus[1]++) {
       //      iterClus->Reset();
      iterClus->gotoLocation(locClus);
      Int_t nMdcs = fitpar.getMdcSizesCells()->nModInSeg(locClus[0],locClus[1]);
      HMdcClus * fClst;
      while((fClst=(HMdcClus*)iterClus->Next())) {
        fittersArr[0].reset();
        if(nMdcs==1) fitMod(fClst);             // One mdc per segment
        else         fitChambClus(fClst);       // Two mdc per segment
      }
    }
  }
  if(isGeant) sortGeantInf();
}

void HMdc12Fit::fitAlgorithm3(void) {
  // Segment fit
  // Magnet off + combined clusters in sector
  Int_t maxNumSectors = ((HMatrixCategory*)fClusCat)->getSize(0);
  locClus[1]          = 0;
  Int_t fitType       = HMdcTrackDSet::getFitType();
  for(Int_t sec=0; sec<maxNumSectors; sec++){
     //      iterClus->Reset();
    locClus[0]=sec;
    locTrkCand[0]=sec;
    iterClus->gotoLocation(locClus);
    Int_t nMdcsSeg1=fitpar.getMdcSizesCells()->nModInSeg(sec,0);
    Int_t nMdcsSeg2=fitpar.getMdcSizesCells()->nModInSeg(sec,1);
    HMdcClus * fClstSeg1;
    while((fClstSeg1=(HMdcClus*)iterClus->Next())) {
      // Segment 1:
      Bool_t flag = kFALSE;
      Int_t typeClFnSeg1=fClstSeg1->getTypeClFinder();
      fittersArr[0].reset();
      if(nMdcsSeg1==1)           flag = fitMod(fClstSeg1); // One mdc per segment
      else if(typeClFnSeg1==0) {                           // Two mdc per segment
        if(fitType==0)           flag = fitSeg(fClstSeg1);
        else if(fitType==1)      flag = fitCombClusIndFit(fClstSeg1);
      } else if(typeClFnSeg1==2) flag = fitMixedClus(fClstSeg1);
      
      // Segment 2:
      if(!flag) continue;
      
      // HMdcTrkCand filling:
      fitter = fittersArr[0].getFitter(0); // ???
      if(fitter->getSegIndex()<0) continue;
      HMdcTrkCand* fTrkCand = fillTrkCandISeg();
      if(fTrkCand==0) continue;
      
      Int_t firstCh,lastCh;
      fClstSeg1->getIndexRegChilds(firstCh,lastCh);
      fittersArr[1].reset();
      for(Int_t ind=firstCh;ind<=lastCh;ind++) {
        HMdcClus* fClstSeg2 = (HMdcClus*)fClusCat->getObject(ind);
        if(fClstSeg2==0) {
          Error("fitAlgorithm3","Absent cluster with ind=%i!",ind);
          continue;
        }
        flag=kFALSE;
        Int_t typeClFnSeg2 = fClstSeg2->getTypeClFinder();
        if(nMdcsSeg2==1)      flag = fitMod(fClstSeg2,1); // One mdc per segment
        else if(typeClFnSeg2==0) {                        // Two mdc per segment
          if(fitType==0)      flag = fitSeg(fClstSeg2,1);
          else if(fitType==1) flag = fitCombClusIndFit(fClstSeg2,1);
        }
        if(!fillHitSeg) continue;
        if(!flag || fitter->getSegIndex()<0) continue;
        // HMdcTrkCand filling:
        fTrkCand = fillTrkCandOSeg(fTrkCand);
        if(!fTrkCand) break;
      }
    }
  }
  if(isGeant) sortGeantInf();
}


void HMdc12Fit::fitAlgorithm3b(void) {
  // Sector fit
  // Magnet off + combined clusters in sector
  Int_t fitType = HMdcTrackDSet::getFitType();
  HMdcClus* fClst1;
  while((fClst1=(HMdcClus*)iterClus->Next())) {
    if(fClst1->getIndexParent() >= 0) continue;
    fittersArr[0].reset();
    Int_t first,last;
    fClst1->getIndexRegChilds(first,last);
    HMdcClus* fClst2 = (first<0) ? 0:(HMdcClus*)fClusCat->getObject(first);
    Int_t sec=fClst1->getSec();
    Int_t seg=fClst1->getIOSeg();
    Bool_t flag=kFALSE;
    if(fClst2==0) {
      Int_t typeClFn=fClst1->getTypeClFinder();
      Int_t nMdcsSeg=fitpar.getMdcSizesCells()->nModInSeg(sec,seg);
      if(nMdcsSeg==1)        flag = fitMod(fClst1);  // One mdc per segment
      else if(typeClFn==0) {                         // Two mdc per segment
        if(fitType==0)       flag = fitSeg(fClst1);
        else if(fitType==1)  flag = fitCombClusIndFit(fClst1);
      } else if(typeClFn==2) flag = fitMixedClus(fClst1);
      if(!flag) continue;
      if(!fillHitSeg) continue;
      fitter = fittersArr[0].getFitter(0); // ???
      if(fitter->getSegIndex()<0) continue;
      fillTrkCandISeg();
    } else {
      flag = fitSec(fClst1,fClst2);  //??????????????
    }
  }
  if(isGeant) sortGeantInf();
}

void HMdc12Fit::fitAlgorithm4(void) {
  // For cosmic data !!!
  HMdcClus* fClstSeg;
  Int_t  fitType = HMdcTrackDSet::getFitType();
  while((fClstSeg=(HMdcClus*)iterClus->Next())) {
    Int_t sec=fClstSeg->getSec();
    Int_t seg=fClstSeg->getIOSeg();
    Int_t typeClFnSeg=fClstSeg->getTypeClFinder();
    fittersArr[0].reset();
    Int_t nMdcsSeg=fitpar.getMdcSizesCells()->nModInSeg(sec,seg);
    if(nMdcsSeg==1)           fitMod(fClstSeg); // One mdc per segment
    else if(typeClFnSeg==0) {                          // Two mdc per segment
      if(fitType==0)          fitSeg(fClstSeg);
      else if(fitType==1)     fitCombClusIndFit(fClstSeg);
    } else if(typeClFnSeg==2) fitMixedClus(fClstSeg);
  }
  if(isGeant) sortGeantInf();
}

Bool_t HMdc12Fit::fitMod(HMdcClus* fClst, Int_t arrInd) {
  // fit of segment with one mdc
  Int_t modClus = fClst->getMod();
  Int_t iMod;
  if(modClus<0) iMod = (fClst->getNDrTimesMod(0)) ? 0:1; // !!! nado uprostit'
  else          iMod = modClus&1;
    
  fitter = fittersArr[arrInd].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if(!fitter->setClustAndFill(fClst)) return kFALSE;
  fitter->setRegionOfWires();
  if(fClst->getNLayersMod(iMod)<5) isFitted = kFALSE;
  else                             isFitted = fitter->fitCluster();
  if(!fillHitSeg) return kTRUE;
  if(!isFitted) {
    hitSegFiller.setClusParam(fClst,HMdcTrackDSet::isCoilOff());
    if(fillHitSegByClus(iMod)) fClst->setSegIndex(fitter->getSegIndex());
  } else /*while(isFitted)*/ {
    hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
    if(fillHitSegByFit(iMod)) fClst->setSegIndex(fitter->getSegIndex());
    //???   nFitedTimes = fitter->getWiresArr().unsetHits();
    //???   isFitted    = fitter->fitCluster();
  }
  return kTRUE;
}

Bool_t HMdc12Fit::fitSeg(HMdcClus* fClst, Int_t arrInd) {
  // fit of one mdc in segment
  fitter = fittersArr[arrInd].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if(fClst==NULL || !fitter->setClustAndFill(fClst)) return kFALSE;
  fitter->setRegionOfWires();
  isFitted = fitter->fitCluster();
  if(!fillHitSeg) return kTRUE;
  if(!isFitted) {
    hitSegFiller.setClusParam(fClst,HMdcTrackDSet::isCoilOff());
    if(fillHitSegByClus()) fClst->setSegIndex(fitter->getSegIndex());
  } else /*while(isFitted)*/ {
    hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
    if(fillHitSegByFit()) fClst->setSegIndex(fitter->getSegIndex());
    
    
    // Tests for off vertex cluster finder:
    if(fClst->getIOSeg() == 0 && findOffVertTrk) {
      HMdcSeg* pSeg = (HMdcSeg*)fSegCat->getObject(fitter->getSegIndex());
      if(fClst->getNBins() == 0) {                       // Off vertex cluster
        Int_t nUnWires = pSecListCells->getNFittedUnWires2(0,pSeg);
        Int_t nWrTot   = pSeg->getSumWires();
        if(nWrTot-nUnWires >= nUnWires) {
          setFake(pSeg);
          return kFALSE;
        }
        // Add segment fitted wires to the nFittedCounter:
        pSeg->ResetBit(kRealKeep);
        pSecListCells->addToNFittedCounters(0,pSeg);
        pSeg->SetBit(kInFittedList);
      } else if(pSeg->getNLayers() < 7) setFake(pSeg);  // Normal cluster    <= nLayersCutOVT
    }


    //???   nFitedTimes=fitter->getWiresArr().unsetHits();
    //???   isFitted=fitter->fitCluster();
  }
  return kTRUE;
}

Bool_t HMdc12Fit::fitSec(HMdcClus* fClst1, HMdcClus* fClst2) {
  // fit of all mdcs in segment
  fitter = fittersArr[0].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if(!fitter->setClustAndFill(fClst1,fClst2)) return kFALSE;
  fitter->setRegionOfWires();
  HMdcTrkCand* fTrkCand = 0;
  isFitted = fitter->fitCluster();
  if(!fillHitSeg) return kTRUE;
  if(!isFitted) {
    hitSegFiller.setClusParam(fClst1,HMdcTrackDSet::isCoilOff());
    if(fillHitSegByClus(-1)) {
      fClst1->setSegIndex(fitter->getSegIndex());
      if(fTrkCandCat) {
        fTrkCand = fillTrkCandISeg();
        if(fTrkCand == 0) return kFALSE;
      }
    }
    if(fillHitSegByClus(-2)) {
      fClst2->setSegIndex(fitter->getSegIndex());
      if(fTrkCand) fTrkCand = fillTrkCandOSeg(fTrkCand);
    }
  } else /*while(isFitted)*/ {
    hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
    if(fillHitSegByFit(-1)) {
      fClst1->setSegIndex(fitter->getSegIndex());
      if(fTrkCandCat) {
        fTrkCand=fillTrkCandISeg();
        if(fTrkCand==0) return kFALSE;
      }
    }
    if(fillHitSegByFit(-2)) {
      fClst2->setSegIndex(fitter->getSegIndex());
      if(fTrkCand) fTrkCand = fillTrkCandOSeg(fTrkCand);
    }
    //???   nFitedTimes=fitter->getWiresArr().unsetHits();
    //???   isFitted=fitter->fitCluster();
  }
  return kTRUE;
}

Bool_t HMdc12Fit::fit2Sectors(HMdcClus* fClst1, HMdcClus* fClst2,
                              HMdcClus* fClst3, HMdcClus* fClst4) {
  // For cosmic data only.
  // Fit of all mdcs in 2 sectors.
  fitter = fittersArr[0].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if(!fitter->setClustAndFill(fClst1,fClst2,fClst3,fClst4)) return kFALSE;
  fitter->setRegionOfWires();
  isFitted = fitter->fitCluster();
  if(!fillHitSeg) return kTRUE;
  if(!isFitted) {
    //???????????!!!!!!!!!!!!!!!!!!+++
    hitSegFiller.setClusParam(fClst1,HMdcTrackDSet::isCoilOff());
    if(fillHitSegByClus(-1) && fClst1!=NULL) fClst1->setSegIndex(fitter->getSegIndex());
    if(fillHitSegByClus(-2) && fClst2!=NULL) fClst2->setSegIndex(fitter->getSegIndex());
    
    if(fClst3) hitSegFiller.setClusParam(fClst3,HMdcTrackDSet::isCoilOff());
    else       hitSegFiller.setClusParam(fClst4,HMdcTrackDSet::isCoilOff());
    if(fillHitSegByClus(-3) && fClst3!=NULL) fClst3->setSegIndex(fitter->getSegIndex());
    if(fillHitSegByClus(-4) && fClst4!=NULL) fClst4->setSegIndex(fitter->getSegIndex());
    
  } else /*while(isFitted)*/ {
    hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
    if(fillHitSegByFit(-1) && fClst1!=NULL) fClst1->setSegIndex(fitter->getSegIndex());
    if(fillHitSegByFit(-2) && fClst2!=NULL) fClst2->setSegIndex(fitter->getSegIndex());
    if(fitter->getWiresArr().getOutputListCells2().getNDrTimes() > 0) {
      HMdcTrackParam parInSec2(*(fitter->getFinalParam()));
      Int_t sec2 = fitter->getWiresArr().getSector2();
      fitpar.getMdcSizesCells()->transLineToAnotherSec(parInSec2,sec2);
      hitSegFiller.setFitParamForSecondSec(parInSec2);
      if(fillHitSegByFit(-3) && fClst3!=NULL) fClst3->setSegIndex(fitter->getSegIndex());
      if(fillHitSegByFit(-4) && fClst4!=NULL) fClst4->setSegIndex(fitter->getSegIndex());
    }
    //???   nFitedTimes=fitter->getWiresArr().unsetHits();
    //???   isFitted=fitter->fitCluster();
  }
  return kTRUE;
}

Bool_t HMdc12Fit::fitNSectors(HMdcClus* cl1, HMdcClus* cl2,HMdcClus* cl3, HMdcClus* cl4,
                              HMdcClus* cl5, HMdcClus* cl6,HMdcClus* cl7, HMdcClus* cl8) {
  // For cosmic data only.
  // Fit of all mdcs in 2 sectors.
  fitter = fittersArr[0].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if( !fitter->setClustAndFill(cl1,cl2,cl3,cl4,cl5,cl6,cl7,cl8) ) return kFALSE;
  fitter->setRegionOfWires();
  isFitted = fitter->fitCluster();
  if(!fillHitSeg) return kTRUE;
  if(!isFitted) {
    //???????????!!!!!!!!!!!!!!!!!!+++
    hitSegFiller.setClusParam(cl1,HMdcTrackDSet::isCoilOff());
    if(cl1!=NULL && fillHitSegByClus(-1)) cl1->setSegIndex(fitter->getSegIndex());
    if(cl2!=NULL && fillHitSegByClus(-2)) cl2->setSegIndex(fitter->getSegIndex());
    
    if(cl3!=NULL) hitSegFiller.setClusParam(cl3,HMdcTrackDSet::isCoilOff());
    else          hitSegFiller.setClusParam(cl4,HMdcTrackDSet::isCoilOff());
    if(cl3!=NULL && fillHitSegByClus(-3)) cl3->setSegIndex(fitter->getSegIndex());
    if(cl4!=NULL && fillHitSegByClus(-4)) cl4->setSegIndex(fitter->getSegIndex());
    if(cl5==NULL && cl6==NULL) return kTRUE;
    if(cl5!=NULL) hitSegFiller.setClusParam(cl5,HMdcTrackDSet::isCoilOff());
    else          hitSegFiller.setClusParam(cl6,HMdcTrackDSet::isCoilOff());
    if(cl5!=NULL && fillHitSegByClus(-5)) cl5->setSegIndex(fitter->getSegIndex());
    if(cl6!=NULL && fillHitSegByClus(-6)) cl6->setSegIndex(fitter->getSegIndex());
    if(cl7==NULL && cl8==NULL) return kTRUE;
    if(cl7!=NULL) hitSegFiller.setClusParam(cl7,HMdcTrackDSet::isCoilOff());
    else          hitSegFiller.setClusParam(cl8,HMdcTrackDSet::isCoilOff());
    if(cl7!=NULL && fillHitSegByClus(-7)) cl7->setSegIndex(fitter->getSegIndex());
    if(cl8!=NULL && fillHitSegByClus(-8)) cl8->setSegIndex(fitter->getSegIndex());
  } else {
    hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
    if(cl1 && fillHitSegByFit(-1)) cl1->setSegIndex(fitter->getSegIndex());
    if(cl2 && fillHitSegByFit(-2)) cl2->setSegIndex(fitter->getSegIndex());
    if(fitter->getWiresArr().getOutputListCells2().getNDrTimes() > 0) {
      HMdcTrackParam parInSec2(*(fitter->getFinalParam()));
      Int_t sec2 = fitter->getWiresArr().getSector2();
      fitpar.getMdcSizesCells()->transLineToAnotherSec(parInSec2,sec2);
      hitSegFiller.setFitParamForSecondSec(parInSec2);
      if(cl3!=NULL && fillHitSegByFit(-3)) cl3->setSegIndex(fitter->getSegIndex());
      if(cl4!=NULL && fillHitSegByFit(-4)) cl4->setSegIndex(fitter->getSegIndex());
    }
    if(cl5==NULL && cl6==NULL) return kTRUE;
    if(fitter->getWiresArr().getOutputListCells3().getNDrTimes() > 0) {
      HMdcTrackParam parInSec3(*(fitter->getFinalParam()));
      Int_t sec3 = fitter->getWiresArr().getSector3();
      fitpar.getMdcSizesCells()->transLineToAnotherSec(parInSec3,sec3);
      hitSegFiller.setFitParamForSecondSec(parInSec3);
      if(cl5!=NULL && fillHitSegByFit(-5)) cl5->setSegIndex(fitter->getSegIndex());
      if(cl6!=NULL && fillHitSegByFit(-6)) cl6->setSegIndex(fitter->getSegIndex());
    }
    if(cl7==NULL && cl8==NULL) return kTRUE;
    if(fitter->getWiresArr().getOutputListCells4().getNDrTimes() > 0) {
      HMdcTrackParam parInSec4(*(fitter->getFinalParam()));
      Int_t sec4 = fitter->getWiresArr().getSector4();
      fitpar.getMdcSizesCells()->transLineToAnotherSec(parInSec4,sec4);
      hitSegFiller.setFitParamForSecondSec(parInSec4);
      if(cl7!=NULL && fillHitSegByFit(-7)) cl7->setSegIndex(fitter->getSegIndex());
      if(cl8!=NULL && fillHitSegByFit(-8)) cl8->setSegIndex(fitter->getSegIndex());
    }
  }
  return kTRUE;
}

Bool_t HMdc12Fit::fitCombClusIndFit(HMdcClus* fClst, Int_t arrInd) {
  // CombinedClusters + Independent fit
  fitter = fittersArr[arrInd].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if(!fitter->setClustAndFill(fClst)) return kFALSE;
  Int_t hitIndex[2];
  Int_t seg = fClst->getIOSeg();
  Int_t nFittedHits = 0;
  for(Int_t iMod=0; iMod<2; iMod++) {
    Int_t mod=iMod+seg*2;
    fitter->setRegionOfWires(mod);
    if(fClst->getNLayersMod(iMod)<5) isFitted = kFALSE;
    else                             isFitted = fitter->fitCluster(mod);
    if(!fillHitSeg) continue;
    if(!isFitted) {
      hitSegFiller.setClusParam(fClst,HMdcTrackDSet::isCoilOff());
      hitIndex[iMod] = fillHitByClus(mod);
    } else {
      nFittedHits++;
      hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
      hitIndex[iMod] = fillHitByFit(mod);
    }
  }
  isFitted = nFittedHits==2;
  if(fillHitSeg) fillSegBy2Hits(hitIndex[0],hitIndex[1]);
  return kTRUE;
}

Bool_t HMdc12Fit::fitChambClus(HMdcClus* fClst, Int_t arrInd) {
  // fit of ChamberClusters:
  Int_t modClus=fClst->getMod();
  if(modClus<0) return kFALSE;
  fitter = fittersArr[arrInd].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if(!fitter->setClustAndFill(fClst)) return kFALSE;
  Int_t iMod=modClus&1;
  fitter->setRegionOfWires();
  isFitted = fitter->fitCluster();
  if(!fillHitSeg) return kTRUE;
  if(!isFitted) {
    hitSegFiller.setClusParam(fClst,HMdcTrackDSet::isCoilOff());
    if(modClus==modForSeg[fitter->getWiresArr().getSegment()]) {
      if(fillHitSegByClus(iMod)) fClst->setSegIndex(fitter->getSegIndex());
    } else fillHitByClus(modClus);
  } else /*while(isFitted)*/ {
    hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
    if(modClus==modForSeg[fitter->getWiresArr().getSegment()]) {
      if(fillHitSegByFit(iMod)) fClst->setSegIndex(fitter->getSegIndex());
    } else fillHitByFit(modClus);
    //???   nFitedTimes=fitter->getWiresArr().unsetHits();
    // ???   isFitted=fitter->fitCluster();
  }
  return kTRUE;
}

Bool_t HMdc12Fit::fitMixedClus(HMdcClus* fClst, Int_t arrInd) {
  // fit of MixedClusters:
  Int_t modClus=fClst->getMod();
  if(modClus<0) return kFALSE;
  Int_t iMod=modClus&1;
  Int_t addMod=fClst->getIOSeg()*2 + (iMod^1);
  if(fClst->getNLayersMod(iMod^1)<HMdcTrackDSet::getMixFitCut()) return kFALSE;
  fitter = fittersArr[arrInd].getNewFitter();
  if(pTrackInfSim) pTrackInfSim->setWiresArr(&(fitter->getWiresArr()));
  if(!fitter->setClustAndFill(fClst)) return kFALSE;
  fitter->setRegionOfWires();
  isFitted = fitter->fitCluster();
  if(!isFitted) {
    if(fillHitSeg && fClst->getNLayersMod(iMod^1) >= HMdcTrackDSet::getMixClusCut()) {
      hitSegFiller.setClusParam(fClst,HMdcTrackDSet::isCoilOff());
      if(fillHitSegByClus(iMod)) fClst->setSegIndex(fitter->getSegIndex());
    }
  } else /*while(isFitted)*/ {
    if(fitter->getWiresArr().getOutputListCells().getNLayersMod(addMod) <
                                         HMdcTrackDSet::getMixFitCut()) isFitted = kFALSE;
    else if(fillHitSeg) {
//    if(fitter->listCells.getNLayersMod(addMod) >= HMdcTrackDSet::getMixFitCut()) {
      hitSegFiller.setFitParam(*(fitter->getFinalParam())); //!
      fillHitSegByFit(iMod); // ??? esli cut ne proshol mozhet zaolnit' clust?
    };
    //???   nFitedTimes=fitter->getWiresArr().unsetHits();
    //???   isFitted=fitter->fitCluster();
  }
  return kTRUE;
}

void HMdc12Fit::fillClusInf(HMdcHit* fHit, Int_t iMod, Char_t fitSt,Int_t indClusFit) {
  fHit->setTrackFinder(1);
  if(fClusInfCat==0) return; // Cosmic data
  Int_t index;
  HMdcClusInf* fClusInf=(HMdcClusInf*)fClusInfCat->getNewSlot(locClInf,&index);
  if(!fClusInf) {
    Warning("fillClusInf","No slot HMdcClusInf available");
    return;
  }
  fClusInf = (isGeant) ? (HMdcClusInf*)(new(fClusInf) HMdcClusInfSim) :
                                        new(fClusInf) HMdcClusInf;
  fHit->setClusInfIndex(index);
  fClusInf->setFitStat(fitSt);
  Int_t mod = fHit->getModule();
  HMdcWiresArr& wires = fitter->getWiresArr();
  HMdcClus* fClst = wires.getClust2() && mod>1 ? 
                    wires.getClust2() : wires.getClust1();
  
  Int_t nmods=fClst->getMod();
  fClusInf->setNModInCl((nmods<0) ? -nmods:1); //?????????peredelat!
//   nmods=0;
//   for(Int_t m=0;m<4;m++) if(fitter->nTimesInFit[m]>0) nmods++;
  fClusInf->setNModInFit(wires.getNModulesInFit());
  fClusInf->setClusIndex(fClst->getOwnIndex());
  fClusInf->setClusFitIndex(fitter->getIndClusFit());
  
  fClusInf->setXYHit(fHit->getX(),fHit->getY());
  
  HMdcSizesCellsMod* fSCMod = fitpar.getSCellsModArr(wires.getSector())[mod];
  Double_t xm,ym,zm;
  fSCMod->calcIntersection(
      fClst->getXTarg(),fClst->getYTarg(),fClst->getZTarg(),
      fClst->getX(),    fClst->getY(),    fClst->getZ(),    xm,ym,zm);
  fSCMod->transTo(xm,ym,zm);
  fClusInf->setXYClus(xm,ym);
  
  if(iMod==0) {
    fClusInf->setLevelClFinding(fClst->getClFnLevelM1());
    fClusInf->setClusSize(fClst->getClusSizeM1());
    fClusInf->setNMergClust(fClst->getNMergClustM1());
    fClusInf->setNDrTimes(fClst->getNDrTimesM1());
    fClusInf->setSigma1(fClst->getSigma1M1());
    fClusInf->setSigma2(fClst->getSigma2M1());
    fClusInf->setAlpha(fClst->getAlphaM1());
    fClusInf->setNLayers(fClst->getNLayersMod(0));
    if(!fitSt) {
      fClusInf->setNLayersFitted(fClst->getNLayersMod(0));
      fClusInf->setNDrTimesFitted(fClst->getNDrTimesM1());
    }
  } else {
    fClusInf->setLevelClFinding(fClst->getClFnLevelM2());
    fClusInf->setClusSize(fClst->getClusSizeM2());
    fClusInf->setNMergClust(fClst->getNMergClustM2());
    fClusInf->setNDrTimes(fClst->getNDrTimesM2());
    fClusInf->setSigma1(fClst->getSigma1M2());
    fClusInf->setSigma2(fClst->getSigma2M2());
    fClusInf->setAlpha(fClst->getAlphaM2());
    fClusInf->setNLayers(fClst->getNLayersMod(1));
    if(!fitSt) {
      fClusInf->setNLayersFitted(fClst->getNLayersMod(1));
      fClusInf->setNDrTimesFitted(fClst->getNDrTimesM2());
    }
  }
  if(fitSt) {
    fClusInf->setNLayersFitted(fitter->getWiresArr().getOutputListCells().getNLayersMod(mod));
    fClusInf->setNDrTimesFitted(fitter->getWiresArr().getOutputListCells().getNDrTimesMod(mod));
  }
  if(isGeant) {
    HMdcClusSim* fClSim=(HMdcClusSim*)fClst;
    ((HMdcClusInfSim*)fClusInf)->setTracksList(fClSim->getNTracksMod(iMod),
        fClSim->getListTrM(iMod),fClSim->getNTimesM(iMod));
  }
}

void HMdc12Fit::fillClusInf(HMdcSeg* fSeg,Bool_t secondSec) {
  HMdcWiresArr& wires = fitter->getWiresArr();
  HMdcClus* fClst1 = wires.getClust1();
  HMdcClus* fClst2 = wires.getClust2();
  if(secondSec) {
    fClst1 = wires.getClust3();
    fClst2 = wires.getClust4();
  }
  Int_t nMrCl = fClst1->getNMergClust();
  if(nMrCl>99) nMrCl=99;
  HMdcClus* fClst=(fClst2 && fSeg->getIOSeg()==1) ? fClst2 : fClst1;
  fSeg->setClusInfo(fClst->getNBins(),fClst->getNDrTimes(),fClst1->getNMergClust());
  
  fSeg->setClusIndex(fClst->getOwnIndex());
}

Bool_t HMdc12Fit::fillSegBy2Hits(Int_t ind1, Int_t ind2) {
  HMdcWiresArr& wires = fitter->getWiresArr();
  Int_t segment = wires.getSegment();
  Int_t mod1    = segment*2;
  Int_t mod2    = mod1+1;
  Int_t sector  = wires.getSector();
  HMdcSizesCellsMod* fSCMod1=fitpar.getSCellsModArr(sector)[mod1];
  HMdcSizesCellsMod* fSCMod2=fitpar.getSCellsModArr(sector)[mod2];
  if(!fSCMod1 || !fSCMod2) return kFALSE;
  HMdcHit* h1=(HMdcHit*)fHitCat->getObject(ind1);
  HMdcHit* h2=(HMdcHit*)fHitCat->getObject(ind2);
  if(h1==0 || h2==0) return kFALSE;
  Float_t x1,y1,z1,ex1,ey1;
  Float_t x2,y2,z2,ex2,ey2;
  h1->getXY(x1,ex1,y1,ey1);
  h2->getXY(x2,ex2,y2,ey2);
  fSCMod1->transFromZ0(x1,y1,z1);
  fSCMod2->transFromZ0(x2,y2,z2);
  ey1 /= sqrt(1.+fSCMod1->B()*fSCMod1->B()); // simple error recalculation
  ey2 /= sqrt(1.+fSCMod2->B()*fSCMod2->B()); // (let eX_sec=eX_mod,...)
  HMdcSeg* fSeg = getSegSlot(sector,segment);
  if(!fSeg) return kFALSE;
  for(Int_t l=0; l<6; l++) {
    copySignId(h1,0,l,fSeg);
    copySignId(h2,1,l,fSeg);
  }
  Double_t zm, r0, theta, phi;
  Double_t eZm, eR0, eTheta, ePhi;
  HMdcSizesCells::calcMdcSeg(x1,y1,z1,x2,y2,z2,ex1,ey1,0.,ex2,ey2,
      -fSCMod1->A(),-fSCMod1->B(),-fSCMod2->A(),-fSCMod2->B(),
      zm,eZm, r0,eR0,theta,eTheta, phi,ePhi);
  fSeg->setPar(zm, eZm, r0, eR0, theta, eTheta, phi, ePhi);
  
  HMdcSizesCellsSec& fSCSec=(*(fitpar.getMdcSizesCells()))[sector];
  Double_t zPrime=zm; 
  Double_t rPrime=r0;
  if(&fSCSec) fSCSec.calcRZToTargLine(x1,y1,z1,x2,y2,z2,zPrime,rPrime);
  fSeg->setZprime(zPrime);
  fSeg->setRprime(rPrime);
  
  fSeg->setHitInd(0,ind1);
  fSeg->setHitInd(1,ind2);
  fSeg->setChi2(-2.);
  if(isGeant) ((HMdcSegSim*)fSeg)->calcNTracks();
  
  if(h1->getChi2()>=0 || h2->getChi2()>=0) fitter->setFitStatus(kTRUE); // ??? podumat'!!!

  fillClusInf(fSeg);
  return kTRUE;
}

void HMdc12Fit::copySignId(HMdcHit* hit,Int_t mod,Int_t lay,HMdcSeg* seg) {
  // do Signals Id copy from Hit to Seg
  Int_t nHits=hit->getNCells(lay);
  if(nHits<=0||nHits>4) return;
  Int_t id[4]={0,0,0,0};
  Int_t cell=hit->getCell(lay,0);
  for(Int_t i=0; i<nHits; i++) {
    Int_t n=hit->getCell(lay,i) - cell;
    if(n>=0&&n<4) id[n] = hit->getSignId(lay,i);
  }
  seg->setSignId(lay+mod*6,cell,id[0],id[1],id[2],id[3]);
}

Int_t HMdc12Fit::fillHitByFit(Int_t mod) {
  HMdcWiresArr& wires = fitter->getWiresArr();
  HMdcList24GroupCells* lCells = &(fitter->getWiresArr().getOutputListCells());
  Int_t sector = wires.getSector();
  if(mod>=4) {  // Cosmic
    lCells = &(fitter->getWiresArr().getOutputListCells2());
    sector = wires.getSector2();
    if(sector<0) return -1;
    mod -= 4; 
  }
  if(lCells->getNDrTimesMod(mod) == 0) return -1;
  HMdcSizesCellsMod* fSCMod=fitpar.getSCellsModArr(sector)[mod];
  if(!fSCMod) return -1;
  Int_t index=-1;
  HMdcHit* fHit = getHitSlot(sector,mod,index);
  if(!fHit) return -1;
  hitSegFiller.fillMdcHit(fSCMod,fHit);
  fHit->setChi2(fitter->getChi2()); // getFunMin())/NDF
  setGroup(lCells, fHit, mod);
  fillClusInf(fHit,mod&1,1,fitter->getIndClusFit()); // Cluster inform.
  return index;
}

Bool_t HMdc12Fit::fillHitSegByFit(Int_t iMod) {
  // iMod==0   - first MDC in segment
  // iMod==1   - second MDC in segment
  // iMod==-99 - two MDC in segment
  // iMod==-1  - first segment of sector track
  // iMod==-2  - second segment of sector track
  // iMod==-3  - first segment of second sector in track (cosmic)
  // iMod==-4  - second segment of second sector in track (cosmic)
  HMdcWiresArr& wires = fitter->getWiresArr();
  Int_t segInd = wires.getSegment();
  if(iMod<=-1 && iMod>=-4) segInd = -iMod-1;  // sector fit !!!???
  Int_t segment = segInd&1;
  HMdcList24GroupCells* lCells = &(wires.getOutputListCells());
  Int_t sector = wires.getSector();
  if(segInd>1) {
    lCells = &(wires.getOutputListCells2());
    sector = wires.getSector2();
    if(sector<0) return kFALSE;
  }
  if(lCells->getNDrTimes(segment*12,segment*12+11) == 0) return kFALSE;
  
  HMdcSeg* fSeg = getSegSlot(sector,segment);
  if(!fSeg) return kFALSE;
  Int_t indx1=-1;
  if(iMod!=1) indx1=fillHitByFit(segInd*2);
  Int_t indx2=-1;
  if(iMod!=0) indx2=fillHitByFit(segInd*2+1);
  if(indx1>=0) fSeg->setHitInd(0,indx1);
  if(indx2>=0) fSeg->setHitInd(1,indx2);
  fSeg->setChi2(fitter->getChi2()); // getFunMin());
  setGroup(lCells, fSeg, segment);
  HMdcSizesCellsSec& fSCSec=(*(fitpar.getMdcSizesCells()))[sector];
  const HGeomVector* targ=0;
  if(&fSCSec) targ=&(fSCSec.getTargetMiddlePoint());
  if( findOffVertTrk && segment==0 && wires.getClust1()->getNBins()==0) fSeg->setOffVertexClus();
 
  hitSegFiller.fillMdcSeg(fSeg,targ);
  // Cluster param:
  fillClusInf(fSeg,segInd>1);
  return kTRUE;
}

Int_t HMdc12Fit::fillHitByClus(Int_t mod) {
  HMdcWiresArr& wires = fitter->getWiresArr();
  HMdcList24GroupCells* lCells = &(fitter->getWiresArr().getInputListCells());
  Int_t sector = wires.getSector();
  if(mod>=12) {  // Cosmic
    lCells = &(wires.getInputListCells4());
    sector = wires.getSector4();
  } else if(mod>=8) {  // Cosmic
    lCells = &(wires.getInputListCells3());
    sector = wires.getSector3();
  } else if(mod>=4) {  // Cosmic
    lCells = &(wires.getInputListCells2());
    sector = wires.getSector2();
  }
  mod &= 3;
  if(sector<0) return -1;
  
  if(lCells->getNDrTimesMod(mod) == 0) return -1;
  HMdcSizesCellsMod* fSCMod = fitpar.getSCellsModArr(sector)[mod];
  if(!fSCMod) return -1;
  Int_t index = -1;
  HMdcHit* fHit = getHitSlot(sector,mod,index);
  if(!fHit) return -1;
  hitSegFiller.fillMdcHitByClus(fSCMod,fHit);
  fHit->setChi2(-1.);
  setGroup(lCells, fHit,mod);
  fillClusInf(fHit,mod&1,0); // Cluster inform.
  return index;
}

Bool_t HMdc12Fit::fillHitSegByClus(Int_t iMod) {
  // iMod==0   - first MDC in segment
  // iMod==1   - second MDC in segment
  // iMod==-99 - two MDC in segment
  // iMod==-1  - first segment of sector track
  // iMod==-2  - second segment of sector track
  // iMod==-3  - first segment of second sector in track (cosmic)
  // iMod==-4  - second segment of second sector in track (cosmic)
  // iMod==-5 ...-8  - third & fourth sector in track (cosmic)
  HMdcWiresArr& wires = fitter->getWiresArr();
  Int_t segInd = wires.getSegment();
  if(iMod<=-1 && iMod>=-8) segInd = -iMod-1;  // sector fit !!!???
  Int_t secInd = segInd/4;
  Int_t segment = segInd&1;
  HMdcList24GroupCells* lCells = &(wires.getInputListCells());
  Int_t sector = wires.getSector();
  if(secInd==1) {
    lCells = &(wires.getInputListCells2());
    sector = wires.getSector2();
  } else if(secInd==2) {
    lCells = &(wires.getInputListCells3());
    sector = wires.getSector3();
  } else if(secInd==3) {
    lCells = &(wires.getInputListCells4());
    sector = wires.getSector4();
  }
  if(sector<0) return kFALSE;
  if(lCells->getNDrTimes(segment*12,segment*12+11) == 0) return kFALSE;
  
  HMdcSeg* fSeg = getSegSlot(sector,segment);
  if(fSeg == NULL) return kFALSE;
  Int_t indx1 = -1;
  if(iMod != 1) indx1 = fillHitByClus(segInd*2);
  Int_t indx2 = -1;
  if(iMod != 0) indx2 = fillHitByClus(segInd*2+1);
  if(indx1 >= 0) fSeg->setHitInd(0,indx1);
  if(indx2 >= 0) fSeg->setHitInd(1,indx2);
  fSeg->setChi2(-1.);
  setGroup(lCells, fSeg, segment);
  HMdcSizesCellsSec& fSCSec = (*(fitpar.getMdcSizesCells()))[sector];
  const HGeomVector* targ   = (&fSCSec) ? &(fSCSec.getTargetMiddlePoint()) : 0;
  hitSegFiller.fillMdcSegByClus(fSeg,targ,fitter->getInitParam());
  // Cluster param:
  fillClusInf(fSeg,segInd>1);
  return kTRUE;
}

HMdcHit* HMdc12Fit::getHitSlot(Int_t sec, Int_t mod, Int_t& index) {
  // Geting a slot and setting address
  locHit[0] = sec;
  locHit[1] = mod;
  HMdcHit* fHit = (HMdcHit*)fHitCat->getNewSlot(locHit,&index);
  if(!fHit) {
    Warning("getHitSlot"," No slot in catMdcHit available");
    index=-1;
    return 0;
  }
  if(isGeant) fHit=(HMdcHit*) new(fHit) HMdcHitSim;
  else fHit= new(fHit) HMdcHit;
  fHit->setSecMod(sec,mod);
  return fHit;
}

HMdcSeg* HMdc12Fit::getSegSlot(Int_t sec,Int_t segment) {
  // Geting a slot and setting address
  if(sec<0) return 0;
  locSeg[0] = sec;
  locSeg[1] = segment;
  Int_t index;
  HMdcSeg* fSeg = (HMdcSeg*)fSegCat->getNewSlot(locSeg,&index);
  if(!fSeg) {
    Warning("getSegSlot"," No slot in catMdcSeg available");
    fitter->setSegIndex(-1);
    return 0;
  }
  fitter->setSegIndex(index);      // For cosmic data it is not used
  fSeg = (isGeant) ? (HMdcSeg*)(new(fSeg) HMdcSegSim) : new(fSeg) HMdcSeg;
  fSeg->setSec(sec);
  fSeg->setIOSeg(segment);
  if( !isCosmicData ) {
    if(indFirstSeg[sec] < 0) indFirstSeg[sec] = index;
    indLastSeg[sec] = index;
  }
  return fSeg;
}

HMdcTrkCand* HMdc12Fit::fillTrkCandISeg(void) {
  if(fTrkCandCat == NULL) return NULL;
  Int_t index;
  HMdcTrkCand* fTrkCandNew = (HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
  if( fTrkCandNew == NULL ) {
    Warning("fillTrkCandISeg"," No slot available in catMdcTrkCand");
    fitter->setSegIndex(-1);
    return NULL;
  }
  Int_t sec = fitter->getWiresArr().getSector();
  if(indFirstSeg[sec] < 0) indFstTrCand[sec] = index;
  indLstTrCand[sec] = index;
  return new(fTrkCandNew) HMdcTrkCand(sec,fitter->getSegIndex(),index);
}

HMdcTrkCand* HMdc12Fit::fillTrkCandOSeg(HMdcTrkCand* fTrkCand) {
  if(fTrkCandCat==0 || fTrkCand==0) return 0;
  Int_t index;
  HMdcTrkCand* fTrkCandNew = (HMdcTrkCand*)fTrkCandCat->getNewSlot(locTrkCand,&index);
  if(!fTrkCandNew) {
    Warning("fillTrkCandOSeg"," No slot available in catMdcTrkCand");
    fitter->setSegIndex(-1);
    return NULL;
  }
  Int_t sec = fTrkCand->getSec();
  if(indFstTrCand[sec] < 0) indFstTrCand[sec] = index;
  indLstTrCand[sec] = index;
  return new(fTrkCandNew) HMdcTrkCand(fTrkCand,fitter->getSegIndex(),index);
}

Double_t HMdc12Fit::calcVertexDist( Double_t x0, Double_t y0, Double_t z0,
			      Double_t x1, Double_t y1, Double_t z1,
			      Double_t x2, Double_t y2, Double_t z2) {
  // Calculeting a min distance betveen point x0, y0, z0 and line
  return sqrt( ((y2-y1)*(z1-z0) - (z2-z1)*(y1-y0))*((y2-y1)*(z1-z0) - (z2-z1)*(y1-y0)) +
	       ((z2-z1)*(x1-x0) - (x2-x1)*(z1-z0))*((z2-z1)*(x1-x0) - (x2-x1)*(z1-z0)) +
               ((x2-x1)*(y1-y0) - (y2-y1)*(x1-x0))*((x2-x1)*(y1-y0) - (y2-y1)*(x1-x0)) )/
    sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
}

void HMdc12Fit::setGroup(HMdcList24GroupCells* cells, HMdcSeg* fSeg, Int_t seg) {
  // Filling list of cells in HMdcSeg
  Int_t lc = seg*12;
  for(Int_t l=0; l<12; l++,lc++) fSeg->setLayerGroup(l,cells->getOneLayerGroup(lc));

  if(pTrackInfSim) {
    HMdcClus *pClus = fitter->getWiresArr().getClust1();
    Int_t indPar  = pClus->getIndexParent();
    HMdcSegSim *pSegPar = NULL;
    if(indPar>=0) {
      HMdcClus *pClus = (HMdcClus*)fClusCat->getObject(indPar);
      Int_t segIndex = pClus == NULL ? -1 : pClus->getSegIndex();
      if(segIndex >= 0) pSegPar = (HMdcSegSim*)fSegCat->getObject(segIndex);
    }
    pTrackInfSim->fillSegTrackInf((HMdcSegSim*)fSeg,cells,pSegPar);
  }
}

void HMdc12Fit::setGroup(HMdcList24GroupCells* cells, HMdcHit* fHit, Int_t mod) {
  // Filling list of cells in HMdcHit
  Int_t lc = mod*6;
  for(Int_t l=0; l<6; l++,lc++) fHit->setLayerGroup(l,cells->getOneLayerGroup(lc));
  if(pTrackInfSim) pTrackInfSim->fillHitTrackInf((HMdcHitSim*)fHit,cells);
}

void HMdc12Fit::printStatusM(void) {
  // prints the parameters to the screen
  const Char_t* hAuth  = "1 = Alexander, 2 = tuned ver. 1";
  const Char_t* hTmVer = "0 = polynomial,  1 = HMdcCal2ParSim::calcTime";
  const Char_t* hFType = "0 = segment fit,  1 = independent mdc fit";
  const Char_t* hDCat  = "1 = create and fill catMdcClusFit&catMdcWireFit 0 = don't cr.";
  const Char_t* hDPr   = "1 = print debug information, 0 = don't print";
  const Char_t* hUWOff = "1 = remove signal time on wire from drift time, 0 = don't remove";
  const Char_t* hCITrp = "1 = for all clusters, 2 = for not fitted+, 0 = don't calc. init tr.param.";
  printf("--------------------------------------------------------------------------------------------\n");
  printf("HMdc12FitSetup:\n");
  printf("CalcInitTrParam =  %i :  %s\n",HMdcTrackDSet::getCalcInitValueFlag(),hCITrp);
  if(HMdcTrackDSet::getCalcInitValueFlag() > 0) printf("  dDistCut Seg1 = %.2f  Seg2 = %.2f[mm]\n",
      HMdcTrackDSet::getCalcInitValueCut(0),HMdcTrackDSet::getCalcInitValueCut(1));
  printf("FitVersion      =  %i :  %s\n",HMdcTrackDSet::getFitAuthor(),hAuth);
  printf("DrTimeCalcVers  =  %i :  %s\n",HMdcTrackDSet::getFitVersion(),hTmVer);
  printf("FitType         =  %i :  %s\n",HMdcTrackDSet::getFitType(),hFType);
  printf("useWireOffset   =  %i :  %s\n",HMdcTrackDSet::getUseWireOffset(),hUWOff);
  printf("DebugCategorie  =  %i :  %s\n",HMdcTrackDSet::fNTuple(),hDCat);
  printf("DebugPrint      =  %i :  %s\n",HMdcTrackDSet::fPrint(),hDPr);
  printf("--------------------------------------------------------------------------------------------\n");
  prntSt = kTRUE;
}

Bool_t HMdc12Fit::finalize(void) {
  return kTRUE;
}

void HMdc12Fit::suppressFakes(Int_t sec,Int_t segIO) {
  if(indLastSeg[sec] <= indFirstSeg[sec]) return;
  currSeg = segIO;
  Int_t nShLayCut = nSharedLayersCut[currSeg];
  Int_t nLayOrCut = nLayerOrientCut[currSeg];
  Int_t nDCMinCut = nDCellsMinCut[currSeg];
  Int_t nDCCut    = nDCellsCut[currSeg];

  HMdcEvntListCells *pEvListCells = HMdcEvntListCells::getExObject();

  pSecListCells  = &((*pEvListCells)[sec]);
  indFirst       = indFirstSeg[sec];
  indLast        = indLastSeg[sec];
  indFirstTrCand = indFstTrCand[sec];
  indLastTrCand  = indLstTrCand[sec];

  for(Int_t is1=indFirst+1; is1<=indLast; is1++) {
    HMdcSeg* seg1 = (HMdcSeg*)fSegCat->getObject(is1);
    if(seg1->getIOSeg() != currSeg) continue;
    if(seg1->isFake()) continue;
    Short_t x1       = seg1->getX1Sh();
    Short_t y1       = seg1->getY1Sh();
    Short_t x2       = seg1->getX2Sh();
    Short_t y2       = seg1->getY2Sh();
    Int_t   nWires1  = seg1->getSumWires();
    Int_t   nLayers1 = seg1->getNLayers();
    Float_t chi2S1   = seg1->getChi2();

    for(Int_t is2 = indFirst; is2<is1; is2++) {
      HMdcSeg* seg2 = (HMdcSeg*)fSegCat->getObject(is2);
      if(seg2->getIOSeg() != currSeg) continue;
      if(seg2->isFake()) continue;
//        if(TMath::Abs(x1 - seg2->getX1Sh())>700 || TMath::Abs(x2 - seg2->getX2Sh())>700 ) continue;
//        if(TMath::Abs(y1 - seg2->getY1Sh())>200 || TMath::Abs(y2 - seg2->getY2Sh())>200 ) continue;
       if(TMath::Abs(x1 - seg2->getX1Sh())>400 || TMath::Abs(x2 - seg2->getX2Sh())>400 ) continue;
       if(TMath::Abs(y1 - seg2->getY1Sh())>150 || TMath::Abs(y2 - seg2->getY2Sh())>150 ) continue;

      Int_t nLayOr, nSharedLayers;
      Int_t nSharedCells = seg1->getNSharedCells(seg2,nSharedLayers,nLayOr);

      if(nSharedLayers<nShLayCut || nLayOr<nLayOrCut) continue;

      Int_t dCells1   = nWires1             - nSharedCells;
      Int_t dCells2   = seg2->getSumWires() - nSharedCells;
      Int_t dCellsMin = TMath::Min(dCells1,dCells2);

      if((dCellsMin<nSharedCells && dCellsMin<nDCMinCut) || dCells1+dCells2 < nDCCut) { // <nSharedCeels-2 (?)
        Float_t chi2S2   = seg2->getChi2();
        Int_t   nLayers2 = seg2->getNLayers();
        Int_t   prefer   = 0;
        if(chi2S2 < 0.) prefer  = 1;
        if(chi2S1 < 0.) prefer += 2;
        if(prefer == 3 || prefer == 0) { 
          if(nLayers1 != nLayers2) prefer = nLayers1 > nLayers2 ? 1:2;
          else if(prefer == 0)     prefer = chi2S1   < chi2S2   ? 1:2; // Both segs are fitted
          else                     prefer = dCells1  > dCells2  ? 1:2; // Both segs are not fitted
        }
        if(prefer == 1) {
          if(isGeant) testGeantInf((HMdcSegSim*)seg2,(HMdcSegSim*)seg1); // must be before call setFake
          if(currSeg == 1) setFakeFlageAndIndexis(seg2,is2,seg1,is1);
          setFake(seg2);
        } else {
          if(isGeant) testGeantInf((HMdcSegSim*)seg1,(HMdcSegSim*)seg2); // must be before call setFake
          if(currSeg == 1) setFakeFlageAndIndexis(seg1,is1,seg2,is2);
          setFake(seg1);
        }      
        if(prefer == 2) break;
      }
    }
  }
  //-----Suppression. Step 2----------------------------------------------
  addToNFittedCounter();

  Int_t n1,n2;
  do {
    n1 = markReals(1);  //??????????????
    n2 = markFakes(1);  //  1  ??????????????
  } while( n1+n2 > 0);

//-------------------------------------------------

  //--Finalize indexis ------------------------------------------------
  for(Int_t is=indFirst; is<=indLast; is++) {
    HMdcSeg* seg = (HMdcSeg*)fSegCat->getObject(is);
    if(seg->getIOSeg() == currSeg && seg->isFake() ) setFakeInd(is);
  }
}

void HMdc12Fit::setFake(HMdcSeg* segFake) {
//-  if(segFake->isFake()) return;
  segFake->setFakeFlag();
  segFake->SetBit(kKeep);   // Don't change fakeFlag in next steps of fake supretion
}

void HMdc12Fit::setFakeInd(Int_t index) {
  // Set fake flag in HMdcTrkCand:
  if(indFirstTrCand >= 0) for(Int_t itr=indFirstTrCand; itr<=indLastTrCand; itr++) {
    HMdcTrkCand* trkCand = (HMdcTrkCand*)fTrkCandCat->getObject(itr);
    if(currSeg == 0) {
      if(index == trkCand->getSeg1Ind()) trkCand->setFakeFlagSeg1();
    } else {
      if(index == trkCand->getSeg2Ind()) trkCand->setFakeFlagSeg2();
    }
  }
  // Set fake flag in HMdcClusFit:
  if(fClusFitCat == NULL) fClusFitCat = HMdcGetContainers::getObject()->getCatMdcClusFit();
  if(fClusFitCat != NULL && clusFitAlg == 1) {
    // next code correct if indexis of HMdcSeg and HMdcClusFit is the same (true for clusFitAlg=1)!
    HMdcClusFit *clFit = (HMdcClusFit*)fClusFitCat->getObject(index);
    if(clFit == NULL) Error("setFake","Pointer to the HMdcClusFit object = NULL!");
    else  clFit->setFakeFlag();
  }
}

void HMdc12Fit::testGeantInf(HMdcSegSim* segFake,HMdcSegSim* segGood) {
  if(segGood->isFake()) return;
  if(segFake->isFakeContribution(0)) return;
  Int_t trf = segFake->getTrack(0);
  if(trf<=0 || trf != segGood->getTrack(0)) return;
  if(segGood->getChi2() < 0.) return;
  Int_t nTmG = segGood->getNTimes(0);
  if(nTmG<5) return;
  Int_t nTmF = segFake->getNTimes(0);
  if(nTmF <= nTmG || ((nTmF>=8 || nTmF == nTmG+1) && segGood->getSumWires()-nTmG < nTmG-2)) {
    if(segGood->resetFakeContributionFlag()) segFake->setFakeContributionFlag();
  }
}
  
HMdcSeg* HMdc12Fit::getInnerSegment(Int_t clIndPar) const {
  HMdcClus *clusPar = clIndPar >= 0 ? (HMdcClus*)fClusCat->getObject(clIndPar) : NULL;
  if(clusPar == NULL) return NULL;           // Should happens never.
  Int_t segIndPar = clusPar->getSegIndex();  // inner segment
  return segIndPar >= 0 ? (HMdcSeg*)fSegCat->getObject(segIndPar) : NULL;
}

Int_t HMdc12Fit::getInnerClusInd(HMdcSeg* outerSegment,HMdcClus* &outerClus) const {
  Int_t clInd = outerSegment->getClusIndex();
  outerClus = clInd >= 0 ? (HMdcClus*)fClusCat->getObject(clInd) : NULL;
  if(outerClus == NULL) return -1;           // Should happens never.
  return outerClus->getIndexParent();
}

void HMdc12Fit::setFakeFlageAndIndexis(HMdcSeg* segOFake,Int_t indFakeSeg,HMdcSeg* segOGood,Int_t indGoodSeg) {
  if( segOFake->isFake() ) return;
//---  segOFake->setFakeFlag();  // setFake(...) ???????????????? nado peredelat'

  HMdcClus *clusOFake = NULL;
  Int_t indClusIFake = getInnerClusInd(segOFake,clusOFake);
  HMdcClus *clusGood = NULL;
  Int_t indClusIGood = getInnerClusInd(segOGood,clusGood);
  if(indClusIFake == indClusIGood || indClusIFake<0 || indClusIGood<0) return;
  
  HMdcSeg  *segIFake  = getInnerSegment(indClusIFake);

  clusOFake->setSegIndex(indGoodSeg);  // In the clusOFake set index of the segGood

  if(indFirstTrCand >= 0) for(Int_t itr=indFirstTrCand; itr<=indLastTrCand; itr++) {
    HMdcTrkCand* trkCand = (HMdcTrkCand*)fTrkCandCat->getObject(itr);
    if(indFakeSeg == trkCand->getSeg2Ind()) trkCand->setSeg2Ind(indGoodSeg);
  }
 
  if(segIFake->isFake() || segIFake->getChi2() < 0.) return;

//????????       if(isGeant) testGeantInf((HMdcSegSim*)seg2,(HMdcSegSim*)seg1); // must be before call setFake
  
  HMdcSeg  *segIGood = getInnerSegment(indClusIGood);
  
  // If inner segment of segOGood is fake set in segOGood index of cluster from segOFake:
  if(segIGood->isFake() || segIGood->getChi2() < 0.) segOGood->setClusIndex(segOFake->getClusIndex());
  
// No IOSegMatch in segment!
//   if(isGeant) {
//     // Check IO matching:
//     HMdcSegSim* segOGoodS = (HMdcSegSim*)segOGood;
//     HMdcSegSim* segIFakeS = (HMdcSegSim*)segIFake;
//     if(segOGoodS->getNNotFakeTracks() <= 0) return;
//     if(segIFakeS->getNNotFakeTracks() <= 0) return;
//     if(segOGoodS->getTrack(0) == segIFakeS->getTrack(0)) {
//       segOGoodS->setIOSegMatchBit(0);
//       segIFakeS->setIOSegMatchBit(0);
//     } else segIFakeS->resetIOSegMatchBit(0);
//   }
}

void HMdc12Fit::addToNFittedCounter(void) { 
  for(Int_t ind=indFirst;ind<=indLast;ind++) {  // Loop of MdcSeg in ONE sector
    HMdcSeg* pSeg = (HMdcSeg*)fSegCat->getObject(ind);
    if( pSeg->getIOSeg() != currSeg ) continue;
    if( pSeg->isFake() ) pSeg->SetBit(kKeep);  // Don't change fake flag in the next fake suppresion proc.
    else if( pSeg->getChi2() >= 0. ) {
      if( pSeg->TestBit(kInFittedList) ) continue;
      pSeg->ResetBit(kRealKeep);
      pSecListCells->addToNFittedCounters(currSeg,pSeg);
      pSeg->SetBit(kInFittedList);
    }
  }
}

Float_t HMdc12Fit::calcWeight(HMdcSeg* pSeg, Float_t *cut) const {
  Int_t nWrTot   = 0;
  Int_t nWrForWt = 0;
  Int_t mod      = currSeg*2;
  Int_t cells[4];
  Int_t ns = pSeg->TestBit(kInFittedList) ? 1 : 0;
  for(Int_t layInd=0;layInd<12;layInd++) {
    if(layInd == 6) mod++;
    Int_t nCells = pSeg->getLayerListCells(layInd,cells);
    nWrTot += nCells;
    const HMdcLayListCells &layList = (*pSecListCells)[mod][layInd%6];
    for(Int_t i=0;i<nCells;i++) if(layList.getNFitted(cells[i]) > ns) {
      nWrForWt++;                                       // Shared wire
      if(layList.getNRFitted(cells[i]) > 0) nWrForWt++; // Wire in real segment (nSegs>1 !)
    }
  }
//  Int_t level = nWrTot <= 12 ? nWrTot-5 : 7; //  8 => 12 wires in segment
  Int_t level = nWrTot-5;
  if     (level<0) level = 0;
  else if(level>7) level = 7; //  7 => 12 wires in segment
  return Float_t(nWrForWt)/Float_t(nWrTot)/cut[level];
}

Int_t HMdc12Fit::markFakes(Int_t cutNMarked) {
  // Mark segments as "fake" by number of shared wires  and wires in real
  Float_t *cut = cutForFake[currSeg];
  for(Int_t n=0;n<cutNMarked;n++) {
    Float_t  maxWt   = 0.;
    Int_t    nFkCand = 0;                      // For time optimization
    HMdcSeg* pSegMax = NULL;
    for(Int_t ind=indFirst;ind<=indLast;ind++) {  // Loop of MdcSeg in ONE sector
      HMdcSeg* pSeg = (HMdcSeg*)fSegCat->getObject(ind);
      if( pSeg->getIOSeg() != currSeg ) continue;
      if( pSeg->TestBit(kAllTypes) )    continue;  // continue if kFake.or.kReal.or.kKeep
      Float_t wt = calcWeight(pSeg,cut);
      if( wt <= 1. ) continue;
//       if(wt == 0.) {
//         pSeg->SetBit(kKeep);
//         continue;
//       }
      if( pSeg->getChi2() < 0.) pSeg->SetBit(kFake);
      else {
        nFkCand++;
        if(wt <= maxWt) continue;
        maxWt   = wt;
        pSegMax = pSeg;
      }
    }
    if(pSegMax != NULL) {
      if( pSegMax->TestBit(kReal) ) {
        pSecListCells->subFittedAndRealCounters(currSeg,pSegMax);
        pSegMax->ResetBit(kReal);
      } else pSecListCells->subFittedCounters(currSeg,pSegMax);
      pSegMax->ResetBit(kInFittedList);
      pSegMax->SetBit(kFake);
      if(nFkCand == 1) return n+1;             // Time optimization
    } else             return n;
  }
  return cutNMarked;
}

Int_t HMdc12Fit::markReals(Int_t cutNMarked) {
  // Mark segments as "real" by number of shared wires and wires in real
  Float_t *cut = cutForReal[currSeg];
  for(Int_t n=0;n<cutNMarked;n++) {
    Float_t  minWt   = 10.;
    Int_t    nRlCand = 0;                      // For time optimization
    HMdcSeg* pSegMin = NULL;
    for(Int_t ind=indFirst;ind<=indLast;ind++) {  // Loop of MdcSeg in ONE sector
      HMdcSeg* pSeg = (HMdcSeg*)fSegCat->getObject(ind);
      if( pSeg->getIOSeg() != currSeg ) continue;
      if( pSeg->getChi2() < 0. )        continue;
      if( pSeg->TestBit(kRealKeep) )    continue;  // continue if kReal.or.kKeep
      Float_t wt = calcWeight(pSeg,cut);
      if(wt == 0.) {
        pSeg->SetBit(kKeep);
        continue;
      }
      if( wt >= 1. ) continue;
      nRlCand++;
      if(wt >= minWt) continue;
      minWt   = wt;
      pSegMin = pSeg;
    }
    if(pSegMin != NULL) {
      if( pSegMin->TestBit(kFake) ) {
        pSecListCells->addToFittedAndRealCounters(currSeg,pSegMin);
        pSegMin->SetBit(kInFittedList);
        pSegMin->ResetBit(kFake);
      } else pSecListCells->addToNRFittedCounters(currSeg,pSegMin);
      pSegMin->SetBit(kReal);
      if(nRlCand == 1) return n+1;             // Time optimization
    } else             return n;
  }
  return cutNMarked;
}
