//*-- AUTHOR : Y.C.Pachmayer
//*-- Modified :
///////////////////////////////////////////////////////////////////////////////
//
// HMdcCosmicsCandidate
//
//
///////////////////////////////////////////////////////////////////////////////


using namespace std;
#include "hmdccosmicscandidate.h"
#include "TH1.h"
#include "TH2.h"
#include "TROOT.h"
#include "TMath.h"
#include "hades.h"
#include "hevent.h"
#include "hruntimedb.h"
#include "haddef.h"
#include "hiterator.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdcdetector.h"
#include "tofdef.h"
#include "rpcdef.h"
#include "hspectrometer.h"
#include "htofgeompar.h"
#include "hmdccal1sim.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "hrpchit.h"
#include "hrpccluster.h"
#include "hmdcclussim.h"
#include "hmdcsizescells.h"
#include "hcategory.h"
#include "hgeomtransform.h"
#include "hmdcgetcontainers.h"
#include "hgeomvolume.h"
#include <iostream>
#include <fstream>

ClassImp(HMdcCosmicsCandidate)
ClassImp(HCosmicCalibEvSkip)


HMdcCosmicsCandidate::HMdcCosmicsCandidate(const Text_t *name,const Text_t *title) 
    : HReconstructor(name,title) {
  initVariables();
}

HMdcCosmicsCandidate::HMdcCosmicsCandidate(void)
    : HReconstructor("HMdcCosmicsCandidate","HMdcCosmicsCandidate") {
  initVariables();
}

HMdcCosmicsCandidate::~HMdcCosmicsCandidate(void) {
  HMdcSizesCells::deleteCont();
  HMdcGetContainers::deleteCont();
  if(histFile != NULL) delete histFile;
  histFile = NULL;
}

void HMdcCosmicsCandidate::initVariables(void) {
  returnFlag     = kSkipEvent;
  pCatTof        = NULL;
  pCatRpc        = NULL;
  pCatMdcCal1    = NULL;
  useTofCat      = 0;
  useRpcCat      = 0;
  hsBeta         = NULL;
  plStat         = NULL;
  plQualVsWrLo   = NULL;
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) {
    hsDCell[s][m] = NULL;
    for(Int_t l=0;l<6;l++) hsDCellL[s][m][l] = NULL;
  }

  makeHists      = kFALSE;
  histFile       = NULL;
  histFileName   = "./cosmCandHists.root";
  histFileOption = "NEW";
  setParContStat = kFALSE;
  zTofShift      = 0.;
  zRpcShift      = 0.;
  isGeant        = kFALSE;
  
  // Cuts:
  betaMin        = 0.5;
  betaMax        = 1.5;
  maxNWiresOutTr = 4;
  selQualCut     = 0.85;
  for(Int_t m=0; m<4; m++) dCellWind[m]    = 15.;
  nLayInModCut   = 3;
  nEventsTot     = 0;
  nEventsSel     = 0;
  nEventsSelSc   = 0;
  for(Int_t i=0;i<9;i++) {
    nSelTr[i]       = 0;
    trackScaling[i] = i<4 ? 0. : 1.1;
    nSelTrSc[i]     = 0;
  }
}

void HMdcCosmicsCandidate::setBetaCut(Double_t min,Double_t max) {
  if(max>0. && min>=0. && max>min) {
    betaMin = min;
    betaMax = max;
  } else {
    Error("setBetaCut","Error in beta cut setting: min=%f max=%f",min,max);
    exit(1);
  }
}

void HMdcCosmicsCandidate::setDCellWindow(Float_t wM1,Float_t wM2,Float_t wM3,Float_t wM4) {
  if(wM1>=0.) dCellWind[0] = wM1;
  if(wM2>=0.) dCellWind[1] = wM2;
  if(wM3>=0.) dCellWind[2] = wM3;
  if(wM4>=0.) dCellWind[3] = wM4;
}
  
