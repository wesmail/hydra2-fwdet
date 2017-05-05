//Task for all Matchings, fills hmetamatch
//*-- Author : Anar Rustamov (27.08.2003)
//*-- Modified : 14/10/2004 by V. Pechenov
//*-- Modified : 07/12/2007 by A. Rustamov
//*-- Modified : 16/12/2009 by V. Pechenov

using namespace std;

#include "hmetamatchF2.h"
#include "hmetamatch2.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hmdcdetector.h"
#include "hspecgeompar.h"
#include "tofdef.h"
#include "richdef.h"
#include "rpcdef.h"
#include "emcdef.h"
#include "hmatrixcategory.h"
#include "hshowergeometry.h"
#include "hgeomtransform.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "hmdcseg.h"
#include "hmdctrkcand.h"
#include "hmdctrackgspline.h"
#include "hmdctrackgcorrpar.h"
#include "hshowerhitsim.h"
#include "hemccal.h"
#include "hemccluster.h"
#include "hmdcgetcontainers.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "htofgeompar.h"
#include "hrpcgeompar.h"
#include "hemcgeompar.h"
#include "hgeomcompositevolume.h"
#include "hrichhit.h"
#include "hmetamatchpar.h"
#include "hmdcsizescells.h"
#include "hrpccluster.h"
#include <iostream>

ClassImp(HMetaMatchF2)

Float_t HMetaMatchF2::scaleRpcRMS   [2] = {1.0,1.0};
Float_t HMetaMatchF2::scaleShowerRMS[2] = {1.0,1.0};
Float_t HMetaMatchF2::scaleTofRMS   [2] = {1.0,1.0};
Float_t HMetaMatchF2::scaleEmcRMS   [2] = {1.0,1.0};
Float_t HMetaMatchF2::scaleRpcCut   =1.0;
Float_t HMetaMatchF2::scaleShowerCut=1.0;
Float_t HMetaMatchF2::scaleTofCut   =1.0;
Float_t HMetaMatchF2::scaleEmcCut   =1.0;

HMetaMatchF2::HMetaMatchF2() {
  setInitParam();
}

HMetaMatchF2::HMetaMatchF2(const Text_t *name,const Text_t *title):
  HReconstructor(name,title) {
  setInitParam();
}

void HMetaMatchF2::setInitParam(void) {
  fMatchPar      = NULL;
  fCatTrkCand    = NULL;
  fCatMdcSeg     = NULL;
  fCatRich       = NULL;
  pSpline        = NULL;
  fTrkCandIter   = NULL;
  fTofGeometry   = NULL;
  fCatTof        = NULL;
  iterTof        = NULL;
  fCatTofCluster = NULL;
  iterTofCluster = NULL;
  iterRich       = NULL;
  fTrkCandIter   = NULL;
  fCatShower     = NULL;
  iterShower     = NULL;
  fCatEmc        = NULL;
  fCatEmcCluster = NULL;
  fShrGeometry   = NULL;
  fRpcGeometry   = NULL;
  fEmcGeometry   = NULL;
  fCatRpcCluster = NULL;
  iterRpcCluster = NULL;
  stNotMatTracks = kFALSE;
  sigmaEmc       = 0.;
  
  
  for(Int_t s=0;s<6;s++) labTrans[s]  = NULL;
  for(Int_t i=0;i<9;i++) qual2TofC[i] = 0.f;
  for(Int_t i=0;i<3;i++) indTofC[i]   = -1;
}

HMetaMatchF2::~HMetaMatchF2() {
     HMdcSizesCells::deleteCont();
   //HMdcGetContainers::deleteCont();

  if(fTrkCandIter) {
     delete fTrkCandIter;
     fTrkCandIter=0;
  }
  if(iterRich){
    delete iterRich;
    iterRich=0;
  }
  if(iterTof) {
    delete iterTof;
    iterTof=0;
  }
  if(iterTofCluster) {
    delete iterTofCluster;
    iterTofCluster=0;
  }
  if(iterShower) {
    delete iterShower;
    iterShower=0;
  }
}

