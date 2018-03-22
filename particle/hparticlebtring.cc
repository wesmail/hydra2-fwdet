//*-- Author   : Patrick Sellheim, Georgy Kornakov
//*-- Created  : 09/01/2014

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HParticleBtRing
//
//  In this container all ring identification output provided by RICH
//  backtracking is stored. HParticleCand::getRichBTInd() returns the
//  ID to identify the corresponding backtracking container.
//  -1 is returned if no backtracking information is provided.
//  Only  tracks that fulfill the criteria of the track selection
//  function and have at least one fired pad are stored. Track Fragments
//  used for shared maxima information are only calculated in 4 degree range
//  around good particle candidate with backtracking hits.
//
//
//
//   
//  ------------------------------------------------------------------------------
//
//    Track selection function for lepton candidates
//    ----------------------------------------------
//
//
//    Bool_t selectEpEm = kFALSE;
//    if(pcand->isFlagAND(4,
//			Particle::kIsAcceptedHitInnerMDC,
//			Particle::kIsAcceptedHitOuterMDC,
//			Particle::kIsAcceptedHitMETA,
//			Particle::kIsAcceptedRK)&&
//
//       pcand->getInnerSegmentChi2() > 0
//       &&
//       isGoodLepton(pcand)
//       &&
//       ( (sys == 0 && beta > 0.95) || (sys == 1 && beta > 0.93) )
//        &&
//       (MDCdEdx < 10)
//       &&
//       pcand->getChi2() < 1000) selectEpEm=kTRUE;
//       )
//           return kTRUE;
//       else
//	     return kFALSE;
//  ------------------------------------------------------------------------------
//
//
//
//  Example
//  -------
//
//    HCategory* btCat = (HCategory*)HCategoryManager::getCategory(catParticleBtRing);
//    HParticleBtRing*     btRing;
//    btRing = HCategoryManager::getObject(btRing,btCat,pcand1->getRichBTInd());
//    btRing->getMaxima();
//    btRing->getMaximaSharedTrack(pcand2->getIndex());
//
//
//
//  Hints for usage
//
//  Good Rings have at least 2 Maxima  and even more PadsRing. There are also
//  rings with 1 Maxima, but this data sample is dominated by noise and will
//  be unefficient to suppress large numbers of pions. Thresholds on other
//  observables can be used to improve hadron suppression.
//  Shared maxima are useful to identify overlapping rings from close pairs
//  and hadronic tracks which pick up charge from good rings.
//
//
//  ------------------------------------------------------------------------------
//
//  Output variables:
//  ----------------
//
//  PadsRing:   Number of pads with good charge (q>65) on ring prediction area.
//  PadsSum:    Number of all pads from clusters with at least one pad on ring.
//              prediction area.
//  ChargeRing: Charge of pads with good charge (q>65) on ring prediction area.
//  ChargeSum:  Charge of all pads from clusters with at least one pad on ring
//              prediction area.
//
//  Clusters:     Number of clusters with at least one pad on ring prediction area.
//  Maxima:       Number of maxima from contributing clusters inside 6 sigma area.
//  MaximaCharge: Sum of charge of all maxima pads contributing clusters inside 6 sigma area.
//  NearbyMaxima: Number of maxima from contributing clusters inside 3 sigma area.
//
//  Chi2:       Ring quality based on maxima position.
//  MeanDist:   Shift of ring center which minimizes Chi2 value
//  RingMatrix: Charge of  pads matching with the ring prediction region divided by the charge of all pads in
//              pad region with a radius of 8 pads around ring center.
//
//  MaximaShared:                Number of maxima shared with other tracks.
//  MaximaSharedFragement:      Number of maxima shared with other track Fragments in 4 degree
//                               area around particle candidate
//  MaximaChargeShared:          Sum  charge of all maxima pads that is shared other tracks.
//  MaximaChargeSharedFragment: Sum  charge of all maxima pads that is shared with other
//                               track fragments in 4 degree area around ring.
//  NearbyMaximaShared:          Number of nearby maxima shared with other tracks.
//
//
//  MaximaSharedTrack:          Number of maxima shared with one specific track.
//                              HParticleCand index is needed as input value.
//  MaximaSharedFragmentTrack: Number of maxima shared with one specific track fragment in
//                              4 degree area around ring.
//                              HParticleCand index is needed as input value.
//  MaximaChargeSharedTrack:    Sum  charge of all maxima pads that is with one specific track.
//                              HParticleCand index is needed as input value.
//  MaximaChargeSharedFragmentTrack: Sum  charge of all maxima pads that is with one specific
//                              track fragment in 4 degree area around ring.
//                              HParticleCand index is needed as input value.
//
//  NearbyMaximaSharedTrack: Number of nearby maxima shared with one specific track.
//                           HParticleCand index is needed as input value.
//  --------------------------------------------------------------------
//


