//*-- AUTHOR : Vladimir Pechenov
//*-- Modified : 02/02/07

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////
//
// HMdcGeantSeg
//
//   Class collect and keep GEANT and mdc digitized information about
//   one inner or outer mdc segment
//
//
// HMdcGeantTrack
//
//   Class collect and keep GEANT and mdc digitized information about
//   one track. Keep also array of HMdcGeantSeg objects for this track.
//
//
// HMdcGeantEvent
//
//   Class fill and keep array of HMdcGeantTrack objects for one event.
//
//
// HMdcTrackInfSim
//
//   Class fill HMdcClusSim, HMdcSegSim and HMdcHitSim objects
//   by information from HMdcGeantTrack and HMdcGeantSeg
//
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Meaning of HMdcGeantTrack::trackStatus, HMdcGeantSeg::segmentStatus
// and HMdcClusSim::trackStatus[tr] bits.
//
// Bits in HMdcGeantTrack::trackStatus:
// ------------------------------------
//  Bit 8, GEANT_BUG          : =0 - GEANT bug was found in this track
//  Bit 7, NO_META_HIT        : =0 - track don't reach META
//  Bit 6, NOT_RECONSTRUCTABLE: =0 - not reconstructable track. It can be if
//                                   track not present in mdc,
//                                   track cross more then one sector,
//                                   numder of segments <1 or >2,
//                                   upstream direction of at list one segment,
//                                   track doesn't cross inner or outer segment,
//                                   num. of segments with digi.wires <1 or >2,
//                                   num.digi.wires in track < 5*num.segments,
//                                   at list one seg. has <3 layers or <5 wires
//  Bits 1-5 are eq.1
//
// Bits in HMdcGeantSeg::segmentStatus trackStatus bits plus:
// ----------------------------------------------------------
//  Bit 5, SEG_NOT_RECONS_ABLE: =0 - segment has <3 layers or <5 digi.wires
//  Bit 4, HIT_NOT_RECONS_ABLE: =0 - at list one mdc in segment has
//                                   <3 fired layers or <5 wires
//
// Bits in HMdcClusSim::trackStatus[tr] are segmentStatus bits plus:
// -----------------------------------------------------------------
//  Bit 3, CLFLEVEL_TOO_HIGH  : =0 - segment can't be found due to high level
//                                   of cluster finder
//  Bit 2, FAKE_CONTRIBUTION  : =0 - fake contribution in cluster
//  Bit 1, SEGS_MATH_OK       : =0 - no inner-outer segments matching
////////////////////////////////////////////////////////////////

using namespace std;
#include "hmdcgeanttrack.h"
#include "hades.h"
#include "hevent.h"
//#include "hcategory.h"
#include "hmatrixcategory.h"
#include "hiterator.h"
#include "hmdcsizescells.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hmdccal1sim.h"
#include "hmdcdef.h"
#include "hphysicsconstants.h"
#include "hspectrometer.h"
#include "hmdclistcells.h"
#include "hmdcclussim.h"
#include "hmdcsegsim.h"
#include "hmdchitsim.h"
#include "hmdcgetcontainers.h"
#include "hmdcwiredata.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

ClassImp(HMdcGeantSeg)

ClassImp(HMdcGeantTrack)

ClassImp(HMdcGeantEvent)


#define DEFAULT_SET         255
#define SEGS_MATH_OK          1
#define GEANT_BUG           127
#define NO_META_HIT         191
#define NOT_RECONSTRUCTABLE 223
#define SEG_NOT_RECONS_ABLE 239
#define HIT_NOT_RECONS_ABLE 247
#define CLFLEVEL_TOO_HIGH   251
#define FAKE_CONTRIBUTION   253

HMdcGeantSeg::HMdcGeantSeg(Short_t ns) {
  clear(ns);
}

void HMdcGeantSeg::clear(Short_t ns) {
  nHitsMdc[0]    = 0;
  nHitsMdc[1]    = 0;
  segNumber      = ns;
  sec            = -1;
  mod            = -1;
  pClusBest      = NULL;
  pClusBestCh[0] = NULL;
  pClusBestCh[1] = NULL;
  pSegBest       = NULL;
  trackNumber    = -1;
  userFlag       = 0;
  for(Int_t m=0;m<2;m++) for(Int_t l=0;l<7;l++) geantLay[m][l]=0;
  HMdcList12GroupCells::clear();
}

void HMdcGeantSeg::addFirstHit(HGeantMdc* pGeantMdc,Bool_t* mdcSecSetup,Short_t ns) {
  for(Int_t m=0;m<2;m++) for(Int_t l=0;l<7;l++) geantLay[m][l]=0;
  HMdcList12GroupCells::clear();
  mod        = pGeantMdc->getModule();
  ioseg      = mod >> 1;
  Int_t modI = mod & 1;
  Int_t lay  = pGeantMdc->getLayer();
  sec        = pGeantMdc->getSector();
  segNumber  = ns;
  nHitsMdc[0]=nHitsMdc[1]=0;
  if(lay!=6) nHitsMdc[modI]++;
  geantLay[modI][lay] = pGeantMdc;
  direction = dirHit(pGeantMdc);
  if(mdcSecSetup[ioseg*2] && mdcSecSetup[ioseg*2+1]) mod = -2;
}

Char_t HMdcGeantSeg::dirHit(HGeantMdc* pGeantMdc) {
  Float_t ath, aph;
  pGeantMdc->getIncidence(ath,aph);
  return dirTheta(ath);
}

Bool_t HMdcGeantSeg::addHit(HGeantMdc* pGeantMdc) {
  if(pGeantMdc->getSector() != sec) return kFALSE;
  Char_t mod  = pGeantMdc->getModule();
  if(mod>>1 != ioseg) return kFALSE;
  if(direction != dirHit(pGeantMdc)) return kFALSE;
  Int_t modI = mod & 1;
  Int_t lay  = pGeantMdc->getLayer();
  if(geantLay[modI][lay]) return kFALSE;
  geantLay[modI][lay] = pGeantMdc;
  if(lay!=6) nHitsMdc[modI]++;
  return kTRUE;
}

Int_t HMdcGeantSeg::getModIOfGeantTrack(void) const {
  // return -2 if track present in both mdc of segment
  // otherwise return mdc index (0 or 1)
  if(nHitsMdc[0]>0 && nHitsMdc[1]>0) return -2;
  if(nHitsMdc[0]>0) return 0;
  if(nHitsMdc[1]>0) return 1;
  return -500;
}

void HMdcGeantSeg::setAnotherHit(HGeantMdc* pGeantMdc) {
  Int_t modI = pGeantMdc->getModule() & 1;  // =0 or 1 !
  Int_t lay  = pGeantMdc->getLayer();
  if(geantLay[modI][lay]) geantLay[modI][lay] = pGeantMdc;
}


Int_t HMdcGeantSeg::getFirstLayer12(void) const {
  if(nHitsMdc[0]) {
    for(Int_t l=0;l<6;l++) if(geantLay[0][l]) return l;
  } else for(Int_t l=0;l<6;l++) if(geantLay[1][l]) return l+6;
  return -1;
}

Int_t HMdcGeantSeg::getLastLayer12(void) const {
  if(nHitsMdc[1]) {
    for(Int_t l=5;l>=0;l--) if(geantLay[1][l]) return l+6;
  } else for(Int_t l=5;l>=0;l--) if(geantLay[0][l]) return l;
  return -1;
}

HGeantMdc* HMdcGeantSeg::getMdcLayerHit(Int_t m,Int_t l) {
  return (nMdcOk(m) && l>=0 && l<7) ? geantLay[m&1][l]:0;
}

Int_t HMdcGeantSeg::getNGMdcs(void) const {
  if(nHitsMdc[0]>0 && nHitsMdc[1]>0)   return 2;
  if(nHitsMdc[0]==0 && nHitsMdc[1]==0) return 0;
  return 1;
}
  
void HMdcGeantSeg::print(void) {
  printf("%3i) %i sec. %i seg.   GeantMdc:%4i hits (layers ",
      segNumber,sec+1,ioseg+1,getNGMdcHits());
  if(direction>0) printf("%2i -> %2i)",getFirstLayer12()+1,getLastLayer12()+1);
  else            printf("%2i -> %2i)",getLastLayer12()+1,getFirstLayer12()+1);
  if(areWiresColl) printf("   MdcCal1:%4i hits in %2i layers",
      getNDrTimes(),getNLayers());
  printf("\n");
}

Bool_t HMdcGeantSeg::getLayerHitPos(Int_t m, Int_t l, HGeomVector& hit,Bool_t extrFlag) {
  // Return geant "hit" position in mdc layer.
  // If no GeantMdc hit in this layer and extrFlag=kTRUE
  // function does extrapolation to this layer.
  if(!nMdcOk(m) || l<0 || l>5) return kFALSE;
  Int_t modI = m & 1;
  if(geantLay[modI][l]) {
    Float_t ax,ay,tof,momLay;
    geantLay[modI][l]->getHit(ax,ay,tof,momLay);
    hit.setXYZ(ax,ay,0.);
    return kTRUE;
  }
  if(!extrFlag) return kFALSE;
  //================= If no GeantMdc hit in layer "l": =================
  if(nHitsMdc[modI]<1 || (nHitsMdc[modI]==1 && geantLay[modI][6]==0)) return kFALSE; // must be two hits at list.
  Int_t layLFirst,layLSecond,layRFirst,layRSecond;
  layLFirst=layLSecond=layRFirst=layRSecond=-1;
  for(Int_t lay=l-1;lay>=0; lay--) {
    if(l>2 && lay<3 && geantLay[modI][6]) {
      if(layLFirst<0) layLFirst=6;
      else if(layLSecond<0 && layLFirst!=6) layLSecond=6;
    }
    if(geantLay[modI][lay]) {
      if(layLFirst<0) layLFirst=lay;
      else if(layLSecond<0) layLSecond=lay;
    }
  }
  for(Int_t lay=l+1;lay<6; lay++) {
    if(l<3 && lay>2 && geantLay[modI][6]) {
      if(layRFirst<0) layRFirst=6;
      else if(layRSecond<0 && layRFirst!=6) layRSecond=6;
    }
    if(geantLay[modI][lay]) {
      if(layRFirst<0) layRFirst=lay;
      else if(layRSecond<0) layRSecond=lay;
    }
  }
  if(layLFirst<0)      calcMdcHitPos(modI,layRFirst,  layRSecond, hit,l);
  else if(layRFirst<0) calcMdcHitPos(modI,layLSecond, layLFirst,  hit,l);
  else                 calcMdcHitPos(modI,layLFirst,  layRFirst,  hit,l);
  return kTRUE;
}

Int_t HMdcGeantSeg::getFirstGeantMdcLayer(Int_t m) const {
  // Return first layer with geant hit in mdc "m".
  if(!nMdcOk(m)) return -1;
  Int_t modI = m & 1;
  for(Int_t lay=0;lay<6;lay++) if(geantLay[modI][lay]) return lay;
  return -1;
}

Int_t HMdcGeantSeg::getLastGeantMdcLayer(Int_t m) const {
  // Return first layer with geant hit in mdc "m".
  if(!nMdcOk(m)) return -1;
  Int_t modI = m & 1;
  for(Int_t lay=5;lay>=0;lay--) if(geantLay[modI][lay]) return lay;
  return -1;
}

