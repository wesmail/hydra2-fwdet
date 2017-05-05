//*-- Author : Anar Rustamov
//*-- Modified : 24/11/2004 by V. Pechenov
//*-- Modified : 02/12/2004 by V. Pechenov

///////////////////////////////////////////////////////////////////////////
//
//  HMetaMatch2
//
//  Keep indexes of hits for one track, matching qualities, and
//  indexes of track objects (HSplineTrack, HKickTrackB,...).
//
//  Indexis of hits:
//    trkCandInd                      -  index of HTrkCand object
//                ----------- System=0 -----------------
//    rpcClstInd;                  -  index of HRpcCluster object
//    showerHitInd                    -  index of HShowerHit object
//                ----------- System=1 -----------------
//    tofHitInd                       -  index of HTofHit or HTofCluster object
//                ----------- RICH -----------------
//    richInd[RICH_TAB_SIZE]    -  arr.of indexes of HRichHit objects
//    richIPUInd[RICH_TAB_SIZE] -  arr.of indexes of HRichHitIPU objects
//
//    Arrais richInd and richIPUInd are sorted by matching quality
//    and can keep up to 3 ring indexes.
//
// Quality MDC SHOWER matching:
//   Xs,Ys - shower hit position in coordinate system of shower module
//   dXs,dYs - hit position errors (== HShowerHit::getSigmaX(), getSigmaY())
//   Xm,Ym - mdc segment cross point with shower module in coordinate system
//           of corresponding shower module
//   showerSigmaXOffset & showerSigmaYOffset - from HMetaMatchPar cont.
//   showerSigmaXMdc & showerSigmaYMdc - from HMetaMatchPar cont.
//   qualityShower =
//     sqrt[((Xs - Xm - showerSigmaXOffset)/sqrt(dXs^2 + showerSigmaXMdc^2))^2 +
//          ((Ys - Ym - showerSigmaYOffset)/sqrt(dYs^2 + showerSigmaYMdc^2))^2]
//
// Tof:
//   Xt,Yt - toh hit (or tof cluster position in coordinate system of tof module
//   Xm,Ym - mdc segment cross point with tof module in coordinate system
//           of corresponding tof module
//   tofSigmaXOffset, tofSigmaYOffset - from HMetaMatchPar cont.
//   tofSigmaX, tofSigmaY - from HMetaMatchPar cont.
//   qualityTof = sqrt[ ((Xt - Xm - tofSigmaXOffset)/tofSigmaX)^2 +
//                      ((Yt - Ym - tofSigmaYOffset)/tofSigmaY)^2 ]
//
///////////////////////////////////////////////////////////////////////////

#include "hmetamatch2.h"
#include "TMath.h"
using namespace std;

HMetaMatch2::HMetaMatch2() {
  sector           = -1;
  trkCandInd       = -1;
  ownIndex         = -1;
  setDefForRest();
}

HMetaMatch2::HMetaMatch2(Short_t sec, Int_t tkInd, Int_t ind) {
  sector           = sec;
  trkCandInd       = tkInd;
  ownIndex         = ind;
  setDefForRest();
}

void HMetaMatch2::setDefForRest(void) {
  nRichId    = 0;
  nRichIPUId = 0;
  nRpcClust  = 0;
  nShrHits   = 0;
  nTofHits   = 0;

  for(Int_t i = 0; i < META_TAB_SIZE; i++) {
    rpcClstInd[i]     = -1;
    shrHitInd[i]      = -1;
    tofClstInd[i]     = -1;
    tofHit1Ind[i]     = -1;
    tofHit2Ind[i]     = -1;

    rpcQuality[i]     = 0.f;
    rpcDX[i]          = 0.f;
    rpcDY[i]          = 0.f;

    shrQuality[i]     = 0.f;
    shrDX[i]          = 0.f;
    shrDY[i]          = 0.f;

    tofClstQuality[i] = 0.f;
    tofClstDX[i]      = 0.f;
    tofClstDY[i]      = 0.f;

    tofHit1Quality[i] = 0.f;
    tofHit1DX[i]      = 0.f;
    tofHit1DY[i]      = 0.f;

    tofHit2Quality[i] = 0.f;
    tofHit2DX[i]      = 0.f;
    tofHit2DY[i]      = 0.f;
 
 
    rkIndShower[i] = -1;
    rkIndTofCl[i]  = -1;
    rkIndTof1[i]   = -1;
    rkIndTof2[i]   = -1;
    rkIndRpc[i]    = -1;

    kfIndShower[i] = -1;
    kfIndTofCl[i]  = -1;
    kfIndTof1[i]   = -1;
    kfIndTof2[i]   = -1;
    kfIndRpc[i]    = -1;

    rungeKuttaInd = -1;
   
 
  }

  for(Int_t i = 0; i < RICH_TAB_SIZE; i++) {
    richInd[i]    = -1;
    richIPUInd[i] = -1;
  }

  splineInd       = -1;
  //rungeKuttaInd   = -1;
  kalmanFilterInd = -1;
  flag            =  0;

  isFakeInner =kFALSE;
  isFakeOuter =kFALSE;

}

