//Task for all Matchings, fills hmetamatch
//*-- Author : V. Pechenov (29.07.2009)

using namespace std;

#include "hmdcclusmetamatch.h"
#include "hades.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hspecgeompar.h"
#include "tofdef.h"
#include "showerdef.h"
#include "emcdef.h"
#include "hshowergeometry.h"
#include "hemcgeompar.h"
#include "htofhitsim.h"
#include "htofclustersim.h"
#include "htofgeompar.h"
#include "hmetamatchpar.h"
#include "hmdcsizescells.h"
#include "hmdcgetcontainers.h"
#include "hmdcdetector.h"
#include "hshowerhittoftrack.h"
#include "hmdccluster.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "rpcdef.h"
#include "hrpcgeompar.h"
#include "hrpccluster.h"
#include "hemccluster.h"
#include <iostream>

ClassImp(HMdcClusMetaMatch)

HMdcClusMetaMatch::HMdcClusMetaMatch() {
  setInitParam();
}

void HMdcClusMetaMatch::setInitParam(void) {
  pMatchPar      = NULL;
  pTofGeometry   = NULL;
  pCatTof        = NULL;
  iterTof        = NULL;
  pCatTofCluster = NULL;
  iterTofCluster = NULL;
  pRpcGeometry   = NULL;
  pCatRpc        = NULL;
  iterRpc        = NULL;
  pShrGeometry   = NULL;
  pCatShower     = NULL;
  iterShower     = NULL;
  fEmcGeometry   = NULL;
  fCatEmc        = NULL;
  
  qualityMulp2   = 1.5*1.5;
  
  fillPlots      = kFALSE;
  for(Int_t s=0;s<6;s++) {
    rpcPlots[s] = 0;
    shrPlots[s] = 0;
    tofPlots[s] = 0;
  }
}

HMdcClusMetaMatch::~HMdcClusMetaMatch() {
  if(iterTof) {
    delete iterTof;
    iterTof=0;
  }
  if(iterShower) {
    delete iterShower;
    iterShower=0;
  }
}

Bool_t HMdcClusMetaMatch::init(void) {
  if (!gHades) return kFALSE;
  HRuntimeDb *rtdb = gHades->getRuntimeDb();
  if(!rtdb)    return kFALSE;
  HEvent *event = gHades->getCurrentEvent();
  if(!event)   return kFALSE;
  HSpectrometer *spec = gHades->getSetup();

  // TOF:
  pCatTof = event->getCategory(catTofHit);
  if(pCatTof == NULL) Warning("init",
    "No catTofHit in input! \n Matching with TofHits will be skipped!");
  else {
    iterTof = (HIterator*)pCatTof->MakeIterator();
    pTofGeometry = (HTofGeomPar *)rtdb->getContainer("TofGeomPar");
  }
  pCatTofCluster = event->getCategory(catTofCluster);
  if(pCatTofCluster == NULL) Warning("init",
    "NO catTofCluster in input! \n Matching with TofClusters will be skipped!");
  else iterTofCluster = (HIterator*)pCatTofCluster->MakeIterator();
  if(pCatTof == NULL && pCatTofCluster == NULL) {
    Error("init","No catTofHit and catTofCluster in input! Stop!");
    return kFALSE;
  }


  // RPC:
  pCatRpc = event->getCategory(catRpcCluster);
  if(!pCatRpc) {
    Warning("init","NO catRpcCluster in input! \n Matching with RpcClusters will be skipped!");
  } else {
    iterRpc = (HIterator*)pCatRpc->MakeIterator("native");
    pRpcGeometry   = (HRpcGeomPar*)rtdb->getContainer("RpcGeomPar");
  }

  
  // Shower & Emc:
  if(spec->getDetector("Shower")) {
    pCatShower = event->getCategory(catShowerHit);
    if(pCatShower != NULL) {
      iterShower   = (HIterator*)pCatShower->MakeIterator();
      pShrGeometry = (HShowerGeometry*)rtdb->getContainer("ShowerGeometry");
    }
  } else if(spec->getDetector("Emc")) {
    fCatEmc    = event->getCategory(catEmcCluster);
    if(fCatEmc != NULL) fEmcGeometry = (HEmcGeomPar *)rtdb->getContainer("EmcGeomPar");
  }
  if(pCatShower == NULL && fCatEmc == NULL) {
    Warning("init","No catShowerHit and catEmcCluster in input! Will be ignored!");
    iterShower = NULL;
  }
  
  pMatchPar    = (HMetaMatchPar*)rtdb->getContainer("MetaMatchPar");
  
  printf("HMdcClusMetaMatch is inited!\n");
  
  return kTRUE;
}