Bool_t HMdcCosmicsCandidate::init(void) {
  cout << " HMdcCosmicsCandidate " << endl;
  if(!gHades) return kFALSE;

  sizes = HMdcSizesCells::getObject();
  pGetCont = HMdcGetContainers::getObject();

  if     (useTofCat==1) pCatTof = gHades->getCurrentEvent()->getCategory(catTofHit);
  else if(useTofCat==2) pCatTof = gHades->getCurrentEvent()->getCategory(catTofCluster);
  if(useTofCat>0 && pCatTof==NULL) {
    Error("HMdcCosmicsCandidate::init()","NO TOF hits! Stop!");
    return kFALSE;
  }

  if     (useRpcCat==1) pCatRpc = gHades->getCurrentEvent()->getCategory(catRpcHit);
  else if(useRpcCat==2) pCatRpc = gHades->getCurrentEvent()->getCategory(catRpcCluster);
  if(useRpcCat>0 && pCatRpc==NULL) {
    Error("HMdcCosmicsCandidate::init()","NO RPC hits! Stop!");
    return kFALSE;
  }
  
  if(pCatTof==NULL && pCatRpc==NULL) {
    Error("HMdcCosmicsCandidate::init()","NO META hits! Stop!");
    return kFALSE;
  }
      
  pCatMdcCal1 = gHades->getCurrentEvent()->getCategory(catMdcCal1);
  if (pCatMdcCal1 == NULL) {
    Error("HMdcCosmics::init()","NO catMdcCal1!");
    return kFALSE;
  }
  pCatMdcClus = pGetCont->getCatMdcClus(kTRUE);
  if(pCatMdcClus == NULL) {
    Error("HMdcCosmics::init()","NO catMdcClus!");
    return kFALSE;
  }

  //  MDC setup:
  HMdcDetector*  fMdcDet = pGetCont->getMdcDetector();
  for(Int_t s=0; s<6; s++) for(Int_t m=0; m<4; m++) {
    if(fMdcDet->getModule(s,m) == 0) mdcsetup[s][m] = 0;
    else                             mdcsetup[s][m] = 1;
  }

  printParam();
  if(makeHists && !createHists()) return kFALSE;
  return kTRUE;

}

Bool_t HMdcCosmicsCandidate::reinit(void) { 
  if(!sizes->initContainer()) return kFALSE;
  isGeant = HMdcGetContainers::isGeant(); 
  for(Int_t m=0;m<4;m++) {
    for(Int_t s=0;s<6;s++) if(pGetCont->isModActive(s,m)) {
      HMdcSizesCellsMod &scMod = (*sizes)[s][m];
      HMdcSizesCellsLayer &scLay = scMod[3]; // layer 2 only!
      HGeomVolume *layVol = scLay.getGeomVolume();
      HGeomVector* pnt1   = layVol->getPoint(0);
      HGeomVector* pnt2   = layVol->getPoint(1);
      laySize[m].y1       = pnt1->getY();
      laySize[m].x2       = pnt2->getX();
      laySize[m].y2       = pnt2->getY();
      laySize[m].inc      = (pnt1->getX()-pnt2->getX())/(pnt1->getY()-pnt2->getY());
      break;
    }
  }
  return kTRUE;
}

void HMdcCosmicsCandidate::printParam(void) {
  printf("------------------------------------------------------------\n");
  printf("  HMdcCosmicsCandidate:\n  ");
  if     (useTofCat == 1) printf("TofHits");
  else if(useTofCat == 2) printf("TofClusters");
  if(useTofCat>0 && useRpcCat>0) printf(" & ");
  if     ( useRpcCat==1 ) printf("RpcHits");
  else if( useRpcCat==2 ) printf("RpcClusters");
  printf(" are used for cosmic track finder.\n");
  printf("  Beta cut                   : betaMin=%g, betaMax=%g\n",betaMin,betaMax);
  if(zTofShift != 0.f) printf("  Shift of TofHits Z position: %.1f mm\n",zTofShift);
  printf("  Cells cut (MDC I,II,III,IV): %.1f, %.1f, %.1f, %.1f\n",
         dCellWind[0],dCellWind[1],dCellWind[2],dCellWind[3]);
  if(makeHists) printf("  (See histograms in the file)\n");
  printf("  Min.number of layres per module = %i\n",nLayInModCut);
  printf("------------------------------------------------------------\n");
}