Bool_t HMetaMatchF2::init() {
  if (!gHades) return kFALSE;
  HRuntimeDb *rtdb = gHades->getRuntimeDb();
  if(!rtdb) return kFALSE;
  HEvent *event = gHades->getCurrentEvent();
  if(!event) return kFALSE;
  
  HMdcTrackGCorrPar* corr=(HMdcTrackGCorrPar*)(rtdb->getContainer("MdcTrackGCorrPar"));
  if(corr){
      pSpline = corr->getSPline();
  } else {
      Error("init()","ZERO pointer for MdcTrackGCorrPar received!");
      return kFALSE;
  }
  fGetCont    = HMdcGetContainers::getObject();
  pSizesCells = HMdcSizesCells::getObject();
  fGetCont->getMdcGeomPar();  // for spline ???
  fGetCont->getSpecGeomPar(); // for lab-sec. transf
  fCatTrkCand=event->getCategory(catMdcTrkCand);
  if (!fCatTrkCand) {
     Error("init","NO catMdcTrkCand in input! STOP!!!");
     return kFALSE;
  }
  fTrkCandIter=(HIterator*)fCatTrkCand->MakeIterator();
  if(!fTrkCandIter) return kFALSE;
  fCatMdcSeg=event->getCategory(catMdcSeg);

  // TOF:
  fCatTof=event->getCategory(catTofHit);
  if(!fCatTof) Warning("init",
      "No catTofHit in input! \n Matching with TofHits will be skipped!");
  else iterTof=(HIterator*)fCatTof->MakeIterator();
  fCatTofCluster=event->getCategory(catTofCluster);
  if(!fCatTofCluster) {
    Warning("init","NO catTofCluster in input! \n Matching with TofClusters will be skipped!");
  } else iterTofCluster=(HIterator*)fCatTofCluster->MakeIterator();
  if(fCatTof || fCatTofCluster) fTofGeometry=
      (HTofGeomPar *)rtdb->getContainer("TofGeomPar");

  // Shower:
  fCatShower = event->getCategory(catShowerHit);
  if(fCatShower == NULL) {
    Warning("init","NO catShowerHit in input! \n Matching with Shower will be skipped!");
  } else {
    iterShower   = (HIterator*)fCatShower->MakeIterator("native");
    fShrGeometry = (HShowerGeometry*)rtdb->getContainer("ShowerGeometry");
  }
  
  // Emc:
  if(fCatShower == NULL) {
    fCatEmc        = event->getCategory(catEmcCal);
    fCatEmcCluster = event->getCategory(catEmcCluster);
    if(fCatEmc == NULL)        Warning("init","No catEmcCal in input! \n Matching with EmcCal will be skipped!");
    if(fCatEmcCluster == NULL) Warning("init","NO catEmcCluster in input! \n Matching with EmcCluster will be skipped!");
    if(fCatEmc!=NULL || fCatEmcCluster!=NULL) fEmcGeometry = (HEmcGeomPar *)rtdb->getContainer("EmcGeomPar");
  }
  
 
  // RPC:
  fCatRpcCluster = event->getCategory(catRpcCluster);
  if(!fCatRpcCluster) {
    Warning("init","NO catRpcCluster in input! \n Matching with RpcClusters will be skipped!");
  } else {
    iterRpcCluster = (HIterator*)fCatRpcCluster->MakeIterator("native");
    fRpcGeometry   = (HRpcGeomPar*)rtdb->getContainer("RpcGeomPar");
  }
  
  // RICH:
  fCatRich=event->getCategory(catRichHit);
  if(fCatRich) iterRich=(HIterator*)fCatRich->MakeIterator("native");
  else Warning("init","NO RICH catRichHit in input! \n Matching with Rich will be skipped!");

  fCatMetaMatch=event->getCategory(catMetaMatch);
  if(!fCatMetaMatch) {
    Int_t size[2]={6,8000};
    fCatMetaMatch=new HMatrixCategory("HMetaMatch2",2,size,0.5);
    if(fCatMetaMatch)
       event->addCategory(catMetaMatch,fCatMetaMatch,"Tracks");
  }
  fMatchPar=(HMetaMatchPar*)rtdb->getContainer("MetaMatchPar");

  return kTRUE;
}

Bool_t HMetaMatchF2::reinit() {
  if(!pSizesCells->initContainer()) return kFALSE;
  if (pSizesCells->hasChanged()) {
     // Geometry transformation from module to sector coord system for MDCs
     for(Int_t is=0; is<6; is++) {
	HMdcSizesCellsSec& fSCSec = (*pSizesCells)[is];
	if(&fSCSec == 0) continue; // sector is not active
	for(Int_t im=0; im<4; im++) {
	   HMdcSizesCellsMod& fSCMod=fSCSec[im];
	   if (&fSCMod) {
	      const HGeomTransform* tr = fSCMod.getSecTrans();
	      pSpline->takeMiddleParams(tr,is,im);
	   }
	}
     }
  }
  pSpline->initMiddleParamsAll();
  pSpline->setKickPointer(&kickplane);
  for(Int_t s=0; s<6; s++) {
     if(fGetCont->getMdcDetector()->isSectorActive(s)) {
	labTrans[s] = &(fGetCont->getLabTransSec(s));
	if(labTrans[s]==0) return kFALSE;
     } else labTrans[s]=0;
  }
  if(fTofGeometry && !HMdcGetContainers::isInited(fTofGeometry)) return kFALSE;
  if(fShrGeometry && !HMdcGetContainers::isInited(fShrGeometry)) return kFALSE;
  if(fRpcGeometry && !HMdcGetContainers::isInited(fRpcGeometry)) return kFALSE;
  if(fEmcGeometry && !HMdcGetContainers::isInited(fEmcGeometry)) return kFALSE;

  if(fMatchPar==0 || !HMdcGetContainers::isInited(fMatchPar)) {
    Error("reinit","no parameters for matching!");
    return kFALSE;
  }
  setMatchingParam();
  return kTRUE;
}