Bool_t HMdcClusMetaMatch::reinit(void) {
//  if(!pSizesCells->initContainer()) return kFALSE;
      
  HMdcGetContainers* pGetCont = HMdcGetContainers::getObject();

  if(pTofGeometry && !HMdcGetContainers::isInited(pTofGeometry)) return kFALSE;
  if(pShrGeometry && !HMdcGetContainers::isInited(pShrGeometry)) return kFALSE;
  
  if(pMatchPar==NULL || !HMdcGetContainers::isInited(pMatchPar)) {
    Error("reinit","no parameters for matching!");
    return kFALSE;
  }
  
  for(Int_t sec=0; sec<6; sec++) {
    if(pGetCont->getMdcDetector()->isSectorActive(sec)) {
      const HGeomTransform& labSecTr = pGetCont->getLabTransSec(sec);
      if((&labSecTr) == NULL) return kFALSE;
      if(pTofGeometry != NULL) {
        for(Int_t tofMod=0;tofMod<8;tofMod++) {
          HModGeomPar *pTofMod = pTofGeometry->getModule(sec,tofMod);
          if(pTofMod==0) {
            Error("reinit","Sec.%i Can't get tof module %i!",sec+1,tofMod+1);
            return kFALSE;
          }
          HGeomTransform trans(pTofMod->getLabTransform());
          HMdcSizesCells::copyTransfToArr(trans,tofLabModTrans[sec][tofMod]);
          trans.transTo(labSecTr);
          HMdcSizesCells::copyTransfToArr(trans,tofSecModTrans[sec][tofMod]);
        }
      }
      
      // Shower mod=0 is used only:
      if(pShrGeometry != NULL) {
        HGeomTransform transS(pShrGeometry->getTransform(sec));
        transS.transTo(labSecTr);
        HMdcSizesCells::copyTransfToArr(transS,shrSecModTrans[sec]);
      }
      
      // EMC:
      if (fEmcGeometry) {
        HModGeomPar *pmodgeom = fEmcGeometry->getModule(sec);
        HGeomTransform transS(pmodgeom->getLabTransform());
        transS.transTo(labSecTr);
        HMdcSizesCells::copyTransfToArr(transS,shrSecModTrans[sec]);
      }
      
      // RPC:
      if(pCatRpc != NULL) {
        HModGeomPar *pRpcMod = pRpcGeometry->getModule(sec,0);
        if(pRpcMod==0) {
          Error("reinit","Sec.%i Can't get rpc geometry!",sec+1);
          return kFALSE;
        }
        HGeomTransform transR(pRpcMod->getLabTransform());
        transR.transTo(labSecTr);
        HMdcSizesCells::copyTransfToArr(transR,rpcSecModTrans[sec]);
      }

      sigma2RpcX[sec]      = pMatchPar->getRpcSigmaXMdc(sec);
      sigma2RpcX[sec]     *= sigma2RpcX[sec];
      sigma2RpcY[sec]      = pMatchPar->getRpcSigmaYMdc(sec);
      sigma2RpcY[sec]     *= sigma2RpcY[sec];
      offsetRpcX[sec]      = pMatchPar->getRpcSigmaXOffset(sec);
      offsetRpcY[sec]      = pMatchPar->getRpcSigmaYOffset(sec);
      quality2RpcCut[sec]  = pMatchPar->getRpcQualityCut(sec);
      quality2RpcCut[sec] *= quality2RpcCut[sec];

      sigma2ShrX[sec]      = pMatchPar->getShowerSigmaXMdc(sec);
      sigma2ShrX[sec]     *= sigma2ShrX[sec];
      sigma2ShrY[sec]      = pMatchPar->getShowerSigmaYMdc(sec);
      sigma2ShrY[sec]     *= sigma2ShrY[sec];
      offsetShrX[sec]      = pMatchPar->getShowerSigmaXOffset(sec);
      offsetShrY[sec]      = pMatchPar->getShowerSigmaYOffset(sec);
      quality2ShrCut[sec]  = pMatchPar->getShowerQualityCut(sec);
      quality2ShrCut[sec] *= quality2ShrCut[sec];

      sigma2EmcX[sec]      = pMatchPar->getShowerSigmaXMdc(sec);
      sigma2EmcX[sec]     *= sigma2ShrX[sec];
      sigma2EmcY[sec]      = pMatchPar->getShowerSigmaYMdc(sec);
      sigma2EmcY[sec]     *= sigma2ShrY[sec];
      offsetEmcX[sec]      = pMatchPar->getShowerSigmaXOffset(sec);
      offsetEmcY[sec]      = pMatchPar->getShowerSigmaYOffset(sec);
      quality2EmcCut[sec]  = pMatchPar->getShowerQualityCut(sec);
      quality2EmcCut[sec] *= quality2ShrCut[sec];

      sigma2TofXi[sec]     = 1./pMatchPar->getTofSigmaX(sec);
      sigma2TofXi[sec]    *= sigma2TofXi[sec];
      sigma2TofYi[sec]     = 1./pMatchPar->getTofSigmaY(sec);
      sigma2TofYi[sec]    *= sigma2TofYi[sec];
      offsetTofX[sec]      = pMatchPar->getTofSigmaXOffset(sec);
      offsetTofY[sec]      = pMatchPar->getTofSigmaYOffset(sec);
      quality2TofCut[sec]  = pMatchPar->getTofQualityCut(sec);
      quality2TofCut[sec] *= quality2TofCut[sec];
      
      if(fillPlots) {
        Char_t name[100];
        Char_t title[100];
        sprintf(name,"RpcSec%i",sec+1);
        sprintf(title,"Sec.%i Rpc;X_{RPC}-X_{MDC} [mm];Y_{RPC}-Y_{MDC} [mm];",sec+1);
        rpcPlots[sec] = new TH2F(name,title,100,-250.,250.,100,-250.,250.);
        if(pShrGeometry != NULL) {
          sprintf(name,"ShrSec%i",sec+1);
          sprintf(title,"Sec.%i Shower;X_{Shower}-X_{MDC} [mm];Y_{Shower}-Y_{MDC} [mm];",sec+1);
          shrPlots[sec] = new TH2F(name,title,100,-250.,250.,100,-250.,250.);
        } else {
          sprintf(name,"EmcSec%i",sec+1);
          sprintf(title,"Sec.%i Emc;X_{EMC}-X_{MDC} [mm];Y_{EMC}-Y_{MDC} [mm];",sec+1);
          shrPlots[sec] = new TH2F(name,title,100,-250.,250.,100,-250.,250.);
        } 

        sprintf(name,"TofSec%i",sec+1);
        sprintf(title,"Sec.%i TOF;X_{TOF}-X_{MDC} [mm];Y_{Shower}-Y_{MDC} [mm];",sec+1);
        tofPlots[sec] = new TH2F(name,title,100,-250.,250.,100,-250.,250.);
      }
    }
  }

  return kTRUE;
}

