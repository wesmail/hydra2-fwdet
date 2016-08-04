//*-- AUTHOR Bjoern Spruck
//*-- created : 24.03.06
// Modified for Wall by M.Golubeva 01.11.2006

using namespace std;
#include "hwallonehitf.h"
#include "hwallonehit.h"
#include "hwallrefwinpar.h"
#include "hwalldetector.h"
#include "hwallcalibrater.h"
#include "walldef.h"
#include "hwallraw.h"
#include "hwallcal.h"
#include "hwallcalpar.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include <iostream>
#include <iomanip>

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
//
//  HWallHitFinder: select one hit from 4 cal hits
//
////////////////////////////////////////////////////////////////


void HWallOneHitF::initParContainer() {

  pWallRefWinPar = (HWallRefWinPar *)gHades->getRuntimeDb()
                                         ->getContainer("WallRefWinPar");
}

HWallOneHitF::HWallOneHitF(void)
{
   fCalCat        = 0;
   fOneHitCat     = 0;
   pWallRefWinPar = 0;
   iter           = 0;
   fLoc.set(1,-1);
}

HWallOneHitF::HWallOneHitF(const Text_t *name,const Text_t *title) :
   HReconstructor(name,title)
{
   fCalCat        = 0;
   fOneHitCat     = 0;
   pWallRefWinPar = 0;
   iter           = 0;
   fLoc.set(1,-1);
}

HWallOneHitF::~HWallOneHitF(void)
{
   if (iter) delete iter;
   iter = 0;
}

void HWallOneHitF::fillHit(HWallOneHit *hit, HWallCal *cal) {
}

Bool_t HWallOneHitF::init(void)
{
   initParContainer();

   HWallDetector* wall = (HWallDetector *)gHades->getSetup()->getDetector("Wall");

   if(!wall){
      Error("init","No Wall Det. found.");
      return kFALSE;
   }

   fCalCat = gHades->getCurrentEvent()->getCategory(catWallCal);
   if (!fCalCat) return kFALSE;
   
   fOneHitCat = wall->buildCategory(catWallOneHit);
   if (!fOneHitCat) return kFALSE;
   else
   gHades->getCurrentEvent()->addCategory(catWallOneHit,fOneHitCat,"Wall");

   pWallRefWinPar = (HWallRefWinPar*)gHades->getRuntimeDb()->getContainer("WallRefWinPar");
   if (!pWallRefWinPar) return kFALSE;

   iter = (HIterator *)fCalCat->MakeIterator(); //change to cal when it will be created

   return kTRUE;
}

Int_t HWallOneHitF::execute(void)
{
   HWallCal *cal=0;
   HWallOneHit* hit = NULL;
   fLoc.set(1,-1);

   Float_t calTime,hitTime=-400;
   Float_t calADC,hitAdc=-400;

   // Do the calibration here

   // Multiplicity checks i would postphone to hitfinder...

   // But how do we handle cases with tdc channel mult>4?
   // same question applies to what to do if hits were rejected by TDC itself -> see unpacker
   // maybe this should go to the raw category...

   //Fill cal category
   iter->Reset();
   while ((cal = (HWallCal *)iter->Next()) != 0) {
      Int_t m = cal->getNHits(); // channel hit multiplicty

      if(m <= 0) continue;      // No Hits -> forget it
      if(m > cal->getMaxMult()) m = cal->getMaxMult();

      fLoc[0] = cal->getCell();
      if(fLoc[0] == -1) continue;

      Float_t lower = pWallRefWinPar->getRefWinLow();
      Float_t upper = pWallRefWinPar->getRefWinHigh();

      for(Int_t i = 0; i < m; i ++){
	  hitTime = -400;
	  hitAdc  = -400;
	  calTime = cal->getTime(i+1);
	  calADC  = cal->getAdc(i+1);

	  if(calADC >= 0.0){
	      if(calTime >= lower && calTime <= upper) {
		  hitTime = calTime;
		  hitAdc  = calADC;
		  break;
	      }
	  }
      }

      hit = (HWallOneHit*) fOneHitCat->getSlot(fLoc);
      if(hit != NULL) {
	  hit = new(hit) HWallOneHit;
	  hit->setTime(hitTime);
	  hit->setCharge(hitAdc);
	  hit->setAddress(fLoc[0]);
	  fillHit(hit,cal);
      }
   }

  return 0;
}

ClassImp(HWallOneHitF)