void HMetaMatchF2::setMatchingParam(void) {
  // Matching parameters.
  for(Int_t s=0;s<6;s++) {
    dThRich[s]            = fabs( fMatchPar->getRichThetaMaxCut(s) -
                                  fMatchPar->getRichThetaMinCut(s) )/2.;
    dPhRich[s]            = fMatchPar->getRichSigmaPhi(s);
    dPhRichOff[s]         = fMatchPar->getRichSigmaPhiOffset(s);
    qualityRichCut[s]     = fMatchPar->getRichQualityCut(s);

    sigma2MdcInRpcX[s]    = fMatchPar->getRpcSigmaXMdc(s)*fMatchPar->getRpcSigmaXMdc(s);
    sigma2MdcInRpcY[s]    = fMatchPar->getRpcSigmaYMdc(s)*fMatchPar->getRpcSigmaYMdc(s);
    sRpcX[s]              = fMatchPar->getRpcSigmaXOffset(s);
    sRpcY[s]              = fMatchPar->getRpcSigmaYOffset(s);
    quality2RPCCut[s]     = fMatchPar->getRpcQualityCut(s)*fMatchPar->getRpcQualityCut(s)*scaleRpcCut*scaleRpcCut;

    sigma2MdcInShrX[s]    = fMatchPar->getShowerSigmaXMdc(s)*fMatchPar->getShowerSigmaXMdc(s);
    sigma2MdcInShrY[s]    = fMatchPar->getShowerSigmaYMdc(s)*fMatchPar->getShowerSigmaYMdc(s);
    sShowerX[s]           = fMatchPar->getShowerSigmaXOffset(s);
    sShowerY[s]           = fMatchPar->getShowerSigmaYOffset(s);
    quality2SHOWERCut[s]  = fMatchPar->getShowerQualityCut(s)*fMatchPar->getShowerQualityCut(s)*scaleShowerCut*scaleShowerCut;

    invSigma2TofX[s]      = 1./( (fMatchPar->getTofSigmaX(s)*scaleTofRMS[0])*(fMatchPar->getTofSigmaX(s)*scaleTofRMS[0]));
    invSigma2TofY[s]      = 1./( (fMatchPar->getTofSigmaY(s)*scaleTofRMS[1])*(fMatchPar->getTofSigmaY(s)*scaleTofRMS[1]));
    sTofX[s]              = fMatchPar->getTofSigmaXOffset(s);
    sTofY[s]              = fMatchPar->getTofSigmaYOffset(s);
    quality2TOFCut[s]     = fMatchPar->getTofQualityCut(s)*fMatchPar->getTofQualityCut(s)*scaleTofCut*scaleTofCut;
    richThetaMinCut[s]    = fMatchPar->getRichThetaMinCut(s);
    richThetaMaxCut[s]    = fMatchPar->getRichThetaMaxCut(s);
    
    sigma2MdcInEmcX[s]    = fMatchPar->getEmcSigmaXMdc(s)*fMatchPar->getEmcSigmaXMdc(s);
    sigma2MdcInEmcY[s]    = fMatchPar->getEmcSigmaYMdc(s)*fMatchPar->getEmcSigmaYMdc(s);
    sEmcX[s]              = fMatchPar->getEmcSigmaXOffset(s);
    sEmcY[s]              = fMatchPar->getEmcSigmaYOffset(s);
    quality2EMCCut[s]     = fMatchPar->getEmcQualityCut(s)*fMatchPar->getEmcQualityCut(s)*scaleEmcCut*scaleEmcCut;
    
    nShowerHits[s]        = 0;
    nEmcHits[s]           = 0;
    sigmaEmc              = 92./TMath::Sqrt(12.);   // 92 mm - size of one cell in X and Y
  }
}

Bool_t HMetaMatchF2::finalize() {
  return kTRUE;
}

Int_t HMetaMatchF2::execute() {
  if(fCatTrkCand->getEntries()<=0) return 0;

  collectRpcClusters();
  if(fCatShower != NULL) collectShowerHits();
  else {
    collectEmcHits();
    collectEmcClusters();
  }
  collectTofHits();

  fTrkCandIter->Reset();
  HMdcTrkCand *pTrkCand=0;
  for(Int_t sec=0;sec<6;sec++) {
    setCurrentSector(sec);
    if(!fTrkCandIter->gotoLocation(sectorLoc)) continue;
    while((pTrkCand=(HMdcTrkCand*)(fTrkCandIter->Next()))!=0) {
      if(pTrkCand->getNCandForSeg1()<0) continue; // This TrkCand was analysed in makeOuterSegMatch
      Int_t index1 = pTrkCand->getSeg1Ind();
      if(index1<0) continue;
      segments[0] = (HMdcSeg*)fCatMdcSeg->getObject(index1);
      if(segments[0] ==  NULL) continue;
      makeRichMatching();                 // MDC-RICH matching
      makeOuterSegMatch(pTrkCand);        //Outer segment matching       
    }
  }
  return 0;
}