void HMdcClusMetaMatch::collectMetaHits(void) {
  for(Int_t s=0;s<6;s++) {
    nShowerHits[s] = 0;
    nRpcHits[s]    = 0;
    nTofHits[s]    = 0;
  }
  collectRpcClusters();
  collectShowerHits();
  collectEmcClusters();
  collectTofHits();
}

void HMdcClusMetaMatch::collectRpcClusters(void) {
  if(pCatRpc == NULL) return;
  iterRpc->Reset();
  HRpcCluster *pRpcCluster;
  while((pRpcCluster=(HRpcCluster*)(iterRpc->Next()))!=0) {
    Int_t sec = pRpcCluster->getSector();
    UInt_t& nRpcHSec = nRpcHits[sec];
    if(nRpcHSec>=200) continue; //!!!     
    RpcHit &rpcHit = rpcHitArr[sec][nRpcHSec];
    
    rpcHit.x = pRpcCluster->getXMod() - offsetRpcX[sec];
    rpcHit.y = pRpcCluster->getYMod() - offsetRpcY[sec];
    rpcHit.z = pRpcCluster->getZMod();
    rpcHit.xSigma2i = pRpcCluster->getXRMS()*pRpcCluster->getXRMS();
    rpcHit.xSigma2i = 1.f/(rpcHit.xSigma2i + sigma2RpcX[sec]);
    rpcHit.ySigma2i = pRpcCluster->getYRMS()*pRpcCluster->getYRMS();
    rpcHit.ySigma2i = 1.f/(rpcHit.ySigma2i + sigma2RpcY[sec]);
    nRpcHSec++;
  }
}

