//*-- AUTHOR : Jaroslav Bielcik
//*-- Modified : 13/11/2001 by D. Zovinec
//*-- Modified : 25/05/2000 by R. Holzmann
//*-- Modified : 17/12/99 by Jaroslav Bielcik

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HTofHitFSim                                                               //
// This class is derived from HTofHitF and passes track numbers of the       //
// HGeant hit from raw to hit level                                          //
///////////////////////////////////////////////////////////////////////////////

#include "htofhitfsim.h"
#include "hdebug.h"
#include "hades.h"
#include "hspectrometer.h"
#include "htofdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hlocation.h"
#include "htofrawsim.h"
#include "htofhitsim.h"
#include "htofcalpar.h"
#include "hruntimedb.h"
#include "hstartdef.h"

ClassImp(HTofHitFSim)

 HTofHitFSim::HTofHitFSim(void){
   fTofSimulation=kTRUE;
 }

 HTofHitFSim::HTofHitFSim(const Text_t* name,const Text_t* title) : HTofHitF(name,title){
   fTofSimulation=kTRUE;
 }

 HTofHitFSim::~HTofHitFSim(void) {
 }

 Bool_t HTofHitFSim::init(void) {
 // initialization of fRawCat and fHitCat containers
 initParContainer(gHades->getSetup(),gHades->getRuntimeDb());
 fRawCat =gHades->getCurrentEvent()->getCategory(catTofRaw);
  if (!fRawCat) {
    HTofDetector* tof=(HTofDetector*)(gHades->getSetup()->getDetector("Tof"));
    fRawCat=tof->buildMatrixCategory("HTofRawSim",0.5F);
    if (!fRawCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catTofRaw,fRawCat,"Tof");
  } else {
    if (fRawCat->getClass()!=HTofRawSim::Class()) {
      Error("HTofHitFSim::init()","Misconfigured input category");
      return kFALSE;
    }
  }
  iter = (HIterator*)fRawCat->MakeIterator();  // cannot be "native", because
                                               // iter->getLocation() is used

  //-------------------------------------------------
  if(gHades->getCurrentEvent()->getCategory(catGeantKine) &&
     gHades->getEmbeddingMode()>0)
  {  // embedding mode
      fRawCatTmp =gHades->getCurrentEvent()->getCategory(catTofRawTmp);
      if(!fRawCatTmp){
          Error("init()","No catTofRawTmp in input! You are in embedding mode!");
	  return kFALSE;
      }
      iterTmp = (HIterator*)fRawCatTmp->MakeIterator();  // cannot be "native", because
                                                         // iter->getLocation() is used

      fHitCatTmp=gHades->getCurrentEvent()->getCategory(catTofHitTmp);
      if (!fHitCatTmp) {
	  HTofDetector* tof=(HTofDetector*)(gHades->getSetup()->getDetector("Tof"));
	  fHitCatTmp=tof->buildMatrixCategory("HTofHitSimTmp",0.5F);
	  if (!fHitCatTmp) {
              Error("init()","Could no build catTofHitTmp !");
	      return kFALSE;
	  }
	  else gHades->getCurrentEvent()->addCategory(catTofHitTmp,fHitCatTmp,"Tof");
          fHitCatTmp->setPersistency(kFALSE);
      }

      fCalParSim = new HTofCalPar("TofCalPar_Sim_Embedding","","TofCalProductionSimEmbedding");
      if(fCalParSim)gHades->getRuntimeDb()->addContainer(fCalParSim);
      else {
	  Error("init()","Could no create HTofCalPar for sim params, needed for embedding !");
      }

  }
  //-------------------------------------------------

  fHitCat=gHades->getCurrentEvent()->getCategory(catTofHit);
  if (!fHitCat) {
    HTofDetector* tof=(HTofDetector*)(gHades->getSetup()->getDetector("Tof"));
    fHitCat=tof->buildMatrixCategory("HTofHitSim",0.5F);
    if (!fHitCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catTofHit,fHitCat,"Tof");
  } else {
    if (fHitCat->getClass()!=HTofHitSim::Class()) {
      Error("HTofHitfSim::init()","Misconfigured output category");
      return kFALSE;
    }
  }

  // Get Start Hit category. If StartHit is not defined
  // a Warning is displayed and fStartHitCat is set to 0
  fStartHitCat = gHades->getCurrentEvent()->getCategory(catStart2Hit);
  if (!fStartHitCat) Warning("init","Start hit level not defined; setting start time to 0");

  fActive = kTRUE;
  return kTRUE;
}


HTofHit *HTofHitFSim::makeHit(TObject *address) {
  return new(address) HTofHitSim;
}

void HTofHitFSim::fillHit(HTofHit *hit, HTofRaw *raw) {
  HTofHitSim *hs=(HTofHitSim *)hit;
  HTofRawSim *rs=(HTofRawSim *)raw;
  hs->setNTrack1(rs->getNTrack1());
  hs->setNTrack2(rs->getNTrack2());
}