void HMetaMatchF2::makeOuterSegMatch(HMdcTrkCand* pTrkCand) {
  // Matching outer mdc segments with meta hits.
  // Inner mdc segment is the same.
  Int_t firstTrkCandIndex = pTrkCand->getFirstCandInd();
  Bool_t isMMatch = kFALSE;
  isPrint = kTRUE;   // For Warning(...,"No slot available...
  Int_t trkCandIndex = -1;
  meta = 0;
  while((trkCandIndex = pTrkCand->getNextCandInd())>=0) {
    pTrkCand = (HMdcTrkCand*)fCatTrkCand->getObject(trkCandIndex);
    // MDC-Meta matching with outer mdc segment:
    meta = 0;
    Int_t index2=pTrkCand->getSeg2Ind();
    if(index2<0) continue;               // It can happen by error only.
    segments[1]=(HMdcSeg*)fCatMdcSeg->getObject(index2);
    if(!segments[1]) continue;           // It can happen by error only.

    if(segments[1]->getHitInd(1)<0)
    {
       pSpline->calcSegPoints123(segments,mdcTrackPar);
       }
       else if(segments[1]->getHitInd(0)<0)
       {
       pSpline->calcSegPoints123P4(segments,mdcTrackPar);
       }
    else
    {
       pSpline->calcSegPoints(segments,mdcTrackPar);
    }
    for(Int_t p=0;p<4;p++) mdcTrackPar[p] *= 10.; // cm => mm
    mdcTrackPar[2] = secLabTrans->transFrom(mdcTrackPar[2]);
    mdcTrackPar[3] = secLabTrans->transFrom(mdcTrackPar[3]);

    nTofMatched = 0;
    for(Int_t nTof=0; nTof<nTofHitsSec;nTof++) {
      Char_t clusSize = tofClustSizeSec[nTof];
      if(clusSize > 2) continue;  // Should never happend

      //Variables to store x/y-deviation of META hit and segment normalized to META resolution

      if(clusSize == 2)      quality2TofClustS2(nTof);
      else if(clusSize == 1) quality2TofClustS1(nTof);
      else                   quality2TofHit(nTof);
    }

    nRpcMatched = 0;
    if(fRpcGeometry && fRpcGeometry->getModule(sector,0)){
	transMdcToMeta(fRpcGeometry->getModule(sector,0)->getLabTransform());
	for(Int_t nRpc=0; nRpc<nRpcClustersSec;nRpc++) {
	    Float_t qual2 = quality2Rpc(fRpcClustersSec[nRpc]);
	    if(qual2>quality2RPCCut[sector]) continue;
	    insertQual(qual2,fRpcClustersSec[nRpc]->getIndex(),nRpcMatched,qual2RpcAr,rpcInd);
	}
    }
    
    nShrMatched = 0;
    if(fShrGeometry != NULL) {
      transMdcToMeta(fShrGeometry->getTransform(sector,0)); // Module 0 only!
      for(Int_t nSh=0; nSh<nShowerHitsSec;nSh++) {
        Float_t qual2 = quality2Shower(fShowerHitsSec[nSh]);
        if(qual2>quality2SHOWERCut[sector]) continue;
        insertQual(qual2,indexShrHitSec[nSh],nShrMatched,qual2ShrAr,shrInd);
      }
    }
    
    nEmcMatched     = 0;
    nEmcClusMatched = 0;
    if(fEmcGeometry != NULL) {
      HModGeomPar *pmodgeom = fEmcGeometry->getModule(sector);
      transMdcToMeta(pmodgeom->getLabTransform()); // Module 0 only!
      Double_t zp1    = p1SegInMod.getZ(); //-zEmc;  zEmc=0
      Double_t zp2    = p2SegInMod.getZ(); //-zEmc;  zEmc=0
      Double_t xSegCr = (zp1*p2SegInMod.getX()-p1SegInMod.getX()*zp2)*invDZ; // invDZ = 1./(zp1-zp2);
      Double_t ySegCr = (zp1*p2SegInMod.getY()-p1SegInMod.getY()*zp2)*invDZ;
      for(Int_t nEmc=0; nEmc<nEmcHitsSec;nEmc++) {
        Float_t qual2 = quality2Emc(fEmcHitsSec[nEmc],pmodgeom,xSegCr,ySegCr);
        if(qual2>quality2EMCCut[sector]) continue; //??????????????????????????????
        insertQual(qual2,indexEmcHitSec[nEmc],nEmcMatched,qual2EmcAr,emcInd);
        Short_t status = fEmcHitsSec[nEmc]->getStatus1();
        if(status >= 0) fEmcHitsSec[nEmc]->setStatus1(++status);
      }
      for(Int_t nEmc=0; nEmc<nEmcClusSec;nEmc++) {
        Float_t qual2 = quality2EmcClus(fEmcClusSec[nEmc],xSegCr,ySegCr);
        if(qual2>quality2EMCCut[sector]) continue; //??????????????????????????????
        insertQual(qual2,indexEmcClusSec[nEmc],nEmcClusMatched,qual2EmcClusAr,emcClusInd);
      }
    }

    if(nRpcMatched>0 || nShrMatched>0 || nTofMatched>0 || nEmcClusMatched>0) {  // Is matching:
	isMMatch = kTRUE;
      if(getMetaMatchSlot(trkCandIndex)) {
        if(nRpcMatched>0) meta->setRpcClstMMF(nRpcMatched,rpcInd,qual2RpcAr);
        if(nShrMatched>0) meta->setShrHitMMF(nShrMatched,shrInd,qual2ShrAr);
        else if(nEmcClusMatched>0) meta->setEmcClstMMF(nEmcClusMatched,emcClusInd,qual2EmcClusAr);
        if(nTofMatched>0) meta->setTofClstMMF(nTofMatched,tofInd,qual2TofAr);
      }
    } else if(stNotMatTracks) {                            // No matching:
	if(getMetaMatchSlot(trkCandIndex)) isMMatch = kTRUE;
    }
    if(meta){
	if(pTrkCand ) {
	    meta->setInnerFake(pTrkCand->isSeg1Fake());
	    meta->setOuterFake(pTrkCand->isSeg2Fake());
	}
    }


  }
  
  // No outer segment or no matching (meta!=0 if stNotMatTracks=kTRUE):
  if(!isMMatch && meta==0) {
      getMetaMatchSlot(firstTrkCandIndex);
      if(fCatTrkCand && meta) {
	  HMdcTrkCand* trk=(HMdcTrkCand*)fCatTrkCand->getObject(firstTrkCandIndex);
	  if(trk) {
	      meta->setInnerFake(trk->isSeg1Fake());
	      meta->setOuterFake(trk->isSeg2Fake());
	  }
      }
  }
}

void HMetaMatchF2::transMdcToMeta(const HGeomTransform& modSys) {
  p1SegInMod = modSys.transTo(mdcTrackPar[2]);
  p2SegInMod = modSys.transTo(mdcTrackPar[3]);
  invDZ      = 1./(p1SegInMod.Z()-p2SegInMod.Z());
}

void HMetaMatchF2::insertQual(Float_t qual,Short_t ind,UChar_t& nEl,Float_t qualArr[][3],Short_t indArr[]) {
  Int_t nt = nEl;
  if(nt == MMF_BUF) {
    nt--;
    if(qual>=qualArr[nt][0]) return;
  } else nEl++;    // Counter increasing (if nEl<MMF_BUF).
  if(nt>0) {
    for(Int_t ns=nt-1;nt>0;nt--,ns--) {
      if(qual>=qualArr[ns][0]) break;
      for(Int_t i=0;i<3;i++) qualArr[nt][i] = qualArr[ns][i];
      indArr[nt] = indArr[ns];
    }
  }
  indArr[nt]     = ind;
  qualArr[nt][0] = qual;
  qualArr[nt][1] = dX;
  qualArr[nt][2] = dY;
}

