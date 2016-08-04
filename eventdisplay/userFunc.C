#ifndef __USERFUNC__
#define __USERFUNC__

#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hparticledef.h"

#include "hades.h"
#include "hruntimedb.h"

#include "hgeomtransform.h"
#include "hgeomvector.h"
#include "hmdcsizescells.h"
#include "hrichgeometrypar.h"
#include "hrichpadtab.h"
#include "hrichpad.h"
#include "hrichpadcorner.h"
#include "hrichframe.h"
#include "hrichframecorner.h"

#include "hcategorymanager.h"
#include "hcategory.h"
#include "hstart2hit.h"
#include "hstart2cal.h"
#include "hparticlecand.h"

#include "hgeantdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "richdef.h"
#include "tofdef.h"
#include "walldef.h"
#include "showerdef.h"
#include "rpcdef.h"
#include "hparticledef.h"
#include "hstartdef.h"

#include "TString.h"
#include "TRandom.h"
#include "TStyle.h"
#include "TVector3.h"
#include "TMath.h"
#include "TEveManager.h"
#include "TEveQuadSet.h"
#include "TEveTrans.h"
#include "TEveFrameBox.h"
#include "TEveRGBAPalette.h"
#include "TGLCameraOverlay.h"
#include "TGLViewer.h"

using namespace std;

//---------------------------------------------------------
//             USER FUNC
// This macro provides some helper functions to be
// used in nextEven() function
//---------------------------------------------------------

Bool_t isTrigger(){
    return gHades->getCurrentEvent()->getHeader()->isTBit(13);
}

Bool_t selectVeto()
{
    // aug11

    HCategory *fCatStartCal = HCategoryManager::getCategory(catStart2Cal,1,"catStart2Cal");
    HCategory *fCatStartHit = HCategoryManager::getCategory(catStart2Hit,1,"catStart2Hit");

    Int_t nStartHits = fCatStartHit->getEntries();
    if(nStartHits != 1) {
	return kFALSE;
    }

    HStart2Hit *start   = (HStart2Hit*)fCatStartHit->getObject(0);
    Int_t   mult_start  = start->getMultiplicity();

    if( mult_start > 1 ) return kFALSE;
    Float_t start_time  = start->getTime();

    Int_t nStartCals = fCatStartCal->getEntries();
    Int_t mult_veto = 0;
    for(Int_t n = 0; n < nStartCals; n++) {
	HStart2Cal *start_cal = (HStart2Cal*)fCatStartCal->getObject(n);
	if(start_cal->getModule() == 3
	   &&start_cal->getTime(1) < 40.
	  ) {
	    Float_t veto_time  = start_cal->getTime(1);
	    mult_veto++;
	}
    }
    if(mult_veto != 0) return kFALSE;


    return kTRUE;
}

Bool_t isGoodEvent(){
   return kTRUE;
   return (isTrigger()&&selectVeto());

}

