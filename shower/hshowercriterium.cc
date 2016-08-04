#pragma implementation
#include "hshowerhit.h"
#include "hshowerhitfpar.h"
#include "hshowercriterium.h"
ClassImp(HShowerCriterium)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
// HShowerCriterium
//
// HShowerCriterium contains efinition of shower in pre-Shower detector
// This is used to choose data for HShowerPID level in HShowerHit 
//
//////////////////////////////////////////////////////////////////////


Float_t HShowerCriterium::showerCriterium(HShowerHit* pHit, 
                                      Int_t& ret, HShowerHitFPar* pParams) {
  ret = 0;

  Float_t t1, g1;
  Float_t t2, g2;

  if (pHit->getModule()!=0) {
    ret = -1;
    return 0.0;
  }

  g1 = pParams->getGainPost1(); 
  t1 = pParams->getThresholdPost1(); 
  g2 = pParams->getGainPost2(); 
  t2 = pParams->getThresholdPost2(); 
  
  if (pHit->getSum(1) - pHit->getSum(0)*g1 > t1) {
     ret = 1;
     return pHit->getSumDiff01();
  }

  if (pHit->getSum(2) - pHit->getSum(0)*g2 > t2) {
     ret = 2;
     return pHit->getSumDiff02();
  }
  
  return 0.0; // criterium is failed for pHit 
}