void HMetaMatchF2::insertQualTof(Float_t qual) {
  Int_t nt = nTofMatched;
  if(nt == MMF_BUF) {
    nt--;
    if(qual>=qual2TofBestAr[nt]) return;
  } else nTofMatched++;
  if(nt>0) for(Int_t ns=nt-1;nt>0;nt--,ns--) {
    if(qual>=qual2TofBestAr[ns]) break;
    qual2TofBestAr[nt] = qual2TofBestAr[ns];
    for(Int_t i=0;i<9;i++) qual2TofAr[nt][i] = qual2TofAr[ns][i];
    for(Int_t j=0;j<3;j++) tofInd[nt][j]     = tofInd[ns][j];
  }
  
  qual2TofBestAr[nt] = qual;
  for(Int_t i=0;i<9;i++) qual2TofAr[nt][i] = qual2TofC[i];
  for(Int_t j=0;j<3;j++) tofInd[nt][j]     = indTofC[j];
}

Bool_t HMetaMatchF2::getMetaMatchSlot(Int_t trCandInd) {
  Int_t metaIndex = -1;
  HMetaMatch2 *metaNew=(HMetaMatch2*)fCatMetaMatch->getNewSlot(sectorLoc,&metaIndex);
  if(metaNew == NULL) { 
    if(isPrint) {
      Warning("getMetaMatchSlot","No slot available in sector %i. size of catMetaMatch is %i!",
              sectorLoc[0]+1,fCatMetaMatch->getEntries());
      isPrint = kFALSE;
    }
    return kFALSE;
  }
  meta = new(metaNew) HMetaMatch2(sector,trCandInd,metaIndex);
  
  HMdcTrkCand* pTrkCand = (HMdcTrkCand*)fCatTrkCand->getObject(trCandInd);
  if(pTrkCand) pTrkCand->setMetaMatchInd(metaIndex);
  // Filling MDC-RICH matching result:
  if( nRichId > 0) {
    if(nRichId>3) nRichId=3;
    meta->setNCandForRich(nRichId);
    for(UChar_t i = 0; i < nRichId; i++) meta->setRichInd(i,richInd[i]);
  }
  return kTRUE;
}

Bool_t HMetaMatchF2::quality2TofHit(Int_t hit) {
  // Calc. quality for one cell HTofCluster or HTofHit
  HGeomTransform &tofModSys =
      fTofGeometry->getModule(sector,tofModuleSec[hit])->getLabTransform();
  HGeomVector p1 = tofModSys.transTo(mdcTrackPar[2]);
  HGeomVector p2 = tofModSys.transTo(mdcTrackPar[3]);
  
  Float_t xSeg  = (p1(2)*p2(0)-p1(0)*p2(2))/(p1(2)-p2(2));
  Float_t ySeg  = (p1(2)*p2(1)-p1(1)*p2(2))/(p1(2)-p2(2));
  Float_t dX    = tofHitsSec[hit].getX()-xSeg-sTofX[sector];
  Float_t dY    = tofHitsSec[hit].getY()-ySeg-sTofY[sector];
  Float_t qual2 = dX*dX*invSigma2TofX[sector] + dY*dY*invSigma2TofY[sector];

  if(qual2 < quality2TOFCut[sector]) { // Is matching:
    indTofC[1]   = indexTofHitSec[hit];  
    qual2TofC[3] = qual2;
    qual2TofC[4] = dX;
    qual2TofC[5] = dY;
    insertQualTof(qual2);
    
    // Clean arrays:
    for(Int_t i=3;i<6;i++) qual2TofC[i] = 0.f;
    indTofC[1] = -1;
    return kTRUE;
  }
  return kFALSE;         // No matching
}

Bool_t HMetaMatchF2::quality2TofClustS1(Int_t hit) {
  // Calc. quality for one cell HTofCluster object
  HGeomTransform &tofModSys =
      fTofGeometry->getModule(sector,tofModuleSec[hit])->getLabTransform();
  HGeomVector p1 = tofModSys.transTo(mdcTrackPar[2]);
  HGeomVector p2 = tofModSys.transTo(mdcTrackPar[3]);
  
  Float_t xSeg  = (p1(2)*p2(0)-p1(0)*p2(2))/(p1(2)-p2(2));
  Float_t ySeg  = (p1(2)*p2(1)-p1(1)*p2(2))/(p1(2)-p2(2));
  Float_t dX    = tofHitsSec[hit].getX()-xSeg-sTofX[sector];
  Float_t dY    = tofHitsSec[hit].getY()-ySeg-sTofY[sector];
  Float_t qual2 = dX*dX*invSigma2TofX[sector] + dY*dY*invSigma2TofY[sector];

  if(qual2 < quality2TOFCut[sector]) { // Is matching: 
    indTofC[0]   = indexTofHitSec[hit];
    qual2TofC[0] = qual2;
    qual2TofC[1] = dX;
    qual2TofC[2] = dY;
    insertQualTof(qual2);
    // Clean arrays:
    for(Int_t i=0;i<3;i++) qual2TofC[i] = 0.f;
    indTofC[0] = -1;
    return kTRUE;
  }
  return kFALSE;         // No matching
}