Int_t HMdcCosmicsCandidate::execute(void) {
  
  if(nEventsTot==0 && setParContStat) {
    gHades->getRuntimeDb()->setContainersStatic();
    gHades->setEnableCloseInput(kFALSE);
  }
  
  nEventsTot++;

  nMetaHits = 0;
  nSectors  = 0;
  for(Int_t s=0;s<6;s++) {
    nMetaHitSec[s] = 0;
    for(Int_t m=0;m<4;m++) nWiresStat[s][m] = 0;
  }
  
  collectTofHits();
  collectRpcHits();
  if(nMetaHits>120) return returnFlag;
  
  numWiresAllMod = pCatMdcCal1->getEntries();
  if(numWiresAllMod<5) return returnFlag;
  for(Int_t n=0;n<numWiresAllMod;n++) {
    HMdcCal1 *pCal1 = (HMdcCal1*)pCatMdcCal1->getObject(n);
    Int_t sector = pCal1->getSector();
    Int_t module = pCal1->getModule();
    nWiresStat[sector][module]++;
  }

  if(nMetaHits < 2 || nSectors!=2) return returnFlag;

  Int_t s1 = 0;
  for(;s1<6;s1++) if(nMetaHitSec[s1]>0) break;
  Int_t s2 = s1+1;
  for(;s2<6;s2++) if(nMetaHitSec[s2]>0) break;
  bestTrCand.init(-1,-1);
  for(Int_t i1=0;i1<nMetaHits;i1++) if(metaHits[i1].sec == s1) { 
    for(Int_t i2=0;i2<nMetaHits;i2++) if(metaHits[i2].sec == s2) {
      currTrCand.init(i1,i2);
      if( !calcSectors(currTrCand) ) continue;
      if(currTrCand.nSecs < 2) continue;
      if(!testTrack()) continue;
      if(currTrCand.nLayers < bestTrCand.nLayers) continue;
      if(currTrCand.nLayers == bestTrCand.nLayers) {
        if(currTrCand.nModules < bestTrCand.nModules) continue;
        if(currTrCand.nModules == bestTrCand.nModules && currTrCand.dCellMean>bestTrCand.dCellMean) continue;
      }
      bestTrCand.copy(currTrCand);
    }
  }
  if(  bestTrCand.metaInd1< 0 )   return returnFlag;
  if( !bestTrCand.trackStatus() ) return returnFlag;
  
  plQualVsWrLo->Fill(bestTrCand.numWiresOutTrack() + 0.5,bestTrCand.selectQual());
  
  if(bestTrCand.numWiresOutTrack() > maxNWiresOutTr) return returnFlag;
  if(bestTrCand.selectQual() < selQualCut)           return returnFlag;
  if(bestTrCand.numLayersPerMod() < nLayInModCut)    return returnFlag;
  if(bestTrCand.nModules < 4)                        return returnFlag;
  
//---------------------------------------------------------
// while(bestTrCand.nSecs>2) { // Two sectors can be fitted now only!!!!!!!!!!!
//   Int_t sec = 0;
//   Int_t min = 100;
//   for(Int_t s=0;s<6;s++) if(bestTrCand.nLaySec[s]>0 && bestTrCand.nLaySec[s]<=min) {
//     sec = s;
//     min = bestTrCand.nLaySec[s];
//   }
// //  bestTrCand.nLayers -= min;
//   bestTrCand.nSecs--;
//   bestTrCand.nModules -= bestTrCand.nModSec[sec];
//   bestTrCand.nModSec[sec] = 0;
// //  bestTrCand.nLaySec[sec] = 0;
//   bestTrCand.modList[sec] = 0;
// // ---------------  return returnFlag;
// }
//------------------------------------------------------------

  if( !fillTrack(bestTrCand) )                       return returnFlag;
  
  Int_t &nModules = bestTrCand.nModules;
  nSelTr[nModules]++;
  nEventsSel++;

  if(trackScaling[nModules] <= 0.)                                       return returnFlag;
  if(trackScaling[nModules] < 1.&& rndm.Rndm() > trackScaling[nModules]) return returnFlag;
  nSelTrSc[nModules]++;
  nEventsSelSc++;
// static Int_t nnn={0};
// if(bestTrCand.nSecs>2) {
//   nnn++;
//   printf("nnn=%i   nSecs=%i\n",nnn,bestTrCand.nSecs);
// }
  
  return 0;
 }

