#include "hgeantkine.h"
#include "hlinearcategory.h"
#include "hcategorymanager.h"
#include "hlinkeddataobject.h"
#include "hgeantmdc.h"
#include "hgeanttof.h"
#include "hgeantrich.h"
#include "hgeantshower.h"
#include "hgeantrpc.h"
#include "hgeantwall.h"
#include "hgeantemc.h"
#include "hgeantstart.h"
#include "hgeantfwdet.h"
#include "TRandom.h"
#include "TString.h"

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

//*-- Author : Romain Holzmann, GSI
//*-- Modified : 23/03/05 by Romain Holzmann
//*-- Modified : 05/04/04 by Romain Holzmann
//*-- Modified : 03/11/03 by Romain Holzmann
//*-- Modified : 09/10/03 by Peter Zumbruch
//*-- Modified : 06/10/03 by Romain Holzmann
//*-- Modified : 14/12/99 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt
//
//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
//
//  HGeantKine
// 
//  GEANT KINE data on primary and secondary particles in the simulated event
//
//***************************************************************************
//
// Inline functions:
//
//   void setWeight(Float_t aWeight)          Set weight of track
//   Int_t getTrack() const                   Get GEANT track number
//   Int_t getID() const                      Get GEANT particle id
//   Int_t getParentTrack() const             Get parent track number
//   void setParentTrack(Int_t track)         Set parent track number
//   Float_t getTotalMomentum() const         Get particle momentum (Ptot)
//   Float_t getTransverseMomentum() const    Get transverse momentum (Pt)
//   void getMomentum(HGeomVector& v)         Get momentum vector
//   void setActive(Bool_t flag = kTRUE)      Set track active/inactive
//   void setSuppressed(Bool_t flag = kTRUE)  Set track suppressed/unsuppressed
//   void setUserVal(Float_t val)             Set user specific values (for eaxpmple used for track embedding)
//   Bool_t isActive() const                  Test if track is active
//   Bool_t isPrimary() const                 Test if track is a primary
//   Bool_t isSuppressed() const              Test if track is suppressed
//   Float_t getUserVal()                     Get the specific user flag
//   Int_t setRichHitIndex(Int_t index)       Set index of RICH photon hit
//   Int_t setMdcHitIndex(Int_t index)        Set index of MDC hit
//   Int_t setTofHitIndex(Int_t index)        Set index of TOF hit
//   Int_t setRpcHitIndex(Int_t index)        Set index of RPC hit
//   Int_t setShowerHitIndex(Int_t index)     Set index of SHOWER hit
//   Int_t setWallHitIndex(Int_t index)       Set index of SHOWER hit
//   Int_t getFirstRichHit()                  Get index of first RICH photon hit of this track
//   Int_t getFirstMdcHit()                   Get index of first MDC hit of this track
//   Int_t getFirstTofHit()                   Get index of first TOF hit of this track
//   Int_t getFirstRpcHit()                   Get index of first TOF hit of this track
//   Int_t getFirstShowerHit()                Get index of first RPC hit of this track
//   Int_t getFirstWallHit()                  Get index of first WALL hit of this track
//   Int_t getFirstEmcHit()                   Get index of first EMC hit of this track
//   Int_t getFirstStartHit()                 Get index of first Start hit of this track
//   HLinkedDataObject* nextRichHit()         Get pointer to next RICH photon hit in linked list
//   HLinkedDataObject* nextMdcHit()          Get pointer to next MDC photon hit in linked list
//   HLinkedDataObject* nextTofHit()          Get pointer to next TOF photon hit in linked list
//   HLinkedDataObject* nextShowerHit()       Get pointer to next SHOWER hit in linked list
//   HLinkedDataObject* nextRpcHit()          Get pointer to next RPC hit in linked list
//   HLinkedDataObject* nextWallHit()         Get pointer to next WALL hit in linked list
//   HLinkedDataObject* nextEmcHit()          Get pointer to next EMC hit in linked list
//   HLinkedDataObject* nextStartHit()        Get pointer to next START hit in linked list
//   void setRichCategory(HCategory* p)       Set pointer to RICH photon hit category
//   void setMdcCategory(HCategory* p)        Set pointer to MDC hit category
//   void setTofCategory(HCategory* p)        Set pointer to TOF hit category
//   void setRpcCategory(HCategory* p)        Set pointer to RPC hit category
//   void setShowerCategory(HCategory* p)     Set pointer to SHOWER hit category
//   void setWallCategory(HCategory* p)       Set pointer to WALL hit category
//   void setEmcCategory(HCategory* p)        Set pointer to EMC hit category
//   void setStartCategory(HCategory* p)      Set pointer to StART hit category
//   void resetRichIter()                     Reset iterator on RICH photon hits
//   void resetMdcIter()                      Reset iterator on MDC hits
//   void resetTofIter()                      Reset iterator on TOF hits
//   void resetRpcIter()                      Reset iterator on RPC hits
//   void resetShowerIter()                   Reset iterator on SHOWER hits
//   void resetWallIter()                     Reset iterator on WALL hits
//   void resetEmcIter()                      Reset iterator on EMC hits
//   void resetStartIter()                    Reset iterator on START hits
//
/////////////////////////////////////////////////////////////////////////////

ClassImp(HGeantKine)

HGeantKine::HGeantKine(void) {
  // Default constructor.
  trackNumber = 0;
  parentTrack = 0;
  particleID = 0;
  mediumNumber = 0;
  creationMechanism = 0;
  xVertex = yVertex = zVertex = 0.;
  xMom = yMom = zMom = 0.;
  generatorInfo = 0.;
  generatorInfo1 = 0.;
  generatorInfo2 = 0.;
  generatorWeight = 1.;
  firstRichHit = firstMdcHit = firstTofHit = firstShowerHit = firstRpcHit = firstWallHit = firstEmcHit= firstStartHit = firstFwDetHit = -1;
  pRich = pMdc = pTof = pShow = pRpc = pWall = pEmc = pStart = pFwDet = NULL;
  richIndex = mdcIndex = showIndex = tofIndex = rpcIndex = wallIndex = emcIndex = startIndex = fwDetIndex = 0;
  active = kFALSE;
  suppressed = kFALSE;
  userVal = -1;
  acceptance = 0;
}

HGeantKine::HGeantKine(HGeantKine &aKine) : TObject(aKine) {
  // Copy constructor.
  trackNumber = aKine.trackNumber;
  parentTrack = aKine.parentTrack;
  particleID = aKine.particleID;
  mediumNumber = aKine.mediumNumber;
  creationMechanism = aKine.creationMechanism;
  xVertex = aKine.xVertex;
  yVertex = aKine.yVertex;
  zVertex = aKine.zVertex;
  xMom = aKine.xMom;
  yMom = aKine.yMom;
  zMom = aKine.zMom;
  generatorInfo = aKine.generatorInfo;
  generatorInfo1 = aKine.generatorInfo1;
  generatorInfo2 = aKine.generatorInfo2;
  generatorWeight = aKine.generatorWeight;
  firstRichHit = aKine.firstRichHit;
  firstMdcHit = aKine.firstMdcHit;
  firstTofHit = aKine.firstTofHit;
  firstRpcHit = aKine.firstRpcHit;
  firstShowerHit = aKine.firstShowerHit;
  firstWallHit = aKine.firstWallHit;
  firstEmcHit = aKine.firstEmcHit;
  firstStartHit = aKine.firstStartHit;
  firstFwDetHit = aKine.firstFwDetHit;
  active = aKine.active;
  suppressed = aKine.suppressed;
  userVal = aKine.userVal;
  acceptance = aKine.acceptance;

}

HGeantKine::~HGeantKine(void) {
  // Destructor.
}

void HGeantKine::setParticle(Int_t aTrack, Int_t aID) {
  // Set GEANT track and particle id.
  trackNumber = aTrack;
  particleID = aID;
}

void HGeantKine::setCreator(Int_t aPar, Int_t aMed, Int_t aMech) {
  // Set information on particle creation: parent track number, midium number, and creation mechanism.
  parentTrack = aPar;
  mediumNumber = aMed;
  creationMechanism = aMech;
}

void HGeantKine::setVertex(Float_t ax, Float_t ay, Float_t az) {
  // Set creation vertex (x,y,z in mm) in lab frame.
  xVertex = ax;
  yVertex = ay;
  zVertex = az;
}

void HGeantKine::setMomentum(Float_t apx, Float_t apy, Float_t apz) {
  // Set particle momentum components at creation (in MeV/c).
  xMom = apx;
  yMom = apy;
  zMom = apz;
}

void HGeantKine::setGenerator(Float_t aInfo, Float_t aWeight) {
  // Set information on generator and weight.
  generatorInfo = aInfo;
  generatorWeight = aWeight;
}

void HGeantKine::setGenerator(Float_t aInfo, Float_t aInfo1, Float_t aInfo2) {
  // Set full generator information.
  generatorInfo = aInfo;
  generatorInfo1 = aInfo1;
  generatorInfo2 = aInfo2;
}

