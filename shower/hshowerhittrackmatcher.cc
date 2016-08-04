#include "hshowerhittrackmatcher.h"

#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hshowerhit.h"
#include "hshowertrack.h"
#include "hshowerhitsim.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"

#include <map>
#include <vector>
#include <iostream>

using  namespace std;

ClassImp(HShowerHitTrackMatcher)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
//HShowerHitTrackMatcher
//
// HShowerHitTrackMatcher reconstructor searches 
// data describing the same pad in HShowerHit and HShowerTrack categories
// and join its to one object. 
// Results are stored in HShowerHitSim category
//
// The Shower task is split into several tasks as shown
// in the flow diagram below.
//
//   ----------------------
//  |     HShowerUnpacker  |                                                                              //
//  |   (embedding mode)   | \                                                                            //
//  |                      |  \      ------------------                                                   //
//   ----------------------   |     |  HGeantShower    |                                                  //
//                            |      ------------------\                                                  //
//                            |                         \                                                 //
//                            |      ------------------  \------------->  ----------------------          //
//                            |     |  HGeantWire      |   <------------ |  HShowerHitDigitizer |         //
//                            |      ------------------\                  ----------------------          //
//                            |                         \                                                 //
//                 -------------     ------------------  \------------->  -----------------------         //
//             -- | HShowerRaw  |   |  HShowerRawMatr  |   <------------ |  HShowerPadDigitizer  |        //
//            |    -------------     ------------------\                 |( creates track objects|        //
//            |                                         \                |  for real tracks in   |        //
//   ----------------------          ------------------  \               |  embedding mode too)  |        //
//  |   HShowerCalibrater  |        |  HShowerTrack    |  \<------------  -----------------------         //
//  |   (embedding mode)   |         ------------------\   \                                              //
//   ----------------------                             \   \             -----------------------         //
//            |                      ------------------  \   ----------> |   HShowerCopy         |        //
//             -------------------> |  HShowerCal      |  \<------------ |(add charge of real hit|        //
//                                   ------------------\   \             | in embedding too )    |        //
//                                                      \   \             -----------------------         //
//                                   ------------------  ----\--------->  -----------------------         //
//                                  |  HShowerHitHdr   |   <--\--------- |  HShowerHitFinder     |        //
//                                   ------------------        \          -----------------------         //
//                                   ------------------         \        |                                //
//                                  |  HShowerPID      |   <-----\-------|                                //
//                                   ------------------           \      |                                //
//                                   ------------------            \     |                                //
//                                  |  HShowerHitSim   |   <--------\----|                                //
//                                   ------------------ <            \                                    //
//                                                       \            \                                   //
//                                                        \-------------> ------------------------        //
//                                                                       | HShowerHitTrackMatcher |       //
//                                                                        ------------------------        //
//
//
//  In the case of TRACK EMBEDDING of simulated tracks into
//  experimental data the real data are written by the HShowerUnpacker into
//  HShowerRaw category. The real hits are taken into
//  account by the digitizer (adding of charges). The embedding mode is recognized
    //  automatically by analyzing the
    //  gHades->getEmbeddingMode() flag.
    //            Mode ==0 means no embedding
    //                 ==1 realistic embedding (first real or sim hit makes the game)
    //                 ==2 keep GEANT tracks   (as 1, but GEANT track numbers will always
    //                     win against real data. besides the tracknumber the output will
    //                     be the same as in 1)
    //
    //
    //////////////////////////////////////////////////////////////////////

    HShowerHitTrackMatcher::HShowerHitTrackMatcher(const Text_t *name,const Text_t *title) :
HReconstructor(name,title)
{
    fHitIter   = NULL;
    fTrackIter = NULL;
    m_zeroLoc.set(0);
}

HShowerHitTrackMatcher::HShowerHitTrackMatcher()
{
    fHitIter   = NULL;
    fTrackIter = NULL;
    m_zeroLoc.set(0);
}


HShowerHitTrackMatcher::~HShowerHitTrackMatcher(void) {
    if (fHitIter)   delete fHitIter;
    if (fTrackIter) delete fTrackIter;
}

Bool_t HShowerHitTrackMatcher::init() {
    // creates an iterator which loops over all local maxima
    // and an iterator which loops over all track number information


    m_pTrackCat = gHades->getCurrentEvent()->getCategory(catShowerTrack);
    if (m_pTrackCat) {
	fTrackIter = (HIterator*)m_pTrackCat->MakeIterator();
    }

    m_pHitCat = gHades->getCurrentEvent()->getCategory(catShowerHit);
    if (m_pHitCat) {
	fHitIter = (HIterator*)m_pHitCat->MakeIterator();
    }

    return kTRUE;
}

Bool_t HShowerHitTrackMatcher::finalize(void) {
    return kTRUE;
}

Int_t HShowerHitTrackMatcher::execute()
{
    // searching all pairs in HShowerHit and HShowerTrack, which
    // have the same address. All tracks belonging to the same
    // address are added to the list of tracks stored in HShowerHitSim

    if(fTrackIter == 0 || fHitIter == 0 ) return 0; // nothing to do

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
    // fill list of track numbers to HShowerHitSim
    HShowerHitSim*  pHit;
    fHitIter  ->Reset();
    while((pHit = (HShowerHitSim*)fHitIter->Next()))
    {   //loop over hits
	Int_t addTr = pHit->getAddress();
	map<Int_t,vector<Int_t> >::iterator pos = trackMap.find(addTr);
	if( pos != trackMap.end() ) {  // address found
	    vector<Int_t>& list = pos->second;
	    for( UInt_t i = 0; i < list.size(); i ++ ) {
		pHit->setTrack(list[i]);
	    }
	} else { // should not happen
	    Error("execute()","Corresponding pad address of the HShowerTrack list not found in HShowerHitSim!");
	}
    }
    //---------------------------------------------------------------

    return 0;
}