void HMdcCosmicsCandidate::collectTofHits(void) {
  if(pCatTof == NULL) return;
  Int_t nHits = pCatTof->getEntries();
  if(nHits>120) return;
  for(Int_t n=0;n<nHits;n++) {
    HTofHit *tofHit = (HTofHit*) pCatTof->getObject(n);
    MetaHit &mHit   = metaHits[nMetaHits];
    Int_t sec = tofHit->getSector();
    if( !pGetCont->isSecActive(sec) ) continue;
    Float_t xlab,ylab,zlab;
    tofHit->getXYZLab(xlab,ylab,zlab);
    if( TMath::IsNaN(xlab) ) continue;
    mHit.hit.setXYZ(xlab,ylab,zlab + zTofShift);
    mHit.sec  = sec;
    mHit.tof  = tofHit->getTof();
    mHit.isTOF = kTRUE;
    nMetaHits++;
    if(nMetaHitSec[sec] == 0) nSectors++;
    nMetaHitSec[sec]++;
  }
}

void HMdcCosmicsCandidate::collectRpcHits(void) {
  if(pCatRpc == NULL) return;
  Int_t nHits = pCatRpc->getEntries();
  if(nHits>120) return;
  for(Int_t n=0;n<nHits;n++) {
    MetaHit &mHit   = metaHits[nMetaHits];
    Float_t xlab,ylab,zlab;
    if(useRpcCat==1) {
      HRpcHit *rpcHit = (HRpcHit*)pCatRpc->getObject(n);
      mHit.sec = rpcHit->getSector();
      mHit.tof = rpcHit->getTof();
      rpcHit->getXYZLab(xlab,ylab,zlab);      
    } else {
      HRpcCluster *rpcHit = (HRpcCluster*)pCatRpc->getObject(n);
      mHit.sec = rpcHit->getSector();
      mHit.tof = rpcHit->getTof();
      rpcHit->getXYZLab(xlab,ylab,zlab);
    }
    if( !pGetCont->isSecActive(mHit.sec) ) continue;
    mHit.hit.setXYZ(xlab,ylab,zlab + zRpcShift);
    mHit.isTOF = kFALSE;
    nMetaHits++;
    if(nMetaHitSec[mHit.sec] == 0) nSectors++;
    nMetaHitSec[mHit.sec]++;
  }
}

Bool_t HMdcCosmicsCandidate::testBeta(const MetaHit &h1,const MetaHit &h2,Bool_t mHists) {
  Double_t dt   = h1.tof  - h2.tof;
  if(dt == 0.) return kFALSE;
  Double_t path = (h1.hit-h2.hit).length();
  Double_t beta = path/TMath::Abs(dt)*1.e+6/TMath::C();
  if(mHists) hsBeta->Fill(beta);
//????if(h1.isTOF && h2.isTOF)  //  ????? RPC is not in triger! ... 
  if(beta < betaMin || beta > betaMax) return kFALSE;
  if(mHists) {
    if(dt < 0.) plStat->Fill(h1.sec+0.5,h2.sec+0.5);
    else        plStat->Fill(h2.sec+0.5,h1.sec+0.5);
  }
  return kTRUE;
} 