void HGeantKine::getParticle(Int_t &aTrack, Int_t &aID) {
  // Retrieve GEANT track number and id.
  aTrack = trackNumber;
  aID = particleID;
}

void HGeantKine::getCreator(Int_t &aPar, Int_t &aMed, Int_t &aMech) {
  // Retrieve parenet track number, medium and mechanism of creation.
  aPar = parentTrack;
  aMed = mediumNumber;
  aMech = creationMechanism;
}

void HGeantKine::getVertex(Float_t &ax, Float_t &ay, Float_t &az) {
  // Retrieve creation vertex (in mm) in lab frame.
  ax = xVertex;
  ay = yVertex;
  az = zVertex;
}

void HGeantKine::getVertex(HGeomVector& ver) {
  // Retrieve creation vertex (in mm) in lab frame.
    ver.setXYZ(xVertex,yVertex,zVertex);
}

Float_t HGeantKine::getDistFromVertex() {
  // get distance from primary vertex
    return TMath::Sqrt(xVertex*xVertex + yVertex*yVertex + zVertex*zVertex);
}

void HGeantKine::getMomentum(Float_t &apx, Float_t &apy, Float_t &apz) {
  // Retrieve momentum components at creation time.
  apx = xMom;
  apy = yMom;
  apz = zMom;
}

void HGeantKine::getGenerator(Float_t &aInfo, Float_t &aWeight) {
  // Retrieve generator infomation and track weight. 
  aInfo = generatorInfo;
  aWeight = generatorWeight;
}

void HGeantKine::getGenerator(Float_t &aInfo, Float_t &aInfo1, Float_t &aInfo2) {
  // Retrieve full generator information.
  aInfo = generatorInfo;
  aInfo1 = generatorInfo1;
  aInfo2 = generatorInfo2;
}

Int_t   HGeantKine::getGeneration(HGeantKine* kine)
{

    // returns the generation of the particle kine
    // primaries are gen 0. daughters of
    // primaries    gen 1 etc.

    HGeantKine* kine2 = kine;
    Int_t genCt       = 0;

    while (kine2->getParentTrack()!=0){
	kine2 = HCategoryManager::getObject(kine2,catGeantKine,kine2->getParentTrack()-1);
	if(kine) {
	    genCt++;
	}
    }
    return genCt;
}

Int_t   HGeantKine::getMothers(HGeantKine* kine,vector<HGeantKine*>& mothers)
{
    // return in vector all mothers of the particles of kine
    // index 0 is mother , index 1 grand mother etc.
    //the function return the number of mothers found.

    mothers.clear();
    HGeantKine* kine2 = kine;
    Int_t genCt       = 0;

    while (kine2->getParentTrack()!=0){
	kine2 = HCategoryManager::getObject(kine2,catGeantKine,kine2->getParentTrack()-1);
	if(kine) {
	    mothers.push_back(kine2);
	    genCt++;
	}
    }
    return genCt;
}

Int_t HGeantKine::getAllDaughters(HGeantKine* mother,vector<HGeantKine*>& daughters)
{
  // retrieve daughters from all following generations of this mother.
  //
    daughters.clear();

    if(!mother) return 0;

    HCategory* kineCat = HCategoryManager::getCategory(catGeantKine,0,"catGeantKine");

    if(!kineCat) return 0;

    Int_t n     = kineCat->getEntries();
    Int_t track = mother->getTrack();

    HGeantKine* kine = 0;
    for(Int_t i = track; i < n ; i++) { // starting at the first track after mother
	kine = HCategoryManager::getObject(kine,kineCat,i);
	if(kine->hasAncestor(track,(HLinearCategory*) kineCat)){
             daughters.push_back(kine);
	}
    }

    return daughters.size();
}

Int_t HGeantKine::getDaughters(HGeantKine* mother,vector<HGeantKine*>& daughters)
{
  // retrieve daughters from this mother.
  //
    daughters.clear();

    if(!mother) return 0;

    HCategory* kineCat =  HCategoryManager::getCategory(catGeantKine,0,"catGeantKine");

    if(!kineCat) return 0;

    Int_t n     = kineCat->getEntries();
    Int_t track = mother->getTrack();

    HGeantKine* kine = 0;
    for(Int_t i = track; i < n ; i++) { // starting at the first track after mother
	kine = HCategoryManager::getObject(kine,kineCat,i);
	if(kine->getParentTrack() == track){
             daughters.push_back(kine);
	}
    }

    return daughters.size();
}

HGeantKine*  HGeantKine::getChargedDecayDaughter(HGeantKine* mother)
{
    //-------------------------------------------------------
    // has the particle a decayed daughter ? like pion->muon
    HGeantKine* kine = mother;
    vector<HGeantKine*> daughters;
    HGeantKine::getDaughters(kine,daughters);
    HGeantKine* d = 0;
    Int_t chrgP = HPhysicsConstants::charge(kine->getID());

    for(UInt_t i=0;i<daughters.size();i++){
	HGeantKine* d1=daughters[i];
	Int_t chrg = HPhysicsConstants::charge(d1->getID());
	if(chrg != chrgP || chrg == 0) continue;
	if(d1->getMechanism()==5 ) { d = d1; break;}
    }
    //-------------------------------------------------------

    return d;
}


Float_t HGeantKine::getThetaDeg()
{
    // Convert geant mom to theta angle in degree
    Float_t mom = getTotalMomentum();
    return (mom>0.) ? (TMath::RadToDeg() * TMath::Abs(TMath::ACos(zMom / mom))) : 0.;

}

Float_t HGeantKine::getPhiDeg( Bool_t labSys)
{
    // Convert geant mom to phi angle in degree
    // labSys == kTRUE  -> 0-360 deg (lab)
    //        == kFALSE -> 0-60  deg (sector)
    Float_t phi   = TMath::RadToDeg() * TMath::ATan2( yMom, xMom);
    if (phi < 0.) phi += 360.;
    if(!labSys) phi=fmod(phi,60.f)+60.;
    return phi;
}

Int_t HGeantKine::getSector()
{
    // returns the sector number (0-5)
    Float_t phi   = TMath::RadToDeg() * TMath::ATan2( yMom, xMom);
    if (phi < 0.) phi += 360.;
    return  (phi < 60.)? 5 : ((Int_t) (phi / 60.) - 1);
}

Int_t HGeantKine::getPhiThetaDeg(Float_t& theta,Float_t& phi, Bool_t labSys)
{
    // Convert geant mom to phi/theta angle in degree
    // labSys == kTRUE  -> 0-360 deg (lab)
    //        == kFALSE -> 0-60  deg (sector)
    // returns the sector number (0-5)
    // Meaningful if used with primaries
    Float_t mom = getTotalMomentum();
    theta = (mom>0.) ? (TMath::RadToDeg() * TMath::Abs(TMath::ACos(zMom / mom))) : 0.;
    phi   = TMath::RadToDeg() * TMath::ATan2( yMom, xMom);
    if (phi < 0.) phi += 360.;
    Int_t s = (phi < 60.)? 5 : ((Int_t) (phi / 60.) - 1);
    if(!labSys) phi=fmod(phi,60.f)+60.;
    return s;
}