void HMdcClusMetaMatch::collectShowerHits(void) {
  if(!pCatShower) return;
  iterShower->Reset();
  HShowerHit *pShowerHit;
  while((pShowerHit=(HShowerHit*)(iterShower->Next()))!=0) {
    if(pShowerHit->getModule() != 0) continue;
    Int_t sec = pShowerHit->getSector();
    UInt_t& nShrHSec = nShowerHits[sec];
    if(nShrHSec>=200) continue; //!!!     
    ShowerHit &shrHit = shrHitArr[sec][nShrHSec];
    Float_t x,y;
    pShowerHit->getXY(&x,&y);
    
    shrHit.x = x - offsetShrX[sec];
    shrHit.y = y - offsetShrY[sec];
    shrHit.z = pShowerHit->getZ();
    shrHit.xSigma2i = pShowerHit->getSigmaX()*pShowerHit->getSigmaX();
    shrHit.xSigma2i = 1.f/(shrHit.xSigma2i + sigma2ShrX[sec]);
    shrHit.ySigma2i = pShowerHit->getSigmaY()*pShowerHit->getSigmaY();
    shrHit.ySigma2i = 1.f/(shrHit.ySigma2i + sigma2ShrY[sec]);
    nShrHSec++;
  }
}

void HMdcClusMetaMatch::collectEmcClusters(void) {
  if(fCatEmc == NULL) return;
  Int_t nclus = fCatEmc->getEntries();
  for(Int_t ind=0;ind<nclus;ind++) {
    HEmcCluster *pEmcCluster = (HEmcCluster*)fCatEmc->getObject(ind);
    if( !pEmcCluster->ifActive() ) continue;              // Bad cluster
    Int_t sec = pEmcCluster->getSector();
    UInt_t& nShrHSec = nShowerHits[sec];
    if(nShrHSec>=200) continue; //!!!     
    ShowerHit &shrHit = shrHitArr[sec][nShrHSec];
    
    shrHit.x = pEmcCluster->getXMod() - offsetShrX[sec];
    shrHit.y = pEmcCluster->getYMod() - offsetShrY[sec];
    shrHit.z = 0.;
    shrHit.xSigma2i = pEmcCluster->getSigmaXMod()*pEmcCluster->getSigmaXMod();
    shrHit.xSigma2i = 1.f/(shrHit.xSigma2i + sigma2ShrX[sec]);
    shrHit.ySigma2i = pEmcCluster->getSigmaYMod()*pEmcCluster->getSigmaYMod();
    shrHit.ySigma2i = 1.f/(shrHit.ySigma2i + sigma2ShrY[sec]);
    nShrHSec++;
  }
}