Bool_t HMdcGeantSeg::getFirstAndLastGMdcLayers(Int_t m,Int_t& lFisrt,Int_t& lLast) const {
  // return kFALSE if "m" is not valid or one mdc geant hit exist only.
  if(!nMdcOk(m)) return kFALSE;
  Int_t modI = m & 1;
  if(nHitsMdc[modI]<2) return kFALSE;
  for(lFisrt=0;lFisrt<6;lFisrt++) if(geantLay[modI][lFisrt]) break;
  for(lLast=5; lLast>=0;lLast--)  if(geantLay[modI][lLast])  break;
  return kTRUE;
}

Bool_t HMdcGeantSeg::getMdcHitPos(Int_t m, HGeomVector& hit) {
  // Return geant "hit" position in mdc mid-plane.
  if(!nMdcOk(m)) return kFALSE;
  Int_t modI = m & 1;
  if(geantLay[modI][6]) {
    Float_t ax,ay,tof,momLay;
    geantLay[modI][6]->getHit(ax,ay,tof,momLay);
    hit.setXYZ(ax,ay,0.);
    return kTRUE;
  }
  //=============== If no GeantMdc hit in mdc mid-plane: ===============
  if(nHitsMdc[modI]<2) return kFALSE;
  Int_t layLFirst,layLSecond,layRFirst,layRSecond;
  layLFirst=layLSecond=layRFirst=layRSecond=-1;
  for(Int_t lay=2;lay>=0; lay--) if(geantLay[modI][lay]) {
    if(layLFirst<0)       layLFirst  = lay;
    else if(layLSecond<0) layLSecond = lay;
  }
  for(Int_t lay=3;lay<6; lay++) if(geantLay[modI][lay]) {
    if(layRFirst<0)       layRFirst  = lay;
    else if(layRSecond<0) layRSecond = lay;
  }
  if(layLFirst<0)      calcMdcHitPos(modI,layRFirst,  layRSecond, hit);
  else if(layRFirst<0) calcMdcHitPos(modI,layLSecond, layLFirst,  hit);
  else                 calcMdcHitPos(modI,layLFirst,  layRFirst,  hit);
  return kTRUE;
}

void HMdcGeantSeg::calcMdcHitPos(Int_t modI,Int_t lay1, Int_t lay2,HGeomVector& hit, Int_t lay) {
  // Linear extrapolation of two geant mdc hits to the mdc mid-plane.
  // Not precise solution but well enough!
  Float_t x1,y1,x2,y2,tof,momLay;
  geantLay[modI][lay1]->getHit(x1,y1,tof,momLay);
  geantLay[modI][lay2]->getHit(x2,y2,tof,momLay);
  Float_t l0 = (lay==6)  ? 2.5:lay;
  Float_t l1 = (lay1==6) ? 2.5:lay1;
  Float_t l2 = (lay2==6) ? 2.5:lay2;
  Double_t norm=(l0-(l1))/(l2-l1);
  hit.setXYZ(x1+(x2-x1)*norm,y1+(y2-y1)*norm,0.);
}

Bool_t HMdcGeantSeg::getMdcHitPosSec(Int_t m, HGeomVector& hit) {
  // Return geant "hit" position in mid-plane mdc in sector coor.sys.
  HMdcSizesCells* pMdcSizesCells=HMdcSizesCells::getExObject();
  if(pMdcSizesCells==0) return kFALSE;
  if(!getMdcHitPos(m,hit)) return kFALSE;
  HMdcSizesCellsSec& pSCSec = (*pMdcSizesCells)[sec];
  HMdcSizesCellsMod& pSCMod = pSCSec[m];
  hit=pSCMod.getSecTrans()->transFrom(hit);
  return kTRUE;
}

Bool_t HMdcGeantSeg::getMdcHitPosLab(Int_t m, HGeomVector& hit) {
  // Return geant "hit" position in mid-plane mdc in lab.coor.sys..
  HMdcSizesCells* pMdcSizesCells=HMdcSizesCells::getExObject();
  if(pMdcSizesCells==0) return kFALSE;
  if(!getMdcHitPos(m,hit)) return kFALSE;
  HMdcSizesCellsSec& pSCSec = (*pMdcSizesCells)[sec];
  HMdcSizesCellsMod& pSCMod = pSCSec[m];
  hit = pSCMod.getSecTrans()->transFrom(hit);
  hit = pSCSec.getLabTrans()->transFrom(hit);
  return kTRUE;
}

Bool_t HMdcGeantSeg::getLayerHitPosSec(Int_t m, Int_t l,HGeomVector& hit,Bool_t extrFlag) {
  // Return geant "hit" position in mdc layer in sector coor.sys..
  HMdcSizesCells* pMdcSizesCells=HMdcSizesCells::getExObject();
  if(pMdcSizesCells==0) return kFALSE;
  if(!getLayerHitPos(m,l,hit,extrFlag)) return kFALSE;
  HMdcSizesCellsSec&   pSCSec = (*pMdcSizesCells)[sec];
  HMdcSizesCellsLayer& pSCLay = pSCSec[m][l];
  hit = pSCLay.getSecTransGeant().transFrom(hit);
  return kTRUE;
}

Bool_t HMdcGeantSeg::getLayerHitPosLab(Int_t m, Int_t l, HGeomVector& hit,
    Bool_t extrFlag) {
  // Return geant "hit" position in mdc layer in lab.coor.sys.
  // If no GeantMdc hit in this layer and extrFlag=kTRUE
  // function does extrapolation to this layer.
  HMdcSizesCells* pMdcSizesCells = HMdcSizesCells::getExObject();
  if(pMdcSizesCells==0) return kFALSE;
  if(!getLayerHitPos(m,l,hit,extrFlag)) return kFALSE;
  HMdcSizesCellsSec&   pSCSec = (*pMdcSizesCells)[sec];
  HMdcSizesCellsLayer& pSCLay = pSCSec[m][l];
  hit = pSCLay.getSecTransGeant().transFrom(hit);
  hit = pSCSec.getLabTrans()->transFrom(hit);
  return kFALSE;
}

void HMdcGeantSeg::setStatusFlags(UChar_t& trackStatus) {
  segmentStatus = trackStatus;
  if( !areWiresColl ) return;
  if(getNLayers()<3 || getNDrTimes()<5) {
    trackStatus   &= NOT_RECONSTRUCTABLE;
    segmentStatus  = trackStatus;
    segmentStatus &= SEG_NOT_RECONS_ABLE;
  }
  if(HMdcGeantTrack::isMdcActive(sec,ioseg*2)   && (getNLayersMod(0)<3 ||
     getNDrTimesMod(0)<5)) segmentStatus &= HIT_NOT_RECONS_ABLE;
  if(HMdcGeantTrack::isMdcActive(sec,ioseg*2+1) && (getNLayersMod(1)<3 ||
     getNDrTimesMod(1)<5)) segmentStatus &= HIT_NOT_RECONS_ABLE;
}

void HMdcGeantSeg::analyseClust(HMdcClusSim* pClus,Int_t trInd,Int_t modICl) {
  // Analyse one mod. of seg. if track present in one mod. only or it is mixed cl.:
  Int_t                                         mInd = -2;
  if(pClus->getNLayersInTrack(trInd,0)==0)      mInd = 1;      //Track pres.in one mdc
  else if(pClus->getNLayersInTrack(trInd,1)==0) mInd = 0;      //Track pres.in one mdc
  else if(modICl>=0)                            mInd = modICl; //Mixed cl.finder

  UChar_t status = segmentStatus;
  if( !pClus->isSegmentAmpCut() ) {
    if(pClus->getMinCl(0) > getNLayersMod(0) ||
       pClus->getMinCl(1) > getNLayersMod(1))   status &= CLFLEVEL_TOO_HIGH;
  } else if(pClus->getMinCl() > getNLayers())   status &= CLFLEVEL_TOO_HIGH;

//  if(pClus->getNLayOrientation(trInd) < 2)      status &= FAKE_CONTRIBUTION;
  if( !pClus->is40degCross(trInd) )             status &= FAKE_CONTRIBUTION;
  else if(mInd < 0) {
         if(!isSegClusBetter(pClus,trInd))      status &= FAKE_CONTRIBUTION;
  } else if(!isModClusBetter(pClus,trInd,mInd)) status &= FAKE_CONTRIBUTION;
  pClus->setTrackStatus(trInd,status);
}

Bool_t HMdcGeantSeg::isSegClusBetter(HMdcClusSim* pClus,Int_t trInd) {
  Int_t flag = setSegClustPos(pClus,trInd);
  if(flag) {
    if(flag == 1) return kTRUE;   // Address mismatching or no HMdcSizesCells object
    if(flag == 2) return kFALSE;  // Number layers in cluster <2 from track "trInd"
    if(flag > 2)  return kTRUE;   // Not enough geant mdc hits and others
  }

  Int_t   nLayersCl = pClus->getNLayersInTrack(trInd);
  UChar_t nWiresCl  = pClus->getNTimesInTrack(trInd);
  if(pClusBest != NULL) {
    if(nLayers > nLayersCl) return kFALSE; // fake contribution
    if(nLayers == nLayersCl) {
      if(nWires > nWiresCl) return kFALSE; // fake contrirution
      if(nWires == nWiresCl) {
        Float_t dXn = pClus->dX(trInd);    // Xgeant - pClus->getX();
        Float_t dYn = pClus->dY(trInd);    // Ygeant - pClus->getY();
        if(dXn/2.7*dXn/2.7+dYn*dYn > dX/2.7*dX/2.7+dY*dY) return kFALSE;
      }
    }
    pClusBest->setTrackStatus(trIndBest,pClusBest->getTrackStatus(trIndBest) & FAKE_CONTRIBUTION);
  } else if(pClusBestCh[0] != NULL || pClusBestCh[1] != NULL) {
    // Comparing segement and chamber/mixed clusters:
    for(Int_t m=0;m<2;m++) if(pClusBestCh[m]) {
      if(nLayersCh[m] > nLayersCl)                        return kFALSE;
      if(nLayersCh[m]==nLayersCl && nWiresCh[m]>nWiresCl) return kFALSE;
    }
    // Segmet cluster better:
    if(pClusBestCh[0]) pClusBestCh[0]->setTrackStatus(trIndBestCh[0],
        pClusBestCh[0]->getTrackStatus(trIndBestCh[0]) & FAKE_CONTRIBUTION);
    if(pClusBestCh[1]) pClusBestCh[1]->setTrackStatus(trIndBestCh[1],
        pClusBestCh[1]->getTrackStatus(trIndBestCh[1]) & FAKE_CONTRIBUTION);
    pClusBestCh[0] = pClusBestCh[1] = 0;
  }

  pClusBest = pClus;
  trIndBest = trInd;
  nLayers   = nLayersCl;
  nWires    = nWiresCl;
  dX        = pClus->dX(trInd); //x - pClus->getX();
  dY        = pClus->dY(trInd); //y - pClus->getY();
  return kTRUE;
}

