#include "hshowerpidtrackmatcher.h"

#include "hruntimedb.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hmatrixcatiter.h"
#include "hlocation.h"
#include "hshowerpid.h"
#include "hshowertrack.h"
#include "hshowerpidtrack.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "showerdef.h"


ClassImp(HShowerPIDTrackMatcher)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
// HShowerPIDTrackMatcher
//
// HShowerPIDTrackMatcher reconstructor searches
// data describing the same pad in HShowerPID and HShowerTrack categories
// and join its to one object.
// Results are stored in HShowerPIDTrack category
//
//////////////////////////////////////////////////////////////////////

HShowerPIDTrackMatcher::HShowerPIDTrackMatcher(const Text_t *name,const Text_t *title) :
  HReconstructor(name,title)
{
   fPIDIter=NULL;
   fTrackIter=NULL;
   m_zeroLoc.set(0);
}

HShowerPIDTrackMatcher::HShowerPIDTrackMatcher()
{
   fPIDIter=NULL;
   fTrackIter=NULL;
   m_zeroLoc.set(0);
}


HShowerPIDTrackMatcher::~HShowerPIDTrackMatcher(void) {
     if (fPIDIter) delete fPIDIter;
     if (fTrackIter) delete fTrackIter;
}

Bool_t HShowerPIDTrackMatcher::init() {
  // creates the ShowerPID, ShowerTrack and ShowerPIDTrack categories 
  //  and adds them to the current event
  // creates an iterator which loops over all local maxima in PID
  // and an iterator which loops over all track number information

    printf("initialization of shower pid-track matcher\n");
    
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()
                                                  ->getDetector("Shower");

    m_pTrackCat=gHades->getCurrentEvent()->getCategory(catShowerTrack);
    if (!m_pTrackCat) {
      m_pTrackCat=pShowerDet->buildCategory(catShowerTrack);

      if (!m_pTrackCat) return kFALSE;
      else gHades->getCurrentEvent()
                        ->addCategory(catShowerTrack, m_pTrackCat, "Shower");
    }

    m_pPIDCat=gHades->getCurrentEvent()->getCategory(catShowerPID);
    if (!m_pPIDCat) {
      m_pPIDCat=pShowerDet->buildCategory(catShowerPID);

      if (!m_pPIDCat) return kFALSE;
      else gHades->getCurrentEvent()
                         ->addCategory(catShowerPID, m_pPIDCat, "Shower");
    }

    m_pPIDTrackCat=gHades->getCurrentEvent()->getCategory(catShowerPIDTrack);
    if (!m_pPIDTrackCat) {
      m_pPIDTrackCat=pShowerDet->buildCategory(catShowerPIDTrack);

      if (!m_pPIDTrackCat) return kFALSE;
      else gHades->getCurrentEvent()
                        ->addCategory(catShowerPIDTrack, m_pPIDTrackCat, "Shower");
    }

    fPIDIter=(HIterator*)m_pPIDCat->MakeIterator();
    fTrackIter=(HIterator*)m_pTrackCat->MakeIterator();

    return kTRUE;
}

Bool_t HShowerPIDTrackMatcher::finalize(void) {
   return kTRUE;
}


void HShowerPIDTrackMatcher::addPIDTrack(HShowerPID* pPID, 
         HShowerTrack* pTrack) {
//create one object, which contains data
//from HShowerPID and HShowerTrack objects
 
    HShowerPIDTrack *pPIDTrack;

    pPIDTrack=(HShowerPIDTrack *)m_pPIDTrackCat->getNewSlot(m_zeroLoc);
    if (pPIDTrack!=NULL) {
      pPIDTrack=new(pPIDTrack) HShowerPIDTrack;

      *pPIDTrack = *pPID;
      pPIDTrack->setTrack(pTrack->getTrack());
    }
}

Int_t HShowerPIDTrackMatcher::execute()
{
//searching all pairs in HShowerPID and HShowerTrack, which
//have the same address
//Because HShowerPID and HShowerTrack are sorted by m_nAddress
//reading data in tables is done once!

  HShowerPID *pPID;
  HShowerTrack *pTrack;
 
  fPIDIter->Reset();
  fTrackIter->Reset();

  pTrack = (HShowerTrack *)fTrackIter->Next();
  while((pPID = (HShowerPID*)fPIDIter->Next())) { //loop over PIDs
  //if tracks and hits are sorted resetting fTrackIter is not necessery

    do {
       if (pTrack && pPID->getAddress()==pTrack->getAddress()) { 
         addPIDTrack(pPID, pTrack);  //ok, the same track
         break;
       }
     }
    while((pTrack = (HShowerTrack *)fTrackIter->Next())) ;

    //searching other tracks with the same address number
    while((pTrack = (HShowerTrack *)fTrackIter->Next())) {
       if (pPID->getAddress()==pTrack->getAddress()) { 
         addPIDTrack(pPID, pTrack);  
       } else break;
    }

  }

  return 0;
}