Bool_t HMdcCosmicsCandidate::fillTrack(TrackCand &bestCand) {
  MetaHit &mHit1 = metaHits[bestCand.metaInd1];
  MetaHit &mHit2 = metaHits[bestCand.metaInd2];
  if( !testBeta(mHit1,mHit2,makeHists) ) return kFALSE;
  HMdcClus* prevClus = NULL;
 
  HMdcList12GroupCells listCells;
  for(Int_t sec=0;sec<6;sec++) if(bestCand.secStatus(sec)) {
    HMdcSizesCellsSec&  scSec = (*sizes)[sec];
    hit1 = scSec.getLabTrans()->transTo(mHit1.hit);  // transf. lab => sec.coor.sytem
    hit2 = scSec.getLabTrans()->transTo(mHit2.hit);  // transf. lab => sec.coor.sytem
    for(Int_t seg=0; seg<2;seg++) if(bestCand.segStatus(sec,seg) != 0) {
      Short_t listCrLayers = 0;  // One bit per layers (12 bits). Bit[l]=1 -track cross this layer
      listCells.clear();
      for(Int_t mod=seg*2; mod<seg*2+2;mod++) if(bestCand.modStatus(sec,mod) != 0) {
	HMdcSizesCellsMod& szMod = scSec[mod];
        Int_t ilay = (mod&1)*6;
	for(Int_t lay=0;lay<6;lay++,ilay++) {
	  HMdcSizesCellsLayer& szLayer = szMod[lay];
          Float_t cell1, cell2;
	  if( !szLayer.calcCrossedCells(hit1.getX(),hit1.getY(),hit1.getZ(),
                                        hit2.getX(),hit2.getY(),hit2.getZ(),
                                        cell1, cell2) ) continue;  // Track don't cross layer!          
          Float_t cell12  = (cell1+cell2)/2;
          listCrLayers |= 1<<ilay;

          Short_t nCells   = szLayer.getNCells();

          Int_t nFCell     = TMath::Max(Int_t(cell1 - 30.),0);
          Int_t nLCell     = TMath::Min(Int_t(cell2 + 30.),nCells-1);
	  Int_t nFirstCell = TMath::Max(Int_t(cell1 - dCellWind[mod]),nFCell);
 	  Int_t nLastCell  = TMath::Min(Int_t(cell2 + dCellWind[mod]),nLCell);

          Int_t lCells[nLCell-nFCell+1];
          Int_t   nCs   = 0;
          Int_t   cM    = -1;
          Float_t dCell = 1.e+30;
	  for(Int_t cell=nFCell; cell<=nLCell;cell++) {
	    locCal1.set(4,sec,mod,lay,cell);
	    HMdcCal1* cal1 = (HMdcCal1*)pCatMdcCal1->getObject(locCal1);
	    if(cal1 == NULL) continue;
            Float_t dC = Float_t(cell) - cell12;
            // Fill histograms:
            if(makeHists) {
              hsDCell[sec][mod]->Fill(dC);
              hsDCellL[sec][mod][lay]->Fill(dC);
            }
            if(cell>=nFirstCell && cell<=nLastCell) {
              lCells[nCs] = cell;
              if(TMath::Abs(dC) < dCell) {
                dCell = TMath::Abs(dC);
                cM    = nCs;
              }
              nCs++;
            }
	  }
          if(nCs>0) {
            Int_t layInd = lay+(mod%2)*6;
            listCells.setTime(layInd, lCells[cM], 1);
            // Collecting of wires around "lCells[cM]":
            for(Int_t i=cM-1;i>=0 &&lCells[i+1]-lCells[i]==1;i--) listCells.setTime(layInd,lCells[i],1);
            for(Int_t i=cM+1;i<nCs&&lCells[i]-lCells[i-1]==1;i++) listCells.setTime(layInd,lCells[i],1);
          }
	}
      }
      //???      if(listCells.getNCells() == 0) continue;

      prevClus = fillCluster(sec,seg,listCells,prevClus);
      if(prevClus != NULL) prevClus->setStatus(listCrLayers);  // List of layers crossed by track
    }
  }

  return bestCand.trackStatus(); // Test track:
}


Bool_t HMdcCosmicsCandidate::calcSectors(TrackCand &trackCand) {
  MetaHit &mHit1 = metaHits[trackCand.metaInd1];
  MetaHit &mHit2 = metaHits[trackCand.metaInd2];
  if(mHit1.sec == mHit2.sec) return kFALSE;
  testSectors(trackCand,mHit1.sec,0);
  testSectors(trackCand,mHit2.sec,1);
  Int_t dSec = TMath::Abs(mHit1.sec-mHit2.sec);
  if(dSec == 3) {
    testSectors(trackCand,mHit1.sec-1,0);
    testSectors(trackCand,mHit2.sec-1,1);
    testSectors(trackCand,mHit1.sec+1,0);
    testSectors(trackCand,mHit2.sec+1,1); 
  } else if(dSec == 2 || dSec==4) {
    Int_t sec = TMath::Abs(mHit1.sec+mHit2.sec)/2;
    if(dSec == 4) sec += 3;
    testSectors(trackCand,sec,0);
  }
  return kTRUE;
}