void HMdcClusMetaMatch::collectTofHits(void) {
  if(pCatTof) iterTof->Reset();
  HTofHit *pTofHit;
  if(!pCatTofCluster) {
    // No TofCluster category:
    if(pCatTof)
      while((pTofHit=(HTofHit*)(iterTof->Next()))!=0 ) addTofHit(pTofHit);
  } else {
    // TofCluster category exist:
    iterTofCluster->Reset();
    HTofCluster *pTofCluster;
    while((pTofCluster=(HTofCluster*)(iterTofCluster->Next()))!=0 ) {
      Int_t tofClSize=pTofCluster->getClusterSize();
// ???      if(tofClSize>2) continue;     // tofClSize<=2 only !????????????
      addTofHit(pTofCluster);
      if(tofClSize<2) continue;
      if(pCatTof==0)  continue;
      Int_t sec  = pTofCluster->getSector();
      Int_t mod  = pTofCluster->getModule();
      Int_t cell = pTofCluster->getCell();
      while((pTofHit=(HTofHit*)(iterTof->Next()))!=0 ) {
        if(sec!=pTofHit->getSector() || mod!=pTofHit->getModule() ||
            cell != pTofHit->getCell()) continue;
        if(tofClSize==2) {  // two TofHits adding, tofClSize==2 only !
          addTofHit(pTofHit);                  // first TofHit for TofCluster
          pTofHit = (HTofHit*)(iterTof->Next());   // second ...
          addTofHit(pTofHit);                  // clust.size==2 selected !!!
        } else {            // tofClSize>2 only HTofHits can be used
          addTofHit(pTofHit);
          for(Int_t h=0;h<tofClSize-1;h++) {
            pTofHit = (HTofHit*)(iterTof->Next());
            addTofHit(pTofHit);
          }
        }
        break;
      }
    }
  }
}

void HMdcClusMetaMatch::addTofHit(HTofHit* pTofHit) {
  Int_t   sec      = pTofHit->getSector();
  Int_t   mod      = pTofHit->getModule();
  UInt_t &nTofHSec = nTofHits[sec];
  TofHit &tofHit   = tofHitArr[sec][nTofHSec];
  Float_t Xtof,Ytof,Ztof;
  pTofHit->getXYZLab(Xtof,Ytof,Ztof);
  Double_t *tSysRLab = tofLabModTrans[sec][mod];
  Double_t xt = Xtof-tSysRLab[ 9];
  Double_t yt = Ytof-tSysRLab[10];
  Double_t zt = Ztof-tSysRLab[11];
  tofHit.x    = tSysRLab[0]*xt+tSysRLab[3]*yt+tSysRLab[6]*zt;
  tofHit.y    = tSysRLab[1]*xt+tSysRLab[4]*yt+tSysRLab[7]*zt;
  //tofHit.z   = tSysRLab[2]*xt+tSysRLab[5]*yt+tSysRLab[8]*zt;
  tofHit.x   -= offsetTofX[sec];
  tofHit.y   -= offsetTofY[sec];
  tofHit.mod = mod;
  nTofHSec++;
}

