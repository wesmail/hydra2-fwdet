using namespace std;
#include "htoftrigger.h"
#include "hades.h"
#include "htofraw.h"
#include "htofhit.h"
#include "htofcalpar.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hdebug.h"
#include "tofdef.h"
#include "hevent.h"
#include "heventheader.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include <iostream> 
#include <iomanip>
#include "htofhitsim.h"
//*-- Author : L. Fabbietti & T. Eberl
//*-- Modified : Dec 29 1999

//_HADES_CLASS_DESCRIPTION 
////////////////////////
// HTofTrigger
//
// 
////////////////////////




HTofTrigger::HTofTrigger(void) {
  fHitCat= NULL;
  iterator = 0;
}

HTofTrigger::HTofTrigger(const Text_t *name,const Text_t *title,Int_t multi) :
                                                 HReconstructor (name,title) {
  fHitCat= NULL;
  iterator = 0;
  nThreshold = multi;
}

Bool_t HTofTrigger::init(void) {
  nSec2=nSec5=nMult=nEvCounter=0;
  kineGeant = new HKineSim(); // make Geant Kine container available
  
  fHitCat=gHades->getCurrentEvent()->getCategory(catTofHit);
  if (!fHitCat) {
    fHitCat=gHades->getSetup()->getDetector("Tof")->buildCategory(catTofHit);
    if (!fHitCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catTofHit,fHitCat,"Tof");
  }
 
  iterator = (HIterator*)fHitCat->MakeIterator();

  return kTRUE;
}

Int_t HTofTrigger::execute(void) {
  nEvCounter++;
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HTofTrigger::execute");
#endif

  nMult = 0;
  iterator->Reset();
  HTofHitSim *hit=NULL;

  while ( (hit=(HTofHitSim *)iterator->Next())!=NULL) {

    Int_t ntrack1= hit->getNTrack1();
    Int_t ntrack2= hit->getNTrack2();
    Int_t isector= hit->getSector();

    if (ntrack1!=0) checkTofHit(ntrack1,isector);    
    if (ntrack2!=0) checkTofHit(ntrack2,isector);
  }

  if (checkTofMult()) {
    nSec2 = nSec5 = 0;
    return 0;
  } else {
    nSec2 = nSec5 = 0;
    return 1;
  }

}
  
void HTofTrigger::checkTofHit(Int_t nTrack ,Int_t nSecTof) {

  
    if (kineGeant->checkChargedPart(nTrack ) ) {
      nMult ++;
      if (nSecTof==2) nSec2 ++;  
      if (nSecTof==5) nSec5 ++;
    }  
}

Bool_t  HTofTrigger::checkTofMult() {
  if (nSec2>=(nThreshold/2) && nSec5>=(nThreshold/2) && nMult >=nThreshold)
                                                                 return kTRUE;
  else return kFALSE;
}
ClassImp(HTofTrigger)