void HMdcCosmicsCandidate::testSectors(TrackCand &trackCand,Int_t sec,Int_t part) {
  if     (sec < 0) sec = 5;
  else if(sec > 5) sec = 0;
  part &= 1;
  if( !pGetCont->isSecActive(sec) ) return;
  HMdcSizesCellsSec&  scSec = (*sizes)[sec];
  MetaHit &metaHit1 = metaHits[trackCand.metaInd1];
  MetaHit &metaHit2 = metaHits[trackCand.metaInd2];
  hit1 = scSec.getLabTrans()->transTo(metaHit1.hit);  // from lab to sec.coor.sytem
  hit2 = scSec.getLabTrans()->transTo(metaHit2.hit);  // from lab to sec.coor.sytem

  for(Int_t mod=0;mod<4;mod++) if(nWiresStat[sec][mod] > 0 && isCrossMod(sec,mod)) {
    trackCand.addMod(sec,mod,nWiresStat[sec][mod]);
  }  
}

Bool_t HMdcCosmicsCandidate::isCrossMod(Int_t sec,Int_t mod) {
  if(mdcsetup[sec][mod]==0) return kFALSE;  // No this mdc in setup
  HMdcSizesCellsMod &scMod = (*sizes)[sec][mod];
  Double_t x1 = hit1.getX();
  Double_t y1 = hit1.getY();
  Double_t z1 = hit1.getZ();
  Double_t x2 = hit2.getX();
  Double_t y2 = hit2.getY();
  Double_t z2 = hit2.getZ();
  HMdcSizesCellsLayer &pScLay = scMod[3];
  pScLay.transTo(x1,y1,z1);
  pScLay.transTo(x2,y2,z2);
  LayerSize &laySz = laySize[mod];
  Double_t yc = -z2/(z1-z2)*(y1-y2)+y2;
  if(yc < laySz.y1 || yc > laySz.y2) return kFALSE;
  Double_t xc = TMath::Abs(x2-z2/(z1-z2)*(x1-x2));
  Double_t xu = (yc-laySz.y2)*laySz.inc+laySz.x2;
  if(xc > xu) return kFALSE;
  return kTRUE;
}

Bool_t HMdcCosmicsCandidate::testTrack(void) {
  MetaHit &mHit1 = metaHits[currTrCand.metaInd1];
  MetaHit &mHit2 = metaHits[currTrCand.metaInd2];
  //----------  if( !testBeta(mHit1,mHit2) ) return kFALSE;
  Double_t dCMean = 0.;
  for(Int_t sec=0;sec<6;sec++) if(currTrCand.secStatus(sec)) {
    HMdcSizesCellsSec&  scSec = (*sizes)[sec];
    hit1 = scSec.getLabTrans()->transTo(mHit1.hit);  // transf. lab => sec.coor.sytem
    hit2 = scSec.getLabTrans()->transTo(mHit2.hit);  // transf. lab => sec.coor.sytem
    for(Int_t mod=0; mod<4;mod++) if(currTrCand.modStatus(sec,mod) != 0) {
      HMdcSizesCellsMod& scMod = scSec[mod];
      Int_t modLaySt = 0;
      for(Int_t lay=0;lay<6;lay++) {
        HMdcSizesCellsLayer& szLayer = scMod[lay];
        Float_t cell1, cell2;
        if( !szLayer.calcCrossedCells(hit1.getX(),hit1.getY(),hit1.getZ(),
             hit2.getX(),hit2.getY(),hit2.getZ(),cell1, cell2) ) continue;  // Track don't cross layer!
        Float_t cell12   = (cell1+cell2)/2;
        Short_t nCells   = szLayer.getNCells();
        Int_t nFirstCell = TMath::Max(Int_t(cell1 - dCellWind[mod]),0);
        Int_t nLastCell  = TMath::Min(Int_t(cell2 + dCellWind[mod]),nCells-1);

        Int_t lCells[nLastCell-nFirstCell+1];
        Int_t   nCs   = 0;
        Int_t   cM    = -1;
        Float_t dCell = 1.e+30;
        for(Int_t cell=nFirstCell; cell<=nLastCell;cell++) {
          locCal1.set(4,sec,mod,lay,cell);
          HMdcCal1* cal1 = (HMdcCal1*)pCatMdcCal1->getObject(locCal1);
          if(cal1 == NULL) continue;
          Float_t dC = TMath::Abs(Float_t(cell) - cell12);
          lCells[nCs] = cell;
          if(dC < dCell) {
            dCell = dC;
            cM    = nCs;
          }
          nCs++;
        }
        if(nCs>0) {
          currTrCand.nWires++;
          for(Int_t i=cM-1;i>=0 &&lCells[i+1]-lCells[i]==1;i--) currTrCand.nWires++;
          for(Int_t i=cM+1;i<nCs&&lCells[i]-lCells[i-1]==1;i++) currTrCand.nWires++;
          dCMean += dCell;
          modLaySt++;
        }
      }
      if(modLaySt==0) currTrCand.unsetMod(sec,mod);
      else {
        currTrCand.nLaySec[sec] += modLaySt;
        currTrCand.nLayers      += modLaySt;
        currTrCand.nModSec[sec]++;
        currTrCand.nModules++;
      }
    }
  }
  if(!currTrCand.trackStatus()) return kFALSE;
  currTrCand.dCellMean = dCMean/currTrCand.nLayers;
  return kTRUE;
}