Bool_t HMdcGeantSeg::isModClusBetter(HMdcClusSim* pClus,Int_t trInd,Int_t mInd) {
  Int_t flag = setModClustPos(pClus,trInd,mInd);
  if(flag) {
    if(flag == 1) return kTRUE;   // Address mismatching or no HMdcSizesCells object
    if(flag == 2) return kFALSE;  // Number layers in cluster <2 from track "trInd"
    if(flag >  2) return kTRUE;   // Not enough geant mdc hits and others
  }

  Int_t   nLayersCl = pClus->getNLayersInTrack(trInd);
  UChar_t nWiresCl  = pClus->getNTimesInTrack(trInd);

  if(pClusBest) { // if exist track segment clusters:
    if(nLayers  > nLayersCl)                    return kFALSE;
    if(nLayers==nLayersCl && nWires>=nWiresCl)  return kFALSE;
    pClusBest->setTrackStatus(trIndBest,pClusBest->getTrackStatus(trIndBest) & FAKE_CONTRIBUTION);
    pClusBest = 0;
  } else if(pClusBestCh[mInd]) {
    if(nLayersCh[mInd] > nLayersCl) return kFALSE; // fake contribution
    if(nLayersCh[mInd] == nLayersCl) {
      if(nWiresCh[mInd] > nWiresCl) return kFALSE; // fake contrirution
      if(nWiresCh[mInd] == nWiresCl) {
        Float_t dXn = pClus->dX(trInd); //x - pClus->getX();
        Float_t dYn = pClus->dY(trInd); //y - pClus->getY();
        if(dXn/2.7*dXn/2.7+dYn*dYn > 
           dXCh[mInd]/2.7*dXCh[mInd]/2.7+dYCh[mInd]*dYCh[mInd]) return kFALSE;
      }
    }
    pClusBestCh[mInd]->setTrackStatus(trIndBestCh[mInd],
        pClusBestCh[mInd]->getTrackStatus(trIndBestCh[mInd])&FAKE_CONTRIBUTION);
  }
  pClusBestCh[mInd] = pClus;
  trIndBestCh[mInd] = trInd;
  nLayersCh[mInd]   = nLayersCl;
  nWiresCh[mInd]    = nWiresCl;
  dXCh[mInd]        = pClus->dX(trInd); //x - pClus->getX();
  dYCh[mInd]        = pClus->dY(trInd); //y - pClus->getY();

  if(pClusBestCh[0] && pClusBestCh[1]) {
     // Test for the same wires in two clusters:
     HMdcList12GroupCells lId1,lId2;
     compare(pClusBestCh[0],0,11, &lId1);
     compare(pClusBestCh[1],0,11, &lId2);
     Int_t n1  = lId1.getNDrTimes();
     Int_t n2  = lId2.getNDrTimes();
     Int_t n12 = lId1.nIdentDrTimes(&lId2);
     if(n1==n12 || n2==n12) {
       Int_t mRem = 0;
       if(n1 == n2) {
         if(dXCh[1]/2.7*dXCh[1]/2.7+dYCh[1]*dYCh[1] >
            dXCh[0]/2.7*dXCh[0]/2.7+dYCh[0]*dYCh[0]) mRem = 1;
       } else if(n2==n12) mRem = 1;
       if(mRem == mInd) {
         pClusBestCh[mRem]=0;
         return kFALSE;
       }
       pClusBestCh[mRem]->setTrackStatus(trIndBestCh[mRem],pClusBestCh[mRem]->
           getTrackStatus(trIndBestCh[mRem])&FAKE_CONTRIBUTION);
       pClusBestCh[mRem]=0;
    }
  }
  return kTRUE;
}

Int_t HMdcGeantSeg::setModClustPos(HMdcClusSim* pMdcClusSim, Int_t indtr,
    Int_t mInd) {
  // calculate geant track position on the project plane for chamber claster
  // return x,y and flag
  // flag = 0 - ok.
  // flag = 1 - address mismatching or no HMdcSizesCells object
  // flag = 2 - number layers in cluster <2 from track "trInd"
  // flag = 3 - not enough geant mdc hits and others
  if(pMdcClusSim->getSec() != sec) return 1;
  if(pMdcClusSim->getIOSeg() != ioseg) return 1;
  HMdcSizesCells* pMdcSizesCells=HMdcSizesCells::getExObject();
  if(pMdcSizesCells==0) return 1;
  if(pMdcClusSim->getNLayersInTrack(indtr,mInd) < 2) return 2; // ??????????????????

  HGeomVector hit;
  if(!getMdcHitPosSec(mInd+ioseg*2,hit)) return 3;
  Float_t x,y;
  pMdcClusSim->calcIntersection(hit,x,y);
  pMdcClusSim->setXYGeant(indtr,x,y);
  return 0;
// //----------------END???????????
// 
//   HGeomVector hit1;
//   HGeomVector hit2;
// Float_t dX1,dY1,dX2,dY2,dX4,dY4,dX,dY;
// dX1=dY1=dX2=dY2=dX=dY=-100000.;
// 
// 
//   if(mod>=0) {
//     if(!getMdcHitPosSec(mod,hit1)) return 3;
//     pMdcClusSim->calcIntersection(hit1,x,y);
// pMdcClusSim->print();
// printf("+++ dX3=%.1f dY3=%.1f \n",pMdcClusSim->getX()-x,pMdcClusSim->getY()-y);
//     return 0;
//   }
// 
// 
// pMdcClusSim->print();
// printf("+++=0! nLaysM1=%i nLaysM2=%i\n",pMdcClusSim->getNLayersInTrack(indtr,0),pMdcClusSim->getNLayersInTrack(indtr,1));
// //?    Int_t m = (nLaysM1>0) ? ioseg*2 : ioseg*2+1;
//     Int_t m = mInd + ioseg*2;
//     Int_t l1,l2;
//     if(!getFirstAndLastGMdcLayers(m,l1,l2)) return 3;
// printf("+++ m=%i l1=%i l2=%i\n",m,l1,l2);
//     if(!getLayerHitPosSec(m,l1,hit1,kFALSE)) return 3;
//     if(!getLayerHitPosSec(m,l2,hit2,kFALSE)) return 3;
// Float_t x1,y1,x2,y2;
// pMdcClusSim->calcIntersection(hit1,x1,y1);
// pMdcClusSim->calcIntersection(hit2,x2,y2);
// dX1=pMdcClusSim->getX()-x1;
// dX2=pMdcClusSim->getX()-x2;
// dY1=pMdcClusSim->getY()-y1;
// dY2=pMdcClusSim->getY()-y2;
// 
// //? if(nLaysM2==0) {if(!getMdcHitPosSec(ioseg*2,hit)) return 3;}
// //? else if(!getMdcHitPosSec(ioseg*2+1,hit)) return 3;
// if(!getMdcHitPosSec(m,hit)) return 3;
// pMdcClusSim->calcIntersection(hit,x1,y1);
// dX4=pMdcClusSim->getX()-x1;
// dY4=pMdcClusSim->getY()-y1;
// printf("+++ dX4=%.1f dY4=%.1f \n",pMdcClusSim->getX()-x1,pMdcClusSim->getY()-y1);
// 
//   pMdcClusSim->calcIntersection(hit1,hit2,x,y);
// dX=pMdcClusSim->getX()-x;
// dY=pMdcClusSim->getY()-y;
// printf("***  dX=%.1f  dY=%.1f\n",dX,dY);
// if(dX1>-90000.) printf("+++ dX1=%.1f dY1=%.1f \n+++ dX2=%.1f dY2=%.1f\n",dX1,dY1,dX2,dY2);
//   return 0;
}

Int_t HMdcGeantSeg::setSegClustPos(HMdcClusSim* pMdcClusSim, Int_t indtr) {
  // calculate geant track position on the project plane for segment claster
  // return flag:
  // flag = 0 - ok.
  // flag = 1 - address mismatching or no HMdcSizesCells object
  // flag = 2 - number layers in track "trInd" <2
  // flag = 3 - not enough geant mdc hits and others
  if(pMdcClusSim->getSec() != sec) return 1;
  if(pMdcClusSim->getIOSeg() != ioseg) return 1;
  HMdcSizesCells* pMdcSizesCells=HMdcSizesCells::getExObject();
  if(pMdcSizesCells==0) return 1;

  if(pMdcClusSim->getNLayersInTrack(indtr) < 2) return 2; // ??????????????????

  HGeomVector hit1;
  HGeomVector hit2;
  if(!getMdcHitPosSec(ioseg*2,  hit1)) {
    if(nHitsMdc[0] <= 0) return 3;
    for(Int_t l=5;l>=0;l--) if(geantLay[0][l]) {
      if(!getLayerHitPosSec(ioseg*2,l,hit1,kFALSE)) return 3;
      break;
    }
  }
  if(!getMdcHitPosSec(ioseg*2+1,hit2)) {
    if(nHitsMdc[1] <= 0) return 3;
    for(Int_t l=0;l<6;l++) if(geantLay[1][l]) {
      if(!getLayerHitPosSec(ioseg*2+1,l,hit2,kFALSE)) return 3;
      break;
    }
  }
  Float_t x,y;
  pMdcClusSim->calcIntersection(hit1,hit2,x,y);
  pMdcClusSim->setXYGeant(indtr,x,y);

// pMdcClusSim->print();
// Float_t dX1,dY1,dX2,dY2,dX,dY;
// dX1=dY1=dX2=dY2=dX=dY=-100000.;
// Float_t x1,y1,x2,y2;
// pMdcClusSim->calcIntersection(hit1,x1,y1);
// pMdcClusSim->calcIntersection(hit2,x2,y2);
// dX1=pMdcClusSim->getX()-x1;
// dX2=pMdcClusSim->getX()-x2;
// dY1=pMdcClusSim->getY()-y1;
// dY2=pMdcClusSim->getY()-y2;
// 
// dX=pMdcClusSim->getX()-x;
// dY=pMdcClusSim->getY()-y;
// printf("***  dX=%.1f  dY=%.1f\n",dX,dY);
// if(dX1>-90000.) printf("+++ dX1=%.1f dY1=%.1f \n+++ dX2=%.1f dY2=%.1f\n",dX1,dY1,dX2,dY2);

  return 0;
}

void HMdcGeantSeg::analyseSeg(HMdcSegSim* pSeg,Int_t trInd,HMdcList12GroupCells& wrList,
                              Float_t ch2,Int_t nR) {
  UChar_t status = segmentStatus | SEGS_MATH_OK; // No seg.match. info in MdcSegSim any more!
  if( !HMdcBArray::is40DegWireCross(wrList.getListLayers()) )    status &= FAKE_CONTRIBUTION;
  else if( nR >=  pSeg->getNTimes(trInd) )                       status &= FAKE_CONTRIBUTION;
  else if( !isSegBetter(pSeg,trInd,wrList.getNLayers(),ch2,nR) ) status &= FAKE_CONTRIBUTION;
  pSeg->setTrackStatus(trInd,status);
}