void HMetaMatch2::setRpcClst(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy) {
  if(el>nRpcClust) return;
  rpcClstInd[el] = ind;
  rpcQuality[el] = ql;
  rpcDX[el]      = dx;
  rpcDY[el]      = dy;
}

void HMetaMatch2::setShrHit(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy) {
  if(el>nShrHits) return;
  shrHitInd[el]  = ind;
  shrQuality[el] = ql;
  shrDX[el]      = dx;
  shrDY[el]      = dy;
}

void HMetaMatch2::setTofClst(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy) {
  if(el>nTofHits) return;
  tofClstInd[el]     = ind;
  tofClstQuality[el] = ql;
  tofClstDX[el]      = dx;
  tofClstDY[el]      = dy;
}

void HMetaMatch2::setTofHit1(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy) {
  if(el>nTofHits) return;
  tofHit1Ind[el]     = ind;
  tofHit1Quality[el] = ql;
  tofHit1DX[el]      = dx;
  tofHit1DY[el]      = dy;
}

void HMetaMatch2::setTofHit2(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy) {
  if(el>nTofHits) return;
  tofHit2Ind[el]     = ind;
  tofHit2Quality[el] = ql;
  tofHit2DX[el]      = dx;
  tofHit2DY[el]      = dy;
}

void HMetaMatch2::setRpcClstMMF(UChar_t nln,Short_t ind[],Float_t ql2[][3]) {
  // Filling function for HMetaMatchF2 class
  nRpcClust = nln<=META_TAB_SIZE ? nln : META_TAB_SIZE;
  for(UChar_t l=0;l<nRpcClust;l++) {
    rpcClstInd[l] = ind[l];
    rpcQuality[l] = TMath::Sqrt(ql2[l][0]);
    rpcDX[l]      = ql2[l][1];
    rpcDY[l]      = ql2[l][2];
  }
}

void HMetaMatch2::setShrHitMMF(UChar_t nln,Short_t ind[],Float_t ql2[][3]) {
  // Filling function for HMetaMatchF2 class
  nShrHits = nln<=META_TAB_SIZE ? nln : META_TAB_SIZE;
  for(UChar_t l=0;l<nShrHits;l++) {
    shrHitInd[l]  = ind[l];
    shrQuality[l] = TMath::Sqrt(ql2[l][0]);
    shrDX[l]      = ql2[l][1];
    shrDY[l]      = ql2[l][2];
  }
}

void HMetaMatch2::setEmcClstMMF(UChar_t nln,Short_t ind[],Float_t ql2[][3]) {
  // Filling function for HMetaMatchF2 class
  setShrHitMMF(nln,ind,ql2);
  setEmcClusterFlag();
//   nShrHits = nln<=META_TAB_SIZE ? nln : META_TAB_SIZE;
//   for(UChar_t l=0;l<nShrHits;l++) {
//     shrHitInd[l]  = ind[l];
//     shrQuality[l] = TMath::Sqrt(ql2[l][0]);
//     shrDX[l]      = ql2[l][1];
//     shrDY[l]      = ql2[l][2];
//   }
}