HMdcClus* HMdcCosmicsCandidate::fillCluster(Int_t sec,Int_t seg,HMdcList12GroupCells &listCells,
    HMdcClus* prevClus) {
  locClus.set(2,sec,seg);
  HMdcClus* fClus=(HMdcClus*)pCatMdcClus->getNewSlot(locClus, &index);
  if(fClus == NULL) return fClus;
  if(isGeant) fClus = (HMdcClus*)(new(fClus) HMdcClusSim(listCells));
  else        fClus =             new(fClus) HMdcClus   (listCells);      

  fClus->setOwnIndex(index);
  if(prevClus != NULL)  {
    prevClus->setIndexChilds(index,index);
    fClus->setIndexParent(prevClus->getOwnIndex());
  }
  
  fClus->setSec(sec);
  fClus->setIOSeg(seg);
  fClus->setMod(-1);

  fClus->setTarget(hit1.getX(),20.,hit1.getY(),30.,hit1.getZ(),30.); 
  fClus->setXY    (hit2.getX(),20.,hit2.getY(),30.);
  fClus->setPrPlane(0., 0., hit2.getZ());       // If parA=parB=0 then parD=Z
  if(isGeant) ((HMdcClusSim*)fClus)->calcTrList();
  // Cleaning:
  fClus->setMinCl(0,0);
  fClus->setRealLevel(0);
  fClus->setOwnIndex(index);
  fClus->setNBins(0);
//fClus->print(); //--------------------------------------------------!!!!!!!!!!!!!!!!!!!!!!!!
  return fClus;
}
  
void HMdcCosmicsCandidate::setHistFile(const Char_t* dir,const Char_t* fileNm,const Char_t* opt) {
  // the hist output file is set

  if(dir != NULL)                       histFileName   = dir;
  else                                  histFileName   = "./";
  if( !histFileName.EndsWith("/") )     histFileName  += '/';
  if(fileNm != NULL)                    histFileName  += fileNm;
  else                                  histFileName  += "cosmCandHists";
  if(histFileName.EndsWith(".hld"))     histFileName.Remove(histFileName.Length()-4);
  if( !histFileName.EndsWith(".root") ) histFileName  += ".root";
  if(opt != NULL)                       histFileOption = opt;
}

