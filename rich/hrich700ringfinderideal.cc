//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700RingFinderIdeal
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700ringfinderideal.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hevent.h"
#include "hgeantrich.h"
#include "hlinearcatiter.h"
#include "hmatrixcatiter.h"
#include "hparset.h"
#include "hrichdetector.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "richdef.h"
#include "hrichcalsim.h"
#include "hgeantkine.h"
#include "hrich700digipar.h"
#include "hrichhitsim.h"
#include "hrich700ringfittercop.h"

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

using namespace std;


ClassImp(HRich700RingFinderIdeal)

HRich700RingFinderIdeal::HRich700RingFinderIdeal()
{

}

HRich700RingFinderIdeal::~HRich700RingFinderIdeal()
{

}

Bool_t HRich700RingFinderIdeal::init()
{
   fCatKine = gHades->getCurrentEvent()->getCategory(catGeantKine);
   if (NULL == fCatKine) {
      Error("init()", "Initialization of catGeantKine category failed, returning...");
      return kFALSE;
   }

   fCatRichCal = gHades->getCurrentEvent()->getCategory(catRichCal);
   if (NULL == fCatRichCal) {
      Error("init()", "Initialization of catRicCal category failed, returning...");
      return kFALSE;
   }

   fCatRichHit = gHades->getCurrentEvent()->getCategory(catRichHit);
   if (NULL == fCatRichHit) {
       HRichDetector* pRichDet = static_cast<HRichDetector*>(gHades->getSetup()->getDetector("Rich"));
       if(pRichDet){
	   fCatRichHit = pRichDet->buildLinearCat("HRichHitSim", 10);
	   if (NULL == fCatRichHit) {
	       Error("init()", "Can not create catRichHit");
	       return kFALSE;
	   } else {
	       gHades->getCurrentEvent()->addCategory(catRichHit, fCatRichHit, "Rich");
	   }
       } else {
	   Error("init()", "No RICH detector in Setup");
       }
   }

   fDigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
    if(!fDigiPar) {
	Error("init", "Can not retrieve HRich700DigiPar");
        return kFALSE;
    }

   return kTRUE;

}


Bool_t HRich700RingFinderIdeal::reinit()
{
   return kTRUE;
}


Int_t HRich700RingFinderIdeal::execute()
{
   processEvent();
   return 0;
}

void HRich700RingFinderIdeal::processEvent()
{
   map<Int_t, vector<HRichCalSim*> > hitMap;
   Int_t nofRichCals = fCatRichCal->getEntries();
   for (Int_t iC = 0; iC < nofRichCals; iC++) {
      HRichCalSim* calSim = static_cast<HRichCalSim*>(fCatRichCal->getObject(iC));
      if (NULL == calSim) continue;
      Int_t nofTrackIds = calSim->getNofTrackIds();
      for (Int_t iT = 0; iT < nofTrackIds; iT++) {
         Int_t trackId = calSim->getTrackId(iT);
         if (trackId < 0) continue;
         hitMap[trackId].push_back(calSim);
      }
   }
   for(map<Int_t, vector<HRichCalSim*> >::iterator it = hitMap.begin(); it != hitMap.end(); it++) {
      Int_t trackId = it->first;
      addRichHit(1, trackId, it->second);
   }
}

void HRich700RingFinderIdeal::addRichHit(Int_t sector, Int_t trackId, const vector<HRichCalSim*>& cals)
{
   HRich700Ring ring;
   for (UInt_t i = 0; i < cals.size(); i++) {
      HRichCal* richCal = cals[i];
      Int_t loc[3];
      loc[0] = richCal->getSector();
      loc[1] = richCal->getCol();
      loc[2] = richCal->getRow();
      pair<Double_t, Double_t> xy = fDigiPar->getXY(loc);
      HRich700Hit hit;
      hit.fX = xy.first;
      hit.fY = xy.second;
      ring.fHits.push_back(hit);
    }

    HRich700RingFitterCOP::FitRing(&ring);

    HLocation loc;
    loc.set(1, sector);
    HRichHitSim* hit = (HRichHitSim*)fCatRichHit->getNewSlot(loc);
    hit = new (hit) HRichHitSim();

    if (NULL != hit) {
          // assign MC information
    	Float_t theta,phi;
    	hit->nSector               = fDigiPar->getInterpolatedSectorThetaPhi(ring.fCircleXCenter,ring.fCircleYCenter,theta,phi);
    	hit->fTheta                = theta;
        hit->fPhi                  = phi;
    	hit->track1                = trackId;
    	hit->weigTrack1            = cals.size();
    	hit->fRich700NofRichCals   = cals.size();
    	hit->fRich700CircleCenterX = ring.fCircleXCenter;
    	hit->fRich700CircleCenterY = ring.fCircleYCenter;
    	hit->fRich700CircleRadius  = ring.fCircleRadius;
    	hit->fRich700CircleChi2    = ring.fCircleChi2;

        Float_t x = ring.fCircleXCenter;
        Float_t y = ring.fCircleYCenter;

    	hit->setXY(x,y);
        Int_t pmtID = fDigiPar->getPMTId(x,y);
    	if(pmtID !=- 1){
    	    HRich700PmtData* data = fDigiPar->getPMTData(pmtID);
    	    if(data){
        		hit->setRingCenterX(data->fIndX);
        		hit->setRingCenterY(data->fIndY);
        		hit->setLabXYZ(x,y,data->fZ);
    	    }
    	} else {
    	    hit->setLabXYZ(x,y,-1000);
    	}
    }
}
//---------------------------------------------------------------------------
Bool_t HRich700RingFinderIdeal::finalize()
{
   return kTRUE;
}