Bool_t HMdcClusMetaMatch::hasClusMathToMeta(Int_t sec,const HGeomVector& targ,
                                       Double_t xcl,Double_t ycl,Double_t zcl) {
  if(fillPlots) return testAndFill(sec,targ,xcl,ycl,zcl);
  Double_t xd  = xcl-targ(0);
  Double_t yd  = ycl-targ(1);
  Double_t zd  = zcl-targ(2);
  
  // Matching with rpc:
  if(nRpcHits[sec]>0) {
    Double_t *tSysRSec = rpcSecModTrans[sec];
    Double_t  xci = xcl-tSysRSec[ 9];
    Double_t  yci = ycl-tSysRSec[10];
    Double_t  zci = zcl-tSysRSec[11];
    Double_t  a1  = tSysRSec[2]*xci+tSysRSec[5]*yci;
    Double_t  a2  = 1./(tSysRSec[2]*xd+tSysRSec[5]*yd+tSysRSec[8]*zd);
    Float_t   x   = 0.;
    Float_t   y   = 0.;
    Float_t   zPr = -1000.;
    for(UInt_t n=0;n<nRpcHits[sec];n++) {
      RpcHit &rpcHit = rpcHitArr[sec][n];
      if(rpcHit.z != zPr) {
        // Calculation of cross poin with rpc module (in mod.coor.sys.)
        zPr = rpcHit.z;
        Double_t zc = zci-rpcHit.z;
        Double_t mp = (a1+tSysRSec[8]*zc)*a2;
        Double_t xc = xci-xd*mp;
        Double_t yc = yci-yd*mp;
        zc -= zd*mp;
        x   = tSysRSec[0]*xc+tSysRSec[3]*yc+tSysRSec[6]*zc;
        y   = tSysRSec[1]*xc+tSysRSec[4]*yc+tSysRSec[7]*zc;
      }
      Float_t dX   = rpcHit.x - x;
      Float_t dY   = rpcHit.y - y;
      Float_t qual = dX*dX*rpcHit.xSigma2i + dY*dY*rpcHit.ySigma2i;
      if(qual <= quality2RpcCut[sec]*qualityMulp2) return kTRUE;
    }
  }
  
  // Matching with shower or emc:
  if(nShowerHits[sec]>0) {
    Double_t *tSysRSec = shrSecModTrans[sec];
    Double_t  xci = xcl-tSysRSec[ 9];
    Double_t  yci = ycl-tSysRSec[10];
    Double_t  zci = zcl-tSysRSec[11];
    Double_t  a1  = tSysRSec[2]*xci+tSysRSec[5]*yci;
    Double_t  a2  = 1./(tSysRSec[2]*xd+tSysRSec[5]*yd+tSysRSec[8]*zd);
    Float_t   x   = 0.;
    Float_t   y   = 0.;
    Float_t   zPr = -1000.;
    for(UInt_t n=0;n<nShowerHits[sec];n++) {
      ShowerHit &shrHit = shrHitArr[sec][n];
      if(shrHit.z != zPr) {
        // Calculation of cross poin with shower module (in mod.coor.sys.)
        zPr = shrHit.z;
        Double_t zc = zci-shrHit.z;
        Double_t mp = (a1+tSysRSec[8]*zc)*a2;
        Double_t xc = xci-xd*mp;
        Double_t yc = yci-yd*mp;
        zc -= zd*mp;
        x   = tSysRSec[0]*xc+tSysRSec[3]*yc+tSysRSec[6]*zc;
        y   = tSysRSec[1]*xc+tSysRSec[4]*yc+tSysRSec[7]*zc;
      }
      Float_t dX   = shrHit.x - x;
      Float_t dY   = shrHit.y - y;
      Float_t qual = dX*dX*shrHit.xSigma2i + dY*dY*shrHit.ySigma2i;
      if(qual <= quality2ShrCut[sec]*qualityMulp2) return kTRUE;
    }
  }
  
  // Matching with tof:
  if(nTofHits[sec]>0) {
    Float_t x   = 0.;
    Float_t y   = 0.;
    UChar_t mPr = 255;
    for(UInt_t n=0;n<nTofHits[sec];n++) {
      TofHit &tofHit = tofHitArr[sec][n];
      if(tofHit.mod != mPr) {
        mPr = tofHit.mod;
        Double_t *tSysRSec = tofSecModTrans[sec][tofHit.mod];

        // Calculation of cross poin with tof module (in mod.coor.sys.)
        Double_t xc = xcl-tSysRSec[ 9];
        Double_t yc = ycl-tSysRSec[10];
        Double_t zc = zcl-tSysRSec[11];
        Double_t mp = (tSysRSec[2]*xc+tSysRSec[5]*yc+tSysRSec[8]*zc) /
                      (tSysRSec[2]*xd+tSysRSec[5]*yd+tSysRSec[8]*zd);
        xc -= xd*mp;
        yc -= yd*mp;
        zc -= zd*mp;
        x   = tSysRSec[0]*xc+tSysRSec[3]*yc+tSysRSec[6]*zc;
        y   = tSysRSec[1]*xc+tSysRSec[4]*yc+tSysRSec[7]*zc;
      }
      Float_t dX   = tofHit.x - x;
      Float_t dY   = tofHit.y - y;
      Float_t qual = dX*dX*sigma2TofXi[sec] +dY*dY*sigma2TofYi[sec];

      if(qual <= quality2TofCut[sec]*qualityMulp2) return kTRUE;
    }
  }
  // No matching
  return kFALSE;
}