Bool_t HMdcCosmicsCandidate::createHists(void) {
  histFile = new TFile(histFileName.Data(),histFileOption.Data());
  if(histFile==NULL || !histFile->IsOpen()) {
    Error("createHists","Can't open file %s (opt=%s)!",histFileName.Data(),histFileOption.Data());
    return kFALSE;
  }
  char title[100];
  char name[100];
  if(hsBeta==0) {
    sprintf(title,"Cut: %.2f < #beta < %.2f;#beta",betaMin,betaMax);
    hsBeta= new TH1F("hsBeta",title,300,0.,1.5);
  }
  if(plStat==0) plStat= new TH2F("plStat",
    "Statistic;Sector of first tof;Sector of second tof",6,0.,6., 6,0.,6.);
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) if(mdcsetup[s][m]!=0 && hsDCell[s][m]==0) {
    sprintf(name,"hsDCellS%iM%i",s+1,m+1);
    sprintf(title,"S.%i M.%i Cut: #Delta Cell <= %.1f;#Delta Cell",s+1,m+1,dCellWind[m]);
    hsDCell[s][m] = new TH1F(name,title,300,-30.,30.);
    for(Int_t l=0;l<6;l++) {
      sprintf(name,"hsDCellS%iM%iL%i",s+1,m+1,l+1);
      sprintf(title,"S.%i M.%i L.%i Cut: #Delta Cell <= %.1f;#Delta Cell",s+1,m+1,l+1,dCellWind[m]);
      hsDCellL[s][m][l] = new TH1F(name,title,300,-30.,30.);
    }
  }
  if(plQualVsWrLo==0) plQualVsWrLo = new TH2F("plQualVsWrLo","",50,0.,50.,50,0.,1.0001);
  return kTRUE;
}

Bool_t HMdcCosmicsCandidate::finalize(void) {
  printf("\n-------------------------------------------------\n");
  if(nEventsSel == nEventsSelSc) {
    for(Int_t i=0;i<9;i++) if(nSelTr[i] > 0) {
      printf("%i modules: %10i tracks (%7.2f%c)\n",
      i,nSelTr[i],Float_t(nSelTr[i])/Float_t(nEventsSel)*100.,'%');
    }
    printf("  %i events filtered from %i.\n",nEventsSel,nEventsTot);
  } else {
    for(Int_t i=0;i<9;i++) if(nSelTr[i] > 0) {
      Double_t sc = trackScaling[i] < 1. ? trackScaling[i]*100 : 100;
      printf("%i modules: %10i tracks (%7.2f%c) ==> scaling %6.2f%c => %10i tracks (%7.2f%c)\n",
      i,nSelTr[i],Float_t(nSelTr[i])/Float_t(nEventsSel)*100.,'%',sc,'%',
        nSelTrSc[i],Float_t(nSelTrSc[i])/Float_t(nEventsSelSc)*100.,'%');
    }
    printf("  %i events filtered from %i.\n",nEventsSelSc,nEventsTot);
  }
  printf("-------------------------------------------------\n");
  if(makeHists) {
    printf("  Histograms are stored to the file %s\n",histFileName.Data());
    // writing hists to the root file
    histFile->cd();
    if(hsBeta       != NULL) hsBeta->Write();
    if(plStat       != NULL) plStat->Write();
    if(plQualVsWrLo != NULL) plQualVsWrLo->Write();
    for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) if(hsDCell[s][m]!=NULL) {
      hsDCell[s][m]->Write();
      for(Int_t l=0;l<6;l++) hsDCellL[s][m][l]->Write();
    }
    histFile->Close();
  }
  return kTRUE;
}


void HMdcCosmicsCandidate::scaleDownTrack(UInt_t nMdcTr,Double_t sc) {
  if(nMdcTr<8) trackScaling[nMdcTr] = sc;
}

void HMdcCosmicsCandidate::scaleDownTrack(Double_t *sca) {
  for(Int_t nMdcs=0;nMdcs<9;nMdcs++) trackScaling[nMdcs] = sca[nMdcs];
}

HCosmicCalibEvSkip::HCosmicCalibEvSkip(void) 
    : HReconstructor("CosmicCalibEvSkip","CosmicCalibEvSkip") {
}

HCosmicCalibEvSkip::HCosmicCalibEvSkip(const Text_t *name,const Text_t *title) 
    : HReconstructor(name,title) {

}

Int_t HCosmicCalibEvSkip::execute(void) {
  return kSkipEvent;
}