//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hparticlebtring.h"

#include "TBuffer.h"

// ----------------------------------------------------------------

ClassImp(HParticleBtRing)

// ----------------------------------------------------------------

HParticleBtRing::HParticleBtRing( ){
    init();
}

HParticleBtRing::~HParticleBtRing( ){

}


void HParticleBtRing::init() {
    fPadsRing         = -1;
    fPadsSum          = -1;
    fChargeRing       = -1.;
    fChargeSum        = -1.;

    fClusters         = -1;
    fMaxima           = -1;
    fMaximaCharge     = -1;
    fNearbyMaxima     = -1;

    fChi2             = -1.;
    fMeanDist         = -1.;
    fRingMatrix       = -1.;

    fMaximaShared          = -1;
    fMaximaSharedBad       = -1;
    fMaximaChargeShared    = -1;
    fMaximaChargeSharedBad = -1;
    fNearbyMaximaShared    = -1;

    for(Int_t i=0; i < 10; i++ ){
	fMaximaSharedTrack[i]             = -1;
	fMaximaSharedBadTrack[i]          = -1;
        fMaximaChargeSharedTrack[i]       = -1;
        fMaximaChargeSharedBadTrack[i]    = -1;
        fNearbyMaximaSharedTrack[i]       = -1;
	fMaximaSharedTrackIdx[i]          = -1;
        fMaximaSharedBadTrackIdx[i]       = -1;
        fMaximaChargeSharedTrackIdx[i]    = -1;
        fMaximaChargeSharedBadTrackIdx[i] = -1;
        fNearbyMaximaSharedTrackIdx[i]    = -1;
    }
}

Int_t  HParticleBtRing::getMaximaSharedTrack(Int_t idx)
{
    //Returns number of maxima shared with ring of candidate with given index
    //If number of neighbouring tracks is larger than 5, -1 is returned
    //Shared maxima are searched in 3 sigma area
    if(fMaximaSharedTrackIdx[0] == -1)
	return 0;
    for(Int_t i=0; i < 10; i++){
        if(fMaximaSharedTrackIdx[i] == idx)
            return fMaximaSharedTrack[i];
    }
    for(Int_t i=0; i < 10; i++){
	if(fMaximaSharedBadTrackIdx[i] == idx)
	    return fMaximaSharedBadTrack[i];
    }

    return 0;
}
                            /*
Int_t  HParticleBtRing::getMaximaSharedFragmentTrack(Int_t idx)
{
    //Returns number of maxima shared with ring of candidate with given index
    //
    //If number of neighbouring tracks is larger than 5, -1 is returned
    //Shared maxima are searched in 3 sigma area
    if(fMaximaSharedBadTrackIdx[0] == -1)
	return 0;
    for(Int_t i=0; i < 10; i++){
        if(fMaximaSharedBadTrackIdx[i] == idx)
            return fMaximaSharedBadTrack[i];
    }
    return 0;
}                */

Float_t  HParticleBtRing::getMaximaChargeSharedTrack(Int_t idx)
{
    //Returns charge of maximas shared with ring of candidate with given index
    //If number of neighbouring tracks is larger than 5, -1 is returned
    //Shared maxima are searched in 3 sigma area
    if(fMaximaChargeSharedTrackIdx[0] == -1)
	return 0;
    for(Int_t i=0; i < 10; i++){
        if(fMaximaChargeSharedTrackIdx[i] == idx)
            return fMaximaChargeSharedTrack[i];
    }
    for(Int_t i=0; i < 10; i++){
	if(fMaximaChargeSharedBadTrackIdx[i] == idx)
	    return fMaximaChargeSharedBadTrack[i];
    }


    return 0;
}
       /*
Float_t  HParticleBtRing::getMaximaChargeSharedFragmentTrack(Int_t idx)
{
    //Returns charge of maximas shared with ring of candidate with given index
    //If number of neighbouring tracks is larger than 5, -1 is returned
    //Shared maxima are searched in 3 sigma area
    if(fMaximaChargeSharedBadTrackIdx[0] == -1)
	return 0;
    for(Int_t i=0; i < 10; i++){
        if(fMaximaChargeSharedBadTrackIdx[i] == idx)
            return fMaximaChargeSharedBadTrack[i];
    }
    return 0;
}     */