Bool_t HMdcGeantSeg::isSegBetter(HMdcSegSim* pSeg,Int_t trInd,Int_t nLayersN,Float_t chi2N,Int_t nR) {
  if(pSeg->getChi2() < 0.) chi2N = -1.;
  UChar_t nWiresN  = pSeg->getNTimes(trInd);
 
  if(pSegBest != NULL) {
    if(chi2N < 0. && chi2 >= 0.)   return kFALSE;   // fake contribution
    else   if(chi2N <  0. && chi2 <  0.) {
      if(sNLayers > nLayersN) return kFALSE;   // fake contribution
      if(sNLayers == nLayersN) {
        if(sNWires-nWiresN > 1) return kFALSE; // fake contrirution
        if(sNWires-nWiresN >= -1 && nR > nRestWires) return kFALSE;
      }
    } else if(chi2N >= 0. && chi2 >= 0.) {
      if(sNLayers > nLayersN) return kFALSE;   // fake contribution
      if(sNLayers == nLayersN) {
        if(sNWires-nWiresN > 1) return kFALSE; // fake contrirution
        if(sNWires-nWiresN >= -1) {
          if(nR-nRestWires >  1) return kFALSE;
          if(nR-nRestWires > -1 && chi2N > chi2) return kFALSE;
        }
      }
    }
    pSegBest->setTrackStatus(sTrIndBest,pSegBest->getTrackStatus(sTrIndBest) & FAKE_CONTRIBUTION);
  }
  pSegBest   = pSeg;
  sTrIndBest = trInd;
  sNLayers   = nLayersN;
  sNWires    = nWiresN;
  chi2       = chi2N;
  nRestWires = nR;
  return kTRUE;
}

//===========================================================================

Bool_t  HMdcGeantTrack::mdcSetup[6][4]=
    {{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};

HMdcGeantTrack::HMdcGeantTrack(void) {
  // constructor for the case when geantSegment array created in HMdcGeantTrack
  arrSizeLoc    = 0;
  arrSize       = &arrSizeLoc;
  arrOffset     = 0;
  arrGlobOffset = 0;
  iterMdcCal1   = 0;
  segments      = new TObjArray();
  clear();
}

HMdcGeantTrack::HMdcGeantTrack(TObjArray* arr,Int_t* size,Int_t* offst) {
  // constructor for the case when geantSegment array created out of HMdcGeantTrack
  // parameters:
  // arr - pointer to TObjArray for HMdcGeantSeg objects
  // size - pointer to Int_t value which keep num. of not empty slots
  //        in TObjArray (should not has empty slots between not empty slots)
  // offset - pointer to Int_t value which keep num. of HMdcGeantSeg objects
  //          already used in TObjArray for previous tracks
  segments      = arr;
  arrSize       = size;
  arrOffset     = 0;
  arrGlobOffset = offst;
  pMdcCal1Cat   = NULL;
  iterMdcCal1   = NULL;
  clear();
}

HMdcGeantTrack::~HMdcGeantTrack(void) {
  if(iterMdcCal1) {
    delete iterMdcCal1;
    iterMdcCal1 = 0;
  }
  if(segments && arrGlobOffset==0) {
    segments->Delete();
    delete segments;
    segments=0;
  }
}

void HMdcGeantTrack::clear(void) {
  pGeantKine     = 0;
  debugPrintFlag = kFALSE;
  setDefault();
}

void HMdcGeantTrack::setDefault(void) {
  nGeantMdcHits = 0;
  nSegments     = 0;
  nWSegments    = 0;
  mdcWSector    = -1;
  mdcSector     = -1;
  nIOSeg        = 0;
  nWIOSeg       = 0;
  nWires        = -1;
  directionFlag = kTRUE;
  geantBugFlag  = kFALSE;
  trackStatus   = DEFAULT_SET;
  userFlag      = 0;
}

void HMdcGeantTrack::testMdcSetup(void) {
  if(gHades==0) return;
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) 
      mdcSetup[s][m] = HMdcGetContainers::getObject()->isModActive(s,m);
}

Bool_t HMdcGeantTrack::setMdcCal1Cat(void) {
  HCategory* cat = HMdcGetContainers::getObject()->getCatMdcCal1();
  if(pMdcCal1Cat && pMdcCal1Cat==cat) return kTRUE;
  pMdcCal1Cat=cat;
  if(pMdcCal1Cat==0) return kFALSE;
  if(iterMdcCal1) delete iterMdcCal1;
  iterMdcCal1 = (HIterator*)pMdcCal1Cat->MakeIterator();
  return kTRUE;
}

Short_t HMdcGeantTrack::testGeantTrack(Int_t trNum) {
  // return number of mdc segments
  HCategory* pGeantKineCat = HMdcGetContainers::getObject()->getCatGeantKine();
  if(pGeantKineCat) return testGeantTrack((HGeantKine*)pGeantKineCat->getObject(trNum-1));
  Error("testGeantTrack"," Can't get catGeantKine category!");
  setDefault();
  return 0;
}

Short_t HMdcGeantTrack::testGeantTrack(HGeantKine* pGK) {
  // return number of mdc segments
  pGeantKine = pGK;
  if(pGeantKine==0) {
    setDefault();
    Error("testGeantTrack"," pointer of HGeantKine object = 0!");
    return 0;
  }
  testHitsInDetectors();
  return testMdcHits();
}

void HMdcGeantTrack::testHitsInDetectors(void) {
  trackNumber  = pGeantKine->getTrack();
  mom          = pGeantKine->getTotalMomentum();
  isInMdcFlag  = pGeantKine->getFirstMdcHit()    >= 0;
  isInRichFlag = pGeantKine->getFirstRichHit()   >= 0;
  isInMetaFlag = pGeantKine->getFirstTofHit()    >= 0 ||
                 pGeantKine->getFirstShowerHit() >= 0 ||
                 pGeantKine->getFirstRpcHit()    >= 0;
  if(!isInMetaFlag) trackStatus &= NO_META_HIT;
  if(!isInMdcFlag)  trackStatus &= NOT_RECONSTRUCTABLE;
}

Short_t HMdcGeantTrack::testMdcHits(void) {
  // return number of mdc segments
  setDefault();
  if(!isInMdcFlag) return 0;
  if(arrGlobOffset) arrOffset = *arrGlobOffset;

  pGeantMdcPrev     = 0;
  Char_t  modPrev   = -1;
  Char_t  lay14Prev = -1;
  Float_t tofPrev   = -1.;
  Float_t momPrev   = mom;
  geantBugFlag      = kFALSE;

  if(!addSegAtAndExpand(0)) return 0;
  HCategory* pGeantMdcCat = HMdcGetContainers::getObject()->getCatGeantMdc();
  if(pGeantMdcCat==0) {
    Error("testMdcHits","Can't get catMdcGeantRaw category!");
    return 0;
  }
  pGeantKine->setMdcCategory(pGeantMdcCat);
  pGeantKine->resetMdcIter();
  while((pGeantMdc=(HGeantMdc*)pGeantKine->nextMdcHit()) != NULL) {
    Char_t sec = pGeantMdc->getSector();
    Char_t mod = pGeantMdc->getModule();
    if(!mdcSetup[(Int_t)sec][(Int_t)mod]) continue;
    Char_t lay    = pGeantMdc->getLayer();
    Char_t lay14  = calcLay14(mod,lay);
    Char_t hitDir = HMdcGeantSeg::dirHit(pGeantMdc);
    Float_t ax,ay,tof,momLay;
    pGeantMdc->getHit(ax,ay,tof,momLay);
    Char_t  dMod  = mod-modPrev;
    Char_t  dLay  = lay14-lay14Prev;
    Float_t dMom  = momPrev-momLay;
    Bool_t isDirOk = kTRUE;
    if(modPrev>=0 && (hitDir!=dirDLayer(dLay) || hitDir!=segment->getDirection())) isDirOk = kFALSE;

    // Finding bugs in HGeantMdc:
//     if(isGeantBug1(momLay)) continue;
//     if(isGeantBug2(sec,dMod,dLay,hitDir,tof-tofPrev)) continue;
//     mayBeGeantBug(dMom);
    if( !isGeantBug1(momLay) && !isGeantBug2(sec,dMod,dLay,hitDir,tof-tofPrev)) mayBeGeantBug(dMom);

    if(debugPrintFlag && nGeantMdcHits==0) printf("---- New track: ----\n");
    if(modPrev<0 || !isDirOk || !segment->addHit(pGeantMdc)) {
      // New segment ---------------------------
      if(modPrev>=0 && !addSegment()) break;
      segment->addFirstHit(pGeantMdc,mdcSetup[(Int_t)sec],nSegments);
      segment->setTrackNumber(trackNumber);
    }
    if(lay!=6) collectWires(sec,mod,lay,tof);
    if(debugPrintFlag) printDebug(dMom,dMod);
    nGeantMdcHits++;
    pGeantMdcPrev = pGeantMdc;
    modPrev       = mod;
    lay14Prev     = lay14;
    tofPrev       = tof;
    momPrev       = momLay;
  }
  if(nGeantMdcHits>0) {
    if(segment->getNGMdcHits()>0) nSegments++;
    else nGeantMdcHits--;  // remove one mid-planeHit segment
  }
  if(nGeantMdcHits==0) isInMdcFlag=kFALSE;
  calcNSectors();
  setStatusFlags();
  return nSegments;
}

void HMdcGeantTrack::setStatusFlags(void) {
  Bool_t* mdcSp = mdcSector>=0 ? mdcSetup[(Int_t)mdcSector] : 0;
  if(geantBugFlag)                          trackStatus &= GEANT_BUG;
  if(nSegments<1 || nSegments>2)            trackStatus &= NOT_RECONSTRUCTABLE;
  else if(mdcSector < 0)                    trackStatus &= NOT_RECONSTRUCTABLE;
  else if(!directionFlag)                   trackStatus &= NOT_RECONSTRUCTABLE;
  else if(nSegments == 1) {
    if(nIOSeg==1 && (mdcSp[2] || mdcSp[3])) trackStatus &= NOT_RECONSTRUCTABLE;
    if(nIOSeg==2 && (mdcSp[0] || mdcSp[1])) trackStatus &= NOT_RECONSTRUCTABLE;
  } else if(nWires >= 0) {
    // wires are collected
    if(nWSegments<1 || nWSegments>2)        trackStatus &= NOT_RECONSTRUCTABLE;
    else if(nWires < 5*nWSegments)          trackStatus &= NOT_RECONSTRUCTABLE;
    else if(nWSegments == 1) {
      if(nWIOSeg==1) {
        if(mdcSp[2] || mdcSp[3])            trackStatus &= NOT_RECONSTRUCTABLE;
      } else if(nWIOSeg==2) {
        if(mdcSp[0] || mdcSp[1])            trackStatus &= NOT_RECONSTRUCTABLE;
      }
    }
  }
  for(Short_t ns=0;ns<nSegments;ns++) {
    HMdcGeantSeg* pGeantSeg = getSegment(ns);
    pGeantSeg->setStatusFlags(trackStatus);
  }
}

Bool_t HMdcGeantTrack::addSegAtAndExpand(Short_t segNum) {
  if(*arrSize <= segNum+arrOffset) {
    segment = new HMdcGeantSeg(segNum);
    segments->AddAtAndExpand(segment,segNum+arrOffset);
    (*arrSize)++;
  } else {
    segment = (HMdcGeantSeg*)segments->At(segNum+arrOffset);
    if(segment==0) return kFALSE;
    segment->clear(segNum);
  }
  return kTRUE;
}