static Bool_t selectLeptonsBeta(HParticleCand* pcand)
{
    //  selection function for lepton candidates.

    Bool_t test = kFALSE;
    if( pcand->isFlagAND(5,
			 Particle::kIsAcceptedHitRICH,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitOuterMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegmentChi2() > 0
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(test)
    {
	//if(fabs(pcand->getDeltaTheta()) < 4. && fabs(pcand->getDeltaPhi()) < 4. ){
	    if (pcand->getRichMatchingQuality() < 1.5){
		if(pcand->getBeta() > 0.9) {
		    if(pcand->getMomentum() < 1600) {
			//if(pcand->getMdcdEdx() < 5 ) {
			if(pcand->getMetaMatchQuality() < 4 ) {
			    return kTRUE;
			} else return kFALSE;  // meta qa
			//} else return kFALSE; // mdcdedx
		    } else return kFALSE; // mom
		} else return kFALSE;  // beta
	    }else return kFALSE;  // rich qa
	//}else return kFALSE;  // rich window
    }
    return kFALSE;
}
static Bool_t selectLeptonsBetaNoFitMDC12(HParticleCand* pcand)
{
    //  selection function for lepton candidates.

    Bool_t test = kFALSE;
    if( pcand->isFlagAND(3,
			 Particle::kIsAcceptedHitRICH,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegInd() > 0
       &&
       pcand->getOuterSegInd() > 0
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(test)
    {
      //if (fabs(pcand->getDeltaTheta()) < 4. && fabs(pcand->getDeltaPhi()) < 4. ){
      if (pcand->getRichMatchingQuality() < 1.5){
          if(pcand->getBeta() > 0.9) {
	      if(pcand->getMomentum() < 1600) {

		  //if(pcand->getMdcdEdx() < 5 ) {
		      if(pcand->getMetaMatchQuality() < 4 ) {
			  return kTRUE;
		      } else return kFALSE;
		  //} else return kFALSE;
	      } else return kFALSE;
	  } else return kFALSE;
      } else return kFALSE;
    }
    return kFALSE;
}
static Bool_t selectLeptonsBetaNoFitMDC2(HParticleCand* pcand)
{
    //  selection function for lepton candidates.

    Bool_t test = kFALSE;
    if( pcand->isFlagAND(4,
			 Particle::kIsAcceptedHitRICH,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getOuterSegInd() > 0
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(test)
    {
      //if (fabs(pcand->getDeltaTheta()) < 4. && fabs(pcand->getDeltaPhi()) < 4. ){
      if (pcand->getRichMatchingQuality() < 1.5){
          if(pcand->getBeta() > 0.9) {
	      if(pcand->getMomentum() < 1600) {

		  //if(pcand->getMdcdEdx() < 5 ) {
		      if(pcand->getMetaMatchQuality() < 4 ) {
			  return kTRUE;
		      } else return kFALSE;
		  //} else return kFALSE;
	      } else return kFALSE;
	  } else return kFALSE;
      } else return kFALSE;
    }
    return kFALSE;
}

Bool_t selectLeptonsSharp2Mom(HParticleCand* pcand)
{
    // build in selection function for lepton candidates.
    // Requires besides an RICH hit, RK + META and fitted
    // inner+outer segment.


    Bool_t test = kFALSE;

    Double_t beta = pcand->getBeta();
    Double_t mom  = pcand->getMomentum();
    Int_t system  = pcand->getSystemUsed();

    if (mom > 1200 || beta > 1.1) return test;

    if((system == 0 && beta > 0.96 &&  mom < 150) ||
       (system == 1 && beta > 0.96 &&  mom < 150)
      )
    {   // for low momenta do not use RICH
	if( pcand->isFlagAND(4,
			     Particle::kIsAcceptedHitInnerMDC,
			     Particle::kIsAcceptedHitOuterMDC,
			     Particle::kIsAcceptedHitMETA,
			     Particle::kIsAcceptedRK
			    ) &&
	   !pcand->isFlagAND(5,
			     Particle::kIsAcceptedHitRICH,
			     Particle::kIsAcceptedHitInnerMDC,
			     Particle::kIsAcceptedHitOuterMDC,
			     Particle::kIsAcceptedHitMETA,
			     Particle::kIsAcceptedRK
			    )
	  ) test = kTRUE ;

      }

    if(test) test = pcand->getChi2()                < 100 ? kTRUE: kFALSE;
    if(test) test = pcand->getMetaMatchQuality()    < 2.  ? kTRUE : kFALSE ;

    return test;

}

static Bool_t selectHadrons(HParticleCand* pcand)
{
    //  selection function for lepton candidates.

    Bool_t test = kFALSE;
    if( pcand->isFlagAND(4,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitOuterMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(test)
    {
	if(pcand->getMetaMatchQuality() < 4 ) {
	    return kTRUE;
	} else return kFALSE;
    }
    return kFALSE;
}

static Bool_t selectHadronsNoFitMDC12(HParticleCand* pcand)
{
    //  selection function for lepton candidates.

    Bool_t test = kFALSE;
    if( pcand->isFlagAND(2,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegInd() > 0
       &&
       pcand->getOuterSegInd() > 0
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(test)
    {
	if(pcand->getMetaMatchQuality() < 4 ) {
	    return kTRUE;
	} else return kFALSE;
    }
    return kFALSE;
}
static Bool_t selectHadronsNoFitMDC2(HParticleCand* pcand)
{
    //  selection function for lepton candidates.

    Bool_t test = kFALSE;
    if( pcand->isFlagAND(3,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getOuterSegInd() > 0
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(test)
    {
	if(pcand->getMetaMatchQuality() < 4 ) {
	    return kTRUE;
	} else return kFALSE;
    }
    return kFALSE;
}


static Bool_t rejectEvent()
{
    // USER FUNCTION:
    //
    // Show only events in display which full fill
    // the conditions inside this function.
    //
    // this function is called in nextEvent().
    // If kTRUE is returned the event will be
    // skipped and new events will be read until
    // the function returns kFALSE.
    // The function is called after HParticleSorter.
    // HParticleCand are flagged using the select and
    // sort criteria and can be used for event
    // characterization

    Bool_t badEvent = kFALSE;
    if(gHades->getTaskListStatus() < 0 && gHades->getCurrentEvent()->getHeader()->getEventSeqNumber() > 1) badEvent=kTRUE;
    //################## USER ACTION ##########################

    Bool_t doSelection = kFALSE;


    if(doSelection)  //  modify this part
    {
	// example: select events with a fully reconstructed
	// e+ e- pair
	HCategory* particleCandCat    = (HCategory*) gHades->getCurrentEvent()->getCategory(catParticleCand);

	HParticleCandSim* cand;
	Int_t size = particleCandCat->getEntries();
	Int_t nPos=0;
	Int_t nEle=0;

	for(Int_t i = 0; i < size; i ++){
	    cand = HCategoryManager::getObject(cand,particleCandCat,i);
	    if(cand){
		if(cand->isFlagBit(kIsLepton)) {
		    if(cand->getCharge()>0) nPos++;
		    else                    nEle++;
		}
	    }
	}
	//if(nPos < 1 || nEle < 1) badEvent = kTRUE;  // at least a unlikesign lepton pair
	if(nPos < 1 && nEle < 1) badEvent = kTRUE;    // at least on lepton candidate
	if(badEvent) cout<<"nPos = "<<nPos<<", nEle = "<<nEle<<endl;
    }

    if(badEvent){
	cout<<"#########  rejectEvent : skipped event "<<gHades->getCurrentEvent()->getHeader()->getEventSeqNumber()<<endl;
    }
    return badEvent;
}



#endif // __USERFUNC__