void HGeantKine::setNewTrackNumber(Int_t track) {
  // Set track number to new value and update indexes in all hit categories. 
  // Used in HGeant to re-index after having suppressed non-active tracks;
  // see fillkine.cc.
  Int_t next = 0;
  HLinkedDataObject* p = NULL;

  trackNumber = track;

  if ( (next = (Int_t)firstMdcHit) != -1) {  // update Mdc hits
    resetMdcIter();
    do {
       p = (HLinkedDataObject*)(pMdc->getObject(next));
       ( (HGeantMdc*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

  if ( (next = (Int_t)firstRichHit) != -1) { // update Rich photon hits
    resetRichIter();
    do {
       p = (HLinkedDataObject*)(pRich->getObject(next));
       ( (HGeantRichPhoton*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

  if ( (next = (Int_t)firstTofHit) != -1) {  // update Tof hits
    resetTofIter();
    do {
       p = (HLinkedDataObject*)(pTof->getObject(next));
       ( (HGeantTof*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

  if ( (next = (Int_t)firstRpcHit) != -1) {  // update Rpc hits
    resetRpcIter();
    do {
       p = (HLinkedDataObject*)(pRpc->getObject(next));
       ( (HGeantRpc*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

  if ( (next = (Int_t)firstShowerHit) != -1) { // update Shower hits
    resetShowerIter();
    do {
       p = (HLinkedDataObject*)(pShow->getObject(next));
       ( (HGeantShower*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

 if ( (next = (Int_t)firstWallHit) != -1) { // update Wall hits
    resetWallIter();
    do {
       p = (HLinkedDataObject*)(pWall->getObject(next));
       ( (HGeantWall*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

  if ( (next = (Int_t)firstEmcHit) != -1) {  // update Emc hits
    resetEmcIter();
    do {
       p = (HLinkedDataObject*)(pEmc->getObject(next));
       ( (HGeantEmc*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

  if ( (next = (Int_t)firstStartHit) != -1) {  // update Start hits
    resetStartIter();
    do {
       p = (HLinkedDataObject*)(pStart->getObject(next));
       ( (HGeantStart*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

  if ( (next = (Int_t)firstFwDetHit) != -1) {  // update hits of Forward detector
    resetFwDetIter();
    do {
       p = (HLinkedDataObject*)(pFwDet->getObject(next));
       ( (HGeantFwDet*)p )->setTrack(track);
    } while( (next = p->getNextHitIndex()) != -1);
  }

}

///////////////////////////////////////////////////////////////////////////////

Int_t HGeantKine::setHitIndex(HCategory* pCat,Short_t& firstHit, Int_t index) {
  // Set next hit index in generic linked list.
  //
  if(pCat == NULL) return 1;
  if(pCat->getObject(index) == NULL) return 1;

  if(firstHit == -1) {              // first hit in linked list
     firstHit = (Short_t)index; 
     active = kTRUE;
  } else {
     HLinkedDataObject* p = NULL;
     Int_t next = (Int_t)firstHit;
     do {                               // find end of list
        p = (HLinkedDataObject*)(pCat->getObject(next));
        next = p->getNextHitIndex();
     } while(next != -1);
     p->setNextHitIndex(index);         // append hit to end of list
  }
  return 0;
}

void HGeantKine::sortRichHits(void) {
  // Sort RICH photon hits in linked list for present track.
  //
  if(pRich == NULL) return;
  Int_t nHits = getNRichHits();
  if(nHits < 2) return;  // nothing to do

  HLinkedDataObject* p = NULL;
  for(Int_t i=0; i<nHits-1;i++) {  //  iterate through list nHits times
     resetRichIter();
     Int_t next = (Int_t)firstRichHit;
     do {
        p = (HLinkedDataObject*)(pRich->getObject(next));
	//
	// do here the swapping of hits
	//
        next = p->getNextHitIndex();
     } while(next != -1);
  }
  return;
}

void HGeantKine::sortMdcHits(void) {
  //
  // Sort MDC hits in linked list for actual track by increasing time-of-flight
  // (this is a simple bubble sort, should be replaced by something faster!).
  //
  // As sortVariable is non-persistent, the sort cannot be done a posteriori,
  // i.e. when the tree is read back. It has to be done before writing!
  //
  if(pMdc == NULL) return;
  Int_t nHits = getNMdcHits();
  if(nHits < 2) return;  // nothing to do

  HLinkedDataObject *pPrevPrev, *pPrev, *pHere;
  Float_t tofHere = 1000000.0;
  Float_t tofPrev = 1000000.0;
  for(Int_t i=0; i<nHits-1;i++) {  //  iterate through list nHits-1 times
     resetMdcIter();
     Int_t prev = -2;
     Int_t here = -1;
     Int_t next = (Int_t)firstMdcHit;
     pPrevPrev = pPrev = NULL;
     do {
        pHere = (HLinkedDataObject*)(pMdc->getObject(next));
        if(pHere == NULL) {
           printf("\nNULL in HGeantKine::sortMdcHits() at index %d ! \n",next);
           return;
        }
        prev = here;
        here = next;
        next = pHere->getNextHitIndex();
        tofHere = pHere->sortVariable;  // as sortVariable is non-persistent
	                                // the sort cannot be done a posteriori

        if(pPrev != NULL) {
           if(tofHere < tofPrev) { // swap hits if present hit is earlier
              if(pPrevPrev == NULL) firstMdcHit = (Short_t)here;
              else pPrevPrev->setNextHitIndex(here);
              pPrev->setNextHitIndex(next);
              pHere->setNextHitIndex(prev);
              pPrevPrev = pHere;
              here = prev;
              continue;
           }
        }

        pPrevPrev = pPrev;
        pPrev = pHere;
        tofPrev = tofHere;
     } while(next != -1);
  }
  return;
}

void HGeantKine::sortTofHits(void) {
  // Sort TOF hits in linked list for present track.
  //
  if(pTof == NULL) return;
  Int_t nHits = getNTofHits();
  if(nHits < 2) return;  // nothing to do

  HLinkedDataObject *pPrevPrev, *pPrev, *pHere;
  Float_t tofHere = 1000000.0;
  Float_t tofPrev = 1000000.0;
  for(Int_t i=0; i<nHits-1;i++) {  //  iterate through list nHits-1 times
     resetTofIter();
     Int_t prev = -2;
     Int_t here = -1;
     Int_t next = (Int_t)firstTofHit;
     pPrevPrev = pPrev = NULL;
     do {
        pHere = (HLinkedDataObject*)(pTof->getObject(next));
        if(pHere == NULL) {
           printf("\nNULL in HGeantKine::sortTofHits() at index %d ! \n",next);
           return;
        }
        prev = here;
        here = next;
        next = pHere->getNextHitIndex();
        tofHere = pHere->sortVariable;  // as sortVariable is non-persistent
	                                // the sort cannot be done a posteriori

        if(pPrev != NULL) {
           if(tofHere < tofPrev) { // swap hits if present hit is earlier
              if(pPrevPrev == NULL) firstTofHit = (Short_t)here;
              else pPrevPrev->setNextHitIndex(here);
              pPrev->setNextHitIndex(next);
              pHere->setNextHitIndex(prev);
              pPrevPrev = pHere;
              here = prev;
              continue;
           }
        }

        pPrevPrev = pPrev;
        pPrev = pHere;
        tofPrev = tofHere;
     } while(next != -1);
  }
  return;
}


void HGeantKine::sortRpcHits(void) {
  // Sort RPC hits in linked list for present track FIXME!! Is it ok??
  //
  if(pRpc == NULL) return;
  Int_t nHits = getNRpcHits();
  if(nHits < 2) return;  // nothing to do

  HLinkedDataObject *pPrevPrev, *pPrev, *pHere;
  Float_t tofHere = 1000000.0;
  Float_t tofPrev = 1000000.0;
  for(Int_t i=0; i<nHits-1;i++) {  //  iterate through list nHits-1 times
     resetRpcIter();
     Int_t prev = -2;
     Int_t here = -1;
     Int_t next = (Int_t)firstRpcHit;
     pPrevPrev = pPrev = NULL;
     do {
        pHere = (HLinkedDataObject*)(pRpc->getObject(next));
        if(pHere == NULL) {
           printf("\nNULL in HGeantKine::sortRpcHits() at index %d ! \n",next);
           return;
        }
        prev = here;
        here = next;
        next = pHere->getNextHitIndex();
        tofHere = pHere->sortVariable;  // as sortVariable is non-persistent
	                                // the sort cannot be done a posteriori

        if(pPrev != NULL) {
           if(tofHere < tofPrev) { // swap hits if present hit is earlier
              if(pPrevPrev == NULL) firstRpcHit = (Short_t)here;
              else pPrevPrev->setNextHitIndex(here);
              pPrev->setNextHitIndex(next);
              pHere->setNextHitIndex(prev);
              pPrevPrev = pHere;
              here = prev;
              continue;
           }
        }

        pPrevPrev = pPrev;
        pPrev = pHere;
        tofPrev = tofHere;
     } while(next != -1);
  }
  return;
}

void HGeantKine::sortShowerHits(void) {
  // Sort SHOWER hits in linked list for present track.
  //
  if(pShow == NULL) return;
  Int_t nHits = getNShowerHits();
  if(nHits < 2) return;  // nothing to do

  HLinkedDataObject* p = NULL;
  for(Int_t i=0; i<nHits-1;i++) {  //  iterate through list nHits times
     resetShowerIter();
     Int_t next = (Int_t)firstShowerHit;
     do {
        p = (HLinkedDataObject*)(pShow->getObject(next));
	//
	// do here the swapping of hits
	//
        next = p->getNextHitIndex();
     } while(next != -1);
  }
  return;
}

void HGeantKine::sortWallHits(void) {
  // Sort WALL hits in linked list for present track
  //
  if(pWall == NULL) return;
  Int_t nHits = getNWallHits();
  if(nHits < 2) return;  // nothing to do

  HLinkedDataObject *pPrevPrev, *pPrev, *pHere;
  Float_t tofHere = 1000000.0;
  Float_t tofPrev = 1000000.0;
  for(Int_t i=0; i<nHits-1;i++) {  //  iterate through list nHits-1 times
     resetWallIter();
     Int_t prev = -2;
     Int_t here = -1;
     Int_t next = (Int_t)firstWallHit;
     pPrevPrev = pPrev = NULL;
     do {
        pHere = (HLinkedDataObject*)(pWall->getObject(next));
        if(pHere == NULL) {
           printf("\nNULL in HGeantKine::sortWallHits() at index %d ! \n",next);
           return;
        }
        prev = here;
        here = next;
        next = pHere->getNextHitIndex();
        tofHere = pHere->sortVariable;  // as sortVariable is non-persistent
	                                // the sort cannot be done a posteriori

        if(pPrev != NULL) {
           if(tofHere < tofPrev) { // swap hits if present hit is earlier
              if(pPrevPrev == NULL) firstWallHit = (Short_t)here;
              else pPrevPrev->setNextHitIndex(here);
              pPrev->setNextHitIndex(next);
              pHere->setNextHitIndex(prev);
              pPrevPrev = pHere;
              here = prev;
              continue;
           }
        }

        pPrevPrev = pPrev;
        pPrev = pHere;
        tofPrev = tofHere;
     } while(next != -1);
  }
  return;
}

HLinkedDataObject* HGeantKine::nextHit(HCategory* pCat,Int_t& index) {
  // Return next hit in category made by present track.
   if(index == -1 || pCat == NULL) return NULL;
   HLinkedDataObject* p = (HLinkedDataObject*)(pCat->getObject(index));
   index = p->getNextHitIndex();
   return p;
}

Int_t HGeantKine::getNRichHits(void) {
  // Return number of RICH photon hits made by present track.
  //
   Int_t n = 0;
   if(firstRichHit > -1) {
      resetRichIter();
      while(nextRichHit() != NULL) n++;
      resetRichIter();
   }
   return n;
}

Int_t HGeantKine::getNMdcHits(void) {
  // Return number of MDC hits made by present track.
  //
   Int_t n = 0;
   if(firstMdcHit > -1) {
      resetMdcIter();
      while(nextMdcHit() != NULL) n++;
      resetMdcIter();
   }
   return n;
}

Int_t HGeantKine::getNMdcHits(Int_t module) {
// Return number of MDC hits made by present track
// for MDC module (range: [0,3]).

  Int_t n = 0;
  if (module >= 0 && module <4)
     {
     if(firstMdcHit > -1)
        {
	HGeantMdc* hit = NULL;
	resetMdcIter();
	while((hit = (HGeantMdc*) nextMdcHit()) != NULL)
	   {
	   if (hit->getModule() == module)
	      {
	      n++;
	      } 
	   }
	resetMdcIter();
	return n;
	}
     }
     else
     {
	return -1;
     }
  return n;
}


Int_t HGeantKine::getNTofHits(void) {
  // Return number of TOF hits made by present track.
  //
   Int_t n = 0;
   if(firstTofHit > -1) {
      resetTofIter();
      while(nextTofHit() != NULL) n++;
      resetTofIter();
   }
   return n;
}

Int_t HGeantKine::getNRpcHits(void) {
  // Return number of RPC hits made by present track.
  //
   Int_t n = 0;
   if(firstRpcHit > -1) {
      resetRpcIter();
      while(nextRpcHit() != NULL) n++;
      resetRpcIter();
   }
   return n;
}

Int_t HGeantKine::getNShowerHits(void) {
  // Return number of SHOWER hits made by present track.
  //
   Int_t n = 0;
   if(firstShowerHit > -1) {
      resetShowerIter();
      while(nextShowerHit() != NULL) n++;
      resetShowerIter();
   }
   return n;
}

Int_t HGeantKine::getNWallHits(void) {
  // Return number of WALL hits made by present track.
  //
   Int_t n = 0;
   if(firstWallHit > -1) {
      resetWallIter();
      while(nextWallHit() != NULL) n++;
      resetWallIter();
   }
   return n;
}

Int_t HGeantKine::getNEmcHits(void) {
  // Return number of EMC hits made by present track.
  //
   Int_t n = 0;
   if(firstEmcHit > -1) {
      resetEmcIter();
      while(nextEmcHit() != NULL) n++;
      resetEmcIter();
   }
   return n;
}

Int_t HGeantKine::getNStartHits(void) {
  // Return number of START hits made by present track.
  //
   Int_t n = 0;
   if(firstStartHit > -1) {
      resetStartIter();
      while(nextStartHit() != NULL) n++;
      resetStartIter();
   }
   return n;
}

Int_t HGeantKine::getNFwDetHits(void) {
  // Return number of Forward Detector hits made by present track.
  //
   Int_t n = 0;
   if(firstFwDetHit > -1) {
      resetFwDetIter();
      while(nextFwDetHit() != NULL) n++;
      resetFwDetIter();
   }
   return n;
}


Int_t HGeantKine::getRichHits(vector<HGeantRichPhoton*>& v) {
  // Return  RICH photon hits made by present track.
  //
    v.clear();

    HGeantRichPhoton* hit;
    if(firstRichHit > -1) {
	resetRichIter();
	while((hit = (HGeantRichPhoton*) nextRichHit()) != NULL){
	    v.push_back(hit);
	}
	resetRichIter();
    }
    return v.size();
}

Int_t HGeantKine::getMdcHits(vector<HGeantMdc*>& v) {
  // Return  MDC hits made by present track.
  //
    v.clear();

    HGeantMdc* hit;
    if(firstMdcHit > -1) {
	resetMdcIter();
	while((hit = (HGeantMdc*) nextMdcHit()) != NULL){
	    v.push_back(hit);
	}
	resetMdcIter();
    }
    return v.size();
}

Int_t HGeantKine::getTofHits(vector<HGeantTof*>& v) {
  // Return  Tof hits made by present track.
  //
    v.clear();

    HGeantTof* hit;
    if(firstTofHit > -1) {
	resetTofIter();
	while((hit = (HGeantTof*) nextTofHit()) != NULL){
	    v.push_back(hit);
	}
	resetTofIter();
    }
    return v.size();
}

Int_t HGeantKine::getRpcHits(vector<HGeantRpc*>& v) {
  // Return  Rpc hits made by present track.
  //
    v.clear();

    HGeantRpc* hit;
    if(firstRpcHit > -1) {
	resetRpcIter();
	while((hit = (HGeantRpc*) nextRpcHit()) != NULL){
	    v.push_back(hit);
	}
	resetRpcIter();
    }
    return v.size();
}

Int_t HGeantKine::getWallHits(vector<HGeantWall*>& v) {
  // Return  Wall hits made by present track.
  //
    v.clear();

    HGeantWall* hit;
    if(firstWallHit > -1) {
	resetWallIter();
	while((hit = (HGeantWall*) nextWallHit()) != NULL){
	    v.push_back(hit);
	}
	resetWallIter();
    }
    return v.size();
}

Int_t HGeantKine::getEmcHits(vector<HGeantEmc*>& v) {
  // Return  Emc hits made by present track.
  //
    v.clear();

    HGeantEmc* hit;
    if(firstEmcHit > -1) {
	resetEmcIter();
	while((hit = (HGeantEmc*) nextEmcHit()) != NULL){
	    v.push_back(hit);
	}
	resetEmcIter();
    }
    return v.size();
}

Int_t HGeantKine::getStartHits(vector<HGeantStart*>& v) {
  // Return  Start hits made by present track.
  //
    v.clear();

    HGeantStart* hit;
    if(firstStartHit > -1) {
	resetStartIter();
	while((hit = (HGeantStart*) nextStartHit()) != NULL){
	    v.push_back(hit);
	}
	resetStartIter();
    }
    return v.size();
}

Int_t HGeantKine::getFwDetHits(vector<HGeantFwDet*>& v) {
  // Return  Forward detector hits made by present track.

    v.clear();

    HGeantFwDet* hit;
    if(firstFwDetHit > -1) {
	resetFwDetIter();
	while((hit = (HGeantFwDet*) nextFwDetHit()) != NULL){
	    v.push_back(hit);
	}
	resetFwDetIter();
    }
    return v.size();
}

Bool_t HGeantKine::isInAcceptance(Int_t m0,Int_t m1,Int_t m2,Int_t m3,Int_t sys0,Int_t sys1)
{
    // return kTRUE if the particle has hit at least m0-m3 (default 4
    // out of 7 per module) layers in MDCI-MDCIV and Meta (RPC or Shower >= sys0, Tof>=sys1)

    Int_t nMdc0   = getNMdcHits(0);
    Int_t nMdc1   = getNMdcHits(1);
    Int_t nMdc2   = getNMdcHits(2);
    Int_t nMdc3   = getNMdcHits(3);
    Int_t nShower = getNShowerHits();
    Int_t nRpc    = getNRpcHits();
    Int_t nTof    = getNTofHits();


    if(nMdc0 >= m0 && nMdc1 >= m1  &&
       nMdc2 >= m2 && nMdc3 >= m3  &&
       (nRpc >= sys0 || nShower >= sys0 || nTof >= sys1) ) return kTRUE;
    else                                                   return kFALSE;
}

Bool_t HGeantKine::isInAcceptanceDecay(Int_t m0,Int_t m1,Int_t m2,Int_t m3,Int_t sys0,Int_t sys1)
{
    // return kTRUE if the particle (and his decayed charged daughter it it exist) has hit at
    // least m0-m3 (default 4 out of 7 per module) layers in MDCI-MDCIV and Meta (RPC or Shower >= sys0, Tof>=sys1)

    Int_t cm0, cm1,cm2,cm3,csys0,csys1;

    getNHits(cm0,cm1,cm2,cm3,csys0,csys1);

    if(cm0 >= m0 && cm1 >= m1  &&
       cm2 >= m2 && cm3 >= m3  &&
       (csys0 >= sys0 || csys1 >= sys1) ) return kTRUE;

    HGeantKine* d = HGeantKine::getChargedDecayDaughter(this);

    if(d){
	Int_t c1m0, c1m1,c1m2,c1m3,c1sys0,c1sys1;

	d->getNHits(c1m0,c1m1,c1m2,c1m3,c1sys0,c1sys1);

	cm0   += c1m0;
	cm1   += c1m1;
	cm2   += c1m2;
	cm3   += c1m3;
        csys0 += c1sys0;
        csys1 += c1sys1;


    }

    if(cm0 >= m0 && cm1 >= m1  &&
       cm2 >= m2 && cm3 >= m3  &&
       (csys0 >= sys0 || csys1 >= sys1) ) return kTRUE;

    return kFALSE;
}

Bool_t HGeantKine::isInAcceptanceBit(Int_t m0,Int_t m1,Int_t m2,Int_t m3,Int_t sys0,Int_t sys1)
{
    // return kTRUE if the particle has hit at least m0-m3 (default 4
    // out of 7 per module) layers in MDCI-MDCIV and Meta sys0/1
    // this function works on prefilled acceptance bits and needs no
    // additional categories. If sys 0/1 should be checked exclusive
    // the other system threshold has to larger 1

    Int_t nMdc0   = getNLayerMod(0);
    Int_t nMdc1   = getNLayerMod(1);
    Int_t nMdc2   = getNLayerMod(2);
    Int_t nMdc3   = getNLayerMod(3);
    Int_t nSys0   = getSys(0);
    Int_t nSys1   = getSys(1);


    if(nMdc0 >= m0 && nMdc1 >= m1  &&
       nMdc2 >= m2 && nMdc3 >= m3  &&
       (nSys0 >= sys0 || nSys1 >= sys1) ) return kTRUE;
    else                                  return kFALSE;
}

Bool_t HGeantKine::isInAcceptanceDecayBit(Int_t m0,Int_t m1,Int_t m2,Int_t m3,Int_t sys0,Int_t sys1)
{
    // return kTRUE if the particle (and his decayed charged daughter it it exist) has hit at
    // least m0-m3 (default 4 out of 7 per module) layers in MDCI-MDCIV and Meta (RPC or
    // Shower >= sys0, Tof>=sys1)
    // this function works on prefilled acceptance bits and needs no
    // additional categories. If sys 0/1 should be checked exclusive
    // the other system threshold has to larger 1

    Int_t cm0, cm1,cm2,cm3,csys0,csys1;

    getNHitsBit(cm0,cm1,cm2,cm3,csys0,csys1);

    if(cm0 >= m0 && cm1 >= m1  &&
       cm2 >= m2 && cm3 >= m3  &&
       (csys0 >= sys0 || csys1 >= sys1) ) return kTRUE;


    HGeantKine* d = HGeantKine::getChargedDecayDaughter(this);

    if(d){
	Int_t c1m0, c1m1,c1m2,c1m3,c1sys0,c1sys1;

	d->getNHitsBit(c1m0,c1m1,c1m2,c1m3,c1sys0,c1sys1);

	cm0   += c1m0;
	cm1   += c1m1;
	cm2   += c1m2;
	cm3   += c1m3;
        csys0 += c1sys0;
        csys1 += c1sys1;

        if(csys0>1) csys0=1;
        if(csys1>1) csys1=1;

    }

    if(cm0 >= m0 && cm1 >= m1  &&
       cm2 >= m2 && cm3 >= m3  &&
       (csys0 >= sys0 || csys1 >= sys1) ) return kTRUE;

    return kFALSE;
}

void HGeantKine::fillAcceptanceBit() {
    //
    if(firstMdcHit > -1)
    {
	unsetAllLayers();
	HGeantMdc* hit = NULL;
	resetMdcIter();
	while((hit = (HGeantMdc*) nextMdcHit()) != NULL)
	{
	    Int_t lay = hit->getLayer();
            if(lay>5) continue;
	    Int_t mod = hit->getModule();
	    Int_t io  = mod < 2 ? 0:1;
	    Int_t l   = mod < 2 ? mod*6+lay : (mod-2)*6+lay;
	    setLayer(io,l);
	}
	resetMdcIter();
    }
    if(firstTofHit > -1)                      setSys(1);
    if(firstRpcHit!=-1 || firstShowerHit!=-1) setSys(0);
    setAcceptanceFilled();
}

void  HGeantKine::getNHits(Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1)
{
    // returns the number of hits in MDC modules 0-3,
    // sys0 (shower+rpc) and sys1 (tof) 
    m0    = getNMdcHits(0);
    m1    = getNMdcHits(1);
    m2    = getNMdcHits(2);
    m3    = getNMdcHits(3);
    sys0  = getNShowerHits();
    sys0 += getNRpcHits();
    sys1  = getNTofHits();
}

void  HGeantKine::getNHitsDecay(Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1)
{
    // returns the number of hits in MDC modules 0-3,
    // sys0 (shower+rpc) and sys1 (tof) of the track
    // (and his decayed charged daughter it it exist)
    m0    = getNMdcHits(0);
    m1    = getNMdcHits(1);
    m2    = getNMdcHits(2);
    m3    = getNMdcHits(3);
    sys0  = getNShowerHits();
    sys0 += getNRpcHits();
    sys1  = getNTofHits();


    HGeantKine* d = HGeantKine::getChargedDecayDaughter(this);

    if(d){

	m0    += d->getNMdcHits(0);
	m1    += d->getNMdcHits(1);
	m2    += d->getNMdcHits(2);
	m3    += d->getNMdcHits(3);
	sys0  += d->getNShowerHits();
	sys0  += d->getNRpcHits();
	sys1  += d->getNTofHits();
    }

}

void  HGeantKine::getNHitsBit(Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1)
{
    // returns the number of hits in MDC modules 0-3,
    // sys0 (shower+rpc) and sys1 (tof) of the track
    // number of hits for sys 0/1 can be only 0 or 1
    // this function works on prefilled acceptance bits and needs no
    // additional categories.
    m0    = getNLayerMod(0);
    m1    = getNLayerMod(1);
    m2    = getNLayerMod(2);
    m3    = getNLayerMod(3);
    sys0  = getSys(0);
    sys1  = getSys(1);


}

void  HGeantKine::getNHitsDecayBit(Int_t& m0,Int_t& m1,Int_t& m2,Int_t& m3,Int_t& sys0,Int_t& sys1)
{
    // returns the number of hits in MDC modules 0-3,
    // sys0 (shower+rpc) and sys1 (tof) of the track
    // (and his decayed charged daughter it it exist).
    // number of hits for sys 0/1 can be only 0 or 1
    // this function works on prefilled acceptance bits and needs no
    // additional categories.
    m0    = getNLayerMod(0);
    m1    = getNLayerMod(1);
    m2    = getNLayerMod(2);
    m3    = getNLayerMod(3);
    sys0  = getSys(0);
    sys1  = getSys(1);


    HGeantKine* d = HGeantKine::getChargedDecayDaughter(this);

    if(d){

	m0    += getNLayerMod(0);
	m1    += getNLayerMod(1);
	m2    += getNLayerMod(2);
	m3    += getNLayerMod(3);
	sys0  += getSys(0);
	sys1  += getSys(1);

	if(sys0>1)sys0=1;
	if(sys1>1)sys1=1;

    }
}

Int_t HGeantKine::getSystem(void)
{
  // Return system crossed by the present track
  //  -1: no contact:
  //   0: RPC
  //   1: TOF
  //   2: RPC and TOF

   Int_t n = -1;
   Int_t n0 = 0;
   Int_t n1 = 0;
   if(firstTofHit > -1) n1=1;
   if(firstRpcHit!=-1 || firstShowerHit!=-1) n0=1;


   if ( n0 > 0 && n1 <= 0) n = 0; // hits in RPC, but no hits in TOF --> RPC
   if ( n1 > 0 && n0 <= 0) n = 1; // hits in TOF, but no hits in RPC --> TOF
   if ( n0 > 0 && n1 >  0) n = 2; // hits in TOF and hits in RPC --> BOTH
   return n;
}

UInt_t HGeantKine::getMdcSectorBitArray(void) 
{
  // Returns bit array containing sectors crossed by current track
  //   bit 0: sector 0
  //   bit 1: sector 1
  //   bit 2: sector 2
  //   bit 3: sector 3
  //   bit 4: sector 4
  //   bit 5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5
  //  in hex:  0x2a
  //  decimal: 34

  UInt_t n=0;
  if(firstMdcHit > -1)
    {
      HGeantMdc* hit = NULL;
      resetMdcIter();
      while((hit = (HGeantMdc*) nextMdcHit()) != NULL)
	{
	  n|=(0x1<<hit->getSector());
	}
      resetMdcIter();
      return n&0x3f;
      
    }
  return n&0x3f;
}

UInt_t HGeantKine::getMdcSectorDecimalArray(void) 
{
  // Returns decimal array containing sectors crossed by current track
  //   digit 10^0: sector 0
  //   digit 10^1: sector 1
  //   digit 10^2: sector 2
  //   digit 10^3: sector 3
  //   digit 10^4: sector 4
  //   digit 10^5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5

  UInt_t bit = getMdcSectorBitArray() && 0x3f;
  UInt_t n=0;
  
  for (Int_t p=0; p<6; p++)
    {
      if ((bit>>p)&0x1)
      {
	n+=(UInt_t) pow(10.,p);
      }
    }
  return n;
}

UInt_t HGeantKine::getShowerSectorBitArray(void) 
{
  // Returns bit array containing sectors crossed by current track
  //   bit 0: sector 0
  //   bit 1: sector 1
  //   bit 2: sector 2
  //   bit 3: sector 3
  //   bit 4: sector 4
  //   bit 5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5
  //  in hex: 0x2a
  // decimal: 34

  UInt_t n=0;
  if(firstShowerHit > -1)
    {
      HGeantShower* hit = NULL;
      resetShowerIter();
      while((hit = (HGeantShower*) nextShowerHit()) != NULL)
	{
	  n|=(0x1<<hit->getSector());
	}
      resetShowerIter();
      return n&0x3f;
      
    }
  return n&0x3f;
}

UInt_t HGeantKine::getShowerSectorDecimalArray(void) 
{
  // Returns decimal array containing sectors crossed by current track
  //   digit 10^0: sector 0
  //   digit 10^1: sector 1
  //   digit 10^2: sector 2
  //   digit 10^3: sector 3
  //   digit 10^4: sector 4
  //   digit 10^5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5

  UInt_t bit = getShowerSectorBitArray() && 0x3f;
  UInt_t n=0;
  
  for (Int_t p=0; p<6; p++)
    {
      if ((bit>>p)&0x1)
      {
	n+=(UInt_t) pow(10.,p);
      }
    }
  return n;
}

UInt_t HGeantKine::getTofSectorBitArray(void) 
{
  // Returns bit array containing sectors crossed by current track
  //   bit 0: sector 0
  //   bit 1: sector 1
  //   bit 2: sector 2
  //   bit 3: sector 3
  //   bit 4: sector 4
  //   bit 5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5
  //  in hex: 0x2a
  // decimal: 34

  UInt_t n=0;
  if(firstTofHit > -1)
    {
      HGeantTof* hit = NULL;
      resetTofIter();
      while((hit = (HGeantTof*) nextTofHit()) != NULL)
	{
	  n|=(0x1<<hit->getSector());
	}
      resetTofIter();
      return n&0x3f;
      
    }
  return n&0x3f;
}

UInt_t HGeantKine::getTofSectorDecimalArray(void) 
{
  // Returns decimal array containing sectors crossed by current track
  //   digit 10^0: sector 0
  //   digit 10^1: sector 1
  //   digit 10^2: sector 2
  //   digit 10^3: sector 3
  //   digit 10^4: sector 4
  //   digit 10^5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5

  UInt_t bit = getTofSectorBitArray() && 0x3f;
  UInt_t n=0;
  
  for (Int_t p=0; p<6; p++)
    {
      if ((bit>>p)&0x1)
      {
	n+=(UInt_t) pow(10.,p);
      }
    }
  return n;
}

UInt_t HGeantKine::getRpcSectorBitArray(void) 
{
  // Returns bit array containing sectors crossed by current track
  //   bit 0: sector 0
  //   bit 1: sector 1
  //   bit 2: sector 2
  //   bit 3: sector 3
  //   bit 4: sector 4
  //   bit 5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5
  //  in hex: 0x2a
  // decimal: 34

  UInt_t n=0;
  if(firstRpcHit > -1)
    {
      HGeantRpc* hit = NULL;
      resetRpcIter();
      while((hit = (HGeantRpc*) nextRpcHit()) != NULL)
	{
	  n|=(0x1<<hit->getSector());
	}
      resetRpcIter();
      return n&0x3f;
      
    }
  return n&0x3f;
}

UInt_t HGeantKine::getRpcSectorDecimalArray(void) 
{
  // Returns decimal array containing sectors crossed by current track
  //   digit 10^0: sector 0
  //   digit 10^1: sector 1
  //   digit 10^2: sector 2
  //   digit 10^3: sector 3
  //   digit 10^4: sector 4
  //   digit 10^5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5

  UInt_t bit = getRpcSectorBitArray() && 0x3f;
  UInt_t n=0;
  
  for (Int_t p=0; p<6; p++)
    {
      if ((bit>>p)&0x1)
      {
	n+=(UInt_t) pow(10.,p);
      }
    }
  return n;
}

UInt_t HGeantKine::getRichSectorBitArray(void) 
{
  // Returns bit array containing sectors crossed by current track
  //   bit 0: sector 0
  //   bit 1: sector 1
  //   bit 2: sector 2
  //   bit 3: sector 3
  //   bit 4: sector 4
  //   bit 5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5
  //  in hex: 0x2a
  // decimal: 34

  UInt_t n=0;
  if(firstRichHit > -1)
    {
      HGeantRichPhoton* hit = NULL;
      resetRichIter();
      while((hit = (HGeantRichPhoton*) nextRichHit()) != NULL)
	{
	  n|=(0x1<<hit->getSector());
	}
      resetRichIter();
      return n&0x3f;
    }
  return n&0x3f;
}

UInt_t HGeantKine::getRichSectorDecimalArray(void) 
{
  // Returns decimal array containing sectors crossed by current track
  //   digit 10^0: sector 0
  //   digit 10^1: sector 1
  //   digit 10^2: sector 2
  //   digit 10^3: sector 3
  //   digit 10^4: sector 4
  //   digit 10^5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5

  UInt_t bit = getRichSectorBitArray() && 0x3f;
  UInt_t n=0;
  
  for (Int_t p=0; p<6; p++)
    {
      if ((bit>>p)&0x1)
      {
	n+=(UInt_t) pow(10.,p);
      }
    }
  return n;
}

UInt_t HGeantKine::getSectorBitArray(void)
{
  // Returns bit array containing sectors crossed by current track
  // as an or of all detectors (RICH, TOF, SHOWER, MDC)
  //   bit 0: sector 0
  //   bit 1: sector 1
  //   bit 2: sector 2
  //   bit 3: sector 3
  //   bit 4: sector 4
  //   bit 5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5
  //  in hex:  0x2a
  //  decimal: 34

  UInt_t n=0;
  n|=getRichSectorBitArray();
  n|=getTofSectorBitArray();
  n|=getRpcSectorBitArray();
  n|=getMdcSectorBitArray();
  n|=getShowerSectorBitArray();
  return n&0x3f;
}

UInt_t HGeantKine::getSectorDecimalArray(void)
{
  // Returns decimal array containing sectors crossed by current track
  // as an or of all detectors (RICH, TOF, SHOWER, MDC)
  //   digit 10^0: sector 0
  //   digit 10^1: sector 1
  //   digit 10^2: sector 2
  //   digit 10^3: sector 3
  //   digit 10^4: sector 4
  //   digit 10^5: sector 5
  //
  // i.e. 101010 -> Track crossed sector 1,3 and 5.

  UInt_t bit = getSectorBitArray() && 0x3f;
  UInt_t n=0;
  
  for (Int_t p=0; p<6; p++)
    {
      if ((bit>>p)&0x1)
      {
	n+=(UInt_t) pow(10.,p);
      }
    }
  return n;
}

Int_t HGeantKine::getSectorFromBitArray(void)
{
  // Returns sector if only one sector is crossed by current track
  // otherwise it returns a negative value
  // (-7 is returned if there is no hit)
  // where its absolute value gives the number of sectors crossed.
  // See getSectorBitArray(), getMdcBitArray(), getTofBitArray(), getRpcBitArray(),
  // getRichBitArray(), getShowerBitArray() for more details.

  UInt_t n=getSectorBitArray();
  if (n==0) return -7;
  Int_t sector=-1;
  Int_t ctr=0;
  for (Int_t p=0; p<6; p++)
    {
      if ((n>>p)&0x1) 
	{
	  if (ctr==0) 
	    {
	      sector = p;
	      ctr++;
	    }
	  else ctr++;
	}
    }
    return (ctr>1)?-ctr:sector;
}

void HGeantKine::Streamer(TBuffer &R__b)
{
   // Stream an object of class HGeantKine.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> trackNumber;
      R__b >> parentTrack;
      R__b >> particleID;
      R__b >> mediumNumber;
      R__b >> creationMechanism;
      R__b >> xVertex;
      R__b >> yVertex;
      R__b >> zVertex;
      R__b >> xMom;
      R__b >> yMom;
      R__b >> zMom;
      R__b >> generatorInfo;
      if (R__v > 4) {
         R__b >> generatorInfo1;
         R__b >> generatorInfo2;
      } else {
         generatorInfo1 = 0.;
         generatorInfo2 = 0.;
      }
      R__b >> generatorWeight;

      firstRichHit   = -1;
      firstMdcHit    = -1;
      firstTofHit    = -1;
      firstRpcHit    = -1;
      firstShowerHit = -1;
      firstWallHit   = -1;
      firstEmcHit    = -1;
      firstStartHit  = -1;
      firstFwDetHit    = -1;
 
      if (R__v > 1) {
         R__b >> firstRichHit;
         R__b >> firstMdcHit;
         R__b >> firstTofHit;
	 if (R__v > 5) { // rpc added in vers 6
	     R__b >> firstRpcHit;
	 }
         R__b >> firstShowerHit;
	 if (R__v > 7) { // wall added in vers 8
	     R__b >> firstWallHit;
	 }
	 if (R__v > 8) { // emc added in vers 9
	     R__b >> firstEmcHit;
	 }
	 if (R__v > 9) { // START added in vers 10
	     R__b >> firstStartHit;
	 }
	 if (R__v > 11) { // Forward Detector added in vers 12
	     R__b >> firstFwDetHit;
	 }
      }
      active = kFALSE;

      if (R__v > 2) {
	 R__b >> active;

	 if(R__v > 5)
	 {   // rpc added in vers 6
	     if (firstRichHit  > -1 ||
		 firstMdcHit   > -1 ||
		 firstTofHit   > -1 ||
		 firstRpcHit   > -1 ||
		 firstShowerHit>-1 ) active = kTRUE;  // track made a hit
	     if(R__v > 7)
	     {   // wall added in vers 8
		 if (firstRichHit  > -1 ||
		     firstMdcHit   > -1 ||
		     firstTofHit   > -1 ||
		     firstRpcHit   > -1 ||
		     firstShowerHit> -1 ||
		     firstWallHit  > -1  ) active = kTRUE;  // track made a hit
		 if(R__v > 8)
	         {   // emc added in vers 9
		     if (firstRichHit  > -1 ||
		         firstMdcHit   > -1 ||
		         firstTofHit   > -1 ||
		         firstRpcHit   > -1 ||
		         firstShowerHit> -1 ||
		         firstWallHit  > -1 ||
		         firstEmcHit   > -1  ) active = kTRUE;  // track made a hit
		     if(R__v > 9)
	             {   // emc added in vers 10
		         if (firstRichHit  > -1 ||
    		             firstMdcHit   > -1 ||
		             firstTofHit   > -1 ||
		             firstRpcHit   > -1 ||
		             firstShowerHit> -1 ||
		             firstWallHit  > -1 ||
		             firstEmcHit   > -1 ||
                             firstStartHit   > -1 ) active = kTRUE;  // track made a hit
		         if(R__v > 11)
	                 {   // Forward Detector added in vers 12
		             if (firstRichHit  > -1 ||
    		                 firstMdcHit   > -1 ||
		                 firstTofHit   > -1 ||
		                 firstRpcHit   > -1 ||
		                 firstShowerHit> -1 ||
		                 firstWallHit  > -1 ||
		                 firstEmcHit   > -1 ||
		                 firstStartHit > -1 ||
                                 firstFwDetHit > -1 ) active = kTRUE;  // track made a hit
                         }
    	             }    
    	         }    
	     }
	 } else {
	     if (firstRichHit  > -1 ||
		 firstMdcHit   > -1 ||
		 firstTofHit   > -1 ||
		 firstShowerHit> -1 ) active = kTRUE;  // track made a hit
	 }

      } else { // very old ....
	  if (parentTrack==0) active = kTRUE;       // primary track
	  else                active = kFALSE;
      }

      if (R__v > 3) {
	  R__b >> suppressed;
      } else {
	  suppressed = kFALSE;
      }
      if (R__v > 6) {
	  R__b >> userVal;
      } else {
	  userVal = -1;
      }
      if (R__v > 10) {
	  R__b >> acceptance;
      } else {
	  acceptance = 0;
      }

   } else {
      R__b.WriteVersion(HGeantKine::IsA());
      TObject::Streamer(R__b);
      R__b << trackNumber;
      R__b << parentTrack;
      R__b << particleID;
      R__b << mediumNumber;
      R__b << creationMechanism;
      R__b << xVertex;
      R__b << yVertex;
      R__b << zVertex;
      R__b << xMom;
      R__b << yMom;
      R__b << zMom;
      R__b << generatorInfo;
      R__b << generatorInfo1;
      R__b << generatorInfo2;
      R__b << generatorWeight;
      R__b << firstRichHit;
      R__b << firstMdcHit;
      R__b << firstTofHit;
      R__b << firstRpcHit;
      R__b << firstShowerHit;
      R__b << firstWallHit;
      R__b << firstEmcHit;
      R__b << firstStartHit;
      R__b << firstFwDetHit;
      R__b << active;
      R__b << suppressed;
      R__b << userVal;
      R__b << acceptance;
   }
} 


Bool_t HGeantKine::setChainActive(Int_t track, Bool_t flag, HLinearCategory* cat)
{
  // Set track and all its ancestors up to primary track active. 
  //
  HLinearCategory* catKine;
  if (track < 1) return kFALSE;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return kFALSE;
  HGeantKine* pKine = (HGeantKine*)catKine->getObject(track-1);
  if (pKine == NULL) return kFALSE;

  do {   // iterate over all ancestors until primary is found
    pKine->setActive(flag);
    track = pKine->getTrack();
  } while ((pKine=getParent(track,catKine)) != NULL);
  
  return kTRUE;
}

Bool_t HGeantKine::setAllDescendentsActive(Int_t trackIn, Bool_t flag, HLinearCategory* cat)
{
  // Set track and all of its descendents active.
  //
  HLinearCategory* catKine;
  if (trackIn < 1) return kFALSE;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return kFALSE;
  HGeantKine* pKine = (HGeantKine*)catKine->getObject(trackIn-1);

  Int_t nEntries=catKine->getEntries();
  for (Int_t i=0; i<nEntries; i++) {
    pKine = (HGeantKine*)catKine->getObject(i);
    if (pKine->hasAncestor(trackIn,catKine)) pKine->setActive(flag);
  }
  
  return kTRUE;
}

Bool_t HGeantKine::setAllDescendentsSuppressed(Int_t trackIn, Bool_t flag, HLinearCategory* cat)
{
  // Set track and all of its descendents suppressed. 
  //
  HLinearCategory* catKine;
  if (trackIn < 1) return kFALSE;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return kFALSE;
  HGeantKine* pKine = (HGeantKine*)catKine->getObject(trackIn-1);
  if (pKine == NULL) return kFALSE;

  Int_t nEntries=catKine->getEntries();
  for (Int_t i=0; i<nEntries; i++) {
    pKine = (HGeantKine*)catKine->getObject(i);
    if (pKine->hasAncestor(trackIn,catKine)) {
      pKine->setSuppressed(flag);
      if (flag==kTRUE) pKine->setActive(kFALSE);
    }
  }
  
  return kTRUE;
}

Bool_t HGeantKine::suppressTracks(Int_t id, Float_t acceptedFraction, HLinearCategory* cat) {
  // Suppress randomly a certain fraction of tracks of particle kind id, as well as all of its
  // descendants.   This can be used to correct (lower!) the yield of a certain particle species
  // AFTER the event generator has been run.
  //
  if (id<0 || acceptedFraction < 0. || acceptedFraction > 1.) return kFALSE;
  HLinearCategory* catKine;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  Int_t track=1;
  HGeantKine* pKine;
  pKine = (HGeantKine*)catKine->getObject(track-1);
  if (pKine == NULL) return kFALSE;
  while (pKine && pKine->isPrimary()) { // iterate over primaries and throw dice to
    if ((pKine->getID()) == id) {   // decide if they are to be suppressed or not
      if (gRandom->Rndm() > acceptedFraction)
        setAllDescendentsSuppressed(track, kTRUE, catKine);  // propagate the verdict to all descendents
    }
    track++;
    pKine = (HGeantKine*)catKine->getObject(track-1);
  }
  return kTRUE;
}

HGeantKine* HGeantKine::getParent(Int_t track, HLinearCategory* cat)
{ 
  // Return pointer to parent object, return NULL if none.
  //
  if (track < 1) return NULL;
  HLinearCategory* catKine;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return NULL;
  HGeantKine* pKine = (HGeantKine*)catKine->getObject(track-1);
  if (pKine == NULL) return NULL;
  Int_t parent = pKine->getParentTrack();
  if (parent == 0) return NULL;
  else return (HGeantKine*)catKine->getObject(parent-1);
}


HGeantKine* HGeantKine::getGrandParent(Int_t track, HLinearCategory* cat)
{ 
  // Return pointer to grandparent object, return NULL if none.
  //
  if (track < 1) return NULL;
  HLinearCategory* catKine;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return NULL;
  HGeantKine* pKine = getParent(track,catKine);
  if (pKine == NULL) return NULL;
  Int_t grandparent = pKine->getParentTrack();
  if(grandparent == 0) return NULL;
  else return (HGeantKine*)catKine->getObject(grandparent-1);
}

HGeantKine* HGeantKine::getPrimary(Int_t trackIn, HLinearCategory* cat)
{
  // Return pointer to primary ancestor of track.
  //
  if (trackIn < 1) return NULL;
  HLinearCategory* catKine;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return NULL;
  HGeantKine* pKine = (HGeantKine*)catKine->getObject(trackIn-1);
  if (pKine == NULL) return NULL;
  HGeantKine* pSave;
  Int_t track=0;

  do {   // iterate over all ancestors until primary is found
    pSave = pKine;
    track = pKine->getTrack();
  } while ((pKine=pKine->getParent(track,catKine)) != NULL);
  
  return pSave;
}

Bool_t HGeantKine::hasAncestor(Int_t trackIn, HLinearCategory* cat)
{
  // Check if current track has trackIn as ancestor (or is trackIn).
  //
  if (trackIn < 1) return kFALSE;
  HLinearCategory* catKine;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return kFALSE;
  HGeantKine* pKine = this;
  Int_t track = -1;

  do {   // iterate over all ancestors until track is found
    track = pKine->getTrack();
    if (track == trackIn) return kTRUE;
  } while ((pKine=getParent(track,catKine)) != NULL);
  
  return kFALSE;
}

HGeantKine* HGeantKine::getCommonAncestor(Int_t track1, Int_t track2, HLinearCategory* cat)
{
  // Return pointer to nearest ancestor of 2 tracks.
  // This ancestor is not necessarily a primary. 
  //
  if (track1 < 1 || track2 < 1) return NULL;
  HLinearCategory* catKine;
  if (cat != NULL) catKine = cat;
  else catKine = (HLinearCategory*)gHades->getCurrentEvent()
                                         ->getCategory(catGeantKine);
  if (catKine == NULL) return NULL;
  HGeantKine* pKine1 = (HGeantKine*)catKine->getObject(track1-1);
  if (pKine1 == NULL) return NULL;
  HGeantKine* pKine2 = (HGeantKine*)catKine->getObject(track2-1);
  if (pKine2 == NULL) return NULL;

  Int_t track=0;
  if (track1 < track2) {  // do the search along the shortest branch

    do {   // iterate until common ancestor or primary is found
      track = pKine1->getTrack();
      if (pKine2->hasAncestor(track,catKine)) return pKine1;
    } while ((pKine1=pKine1->getParent(track,catKine)) != NULL);
    return NULL;

  } else {

    do {   // iterate until common ancestor or primary is found
      track = pKine2->getTrack();
      if (pKine1->hasAncestor(track,catKine)) return pKine2;
    } while ((pKine2=pKine2->getParent(track,catKine)) != NULL);
    return NULL;
  }
}
void HGeantKine::printHistory(){

    vector <TString> list;

    Float_t x,y,z;
    Int_t parenttrack,medium,creation;
    TString tmp;
    getCreator(parenttrack,medium,creation);
    getVertex(x,y,z);

    tmp = HPhysicsConstants::pid(getID());
    tmp = Form("=>(track %5i, parent %5i,medium %3i,creation %2i at vertex %7.1f,%7.1f,%7.1f)=>",getTrack(),parenttrack,medium,creation,x,y,z) + tmp;

    list.push_back(tmp);

    if(parenttrack == 0) {
	cout<<"---------------------------------------" <<endl;
	cout<<"ID ="<<HPhysicsConstants::pid(getID())<<" track = "<<setw(5)<<getTrack()<<" , parent track = "<<getParentTrack()<<endl;
	cout<<tmp.Data()<<endl;
	cout<<"---------------------------------------" <<endl;
	return;
    }

    HGeantKine* parent = this;
    
    while ( (parent=parent->getParent(parent->getTrack()))!=0){
	parent->getCreator(parenttrack,medium,creation);
	parent->getVertex(x,y,z);
	tmp = HPhysicsConstants::pid(parent->getID());
	tmp = Form("=>(track %5i, parent %5i,medium %3i,creation %2i at vertex %7.1f,%7.1f,%7.1f)=>",parent->getTrack(),parenttrack,medium,creation,x,y,z) + tmp;
	list.push_back(tmp);
    }
    cout<<"---------------------------------------" <<endl;
    cout<<"ID ="<<HPhysicsConstants::pid(getID())<<" track = "<<setw(5)<<getTrack()<<" , parent track = "<<getParentTrack()<<endl;
    Int_t n=list.size();
    for(Int_t i=n-1;i>=0;i--){
         cout<<list.at(i).Data()<<endl;
    }
    cout<<"---------------------------------------" <<endl;

}
void HGeantKine::print()
{
    // print particle infos

    cout<<"HGeantKine::print()----------------------------------"<<endl;

    cout<<" ID       : "<<setw(5)<<particleID   <<", track     : "<<setw(5)<<trackNumber      <<", parentTrack : "<<setw(5)<<parentTrack <<endl;
    cout<<" medium   : "<<setw(5)<<mediumNumber <<", mechanism : "<<setw(5)<<creationMechanism<<endl;
    cout<<" vertex   : "<<setw(12)<<xVertex      <<",    "<<setw(12)<<yVertex       <<",     "<<setw(12)<<zVertex<<endl;
    cout<<" mom      : "<<setw(12)<<xMom         <<",    "<<setw(12)<<yMom          <<",     "<<setw(12)<<zMom          <<" total  : "<<setw(12)<<getTotalMomentum()<<endl;
    cout<<" geninfo  : "<<setw(12)<<generatorInfo<<", 1: "<<setw(12)<<generatorInfo1<<", 2 : "<<setw(12)<<generatorInfo2<<" weight : "<<setw(12)<<generatorWeight<<endl;
    cout<<" firsthit : Rich : "<<setw(5)<<firstRichHit<<", Mdc : "<<setw(5)<<firstMdcHit<<", Tof : "<<setw(5)<<firstTofHit<<" Rpc : "<<setw(5)<<firstRpcHit<<" Shower : "<<setw(5)<<firstShowerHit<<" Wall : "<<setw(5)<<firstWallHit<<endl;
}


//--------------------------------------------------------
// PLUTO helper functions

Bool_t HGeantKine::isBugInfo()
{
    // caused by GEANT bug: file input number has been written
    // to geninfo (geninfo1 and geninfo are 0) if the input did
    // not provide generator infos (fixed for HGeant2 > 3.2)

    if(generatorInfo>0&&generatorInfo<4&&generatorInfo1==0&&generatorInfo2==-1) return kTRUE;
    else return kFALSE;
}

Bool_t HGeantKine::isExternalSource()
{
    // For PLUTO generatorInfo will be
    // set to the parentPID.
    return (generatorInfo1 > 0);
}

Bool_t HGeantKine::isThermalSource()
{
    // For PLUTO  generatorInfo keeps
    // the source ID. For themal sources
    // the PID is genarated  parentID + 500
    // if generatorInfo is >= 500 and < 600
    // we assume this as thermal source.
    // For other decays as for example
    // pp->p p pi0 the source ID will be coded from
    // the outging particles ==> 141407

    return (generatorInfo >=500 && generatorInfo <600);
}

Bool_t HGeantKine::isSameExternalSource(HGeantKine* kine1,HGeantKine* kine2)
{
    // For PLUTO both particles kine1,kine2 as to come from an external source
    // and the parentPID (generatorInfo1) are equal. both particles
    // have to stem from the same parentIndex (generatorInfo2)(if
    // there are more than 1 source of the same type in the event)
    // CAUTION: For particles from a fireball without decay in PLUTO
    // parentIndex will be -1. In case of multiple particles from
    // such an source they canot be distinguished. The function will
    // retun kTRUE in this case.
    //

    if(!kine1 || !kine2) return kFALSE;
    return (kine1->isExternalSource() && kine2->isExternalSource() &&
	    kine1->getGeneratorInfo1() == kine2->getGeneratorInfo1() &&
	    kine1->getGeneratorInfo2() == kine2->getGeneratorInfo2());
}

Bool_t HGeantKine::isSameExternalSource(HGeantKine* kine1)
{
    if(!kine1) return kFALSE;
    return (kine1->isExternalSource() && isExternalSource()  &&
	    kine1->getGeneratorInfo1() == getGeneratorInfo1() &&
	    kine1->getGeneratorInfo2() == getGeneratorInfo2());
}

Int_t HGeantKine::getNLayer(UInt_t io)
{
    // i0 :  0 = inner seg , 1 = outer seg, 2 = both segs
    Int_t sum=0;
    if(io<2){
	for(UInt_t i=0;i<12;i++){
	    sum+= getLayer(io,i);
	}
    } else {
	for(UInt_t i=0;i<12;i++){
	    sum+= getLayer(0,i);
	    sum+= getLayer(1,i);
	}
    }

    return sum;
}

Int_t HGeantKine::getNLayerMod(UInt_t mod)
{
    Int_t sum=0;
    UInt_t io =0;
    if(mod>1) io =1;
    UInt_t first = 0;
    UInt_t last  = 12;
    if(mod==1||mod==3)first=6;
    if(mod==0||mod==2)last =6;

    for(UInt_t i=first;i<last;i++){
        sum+= getLayer(io,i);
    }
   return sum;
}

void HGeantKine::printLayers()
{
    TString out="";
    for(Int_t i=32;i>0;i--){
	if(i<25&&i%6==0) out+=" ";
	out+= ( (acceptance>>(i-1)) & 0x1 );
    }
    cout<<"    layers "<<out.Data()<<endl;
}