Bool_t HMdcGeantTrack::addSegment(void) {
  if(segment->getNGMdcHits()>0) {
    nSegments++;
    if(!addSegAtAndExpand(nSegments)) return kFALSE;
    if(debugPrintFlag) printf("- New segment:\n");
  } else {
    if(debugPrintFlag) printDebug(pGeantMdcPrev,-1," ! mid-plane segment");
    nGeantMdcHits--;  // remove segment from one mdc mid-plane hit
  }
  return kTRUE;
}

Int_t HMdcGeantTrack::calcLay14(Int_t m, Int_t l) {
  // recalculate module and layer number in layer number from 0 to 13
  // were numb. 3 and 10 correspond mid-planes of mdc's
  Int_t ls = (m&1)*7;
  if(l<3) return l+ls;
  if(l<6) return l+1+ls;
  return 3+ls;             // mid-plane MDC
}

Bool_t HMdcGeantTrack::isGeantBug1(Float_t momLay) {
  // Finding bug in HGeantMdc. Hit momentum > vertex momentum.
  if(mom < momLay-0.001) {
    if(debugPrintFlag) printDebug(pGeantMdc,-1,"! P>Pver");
    geantBugFlag = kTRUE;
    return kTRUE;
  }
  return kFALSE;
}

Bool_t HMdcGeantTrack::isGeantBug2(Char_t sec,Char_t dMod, Char_t dLay,
    Char_t hitDir, Float_t dTof) {
  // Finding bug in HGeantMdc. Too short t.o.f. between two hits.
  if(dTof>0.006 || segment->getNGMdcHits()<=0) return kFALSE;
  if(sec==segment->getSec() && dLay==0 && dMod==0) {
//    if(hitDir != segment->getDirection() || !isGeantBug3()) return kFALSE;
    if(hitDir != segment->getDirection() /*|| !isGeantBug3()*/) return kFALSE;
  } else if(abs(dLay) <= 1) return kFALSE;
  if(debugPrintFlag) printDebug(pGeantMdc,-1,"! dTof<0.006");
  geantBugFlag = kTRUE;
  return kTRUE;
}

Bool_t HMdcGeantTrack::mayBeGeantBug(Float_t dMom) {
  // Finding bug in HGeantMdc. Hit momentum > prev.hit momentum.
  if(dMom > -0.01) return kFALSE;
  geantBugFlag = kTRUE;
  return kTRUE;
}

Bool_t HMdcGeantTrack::isGeantBug3(void) {
  // removing fake HGeantMdc hits from list
  Float_t x1,y1,tof1,p1;
  pGeantMdcPrev->getHit(x1,y1,tof1,p1);
  Float_t x2,y2,tof2,p2;
  pGeantMdc->getHit(x2,y2,tof2,p2);
  if(fabs(x1-x2) > 4.) return kFALSE;
  if(fabs(y1-y2) > 4.) return kFALSE;
  Char_t lay=pGeantMdc->getLayer();
  if(lay!=6) {
    Int_t nwires1 = segment->getNDrTimes();
    Int_t nwires2 = collectWires(pGeantMdc->getSector(),pGeantMdc->getModule(),lay,tof2);
    if(nwires2 > nwires1) {
      pGeantMdcPrev = pGeantMdc;
      segment->setAnotherHit(pGeantMdc);
    }
  }
  return kTRUE;
}

Int_t HMdcGeantTrack::collectWires(Char_t sec, Char_t mod, Char_t lay, Float_t atof) {
  Int_t nwires = 0;
  HMdcEvntListCells* pMdcListCells = HMdcEvntListCells::getExObject();
  if(pMdcListCells) {
    HMdcLayListCells& pLayListCells=(*pMdcListCells)[sec][mod][lay];
    Int_t cell = -1;
    Int_t track1,track2;
    Float_t tof1,tof2;
    UChar_t times;
    while( (times=pLayListCells.nextCellSim(cell,track1,track2,tof1,tof2)) ) {
      if(track1!=trackNumber || fabs(atof-tof1)>0.0005) times &= 2;
      if(track2!=trackNumber || fabs(atof-tof2)>0.0005) times &= 1;
      if(times==0) continue;
      //Int_t notAdded = segment->setTime(lay+(mod&1)*6,cell,times);
      segment->setTime(lay+(mod&1)*6,cell,times);
// Int_t gtr=pLayListCells.getGeantTrack(cell,1);
// if(notAdded!=0) {
// //  HGeantKine *kine = (HGeantKine*)pGeantKineCat->getObject(gtr-1);
//   printf("nn=%i gntr=%i pid=%i %iL %ic atof=%f\n",notAdded,gtr,pGeantKine->getID(),lay+(mod&1)*6,cell,atof);
// }
      nwires++;
    }
    segment->setWiresAreColl();
//if(nwires>0) printf("************** nwires=%i\n",nwires);
    return nwires;
  } else if(setMdcCal1Cat()) {
    locMdcCal1.set(3,sec,mod,lay);
    iterMdcCal1->Reset();
    iterMdcCal1->gotoLocation(locMdcCal1);
    HMdcCal1Sim* pMdcCal1Sim;
    while((pMdcCal1Sim=(HMdcCal1Sim*)iterMdcCal1->Next())) {
      Int_t nHits=pMdcCal1Sim->getNHits();
      if(nHits==0) continue;
      UChar_t times=(nHits>0) ? 1:-nHits;
      if(times==2) times=3;
      if((times&1) && (pMdcCal1Sim->getStatus1()<=0 ||
          pMdcCal1Sim->getNTrack1()!=trackNumber ||
          fabs(atof-pMdcCal1Sim->getTof1())>0.0005)) times&=2;
      if((times&2) && (pMdcCal1Sim->getStatus2()<=0 ||
          pMdcCal1Sim->getNTrack2()!=trackNumber ||
          fabs(atof-pMdcCal1Sim->getTof2())>0.0005)) times&=1;
      if(times==0) continue;
      nwires++;
      segment->setTime(lay+(mod&1)*6,pMdcCal1Sim->getCell(),times);
    }
    segment->setWiresAreColl();
  } else segment->setWiresAreNotColl();
  return nwires;
}

void HMdcGeantTrack::unsetMdc(Int_t s, Int_t m) {
  if(s>=0 && s<6 && m>=0 && m<4) mdcSetup[s][m]=kFALSE;
}

void HMdcGeantTrack::print(void) {
  // nseg<0 - print all segments
  Float_t ax,ay,az;
  pGeantKine->getVertex(ax,ay,az);
  printf("Tr.%i  %s p=%.3f MeV. Vertex=%.1f/%.1f/%.1f ",trackNumber,
      HPhysicsConstants::pid(pGeantKine->getID()),
      pGeantKine->getTotalMomentum(),ax,ay,az);
  if(pGeantKine->isPrimary()) printf("Primary. ");
  else printf("Secondary. ");
  if(isInMdcFlag||isInRichFlag||isInMetaFlag) {
    printf("Present in");
    if(isInRichFlag) printf(" RICH");
    if(isInMdcFlag)  printf(" MDC");
    if(isInMetaFlag) printf(" META");
    printf("\n");
  } else printf("No hits.\n");
  if(nSegments>0) {
    printf(" MDC                 GeantMdc:%3i segments ",nSegments);
    if(mdcSector>=0) printf("in sector %i ",mdcSector+1);
    else  printf("in %i sectors",-mdcSector-1);
    printf("     MdcCal1:");
    if(HMdcEvntListCells::getExObject() || pMdcCal1Cat) {
      if(nWSegments==0) printf(" no hits");
      else {
        printf("%3i segments ",nWSegments);
        if(mdcWSector>=0) printf("in sector %i",mdcWSector+1);
        else printf("in %i sectors",-mdcWSector-1);
      }
    } else printf(" hits not collected");
    printf("\n");
    for(Short_t ns=0;ns<nSegments;ns++)
      ((HMdcGeantSeg*)segments->At(ns+arrOffset))->print();
  }
  if(userFlag) printf("  userFlag=%i\n",userFlag);
  printf("\n");
}

void HMdcGeantTrack::printDebug(Float_t dMom,const Char_t dMod) {
  if(dMom < -0.01) printDebug(pGeantMdc,nGeantMdcHits,"! P>Pprev.");
  else if(dMom>50. && dMod==0 && segment->getNGMdcHits()>1)
      printDebug(pGeantMdc,nGeantMdcHits,"! Pprev.-P>50");
  else printDebug(pGeantMdc,nGeantMdcHits);
}

void HMdcGeantTrack::printDebug(HGeantMdc* pGeantMdc,Int_t i,const Char_t* st) {
  Int_t sec = pGeantMdc->getSector();
  Int_t mod = pGeantMdc->getModule();
  Int_t lay = pGeantMdc->getLayer();
  Int_t lay14=calcLay14(mod,lay);
  Float_t ath, aph;
  pGeantMdc->getIncidence(ath,aph);
  Float_t ax,ay,tof,momLay;
  pGeantMdc->getHit(ax,ay,tof,momLay);
  if(i>=0) printf("%3i)",i);
  else printf("Del.");
  printf(" %3itr. %is %im %2il  l14=%2i t=%7.4f th=%7.3f ph=%7.3f",
   pGeantMdc->getTrack(),sec,mod,lay,lay14,tof,ath,aph);
  printf(" dr=%2i x=%7.1f y=%7.1f p=%.3f",HMdcGeantSeg::dirTheta(ath),
      ax,ay,momLay);
  if(st) printf(" %s",st);
  printf("\n");
}

Int_t HMdcGeantTrack::getNSegsInMdc(Int_t m,Int_t sec) {
  // return number of segments where mdc "m" present in sector "sec"
  // if "sec"<0 - return number of segments where mdc "m" present
  if(!nMdcOk(m) || nSegments==0) return 0;
  Int_t segmdc = m >> 1;
  Int_t nSeg   = 0;
  for(Short_t ns=0; ns<nSegments; ns++) {
    HMdcGeantSeg * geantSeg =(HMdcGeantSeg*)segments->At(ns+arrOffset);
    if(geantSeg->getIOSeg() != segmdc) continue;
    if(sec>=0 && geantSeg->getSec() != sec) continue;
    if(geantSeg->getNGMdcHits(m)) nSeg++;
  }
  return nSeg;
}

Int_t HMdcGeantTrack::getNSegments(Int_t seg,Int_t sec) {
  // return number of segments where mdc "m" present in sector "sec"
  // if "sec"<0 - return number of segments where mdc "m" present
  if(seg<0 || seg>1 || nSegments==0) return 0;
  Int_t nSeg=0;
  for(Short_t ns=0; ns<nSegments; ns++) {
    HMdcGeantSeg * geantSeg =(HMdcGeantSeg*)segments->At(ns+arrOffset);
    if(geantSeg->getIOSeg()  != seg) continue;
    if(sec>=0 && geantSeg->getSec() != sec)    continue;
    nSeg++;
  }
  return nSeg;
}