Bool_t HMetaMatchF2::quality2TofClustS2(Int_t& hit) {
  // Calc. quality for cluster (size=2) and two cells in from this cluster
  // Return best quality value for matching with cluster, cell1 and cell2
  Float_t bestQual2 = -1.f;
  Char_t clusMod = tofModuleSec[hit];
  HGeomTransform &tofModSys = fTofGeometry->getModule(sector,clusMod)->getLabTransform();
  HGeomVector p1  = tofModSys.transTo(mdcTrackPar[2]);
  HGeomVector p2  = tofModSys.transTo(mdcTrackPar[3]);
  Float_t xSeg  = (p1(2)*p2(0)-p1(0)*p2(2))/(p1(2)-p2(2));
  Float_t ySeg  = (p1(2)*p2(1)-p1(1)*p2(2))/(p1(2)-p2(2));
  Float_t dX    = tofHitsSec[hit].getX()-xSeg-sTofX[sector];
  Float_t dY    = tofHitsSec[hit].getY()-ySeg-sTofY[sector];
  Float_t qual2 = dX*dX*invSigma2TofX[sector] + dY*dY*invSigma2TofY[sector];
  if(qual2 < quality2TOFCut[sector]) {
    indTofC[0]   = indexTofHitSec[hit];
    qual2TofC[0] = qual2;
    qual2TofC[1] = dX;
    qual2TofC[2] = dY;
    bestQual2    = qual2;
  }
  
  // First cell in cluster (cluster module == firts cell module):
  hit++;
  dX    = tofHitsSec[hit].getX()-xSeg-sTofX[sector];
  dY    = tofHitsSec[hit].getY()-ySeg-sTofY[sector];
  qual2 = dX*dX*invSigma2TofX[sector] + dY*dY*invSigma2TofY[sector];
  if(qual2 < quality2TOFCut[sector]) {
    indTofC[1]   = indexTofHitSec[hit];
    qual2TofC[3] = qual2;
    qual2TofC[4] = dX;
    qual2TofC[5] = dY;
    if(bestQual2<0. || bestQual2>qual2) bestQual2 = qual2;
  }
  
  // Secont cell in cluster (cluster module can be != firts cell module):
  hit++;
  if(clusMod != tofModuleSec[hit]) {
    HGeomTransform &tofModSys =
        fTofGeometry->getModule(sector,tofModuleSec[hit])->getLabTransform();
    p1     = tofModSys.transTo(mdcTrackPar[2]);
    p2     = tofModSys.transTo(mdcTrackPar[3]);
    xSeg = (p1(2)*p2(0)-p1(0)*p2(2))/(p1(2)-p2(2));
    ySeg = (p1(2)*p2(1)-p1(1)*p2(2))/(p1(2)-p2(2)); 
  }
  dX    = tofHitsSec[hit].getX()-xSeg-sTofX[sector];
  dY    = tofHitsSec[hit].getY()-ySeg-sTofY[sector];
  qual2 = dX*dX*invSigma2TofX[sector] + dY*dY*invSigma2TofY[sector];
  if(qual2 < quality2TOFCut[sector]) {
    indTofC[2]   = indexTofHitSec[hit]; 
    qual2TofC[6] = qual2;
    qual2TofC[7] = dX;
    qual2TofC[8] = dY;
    if(bestQual2<0.f || bestQual2>qual2) bestQual2 = qual2;
  }
  if(bestQual2>=0.f) {  // Is matching:
    insertQualTof(bestQual2);
    // Clean arrays:
    for(Int_t i=0;i<9;i++) qual2TofC[i] = 0.f;
    for(Int_t i=0;i<3;i++) indTofC[i]   = -1;
    return kTRUE;
  }
  return kFALSE;         // No matching
}

Float_t HMetaMatchF2::quality2Rpc(HRpcCluster* pRpcCl)  {
  Double_t zRpc   = pRpcCl->getZMod();      // Zmod can be not eq.0 !
  Double_t zp1    = p1SegInMod.getZ()-zRpc;
  Double_t zp2    = p2SegInMod.getZ()-zRpc;
  Float_t  xSegCr = (zp1*p2SegInMod.getX()-p1SegInMod.getX()*zp2)*invDZ; // invDZ = 1./(zp1-zp2);
  Float_t  ySegCr = (zp1*p2SegInMod.getY()-p1SegInMod.getY()*zp2)*invDZ;
  dX              = pRpcCl->getXMod() - xSegCr - sRpcX[sector];
  dY              = pRpcCl->getYMod() - ySegCr - sRpcY[sector];
  Float_t  dXrms2 = pRpcCl->getXRMS() * scaleRpcRMS[0];
  dXrms2          = dXrms2*dXrms2 + sigma2MdcInRpcX[sector];
  Float_t  dYrms2 = pRpcCl->getYRMS() * scaleRpcRMS[1];
  dYrms2          = dYrms2*dYrms2 + sigma2MdcInRpcY[sector];
  return dX*dX/dXrms2 + dY*dY/dYrms2;
}

Float_t HMetaMatchF2::quality2Shower(HShowerHit* pShrHit)  {
  Float_t xShr, yShr; 
  pShrHit->getXY(&xShr, &yShr);
  Double_t zShr   = pShrHit->getZ(); // Zmod can be not eq.0 !
  Double_t zp1    = p1SegInMod.getZ()-zShr;
  Double_t zp2    = p2SegInMod.getZ()-zShr;
  Float_t  xSegCr = (zp1*p2SegInMod.getX()-p1SegInMod.getX()*zp2)*invDZ; // invDZ = 1./(zp1-zp2);
  Float_t  ySegCr = (zp1*p2SegInMod.getY()-p1SegInMod.getY()*zp2)*invDZ; 
  dX              = xShr-xSegCr-sShowerX[sector];
  dY              = yShr-ySegCr-sShowerY[sector];
  Float_t  dXsig2 = pShrHit->getSigmaX() * scaleShowerRMS[0];
  dXsig2          = dXsig2*dXsig2 + sigma2MdcInShrX[sector];
  Float_t  dYsig2 = pShrHit->getSigmaY() * scaleShowerRMS[1];
  dYsig2          = dYsig2*dYsig2 + sigma2MdcInShrY[sector];
  return dX*dX/dXsig2 + dY*dY/dYsig2;
}

