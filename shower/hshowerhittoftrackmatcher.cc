//*-- AUTHOR : Leszek Kidon
//*-- Modified : 19/04/05 by Jacek Otwinowski
//*-- Modified : 02/06/06 by Jacek Otwinowski
// 	  the code was modified to deal with different
//	  Shower-Tofino correlator algorithm 
//
//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
//
// HShowerHitTofTrackMatcher
//
// HShowerHitTofTrackMatcher reconstructor searches 
// data describing the same pad in HShowerHitTof and HShowerTrack categories
// and joins them to one object. 
// Results are stored in the HShowerHitTofTrack category.
//
//////////////////////////////////////////////////////////////////////

#include "hshowerhittoftrackmatcher.h"

#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "htofinodetector.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hlocation.h"
#include "hshowerhittof.h"
#include "hshowertrack.h"
#include "hshowerhittoftrack.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"
#include <cmath>

#include <map>
#include <vector>
#include <iostream>

using  namespace std;



#define ADDRESS_SHIFT 1.0e+4

ClassImp(HShowerHitTofTrackMatcher)

HShowerHitTofTrackMatcher::HShowerHitTofTrackMatcher(const Text_t *name,const Text_t *title) :
  HReconstructor(name,title)
{
   fHitIter=NULL;
   fTrackIter=NULL;
   m_zeroLoc.set(0);
}

HShowerHitTofTrackMatcher::HShowerHitTofTrackMatcher()
{
   fHitIter=NULL;
   fTrackIter=NULL;
   m_zeroLoc.set(0);
}


HShowerHitTofTrackMatcher::~HShowerHitTofTrackMatcher(void) {
     if (fHitIter) delete fHitIter;
     if (fTrackIter) delete fTrackIter;
}

Bool_t HShowerHitTofTrackMatcher::init() {
  // creates the HShowerHitTof, HShowerTrack and HShowerHitTofTrack categories 
  // and adds them to the current event,
  // creates an iterator which loops over all local maxima
  // and an iterator which loops over track number information

    printf("initialization of Shower/Tofino hit-track matcher\n");
    
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");
    HTofinoDetector *pTofinoDet = 0;
    pTofinoDet = (HTofinoDetector*)gHades->getSetup()
                                                  ->getDetector("Tofino");

    m_pTrackCat=gHades->getCurrentEvent()->getCategory(catShowerTrack);
    if (!m_pTrackCat) {
      m_pTrackCat=pShowerDet->buildCategory(catShowerTrack);

      if (!m_pTrackCat) return kFALSE;
      else gHades->getCurrentEvent()
                 ->addCategory(catShowerTrack, m_pTrackCat, "Shower");
    }

    m_pHitCat=gHades->getCurrentEvent()->getCategory(catShowerHitTof);
    if (!m_pHitCat) {
      m_pHitCat=pShowerDet->buildCategory(catShowerHitTof);

      if (!m_pHitCat) return kFALSE;
      else gHades->getCurrentEvent()
                 ->addCategory(catShowerHitTof, m_pHitCat, "Tofino");
    }

    m_pHitTrackCat = gHades->getCurrentEvent()
                           ->getCategory(catShowerHitTofTrack);
    if (!m_pHitTrackCat) {
      m_pHitTrackCat = new HLinearCategory("HShowerHitTofTrack", 1000);

      if (!m_pHitTrackCat) return kFALSE;
      else gHades->getCurrentEvent()
                 ->addCategory(catShowerHitTofTrack, m_pHitTrackCat, "Tofino");
    }

    fHitIter=(HIterator*)m_pHitCat->MakeIterator();
    fTrackIter=(HIterator*)m_pTrackCat->MakeIterator();

    return kTRUE;
}

Bool_t HShowerHitTofTrackMatcher::finalize(void) {
   return kTRUE;
}

Int_t HShowerHitTofTrackMatcher::execute()
{
    // searching all pairs in HShowerHitTof and HShowerTrack, which
    // have the same address. All tracks belonging to the same
    // address are added to the list of tracks stored in HShowerHitTofTrack



    //---------------------------------------------------------------
    // Fill a vector of track numbers belonging to the same pad
    map<Int_t, vector<Int_t> > trackMap;
    HShowerTrack* pTrack;
    fTrackIter->Reset();
    while((pTrack = (HShowerTrack *) fTrackIter->Next()))
    {
	Int_t addTr   = pTrack->getAddress();
        Int_t trackNr = pTrack->getTrack();
	map<Int_t,vector<Int_t> >::iterator pos = trackMap.find(addTr);
	if( pos == trackMap.end() ) {  // address not yet registered
	    vector<Int_t> v;
            v.push_back(trackNr);
	    trackMap.insert( make_pair( addTr, v ) );
	} else {                       // add track to the list of the pad
	    (pos->second).push_back(trackNr);
	}
    }
    //---------------------------------------------------------------


    //---------------------------------------------------------------
    // Create a HShowerHitTofTrack for each HShowerHitTof and
    // add the list of track numbers
    HShowerHitTof*  pHitTof;
    fHitIter  ->Reset();
    while((pHitTof = (HShowerHitTof*)fHitIter->Next()))
    {   //loop over hits

	// low Shower chamber efficiency case:
	// the module number has to be corrected -> use trueAddr instead of Addr

	Int_t Addr     = pHitTof->getAddress();
	Int_t trueAddr = pHitTof->getTrueAddress();

	if( Addr != trueAddr && !(pHitTof->getSum(0) == 0 && pHitTof->getSum(1) != 0) ) {
	    Error("execute()","trueAddress differnt from Address, but pHitTof->getSum(0) == 0 && pHitTof->getSum(1) != 0 not fullfilled!");
	}

	map<Int_t,vector<Int_t> >::iterator pos = trackMap.find(trueAddr);

	if( pos != trackMap.end() ) {  // address found
	    HShowerHitTofTrack* pHitTofTrack=(HShowerHitTofTrack *)m_pHitTrackCat->getNewSlot(m_zeroLoc);
	    if (pHitTofTrack != NULL) {
		pHitTofTrack = new (pHitTofTrack) HShowerHitTofTrack;
		*pHitTofTrack = *pHitTof; // copy data

		vector<Int_t>& list = pos->second;
		for( UInt_t i = 0; i < list.size(); ++ i ) {
		    pHitTofTrack->setTrack(list[i]);
		}
	    } else { // should not happen
		Error("execute()","Zero pointer retrieved for HShowerHitTofTrack!");
	    }
	} else { // should not happen
	    Error("execute()","Corresponding pad address of the HShowerTrack list not found in HShowerHitTof!");
	}
    }
    //---------------------------------------------------------------
    return 0;
}