HGeantMdc* HMdcGeantTrack::getMdcMidPlaneHit(Int_t nseg,Int_t mod) {
  if(nSegOk(nseg)) return
    ((HMdcGeantSeg*)segments->At(nseg+arrOffset))->getMdcMidPlaneHit(mod);
  return 0;
}

HGeantMdc* HMdcGeantTrack::getMdcLayerHit(Int_t nseg,Int_t mod,Int_t lay) {
  if(nSegOk(nseg)) return
    ((HMdcGeantSeg*)segments->At(nseg+arrOffset))->getMdcLayerHit(mod,lay);
  return 0;
}

Bool_t HMdcGeantTrack::getMdcHitPos(Int_t ns, Int_t m, HGeomVector& hit) {
  // Return geant "hit" position in mid-plane mdc.
  if(nSegOk(ns)) return
    ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getMdcHitPos(m,hit);
  return kFALSE;
}

Bool_t HMdcGeantTrack::getLayerHitPos(Int_t ns, Int_t m, Int_t l,
    HGeomVector& hit, Bool_t extrFlag) {
  // Return geant "hit" position in mdc layer.
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->
      getLayerHitPos(m,l,hit,extrFlag);
  return kFALSE;
}

Bool_t HMdcGeantTrack::getMdcHitPosSec(Int_t ns, Int_t m, HGeomVector& hit) {
  // Return geant "hit" position in mid-plane mdc in sector coor.sys.
  if(nSegOk(ns)) return
      ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getMdcHitPosSec(m,hit);
  return kFALSE;
}

Bool_t HMdcGeantTrack::getMdcHitPosLab(Int_t ns, Int_t m, HGeomVector& hit) {
  // Return geant "hit" position in mid-plane mdc in lab.coor.sys..
  if(nSegOk(ns)) return
      ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getMdcHitPosLab(m,hit);
  return kFALSE;
}

Bool_t HMdcGeantTrack::getLayerHitPosSec(Int_t ns, Int_t m, Int_t l,
    HGeomVector& hit, Bool_t extrFlag) {
  // Return geant "hit" position in mdc layer in sector coor.sys..
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->
      getLayerHitPosSec(m,l,hit,extrFlag);
  return kFALSE;
}

Bool_t HMdcGeantTrack::getLayerHitPosLab(Int_t ns, Int_t m, Int_t l,
    HGeomVector& hit, Bool_t extrFlag) {
  // Return geant "hit" position in mdc layer in lab.coor.sys.
  // If no GeantMdc hit in this layer and extrFlag=kTRUE
  // function does extrapolation to this layer.
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->
      getLayerHitPosLab(m,l,hit,extrFlag);
  return kFALSE;
}

Char_t HMdcGeantTrack::getSegDirection(Int_t ns) {
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->
      getDirection();
  return 0;
}

Char_t HMdcGeantTrack::getSector(Int_t ns)       {
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getSec();
  return -1;
}

Char_t HMdcGeantTrack::getIOSeg(Int_t ns)        {
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getIOSeg();
  return -1;
}

Char_t HMdcGeantTrack::getFirstLayer12(Int_t ns)   {
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getFirstLayer12();
  return -1;
}

Char_t HMdcGeantTrack::getLastLayer12(Int_t ns)    {
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getLastLayer12();
  return -1;
}

Char_t HMdcGeantTrack::getNGMdcHits(Int_t ns)    {
  if(nSegOk(ns)) return ((HMdcGeantSeg*)segments->At(ns+arrOffset))->getNGMdcHits();
  return 0;
}

void HMdcGeantTrack::calcNSectors(void) {
  if(nSegments==0) return;
  Short_t secList = 0;
  Int_t sec       = -1;
  for(Short_t ns=0;ns<nSegments;ns++) {
    HMdcGeantSeg* geantSeg =(HMdcGeantSeg*)segments->At(ns+arrOffset);
    if(geantSeg->getDirection()<1) directionFlag=kFALSE;
    nIOSeg |= (1<<geantSeg->getIOSeg());
    sec=geantSeg->getSec();
    Short_t tst=1<<sec;
    if(secList&tst) continue;
    secList |= tst;
    mdcSector--;
  }
  if(mdcSector==-2) mdcSector=sec;

  if(HMdcEvntListCells::getExObject()==0 && pMdcCal1Cat==0) return;
  secList = 0;
  sec     = -1;
  nWires  = 0;
  for(Short_t ns=0;ns<nSegments;ns++) {
    HMdcGeantSeg* geantSeg =(HMdcGeantSeg*)segments->At(ns+arrOffset);
    Int_t nDrTm=geantSeg->getNDrTimes();
    if(nDrTm==0) continue;
    nWires += nDrTm;
    nWIOSeg |= (1<<geantSeg->getIOSeg());
    sec=geantSeg->getSec();
    Short_t tst=1<<sec;
    nWSegments++;
    if(secList&tst) continue;
    secList |= tst;
    mdcWSector--;
  }
  if(mdcWSector==-2) mdcWSector=sec;
}

HMdcGeantTrack* HMdcGeantEvent::next(Int_t& i) {
  i=(i<0) ? 0:i+1;
  if(i>=nTracks || nTracks==0) return 0;
  return (HMdcGeantTrack*)At(i);
}

HMdcGeantTrack* HMdcGeantEvent::getGeantTrack(Int_t trackNum) {
  if(nTracks <= 0 || trackNum<=0) return 0;
  Int_t nabove = nTracks+1;
  Int_t nbelow = 0;
  while(nabove-nbelow > 1) {
     Int_t middle = (nabove+nbelow) >> 1;
     HMdcGeantTrack* pTrack=(HMdcGeantTrack*)At(middle-1);
     Int_t track = pTrack->getTrack();
     if(trackNum == track) return pTrack;
     if(trackNum  < track) nabove = middle;
     else                  nbelow = middle;
  }
  return 0;
}

//===========================================================================

HMdcGeantEvent* HMdcGeantEvent::pGlobalGEvent=0;

HMdcGeantEvent* HMdcGeantEvent::getObject(Bool_t& isCreated) {
  // In task call this function not earlier then in your init function.
  // In macro first time call this function after
  // HMdcGetContainers::setCatGeantKine(HCategory* cat);
  // HMdcGetContainers::setCatGeantMdc(HCategory* cat);
  isCreated = kFALSE;
  if(pGlobalGEvent==0) {
    if(HMdcGetContainers::getObject()->getCatGeantKine() == 0) return 0;
    if(HMdcGetContainers::getObject()->getCatGeantMdc()  == 0) return 0;
    pGlobalGEvent = new HMdcGeantEvent();
    isCreated     = kTRUE;
  }
  return pGlobalGEvent;
}

void HMdcGeantEvent::deleteCont(void) {
  if(pGlobalGEvent) delete pGlobalGEvent;
  pGlobalGEvent = 0;
}

HMdcGeantEvent::HMdcGeantEvent(void) {
  size           = 0;
  nTracks        = 0;
  pGeantKineCat  = 0;
  pGeantMdcCat   = 0;
//  pMdcCal1Cat    = 0;
  pMdcListCells  = 0;
  isMdcLCellsOwn = kFALSE;
  debugPrintFlag = kFALSE;
  nGSegments     = 0;
  sizeGSegArr    = 0;
  isMdcLCellsOwn = kFALSE;
  HMdcGeantTrack::testMdcSetup();
}

HMdcGeantEvent::~HMdcGeantEvent(void) {
  // destructor
  Delete();
  HMdcEvntListCells::deleteCont();
  geantSegments.Delete();
}

Bool_t HMdcGeantEvent::setGeantKineCat(void) {
  HCategory* cat = HMdcGetContainers::getObject()->getCatGeantKine();
  if(pGeantKineCat && cat==pGeantKineCat) return kTRUE;
  pGeantKineCat=cat;
  if(pGeantKineCat==0) {
    Error("setGeantKineCat"," Can't get catGeantKine category!");
    return kFALSE;
  }
  return kTRUE;
}

Bool_t HMdcGeantEvent::collectTracks(void) {
  nTracks      = 0;
  nGSegments   = 0;
  geantBugFlag = kFALSE;
  xVertex = yVertex = zVertex = -1000.;
  targNum = -1;
  Bool_t setVertex = kTRUE;
  if(pMdcListCells == NULL) pMdcListCells = HMdcEvntListCells::getObject(isMdcLCellsOwn);
  if(isMdcLCellsOwn) pMdcListCells->collectWires();
  if(!setGeantKineCat()) return kFALSE;
  HMdcGeantTrack* pGeantTrack;
  Int_t nTrks = pGeantKineCat->getEntries();
  for(Int_t trk=0;trk<nTrks;trk++) {
    HGeantKine* pGeantKine = (HGeantKine*)pGeantKineCat->getObject(trk);
    if(pGeantKine == NULL) continue;
    if(setVertex && pGeantKine->isPrimary()) {
      pGeantKine->getVertex(xVertex,yVertex,zVertex);
      HMdcSizesCells* pMdcSizesCells = HMdcSizesCells::getExObject();
      if(pMdcSizesCells) targNum = pMdcSizesCells->calcTargNum(zVertex);
      setVertex = kFALSE;
    }
    if(nTracks<size) pGeantTrack=(HMdcGeantTrack*)At(nTracks);
    else {
      pGeantTrack = new HMdcGeantTrack(&geantSegments,&sizeGSegArr,&nGSegments);
      AddAtAndExpand(pGeantTrack, nTracks);
      size++;
    }
    pGeantTrack->setDebugPrintFlag(debugPrintFlag);
    nGSegments += pGeantTrack->testGeantTrack(pGeantKine);
    geantBugFlag = pGeantTrack->isGeantBug();
    if(debugPrintFlag) pGeantTrack->print();
    if(!pGeantTrack->isInMdc()) continue;
    if(pGeantTrack->getNWires()==0) continue;
    nTracks++;
  }
  return kTRUE;
}

void HMdcGeantEvent::print(void) {
  for(Int_t i=0;i<nTracks;i++) ((HMdcGeantTrack*)At(i))->print();
}

void HMdcGeantEvent::clearOSegClus(void) {
  // This method is used for outer clusters when magnet on
  // for each inner segment.
  for(Int_t ind=0;ind<sizeGSegArr;ind++) {
    HMdcGeantSeg* pMdcGSeg = (HMdcGeantSeg*)(geantSegments.At(ind));
    if(pMdcGSeg==NULL || pMdcGSeg->getIOSeg()!=1) continue;
    pMdcGSeg->clearClus();
    pMdcGSeg->clearSeg();
  }
}

void HMdcGeantEvent::clearOClus(void) {
  // This method is used for outer clusters when magnet on
  // for each inner segment.
  for(Int_t ind=0;ind<sizeGSegArr;ind++) {
    HMdcGeantSeg* pMdcGSeg = (HMdcGeantSeg*)(geantSegments.At(ind));
    if(pMdcGSeg==NULL || pMdcGSeg->getIOSeg()!=1) continue;
    pMdcGSeg->clearClus();
  }
}

void HMdcGeantEvent::clearOSeg(void) {
  // This method is used for outer clusters when magnet on
  // for each inner segment.
  for(Int_t ind=0;ind<sizeGSegArr;ind++) {
    HMdcGeantSeg* pMdcGSeg = (HMdcGeantSeg*)(geantSegments.At(ind));
    if(pMdcGSeg==NULL || pMdcGSeg->getIOSeg()!=1) continue;
    pMdcGSeg->clearSeg();
  }
}