Float_t HMetaMatchF2::quality2Emc(HEmcCal* pEmcHit,HModGeomPar *pmodgeom,Double_t xSegCr,Double_t ySegCr)  {
  HGeomVolume* fVol = pmodgeom->getRefVolume()->getComponent(pEmcHit->getCell());
  const HGeomVector& pos  = fVol->getTransform().getTransVector();
  Double_t xEmc   = pos.getX();
  Double_t yEmc   = pos.getY();
  dX              = xEmc-xSegCr-sEmcX[sector];
  dY              = yEmc-ySegCr-sEmcY[sector];
  Float_t  dXsig2 = sigmaEmc * scaleEmcRMS[0];
  dXsig2          = dXsig2*dXsig2 + sigma2MdcInEmcX[sector];
  Float_t  dYsig2 = sigmaEmc * scaleShowerRMS[1];
  dYsig2          = dYsig2*dYsig2 + sigma2MdcInEmcY[sector];
  return dX*dX/dXsig2 + dY*dY/dYsig2;
}

Float_t HMetaMatchF2::quality2EmcClus(HEmcCluster* pEmcClus,Double_t xSegCr,Double_t ySegCr)  {
  Double_t xEmc   = pEmcClus->getXMod();
  Double_t yEmc   = pEmcClus->getYMod();
  dX              = xEmc-xSegCr-sEmcX[sector];
  dY              = yEmc-ySegCr-sEmcY[sector];
  Float_t  dXsig2 = sigmaEmc * scaleEmcRMS[0];
  dXsig2          = pEmcClus->getSigmaXMod()*pEmcClus->getSigmaXMod() + sigma2MdcInEmcX[sector];
  Float_t  dYsig2 = sigmaEmc * scaleShowerRMS[1];
  dYsig2          = pEmcClus->getSigmaYMod()*pEmcClus->getSigmaYMod() + sigma2MdcInEmcY[sector];
  return dX*dX/dXsig2 + dY*dY/dYsig2;
}

void HMetaMatchF2::collectTofHits(void) {
  if(fCatTof) iterTof->Reset();
  HTofHit *pTofHit;
  for(Int_t s=0;s<6;s++) nTofHits[s]=0;
  if(!fCatTofCluster) {
    // No TofCluster category:
    if(fCatTof) while((pTofHit=(HTofHit*)(iterTof->Next()))!=0 ) addTofHit(pTofHit,0);
  } else {
    // TofCluster category exist:
    iterTofCluster->Reset();
    HTofCluster *pTofCluster;
    while((pTofCluster=(HTofCluster*)(iterTofCluster->Next()))!=0 ) {
      Int_t tofClSize = pTofCluster->getClusterSize();
      if(tofClSize>2) continue;                  // tofClSize<=2 only !
      addTofCluster(pTofCluster);
      if(tofClSize<2) continue;
      if(fCatTof==0) continue;
      Int_t sec  = pTofCluster->getSector();
      Int_t mod  = pTofCluster->getModule();
      Int_t cell = pTofCluster->getCell();
      while((pTofHit=(HTofHit*)(iterTof->Next()))!=0 ) {
        if(sec!=pTofHit->getSector() || mod!=pTofHit->getModule() ||
                                        cell!=pTofHit->getCell()) continue;
        if(tofClSize==2) {  // two TofHits adding, tofClSize==2 only !
          addTofHit(pTofHit,0);                  // first TofHit for TofCluster
          pTofHit = (HTofHit*)(iterTof->Next());   // second ...
          addTofHit(pTofHit,0);                  // clust.size==2 selected !!!
        } else {            // tofClSize>2 only HTofHits can be used
          addTofHit(pTofHit,-1);
          for(Int_t h=0;h<tofClSize-1;h++) {
            pTofHit=(HTofHit*)(iterTof->Next());
            addTofHit(pTofHit,-1);
          }
        }
        break;
      }
    }
  }
}

void HMetaMatchF2::addTofCluster(HTofCluster* pTofCluster) {
  addTof(pTofCluster,fCatTofCluster->getIndex(pTofCluster),
      pTofCluster->getClusterSize());
}

void HMetaMatchF2::addTofHit(HTofHit* pTofHit,Int_t clSize) {
  if(pTofHit==0) Error("addTofHit"," Pointer to HTofHit == 0 !");
  else addTof(pTofHit,fCatTof->getIndex(pTofHit),clSize);
}

void HMetaMatchF2::addTof(HTofHit* pTofHit,Int_t index, Int_t clSize) {
  Int_t sec = pTofHit->getSector();
  Int_t &nTofHSec = nTofHits[sec];
  if(nTofHSec>=100) {
    Error("addTof","Sector %i. Number of HTofCluster+HTofHit objects >100 !!!",sec+1);
    return;
  }
  Float_t Xtof,Ytof,Ztof;
  pTofHit->getXYZLab(Xtof,Ytof,Ztof);
  HGeomVector& point = tofHits[sec][nTofHSec];
  point.setXYZ(Xtof,Ytof,Ztof);
  HModGeomPar *module=fTofGeometry->getModule(sec,pTofHit->getModule());
  if(module==0) {
    Error("addTof"," Can't get transformation for tof. %i sec. %imod",
        sec,pTofHit->getModule());
    return;
  }
  HGeomTransform &trans = module->getLabTransform();
  point = trans.transTo(point);    // transformation to coor.sys. of tof module

  indexTofHit[sec][nTofHSec]  = index;
  tofClustSize[sec][nTofHSec] = clSize;
  tofModule[sec][nTofHSec]    = pTofHit->getModule();
  if(nTofHSec<100) nTofHSec++;
}

void HMetaMatchF2::collectRpcClusters(void) {
  for(Int_t s=0;s<6;s++) nRpcClusters[s] = 0;
  if(!fCatRpcCluster) return;
  iterRpcCluster->Reset();
  HRpcCluster *pRpcCluster;
  while((pRpcCluster = (HRpcCluster*)(iterRpcCluster->Next()))!=0) {
    Int_t sec = pRpcCluster->getSector();
    if(nRpcClusters[sec]>=200) {
      Error("collectRpcClusters","Sector %i. Number of HRpcCluster objects >200 !!!",sec+1);
      continue;
    }
    fRpcClusters[sec][nRpcClusters[sec]] = pRpcCluster;
    nRpcClusters[sec]++;
  }
}