void HMetaMatch2::setTofClstMMF(UChar_t nln,Short_t ind[][3],Float_t ql2[][9]) {
  // Filling function for HMetaMatchF2 class
  nTofHits = nln<=META_TAB_SIZE ? nln : META_TAB_SIZE;
  for(UChar_t l=0;l<nTofHits;l++) {
    tofClstInd[l]     = ind[l][0];
    tofHit1Ind[l]     = ind[l][1];
    tofHit2Ind[l]     = ind[l][2];
    
    tofClstQuality[l] = TMath::Sqrt(ql2[l][0]);
    tofClstDX[l]      = ql2[l][1];
    tofClstDY[l]      = ql2[l][2];
    tofHit1Quality[l] = TMath::Sqrt(ql2[l][3]);
    tofHit1DX[l]      = ql2[l][4];
    tofHit1DY[l]      = ql2[l][5];
    tofHit2Quality[l] = TMath::Sqrt(ql2[l][6]);
    tofHit2DX[l]      = ql2[l][7];
    tofHit2DY[l]      = ql2[l][8];
  }
}
   


//void HMetaMatch2::setRungeKuttaInd(Int_t rktr) {
//  rungeKuttaInd=rktr;
//  if(rungeKuttaInd>=0) setRungeKuttaAccept();
//  else unsetRungeKuttaAccept();
//}


Int_t HMetaMatch2::getSystem(void) const               {
  // Return -1 - no meta m., 0 - rpc,shower, 1 - tof, 2-overlap rpc-tof
  Int_t sys = nRpcClust>0 || nShrHits>0 ? 1 : 0;
  if(nTofHits>0) sys |= 2;
  return sys-1;
}

void HMetaMatch2::print(void) {
  printf("HMetaMatch2:  %i sector, index of trkCand=%i  [index(quality)]\n",sector+1,trkCandInd);
  if(nRpcClust>0) {
    printf("  HRpcCluster's:");
    for(UChar_t n=0;n<nRpcClust;n++) {
      if(rpcClstInd[n]>=0) printf("  %i(%.3g)",rpcClstInd[n],rpcQuality[n]);
    }
    printf("\n");
  }
  if(nShrHits>0) {
    printf("  HShowerHit's:");
    for(UChar_t n=0;n<nShrHits;n++) {
      if(shrHitInd[n]>=0) printf("  %i(%.3g)",shrHitInd[n],shrQuality[n]);
    }
    printf("\n");
  }

  if(nTofHits>0) {
    for(UChar_t n=0;n<nTofHits;n++) {
      if(tofClstInd[n]>=0) {
        printf("  HTofCluster: %i(%.3g)",tofClstInd[n],tofClstQuality[n]);
        if(tofHit1Ind[n]>=0) printf("/Hit1=%i(%.3g)",tofHit1Ind[n],tofHit1Quality[n]);
        if(tofHit2Ind[n]>=0) printf("/Hit2=%i(%.3g)",tofHit2Ind[n],tofHit2Quality[n]);
        printf("\n");
      } else {
        if(tofHit1Ind[n]>=0) printf("  HTofHit1: %i(%.3g)",tofHit1Ind[n],tofHit1Quality[n]);
        if(tofHit2Ind[n]>=0) printf("  HTofHit2: %i(%.3g)",tofHit2Ind[n],tofHit2Quality[n]);
        printf("\n");
      }      
    }
  }
  if(nRichId>0) {
    printf("  richHitInd =");
    for(UChar_t n=0;n<nRichId;n++) printf(" %i",richInd[n]);
    printf("\n");
  }
  if(nRichIPUId>0) {
    printf("  richHitIPUInd =");
    for(UChar_t n=0;n<nRichIPUId;n++) printf("  %i",richIPUInd[n]);
    printf("\n");
  }
  if(splineInd>=0 || rungeKuttaInd>=0 || kalmanFilterInd>=0) {
    printf("  Tracks:");
    if(splineInd>=0)       printf("  splineInd=%i",splineInd);
    if(rungeKuttaInd>=0)   printf("  rungeKuttaInd=%i",rungeKuttaInd);
    if(kalmanFilterInd>=0) printf("  kalmanFilterInd=%i", kalmanFilterInd);
    printf("\n");
  } else printf("  No accepted tracks!\n");
}

ClassImp(HMetaMatch2)