Bool_t HMdcClusMetaMatch::testAndFill(Int_t sec,const HGeomVector& targ,
                                       Double_t xcl,Double_t ycl,Double_t zcl) {
  Bool_t exitFlag = kFALSE;
  Double_t xd  = xcl-targ(0);
  Double_t yd  = ycl-targ(1);
  Double_t zd  = zcl-targ(2);
  
  // Matching with rpc:
  if(nRpcHits[sec]>0) {
    TH2F* rpcPlot = rpcPlots[sec];
    Double_t *tSysRSec = rpcSecModTrans[sec];
    Double_t  xci = xcl-tSysRSec[ 9];
    Double_t  yci = ycl-tSysRSec[10];
    Double_t  zci = zcl-tSysRSec[11];
    Double_t  a1  = tSysRSec[2]*xci+tSysRSec[5]*yci;
    Double_t  a2  = 1./(tSysRSec[2]*xd+tSysRSec[5]*yd+tSysRSec[8]*zd);
    Float_t   x   = 0.;
    Float_t   y   = 0.;
    Float_t   zPr = -1000.;
    for(UInt_t n=0;n<nRpcHits[sec];n++) {
      RpcHit &rpcHit = rpcHitArr[sec][n];
      if(rpcHit.z != zPr) {
        // Calculation of cross poin with rpc module (in mod.coor.sys.)
        zPr = rpcHit.z;
        Double_t zc = zci-rpcHit.z;
        Double_t mp = (a1+tSysRSec[8]*zc)*a2;
        Double_t xc = xci-xd*mp;
        Double_t yc = yci-yd*mp;
        zc -= zd*mp;
        x   = tSysRSec[0]*xc+tSysRSec[3]*yc+tSysRSec[6]*zc;
        y   = tSysRSec[1]*xc+tSysRSec[4]*yc+tSysRSec[7]*zc;
      }
      Float_t dX = rpcHit.x - x;
      Float_t dY = rpcHit.y - y;
      if(rpcPlot) rpcPlot->Fill(dX,dY);
      if(!exitFlag) {
        Float_t qual = dX*dX*rpcHit.xSigma2i + dY*dY*rpcHit.ySigma2i;  
        if(qual <= quality2RpcCut[sec]*qualityMulp2) exitFlag = kTRUE;
      }
    }
  }
  
  // Matching with shower:
  if(nShowerHits[sec]>0) {
    TH2F* shrPlot = shrPlots[sec];
    Double_t *tSysRSec = shrSecModTrans[sec];
    Double_t  xci = xcl-tSysRSec[ 9];
    Double_t  yci = ycl-tSysRSec[10];
    Double_t  zci = zcl-tSysRSec[11];
    Double_t  a1  = tSysRSec[2]*xci+tSysRSec[5]*yci;
    Double_t  a2  = 1./(tSysRSec[2]*xd+tSysRSec[5]*yd+tSysRSec[8]*zd);
    Float_t   x   = 0.;
    Float_t   y   = 0.;
    Float_t   zPr = -1000.;
    for(UInt_t n=0;n<nShowerHits[sec];n++) {
      ShowerHit &shrHit = shrHitArr[sec][n];
      if(shrHit.z != zPr) {
        // Calculation of cross poin with shower module (in mod.coor.sys.)
        zPr = shrHit.z;
        Double_t zc = zci-shrHit.z;
        Double_t mp = (a1+tSysRSec[8]*zc)*a2;
        Double_t xc = xci-xd*mp;
        Double_t yc = yci-yd*mp;
        zc -= zd*mp;
        x   = tSysRSec[0]*xc+tSysRSec[3]*yc+tSysRSec[6]*zc;
        y   = tSysRSec[1]*xc+tSysRSec[4]*yc+tSysRSec[7]*zc;
      }
      Float_t dX = shrHit.x - x;
      Float_t dY = shrHit.y - y;
      if(shrPlot) shrPlot->Fill(dX,dY);
      if(!exitFlag) {
        Float_t qual = dX*dX*shrHit.xSigma2i + dY*dY*shrHit.ySigma2i;  
        if(qual <= quality2ShrCut[sec]*qualityMulp2) exitFlag = kTRUE;
      }
    }
  }
  
  // Matching with tof:
  if(nTofHits[sec]>0) {
    TH2F* tofPlot = tofPlots[sec];
    Float_t x   = 0.; 
    Float_t y   = 0.;
    UChar_t mPr = 255;
    for(UInt_t n=0;n<nTofHits[sec];n++) {
      TofHit &tofHit = tofHitArr[sec][n];
      if(tofHit.mod != mPr) {
        mPr = tofHit.mod;
        Double_t *tSysRSec = tofSecModTrans[sec][tofHit.mod];

        // Calculation of cross poin with tof module (in mod.coor.sys.)
        Double_t xc = xcl-tSysRSec[ 9];
        Double_t yc = ycl-tSysRSec[10];
        Double_t zc = zcl-tSysRSec[11];
        Double_t mp = (tSysRSec[2]*xc+tSysRSec[5]*yc+tSysRSec[8]*zc) /
                      (tSysRSec[2]*xd+tSysRSec[5]*yd+tSysRSec[8]*zd);
        xc -= xd*mp;
        yc -= yd*mp;
        zc -= zd*mp;
        x   = tSysRSec[0]*xc+tSysRSec[3]*yc+tSysRSec[6]*zc;
        y   = tSysRSec[1]*xc+tSysRSec[4]*yc+tSysRSec[7]*zc;
      }
      Float_t dX   = tofHit.x - x;
      Float_t dY   = tofHit.y - y;
      if(tofPlot != NULL) tofPlot->Fill(dX,dY);
      if(!exitFlag) {
        Float_t qual = dX*dX*sigma2TofXi[sec] +dY*dY*sigma2TofYi[sec];  
        if(qual <= quality2TofCut[sec]*qualityMulp2) exitFlag = kTRUE;
      }
    }
  }
  // No matching
  return exitFlag;
}