void HMetaMatchF2::collectShowerHits(void) {
  for(Int_t s=0;s<6;s++) nShowerHits[s]=0;
  if(fCatShower == NULL) return;
  iterShower->Reset();
  HShowerHit *pShowerHit;
  while((pShowerHit = (HShowerHit*)(iterShower->Next()))!=0) {
    if(pShowerHit->getModule() != 0) continue;     // Module 0 only !!!
    Int_t sec = pShowerHit->getSector();
    if(nShowerHits[sec]>=200) {
      Error("collectShowerHits","Sector %i. Number of HShowerHit objects >200 !!!",sec+1);
      continue;
    }
    fShowerHits[sec][nShowerHits[sec]] = pShowerHit;
    indexShrHit[sec][nShowerHits[sec]] = fCatShower->getIndex(pShowerHit);
    nShowerHits[sec]++;
  }
}

void HMetaMatchF2::collectEmcHits(void) {
  for(Int_t s=0;s<6;s++) nEmcHits[s]=0;
  if(fCatEmc == NULL) return;
  Int_t ncals = fCatEmc->getEntries();
  for(Int_t ind=0;ind<ncals;ind++) {
    HEmcCal *pEmcCal = (HEmcCal*)fCatEmc->getObject(ind);
    if(pEmcCal->getStatus1() < 0) continue;              // Energy < energyCut for sim.data
    Int_t sec = pEmcCal->getSector();
    fEmcHits[sec][nEmcHits[sec]]        = pEmcCal;
    indexEmcHit[sec][nEmcHits[sec]]     = ind;
    nEmcHits[sec]++;
  }
}

void HMetaMatchF2::collectEmcClusters(void) {
  for(Int_t s=0;s<6;s++) nEmcClusters[s]=0;
  if(fCatEmcCluster == NULL) return;
  Int_t nclus = fCatEmcCluster->getEntries();
  for(Int_t ind=0;ind<nclus;ind++) {
    HEmcCluster *pEmcCluster = (HEmcCluster*)fCatEmcCluster->getObject(ind);
    if( !pEmcCluster->ifActive() ) continue;              // Bad cluster
    Int_t sec = pEmcCluster->getSector();
    fEmcCluster[sec][nEmcClusters[sec]]     = pEmcCluster;
    indexEmcCluster[sec][nEmcClusters[sec]] = ind;
    nEmcClusters[sec]++;
  }
}

void HMetaMatchF2::makeRichMatching(void) {
  
  Float_t mdcPhi    = segments[0]->getPhi()*TMath::RadToDeg() + (sector!=5 ? sector*60.:-60.);
  Float_t mdcTheta  = segments[0]->getTheta()*TMath::RadToDeg();
  Float_t sinMTheta = TMath::Sin(segments[0]->getTheta());
  
  // Matching with HRichHit's:
  nRichId = 0;
  if(fCatRich) {
    HRichHit* pRichHit;
    iterRich->Reset();
    while((pRichHit=(HRichHit*)(iterRich->Next()))!=0) {
      if(pRichHit->getSector() != sector) continue;
      Float_t dTheta  = pRichHit->getTheta()-mdcTheta;
      if(dTheta<richThetaMinCut[sector] || dTheta>richThetaMaxCut[sector]) continue;
      Float_t qualPhi = pRichHit->getPhi() - mdcPhi - dPhRichOff[sector];
      qualPhi = TMath::Abs(qualPhi *sinMTheta/dPhRich[sector]);
      if(qualPhi>qualityRichCut[sector]) continue;
      Short_t ind=fCatRich->getIndex(pRichHit);
      addRing(qualPhi,ind,richInd, nRichId);
    }
  }
}

void HMetaMatchF2::addRing(Float_t quality, Short_t ind,Short_t* indTable,UChar_t& nRich) {
  // Adding matched ring in sorted list
  if(nRich==0 || quality>=qualRich[nRich-1]) {
    // if ring is first in the list or ring must be added to the end of list:
    if(nRich>=RICH_BUF) return;
    indTable[nRich] = ind;
    qualRich[nRich] = quality;
    nRich++;
  } else {
    // if ring must be inserted in list:
    for(Int_t i=0;i<nRich;i++) {
      if(quality>=qualRich[i]) continue;
      if(nRich<RICH_BUF) nRich++;
      for(Int_t ish=nRich-1;ish>i;ish--) {  // Shift of rings in list
        indTable[ish]=indTable[ish-1];      // ...
        qualRich[ish]=qualRich[ish-1];      // ...
      }
      indTable[i]=ind;                      // Inserting ring in list
      qualRich[i]=quality;                  // ...
      return;
    }
  }
}

void HMetaMatchF2::setCurrentSector(Int_t sec) {
  sector          = sec;
  sectorLoc.set(1,sector);
  secLabTrans     = labTrans[sector];

  nRpcClustersSec = nRpcClusters[sector];
  fRpcClustersSec = fRpcClusters[sector];
  
  nShowerHitsSec  = nShowerHits[sector];
  indexShrHitSec  = indexShrHit[sector];
  fShowerHitsSec  = fShowerHits[sector];
  
  nEmcHitsSec     = nEmcHits[sector];
  indexEmcHitSec  = indexEmcHit[sector];
  fEmcHitsSec     = fEmcHits[sector];
  nEmcClusSec     = nEmcClusters[sector];
  indexEmcClusSec = indexEmcCluster[sector];
  fEmcClusSec     = fEmcCluster[sector];
  
  nTofHitsSec     = nTofHits[sector];
  tofHitsSec      = tofHits[sector];
  indexTofHitSec  = indexTofHit[sector];
  tofModuleSec    = tofModule[sector];

  tofClustSizeSec = tofClustSize[sector];
}
