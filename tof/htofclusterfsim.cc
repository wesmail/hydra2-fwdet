//*-- AUTHOR : Dusan Zovinec (23.10.2002)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HTofClusterFSim                                                           //
// This class is derived from HTofClusterF and passes track numbers of the   //
// HGeant cluster from hit to cluster level                                  //
///////////////////////////////////////////////////////////////////////////////

#include "htofclusterfsim.h"
#include "hdebug.h"
#include "hades.h"
#include "hspectrometer.h"
#include "htofdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hlocation.h"
#include "htofhitsim.h"
#include "htofclustersim.h"


#include <vector>
#include <algorithm>
#include <iostream>

ClassImp(HTofClusterFSim)

HTofClusterFSim::~HTofClusterFSim(void) {
}

Bool_t HTofClusterFSim::init(void) {
 // initialization of fHitCat and fClusterCat containers
 initParContainer(gHades->getSetup(),gHades->getRuntimeDb());
 fHitCat =gHades->getCurrentEvent()->getCategory(catTofHit);
  if (!fHitCat) {
    HTofDetector* tof=(HTofDetector*)(gHades->getSetup()->getDetector("Tof"));
    fHitCat=tof->buildMatrixCategory("HTofHitSim",0.5F);
    if (!fHitCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catTofHit,fHitCat,"Tof");
  } else {
    if (fHitCat->getClass()!=HTofHitSim::Class()) {
      Error("HTofClusterFSim::init()","Misconfigured input category");
      return kFALSE;
    }
  }
  iterh = (HIterator*)fHitCat->MakeIterator();

  fClusterCat=gHades->getCurrentEvent()->getCategory(catTofCluster);
  if (!fClusterCat) {
    HTofDetector* tof=(HTofDetector*)(gHades->getSetup()->getDetector("Tof"));
    fClusterCat=tof->buildMatrixCategory("HTofClusterSim",0.5F);
    if (!fClusterCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catTofCluster,fClusterCat,"Tof");
  } else {
    if (fClusterCat->getClass()!=HTofClusterSim::Class()) {
      Error("HTofClusterFSim::init()","Misconfigured output category");
      return kFALSE;
    }
  }
  iterc = (HIterator*)fClusterCat->MakeIterator();

  fActive = kTRUE;
  return kTRUE;
  }


Int_t  HTofClusterFSim::execute() {
// This method executes HTofClusterF::execute() and moreover
// it copies the track numbers from the hit to the cluster level containers

  HTofClusterF::execute();  // do work for real data part

  HTofHitSim *hit=NULL;
  HTofClusterSim *cluster=NULL;

  vector <Int_t> ntrack;
  iterh->Reset();
  iterc->Reset();
  while((cluster = (HTofClusterSim*)iterc->Next()) != NULL) {
    fCLoc[0]=cluster->getSector();
    fCLoc[1]=cluster->getModule();
    fCLoc[2]=cluster->getCell();
    cluster->clear();
    cluster->setNParticipants(1);

    ntrack.clear();
    for(Int_t i = 0; i < cluster->getClusterSize(); i ++){
      hit = (HTofHitSim*)iterh->Next();

      if(find(ntrack.begin(),ntrack.end(),hit->getNTrack1()) == ntrack.end()) ntrack.push_back(hit->getNTrack1());
      if(find(ntrack.begin(),ntrack.end(),hit->getNTrack2()) == ntrack.end()) ntrack.push_back(hit->getNTrack2());

    }
    sort(ntrack.begin(),ntrack.end());

    if(ntrack.size() < 4){  // just copy same track numbers to track1/2
	for(UInt_t i = 0; i < ntrack.size(); i ++){
	    cluster->setNTrack1( ntrack[i] );
	    cluster->setNTrack2( ntrack[i] );
	    cluster->incNParticipants();
	}
    } else {
	// distribute track numbers to track1/2 so that the maximum information is preserved
	for(UInt_t i = 0; i < ntrack.size() && i < 7; i += 2){  // allowed max is 6 (2x3)
	    cluster->setNTrack1( ntrack[i]   );
	    cluster->setNTrack2( ntrack[i+1] );
	    cluster->incNParticipants();              // max here == 3
	}
	if(ntrack.size() == 5 ) { // fill symmetric: use same number twice
	    cluster->setNTrack1( ntrack[4]   );
	    cluster->setNTrack2( ntrack[4] );
	    cluster->incNParticipants();
	}
	if(ntrack.size() > 3 ) {
	    Warning("HTofClusterFSim::execute()","number of track numbers larger than 3! n=%i",(Int_t)ntrack.size());
	}
	if(ntrack.size() > 6 ) {
	    Warning("HTofClusterFSim::execute()","number of track numbers larger than maximum stored tracks! n=%i",(Int_t)ntrack.size());
	}

    }


  }
  return 0;
}

