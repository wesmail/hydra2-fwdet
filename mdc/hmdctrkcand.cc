//*-- AUTHOR : Vladimir Pechenov
//*-- Modified : 18.09.2003 by Vladimir Pechenov


//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////
//
//  HMdcTrkCand (MDC track candidate)
//
//
//  this class keep information about matching between
//  inner and outer segments:
//    seg1Ind - index of inner segment (HMdcSeg object) in category.
//    seg2Ind - index of outer segment in category. 
//              If seg2Ind==-1  - no outer segments (!).
//    nCandForSeg1 - number of outer segments for this inner
//                   (for each combination exist HMdcTrkCand object).     
//                   If nCandForSeg1==0 - no outer segments               
//                   If nCandForSeg1>0 this container is first one from
//                   nCandForSeg1 objects.
//                   If nCandForSeg1==-1 - one of the next objects.
//    nextCandInd  - index of next HMdcTrkCand object.
//                   If nextCandInd==-1 this object is latest from
//                   nCandForSeg1 objects.
//   
//
//  The HMdcTrkCand objects are stored in the matrix category catMdcTrkCand.
//
//  Retrieving of HMdcTrkCand objects.
//    Let pMdcTrkCandCat is poiner to catMdcTrkCand category
//    and iterMdcTrkCand is corresponding iterator then:
//
//    iterMdcTrkCand->Reset();
//    while( (pMdcTrkCand=(HMdcTrkCand*)iterMdcTrkCand->Next()) !=0) {
//      Short_t nOuterSeg=pMdcTrkCand->getNCandForSeg1();
//      if(nOuterSeg==0) {    // no outer segments:
//        ...           
//      } else {
//
//        //----------------------------------------------------------------
//        // If you don't care about number of outer segments for inner one:
//        ...              // your code
//
//        //----------------------------------------------------------------
//        // Otherwise:
//        if(nOuterSeg==-1) continue;
//        Int_t nextObjInd=0;
//        while (nextObjInd>=0) {
//          ...              // your code
//          // Next HMdcTrkCand object geting:
//          nextObjInd = pMdcTrkCand->getNextCandInd();
//          if(nextObjInd>=0) pMdcTrkCand = 
//              (HMdcTrkCand*)pMdcTrkCandCat->getObject(nextObjInd);
//        }
//
//      }
//    }
//--------------------------------------------------------------------------
//  dedx fom Mdc segments can beretrieved via
//  getdedxInnerSeg()         mean value of t2-t1 for inner segment
//  getdedxOuterSeg()         sigma of t2-t1 distribution in inner segment
//  getSigmadedxInnerSeg()    number of wires in inner segment
//                            before truncated mean procedure
//  getSigmadedxOuterSeg()    number of wires in inner segment
//                            cutted by truncated mean procedure
//  getNWirededxInnerSeg()    mean value of t2-t1 for outer segment
//  getNWirededxOuterSeg()    sigma of t2-t1 distribution in outer segment
//  getNWireCutdedxInnerSeg() number of wires in outer segment
//                            before truncated mean procedure
//  getNWireCutdedxOuterSeg() number of wires in outer segment
//                            cutted by truncated mean procedure
//
//  to do it inside a loop the following functions ar provided
//
//  getdedxSeg(Int_t seg)          seg==0: for inner seg , seg==1 for outer seg
//  getSigmadedxSeg(Int_t seg)     seg==0: for inner seg , seg==1 for outer seg
//  getNWirededxSeg(Int_t seg)     seg==0: for inner seg , seg==1 for outer seg
//  getNWireCutdedxSeg(Int_t seg)  seg==0: for inner seg , seg==1 for outer seg
//  setdedxSeg(Int_t seg,Float_t dedx)        seg==0: for inner seg , seg==1 for outer seg
//  setSigmadedxSeg(Int_t seg,Float_t sig)    seg==0: for inner seg , seg==1 for outer seg
//  setNWirededxSeg(Int_t seg,UChar_t nw)     seg==0: for inner seg , seg==1 for outer seg
//  setNWireCutdedxSeg(Int_t seg,UChar_t nw)  seg==0: for inner seg , seg==1 for outer seg
//  getdedx(Int_t mod)          mod =  mdc module (0-3)
//  getSigmadedx(Int_t mod)     mod =  mdc module (0-3)
//  getNWirededx(Int_t mod)     mod =  mdc module (0-3)
//  getNWireCutdedx(Int_t mod)  mod =  mdc module (0-3)
//
/////////////////////////////////////////////////////////////////////////

#include "hmdctrkcand.h"
#include <stdlib.h> 

ClassImp(HMdcTrkCand)

HMdcTrkCand::HMdcTrkCand(Char_t sec, Int_t s1ind, Int_t ind) {
  // Setting inner segment
  sector       = sec;
  seg1Ind      = s1ind;
  seg2Ind      = -1;
  nCandForSeg1 = 0;
  nextCandInd  = -1;
  firstCandInd = ind;
  pFirstCand   = this;
  metaMatchInd = -1;
  dedxInner=dedxOuter=dedxCombined=-1;
  dedxSigmaInner=dedxSigmaOuter=dedxSigmaCombined=-1;
  dedxNWireInner=dedxNWireOuter=dedxNWireCombined=0;
  dedxNWireCutInner=dedxNWireCutOuter=dedxNWireCutCombined=0;
  flag=0;
}
  
HMdcTrkCand::HMdcTrkCand(HMdcTrkCand* fTrkCand, Int_t s2ind, Int_t ind) {
  // Setting outer segment
  fTrkCand->nextCandInd = ind;
  sector       = fTrkCand->sector; 
  seg1Ind      = fTrkCand->seg1Ind;
  seg2Ind      = s2ind;            
  nCandForSeg1 = -1;
  nextCandInd  = -1;
  firstCandInd = fTrkCand->firstCandInd;
  pFirstCand   = fTrkCand->pFirstCand;
  pFirstCand->nCandForSeg1++;
  metaMatchInd = -1;
  dedxInner=dedxOuter=dedxCombined=-1;
  dedxSigmaInner=dedxSigmaOuter=dedxSigmaCombined=-1;
  dedxNWireInner=dedxNWireOuter=dedxNWireCombined=0;
  dedxNWireCutInner=dedxNWireCutOuter=dedxNWireCutCombined=0;
  flag=0;
}

void HMdcTrkCand::clear(void) {
  // reset data members to default values
  seg1Ind=seg2Ind=-1;
  nCandForSeg1=0;
  nextCandInd=-1;
  firstCandInd=-1;
  pFirstCand=this;
  metaMatchInd=-1;
  dedxInner=dedxOuter=dedxCombined=-1;
  dedxSigmaInner=dedxSigmaOuter=dedxSigmaCombined=-1;
  dedxNWireInner=dedxNWireOuter=dedxNWireCombined=0;
  dedxNWireCutInner=dedxNWireCutOuter=dedxNWireCutCombined=0;
  flag=0;
  for(Int_t i=0;i<4;i++){
      dedx        [i] = -1;
      dedxSigma   [i] = -1;
      dedxNWire   [i] =  0;
      dedxNWireCut[i] =  0;
  }
}

void HMdcTrkCand::print(void) const {
  // prints information about HMdcTrakCand on the screen

  printf("HMdcTrkCand: %isec. Inner seg.ind.=%i Outer seg.ind.=%i",
      sector+1,seg1Ind,seg2Ind);
  if(nCandForSeg1==0) printf(" No outer segnets!");
  else if(nCandForSeg1>0) 
      printf(" Num.of.outer segments=%i.",nCandForSeg1);
  printf("\n");
}