void HMdcGeantEvent::getEventVertex(Float_t &ax, Float_t &ay, Float_t &az) const {
  ax = xVertex;
  ay = yVertex;
  az = zVertex;
}

//===========================================================================

void HMdcTrackInfSim::fillClusTrackInf(HMdcClusSim* pClusSim) {
  sector        = pClusSim->getSec();
  segment       = pClusSim->getIOSeg();
  nWiresTot     = pClusSim->getNDrTimes();
  listTmp       = *((HMdcList12GroupCells*)pClusSim);
  modInd        = -2;
  isWrCollected = kFALSE;
  Int_t indPar  = pClusSim->getIndexParent();
  HMdcClusSim*  pParentClus = NULL;
  if(indPar>=0) pParentClus = (HMdcClusSim*)HMdcGetContainers::getObject()->
                              getCatMdcClus()->getObject(indPar);

  collectTracksInf();

  if( !isWrCollected ) pClusSim->calcTrList(); // old calculation
  else {
    Int_t        modICl  = pClusSim->getMod();
    if(modICl>0) modICl &= 1;
    Int_t typeClFinder = pClusSim->getTypeClFinder();
    Int_t maxNTrcks    = pClusSim->getArrsSize();
    Int_t nTrcks       = (numTracks < maxNTrcks) ? numTracks : maxNTrcks;
    for(Int_t n=0; n<nTrcks; n++) {
      Int_t ind = sortedInd[n];
      Int_t bin = pClusSim->addTrack(tracksNum[ind],numWires[ind],
          wiresList[ind].getListLayers(0),wiresList[ind].getListLayers(1));
      HMdcGeantSeg* gntSeg = gntSegList[ind];
      if(gntSeg != NULL) {
        if(typeClFinder == 1 && modICl >= 0)
             pClusSim->setNDigiTimes(bin,gntSeg->getNDrTimesMod(modICl));
        else pClusSim->setNDigiTimes(bin,gntSeg->getNDrTimes());

        gntSeg->analyseClust(pClusSim,bin,modICl);
        if(pParentClus) testIOMatching(ind,pClusSim,bin,pParentClus);
      } else {
        pClusSim->setNDigiTimes(bin,0);
        UChar_t status = ind == embedInd ? DEFAULT_SET : 0;
        if(pParentClus && ind == embedInd) {
          Int_t embRealTrack   = gHades->getEmbeddingRealTrackId();
          Int_t indPar         = pParentClus->getTrackIndex(embRealTrack);
          if(indPar >= 0) {
            status |= SEGS_MATH_OK;
            pParentClus->setTrackStatus(indPar, pParentClus->getTrackStatus(indPar) | SEGS_MATH_OK);
          }
        }
        pClusSim->setTrackStatus(bin,status);
      }
    }
  }
  pClusSim->cleanRest();
}

void HMdcTrackInfSim::testIOMatching(Int_t ind,HMdcClusSim* pClusSim,Int_t bin,
    HMdcClusSim* pClusSimPar) {
  // Check of inner-outer segments matching:
  // ind - segment index in arrays of HMdcTrackInfSim
  // bin - segment index in HMdcClusSim tracks list
  if(pClusSimPar == 0) return;
  HMdcGeantSeg*   gntOuterSeg = gntSegList[ind];
  HMdcGeantTrack* gntTrack    = gntTrackList[ind];
  Int_t  track      = gntTrack->getTrack();
  Int_t  nTrPar     = pClusSimPar->getNTracks();
  Bool_t isMatching = kFALSE;
  Int_t binPar = -1;
  if(!pClusSim->isFakeContribution(bin)) {
    for(Int_t trInd=0;trInd<nTrPar;trInd++) {
      if(pClusSimPar->getTrack(trInd) != track) continue;
//---???      if(pClusSimPar->isFakeContribution(trInd)) continue;

      Char_t dir   = gntOuterSeg->getDirection();
      Int_t segInd = segIngGTrack[ind];
      if(dir==1) segInd--; // Tr.dir.: innerSegment->outerSeg
      else       segInd++; // Tr.dir.: outerSeg->innerSegment
      HMdcGeantSeg* pInnerGSeg = gntTrack->getSegment(segInd);
      if(pInnerGSeg == 0) continue;
      if(pInnerGSeg->getIOSeg() != 0) continue;
      if(dir != pInnerGSeg->getDirection()) continue;
      if(pInnerGSeg->nIdentDrTimes(pClusSimPar)<2) continue;
      binPar = trInd;
      isMatching=kTRUE;
      break;
    }
  }
  if( isMatching ) {
    pClusSim->setTrackStatus(bin,pClusSim->getTrackStatus(bin) | SEGS_MATH_OK);
    pClusSimPar->setTrackStatus(binPar,pClusSimPar->getTrackStatus(binPar) | SEGS_MATH_OK);
  }  
}

void HMdcTrackInfSim::testIOMatching(Int_t ind,HMdcSegSim* pSegSim,Int_t bin,HMdcSegSim* pSegPar) {
  // Check of inner-outer segments matching:
  // ind - segment index in arrays of HMdcTrackInfSim
  // bin - segment index in pSegSim tracks list
  // pSegPar - parent segment for the pSegSim
  if(pSegPar == 0) return;
  HMdcGeantSeg*   gntOuterSeg = gntSegList[ind];
  HMdcGeantTrack* gntTrack    = gntTrackList[ind];
  Int_t  track      = gntTrack->getTrack();
  Int_t  nTrPar     = pSegPar->getNTracks();
  Bool_t isMatching = kFALSE;
  Int_t binPar = -1;
  HMdcCellGroup12 geantGr;
  if(!pSegSim->isFakeContribution(bin)) {
    for(Int_t trInd=0;trInd<nTrPar;trInd++) {
      if(pSegPar->getTrack(trInd) != track) continue;
//---???      if(pSegPar->isFakeContribution(trInd)) continue;
      Char_t dir   = gntOuterSeg->getDirection();
      Int_t segInd = segIngGTrack[ind];
      if(dir==1) segInd--; // Tr.dir.: innerSegment->outerSeg
      else       segInd++; // Tr.dir.: outerSeg->innerSegment
      HMdcGeantSeg* pInnerGSeg = gntTrack->getSegment(segInd);
      if(pInnerGSeg == 0) continue;
      if(pInnerGSeg->getIOSeg() != 0) continue;
      if(dir != pInnerGSeg->getDirection()) continue;
      for(Int_t l=0; l<12; l++) geantGr.setLayerGroup(l,pInnerGSeg->getOneLayerGroup(l));
      if(geantGr.getNSharedCells(pSegPar)<2) continue;
      
      binPar = trInd;
      isMatching=kTRUE;
      break;
    }
  }
  if( isMatching ) {
    pSegSim->setTrackStatus(bin,pSegSim->getTrackStatus(bin) | SEGS_MATH_OK);
    pSegPar->setTrackStatus(binPar,pSegPar->getTrackStatus(binPar) | SEGS_MATH_OK);
  }  
}

// ??????????????? Is it needed ???????????????????:
void HMdcTrackInfSim::addClusModTrackInf(HMdcClusSim* pClusSim) {
  // This function can be used ONLY after
  // fillClusTrackInf(HMdcClusSim* pClusSim) for the same pClusSim!!!
  if( !isWrCollected) {
    pClusSim->calcTrListMod(*pClusSim,0); // old calculation
    pClusSim->calcTrListMod(*pClusSim,1); // old calculation
  } else {
    Short_t nWires[numTracks];
    Int_t sInd[numTracks];
    for(Int_t m=0;m<2;m++) {
      for(Int_t tr=0;tr<numTracks;tr++) nWires[tr] = wiresList[tr].getNDrTimesMod(m);
      TMath::Sort(numTracks, nWires, sInd);
      Int_t maxTrcks     = pClusSim->getArrsSize();
      Int_t nSettedTrcks = 0;
      for(Int_t n=0; n<numTracks && nSettedTrcks<maxTrcks; n++) {
        Int_t ind = sInd[n];
        if(nWires[ind] == 0) break;
        pClusSim->setTrackM(m,nSettedTrcks,tracksNum[ind],nWires[ind]);
        nSettedTrcks++;
      }
      pClusSim->setNTracksM(m,nSettedTrcks);
    }
  }
}

void HMdcTrackInfSim::fillClusModTrackInf(HMdcClusSim* pClusSim,
    HMdcList12GroupCells* wrLst, Int_t modi) {
  // sector and segment in pClusSim must be setted already!
  isWrCollected = kFALSE;
  if(modi<0 || modi>1) return;
  sector    = pClusSim->getSec();
  segment   = pClusSim->getIOSeg();
  nWiresTot = pClusSim->getNDrTimesMod(modi);
  listTmp   = *wrLst;
  modInd    = modi;

  collectTracksInf();

  if( !isWrCollected ) pClusSim->calcTrListMod(*wrLst,modInd); // old calc.
  else {
    Int_t maxNTrcks    = pClusSim->getArrsSize();
    Int_t nSettedTrcks = (numTracks<maxNTrcks) ? numTracks : maxNTrcks;
    for(Int_t n=0; n<nSettedTrcks; n++) {
      Int_t ind = sortedInd[n];
      pClusSim->setTrackM(modInd,n,tracksNum[ind],numWires[ind]);
    }
    pClusSim->setNTracksM(modInd,nSettedTrcks);
  }
}

void HMdcTrackInfSim::fillSegTrackInf(HMdcSegSim* pSegSim,HMdcList24GroupCells* wrLst,
                                      HMdcSegSim* pSegPar) {
  isWrCollected = kFALSE;
  sector        = pSegSim->getSec();
  segment       = pSegSim->getIOSeg();
  nWiresTot     = pSegSim->getSumWires();
  if(!wrLst->getSeg(listTmp,segment)) return;
  modInd        = -2;

  collectTracksInf();

  if( !isWrCollected ) pSegSim->calcNTracks(); // old calculation
  else {
    if(pSegSim->getNTracks() != 0) pSegSim->clearSimInfo();
    Int_t   nSettedTrcks = numTracks <= 5 ? numTracks : 5;
    for(Int_t n=0; n<nSettedTrcks; n++) {
      Int_t   ind    = sortedInd[n];
      UChar_t nWires = numWires[ind] < 256 ? numWires[ind] : 255;
      HMdcGeantSeg* gntSeg = gntSegList[ind];
      if(gntSeg != NULL) {
        pSegSim->addTrack(tracksNum[ind],nWires,gntSeg->getNDrTimes());
        gntSeg->analyseSeg(pSegSim,n,wiresList[ind],
                           pWiresArr->calcChi2(wiresList[ind]),nWiresTot-nWires);
// No IOMatch        if(pSegPar) testIOMatching(ind,pSegSim,n,pSegPar);
      } else {
        pSegSim->addTrack(tracksNum[ind],nWires,0);
        UChar_t status = ind == embedInd ? DEFAULT_SET : SEGS_MATH_OK; //0; No match. info any more!
//         if(pSegPar && ind == embedInd) {
//           Int_t embRealTrack   = gHades->getEmbeddingRealTrackId();
//           Int_t indPar         = pSegPar->getTrackIndex(embRealTrack);
//           if(indPar >= 0) {
//             status |= SEGS_MATH_OK;
//             pSegPar->setTrackStatus(indPar, pSegPar->getTrackStatus(indPar) | SEGS_MATH_OK);
//           }
//         }
        pSegSim->setTrackStatus(n,status);
      }
    }
  }
}