void HMdcClusMetaMatch::deletePlots(void) {
  for(Int_t s=0;s<6;s++) {
    if(rpcPlots[s] != NULL) delete rpcPlots[s];
    if(shrPlots[s] != NULL) delete shrPlots[s];
    if(tofPlots[s] != NULL) delete tofPlots[s];
    rpcPlots[s] = 0;
    shrPlots[s] = 0;
    tofPlots[s] = 0;
  }
}

void HMdcClusMetaMatch::savePlots(void) {
  if(!fillPlots) return;
  gROOT->SetBatch();
  TString fName;
  HMdcGetContainers::getFileName(fName);
  fName += "_ClTofShrMatching.root";
  TFile* pFile = new TFile(fName.Data(),"RECREATE");
  pFile->cd();
  
  TCanvas* cnRpc = new TCanvas("cnRpc","Cluster-Rpc match",10,10,1000,800);
  cnRpc->cd();
  cnRpc->Divide(3,2,0.002,0.002,0);
  for(Int_t sec=0;sec<6;sec++) if(rpcPlots[sec] != NULL) {
    cnRpc->cd(sec+1);
    rpcPlots[sec]->Draw("colz");
  }
  cnRpc->Write();
  delete cnRpc;
  
  TCanvas* cnShr;
  if(fEmcGeometry != NULL) cnShr = new TCanvas("cnEmc","Cluster-Emc match",10,10,1000,800);
  else                     cnShr = new TCanvas("cnShr","Cluster-Shower match",10,10,1000,800);
  cnShr->cd();
  cnShr->Divide(3,2,0.002,0.002,0);
  for(Int_t sec=0;sec<6;sec++) if(shrPlots[sec] != NULL) {
    cnShr->cd(sec+1);
    shrPlots[sec]->Draw("colz");
  }
  cnShr->Write();
  delete cnShr;
  
  TCanvas* cnTof = new TCanvas("cnTof","Cluster-TOF match",10,10,1000,800);
  cnTof->cd();
  cnTof->Divide(3,2,0.002,0.002,0);
  for(Int_t sec=0;sec<6;sec++) if(tofPlots[sec] != NULL) {
    cnTof->cd(sec+1);
    tofPlots[sec]->Draw("colz");
  }
  cnTof->Write();
  delete cnTof;
  
  pFile->Close();
  delete pFile;
  deletePlots();
  gROOT->SetBatch(kFALSE);
}