Int_t  HParticleBtRing::getNearbyMaximaSharedTrack(Int_t idx)
{
    //Returns number of maxima shared with ring of candidate with given index
    //If number of neighbouring tracks is larger than 5, -1 is returned
     //Shared maxima are searched in 3 sigma area
    if(fNearbyMaximaSharedTrack[0] == -1)
	return 0;
    for(Int_t i=0; i < 10; i++){
	if(fNearbyMaximaSharedTrackIdx[i] == idx)
	    return fNearbyMaximaSharedTrack[i];
    }
    return 0;
}

void HParticleBtRing::Streamer(TBuffer &R__b)
{
   // Stream an object of class HParticleBtRing.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fPadsRing;
      R__b >> fPadsSum;
      R__b >> fChargeRing;
      R__b >> fChargeSum;
      R__b >> fClusters;
      R__b >> fMaxima;
      if(R__v > 1)R__b >> fMaximaCharge;
      else                fMaximaCharge=-1;
      R__b >> fNearbyMaxima;
      R__b >> fChi2;
      R__b >> fMeanDist;
      R__b >> fRingMatrix;
      R__b >> fMaximaShared;
      R__b.ReadStaticArray((int*)fMaximaSharedTrack);
      R__b.ReadStaticArray((int*)fMaximaSharedTrackIdx);
      //
      if(R__v > 1) {
	  R__b >> fMaximaSharedBad;
	  R__b.ReadStaticArray((int*)fMaximaSharedBadTrack);
	  R__b.ReadStaticArray((int*)fMaximaSharedBadTrackIdx);
	  R__b >> fMaximaChargeShared;
	  R__b.ReadStaticArray((float*)fMaximaChargeSharedTrack);
	  R__b.ReadStaticArray((int*)fMaximaChargeSharedTrackIdx);
	  R__b >> fMaximaChargeSharedBad;
	  R__b.ReadStaticArray((float*)fMaximaChargeSharedBadTrack);
	  R__b.ReadStaticArray((int*)fMaximaChargeSharedBadTrackIdx);
	  R__b >> fNearbyMaximaShared;
      } else {
	  fMaximaSharedBad       = -1;
	  fMaximaChargeShared    = -1;
	  fMaximaChargeSharedBad = -1;
	  fNearbyMaximaShared    = -1;

	  for(Int_t i=0;i<10;i++){
	      fMaximaSharedBadTrack[i]         = -1;
	      fMaximaSharedBadTrackIdx[i]      = -1;
	      fMaximaChargeSharedTrack[i]      = -1;
	      fMaximaChargeSharedTrackIdx[i]   = -1;
	      fMaximaChargeSharedBadTrack[i]   = -1;
	      fMaximaChargeSharedBadTrackIdx[i]= -1;
	  }


      }
      R__b.ReadStaticArray((int*)fNearbyMaximaSharedTrack);
      R__b.ReadStaticArray((int*)fNearbyMaximaSharedTrackIdx);
      R__b.CheckByteCount(R__s, R__c, HParticleBtRing::IsA());
   } else {
      R__c = R__b.WriteVersion(HParticleBtRing::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << fPadsRing;
      R__b << fPadsSum;
      R__b << fChargeRing;
      R__b << fChargeSum;
      R__b << fClusters;
      R__b << fMaxima;
      R__b << fMaximaCharge;
      R__b << fNearbyMaxima;
      R__b << fChi2;
      R__b << fMeanDist;
      R__b << fRingMatrix;
      R__b << fMaximaShared;
      R__b.WriteArray(fMaximaSharedTrack, 10);
      R__b.WriteArray(fMaximaSharedTrackIdx, 10);
      R__b << fMaximaSharedBad;
      R__b.WriteArray(fMaximaSharedBadTrack, 10);
      R__b.WriteArray(fMaximaSharedBadTrackIdx, 10);
      R__b << fMaximaChargeShared;
      R__b.WriteArray(fMaximaChargeSharedTrack, 10);
      R__b.WriteArray(fMaximaChargeSharedTrackIdx, 10);
      R__b << fMaximaChargeSharedBad;
      R__b.WriteArray(fMaximaChargeSharedBadTrack, 10);
      R__b.WriteArray(fMaximaChargeSharedBadTrackIdx, 10);
      R__b << fNearbyMaximaShared;
      R__b.WriteArray(fNearbyMaximaSharedTrack, 10);
      R__b.WriteArray(fNearbyMaximaSharedTrackIdx, 10);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