void HMdcTrackInfSim::fillHitTrackInf(HMdcHitSim* pHitSim,HMdcList24GroupCells* wrLst) {
  isWrCollected = kFALSE;
  sector        = pHitSim->getSector();
  modInd        = pHitSim->getModule() & 1;
  segment       = pHitSim->getModule() >> 1;
  nWiresTot     = pHitSim->getSumWires();
  if(!wrLst->getSeg(listTmp,segment)) return;

  collectTracksInf();

  if( !isWrCollected ) pHitSim->calcNTracks(); // old calculation
  else {
    Int_t   segListTr[numTracks];
    UChar_t segNTimes[numTracks];
    for(Int_t n=0; n<numTracks; n++) {
      Int_t ind    = sortedInd[n];
      segListTr[n] = tracksNum[ind];
      segNTimes[n] = (numWires[ind] < 256) ? numWires[ind] : 255;
    }
    pHitSim->setNTracks(numTracks,segListTr,segNTimes);
  }
}

Bool_t HMdcTrackInfSim::collectTracksInf(UChar_t sec,UChar_t seg, HMdcList12GroupCells* wrLst) {
  sector        = sec;
  segment       = seg;
  listTmp       = *wrLst;
  modInd        = -2;
  isWrCollected = kFALSE;

  if(!collectTracksInf()) return kFALSE;
  return kTRUE;
}

Bool_t HMdcTrackInfSim::collectTracksInf(void) {
  HMdcEvntListCells* pMdcListCells = HMdcEvntListCells::getExObject();
  if(pMdcListCells == 0) return kFALSE;
  HMdcGeantEvent* pGeantEvent = HMdcGeantEvent::getExObject();
  if(pGeantEvent == 0) return kFALSE;

  HMdcSecListCells& fSecListCells = (*pMdcListCells)[sector];
  Int_t embRealTrack   = gHades->getEmbeddingRealTrackId();
  numTracks = 0;
  embedInd  = -1;
  noiseInd  = -1;
//----------  pWiresArr = NULL;

  Int_t layStr = (modInd != 1) ?  0 : 6;
  Int_t layEnd = (modInd != 0) ? 12 : 6;
  for(Int_t lay=layStr; lay<layEnd; lay++) {
    Int_t   mod  = lay/6 + segment*2;
    Int_t   cell = -1;
    UChar_t nTms = 0;
    HMdcLayListCells& fLayListCells = fSecListCells[mod][lay%6];
    while((cell=listTmp.next(lay,cell,nTms)) >= 0) {
      for(Int_t tm=1; tm<3; tm++) {
        Int_t track = fLayListCells.getGeantTrack(cell,tm);
        if(track == 0) continue;
        if(track<0 || track == embRealTrack) {
          // Embeded real track and noise --------------------------------------
          if(track != embRealTrack) track = -99;
          listTmp.delTime(lay,cell,tm);
          Int_t& ind = (track == embRealTrack) ? embedInd : noiseInd;
          if(ind < 0) {
            if(numTracks < aSize) ind = numTracks++;
            else {
              for(Int_t i=0;i<aSize;i++) if(i!=noiseInd && i!=embedInd) {
                if(ind<0 || numWires[i]<numWires[ind]) ind = i;
              }
            }
            wiresList[ind].clear();
            tracksNum[ind]    = track;
            gntSegList[ind]   = NULL;
            gntTrackList[ind] = NULL;
            numWires[ind]     = 1;
            segIngGTrack[ind] = -1;
          } else numWires[ind]++;
          wiresList[ind].setTime(lay,cell,tm);
        } else {
          // Geant track -------------------------------------------------------
          HMdcGeantTrack* pGeantTrack = pGeantEvent->getGeantTrack(track);
          if(pGeantTrack == NULL) continue; // Geant bug;
          Short_t nGeantSegs = pGeantTrack->getNSegments();
          for(Int_t segn=0;segn<nGeantSegs;segn++) {
            HMdcGeantSeg* pGeantSeg = pGeantTrack->getSegment(segn);
            if( pGeantSeg->getSec()   != sector || pGeantSeg->getIOSeg() != segment ) continue;
            numWires[numTracks] = listTmp.compareAndUnset(pGeantSeg,wiresList+numTracks,modInd);
            if(numWires[numTracks] == 0) continue;
            Int_t ind = numTracks;
            if(numTracks < aSize) numTracks++;
            else {
              for(Int_t i=0;i<aSize;i++) if(i!=noiseInd && i!=embedInd) {
                if(numWires[i] < numWires[ind]) ind = i;
              }
              if(ind == aSize) continue;
              wiresList[ind]  = wiresList[numTracks];
              numWires[ind]   = numWires[numTracks];
            }
            tracksNum[ind]    = track;
            gntSegList[ind]   = pGeantSeg;
            gntTrackList[ind] = pGeantTrack;
            segIngGTrack[ind] = segn;
          }
        }
      }
    }
  }

  isGntBugEvent       = pGeantEvent->isGeantBug();
  numWires[numTracks] = listTmp.getNDrTimes(layStr,layEnd-1);
  if(numWires[numTracks] == 0) isGntBugSeg = kFALSE;
  else {
    HLocation loc;
    loc.set(4,sector,0,0,0);
    HCategory* cat = HMdcGetContainers::getObject()->getCatMdcCal1();
    Int_t layInd=0;
    Int_t cell=-1;
    while(listTmp.getNextCell(layInd,cell)) {
      UChar_t times = listTmp.getTime(layInd,cell);
      loc.set(4,sector,layInd/6+segment*2,layInd%6,cell);
      HMdcCal1Sim* pMdcCal1Sim = (HMdcCal1Sim*)cat->getObject(loc);
      Int_t tr1 = (times&1)!=0 ? pMdcCal1Sim->getNTrack1() : 0;
      Int_t tr2 = (times&2)!=0 ? pMdcCal1Sim->getNTrack2() : 0;
      for(Int_t ind=0;ind<numTracks;ind++) {
        if(tracksNum[ind] == tr1) {
          numWires[ind]++;
          numWires[numTracks]--;
          listTmp.delTime(layInd,cell,1);
          tr1 = -1;
        }
        if(tracksNum[ind] == tr2) {
          numWires[ind]++;
          numWires[numTracks]--;
          listTmp.delTime(layInd,cell,2);
          tr2 = -1;
        }
      }
      if(numWires[numTracks] == 0) break;
//       if(tr1 > 0 && numTracks < aSize) {
//         Int_t ind = numTracks;
//         numTracks++; 
//         numWires[numTracks] = numWires[ind];
//         wiresList[ind].clear();
//         numWires[ind]  = 1;
//         tracksNum[ind] = tr1;
//         numWires[numTracks]--;
//         wiresList[ind].setTime(layInd,cell,1);
//         listTmp.delTime(layInd,cell,1);
//         gntSegList[ind]   = NULL;
//         gntTrackList[ind] = NULL;
//         segIngGTrack[ind] = -1;
//       }
//       if(tr2 > 0 && numTracks < aSize) {
//         Int_t ind = numTracks;
//         numTracks++; 
//         numWires[numTracks] = numWires[ind];
//         wiresList[ind].clear();
//         numWires[ind]  = 1;
//         tracksNum[ind] = tr2;
//         numWires[numTracks]--;
//         wiresList[ind].setTime(layInd,cell,2);
//         listTmp.delTime(layInd,cell,2);
//         gntSegList[ind]   = NULL;
//         gntTrackList[ind] = NULL;
//         segIngGTrack[ind] = -1;
//       }
    }
  }
    
isGntBugSeg = kFALSE;  // The case NumWires/layer > 48 ignored now partially !!! 
//   if(numWires[numTracks] == 0) isGntBugSeg = kFALSE;
//   else {
//     
//     isGntBugSeg = kTRUE;
//     Int_t ind = numTracks;
//     if(numTracks < aSize) numTracks++;
//     else {
//       for(Int_t i=0;i<aSize;i++) if(i!=noiseInd && numWires[i]<numWires[ind]) ind = i;
//       if(ind == embedInd) embedInd = -1;
//       numWires[ind]  = numWires[numTracks];
//     }
//     tracksNum[ind]    = -9;  // GEANT bug!
// printf("-9 => %i  nw=%i\n",tracksNum[ind],numWires[ind]);
// // Int_t lind=0;
// // Int_t c=-1;
// //  HCategory* cat = HMdcGetContainers::getObject()->getCatMdcCal1();
// //   HLocation loc;
// //   loc.set(4,sector,0,0,0);
// // while(listTmp.getNextCell(lind,c)) {
// //     loc[1]=lind/6+segment*2;
// //     loc[2]=lind%6;
// //     loc[3]=c;
// //  HMdcCal1Sim* cal1=(HMdcCal1Sim*)cat->getObject(loc);
// // printf("%i %i %i %i: tttt=%i tof=%f\n",sector,loc[1],lind%6,c,cal1->getNTrack1(),cal1->getTof1());
// // }
//     wiresList[ind]    = listTmp;
//     gntSegList[ind]   = NULL;
//     gntTrackList[ind] = NULL;
//   }

  TMath::Sort(numTracks, numWires, sortedInd);
  isWrCollected = kTRUE;
  return kTRUE;
}

Int_t HMdcTrackInfSim::getTrack(Int_t trInd) const {
  if(trInd>=0 && trInd<numTracks) return 0;
  return tracksNum[sortedInd[trInd]];
}

Short_t HMdcTrackInfSim::getNumWires(Int_t trInd) const {
  if(trInd>=0 && trInd<numTracks) return 0;
  return numWires[sortedInd[trInd]];
}

UChar_t HMdcTrackInfSim::getNumLayers(Int_t trInd, Int_t modi) const {
  if(trInd>=0 && trInd<numTracks) return 0;
  Int_t indx = sortedInd[trInd];
  if(modi==0 || modi==1) return HMdcBArray::getNSet(wiresList[indx].getListLayers(modi));
  return HMdcBArray::getNSet(wiresList[indx].getListLayers(0))+
         HMdcBArray::getNSet(wiresList[indx].getListLayers(1));
}

HMdcList12GroupCells* HMdcTrackInfSim::getListCells(Int_t trInd) {
  if(trInd>=0 && trInd<numTracks) return 0;
  return &(wiresList[sortedInd[trInd]]);
}

HMdcGeantSeg* HMdcTrackInfSim::getMdcGeantSeg(Int_t trInd) {
  if(trInd>=0 && trInd<numTracks) return 0;
  return gntSegList[sortedInd[trInd]];
}

HMdcGeantTrack* HMdcTrackInfSim::getMdcGeantTrack(Int_t trInd) {
  if(trInd>=0 && trInd<numTracks) return 0;
  return gntTrackList[sortedInd[trInd]];
}
