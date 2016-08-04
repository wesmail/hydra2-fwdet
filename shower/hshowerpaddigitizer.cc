//*-- Authors: Leszek Kidon & Jacek Otwinowski
//*-- Last Modified: 08/07/2001 (Marcin Jaskula)
//*-- Last Modified: 17/11/2005 (Jacek Otwinowski)
//*-- Last Modified: 05/12/2006 (Jacek Otwinowski)
//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
//
//  HShowerPadDigitizer
//
// This class digitizes the shower pads. For each fired wire it calculates all
// the pads which the charge couples to. For each pad the track numbers of the
// particles that fired the pad are stored in a linear category (HShowerTrack).
// This category is sortable, in particular the track numbers are sorted by
// the respective pad address.
// All the fired pads are stored in a matrix category (catShowerRawMatr,
// this matrix category is used only for the simulated data).
//
//  The Shower digitization is split into several tasks as shown
//  in the flow diagram below.
//
//   ----------------------                                                                               //
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
//                                  |  HShowerHit      |   <--------\----|                                //
//                                   ------------------ <            \                                    //
//                                                       \            \                                   //
//                                                        \-------------> ------------------------        //
//                                                                       | HShowerHitTrackMatcher |       //
//                                                                        ------------------------        //
//
//
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
//-------------------------------------------------
// Used to suppress the secondaries created in the
// SHOWER itself.
//        0 = realistic (secondaries included)
//        1 primary particle is stored
//        2 the first track number entering the SHOWER in SAME SECTOR is stored
//        3 the first track number entering the TOFINO in SAME SECTOR is stored
//          or the primary track if no TOFINO was found
//        4 (default) the first track number entering the TOFINO in SAME SECTOR is stored
//          or the first track in SHOWER if no TOFINO was found
//
//
//       The mode can be selected by static void HShowerPadDigitizer::setModeTrack(Int_t mode)
//
//////////////////////////////////////////////////////////////////////////////
#include "hshowerpaddigitizer.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hshowerdetector.h"
#include "hcategory.h"
#include "hmatrixcategory.h"
#include "hlinearcategory.h"
#include "hlocation.h"
#include "hshowergeantwire.h"
#include "hshowercal.h"
#include "hshowertrack.h"
#include "hshowerdigipar.h"
#include "hshowergeometry.h"
#include "hdebug.h"
#include "hades.h"
#include "hgeantshower.h"
#include "hgeanttof.h"
#include "hgeantrpc.h"
#include "hgeantkine.h"
#include "showerdef.h"

#include "TMath.h"

#include <algorithm>

#define MIN(A,B) (((A) <= (B)) ? (A) : (B))
#define MAX(A,B) (((A) >= (B)) ? (A) : (B))
#define TABLE_SIZE 10

ClassImp(HShowerPadDigitizer)
Int_t HShowerPadDigitizer::modeTrack = 4;

HShowerPadDigitizer::HShowerPadDigitizer(const Text_t *name,const Text_t *title) :
  HShowerDigitizer(name,title)
{
  fTrackIter = NULL;
  fChannelCoeff = 256.0 / 60.0; // QDC calibration factor
  fShowerRawMatrIter = NULL;
  isRpc = kFALSE;
}

HShowerPadDigitizer::HShowerPadDigitizer()
{
  fTrackIter = NULL;
  fShowerRawMatrIter = NULL;
  isRpc = kFALSE;
}


HShowerPadDigitizer::~HShowerPadDigitizer(void) {
  if (fTrackIter) delete fTrackIter;
  if (fShowerRawMatrIter) delete fShowerRawMatrIter;

}

Bool_t HShowerPadDigitizer::init()
{
    // creates ShowerGeantWire(input), ShowerRawMatr(output) and ShowerTrack
    // categories and adds them to current event
    // creates an iterator for ShowerTrack category

    printf("initialization of shower pad digitizer \n");
    HCategory *pCat;
    HShowerDetector *pShowerDet = (HShowerDetector*)gHades->getSetup()->getDetector("Shower");

    pCat=gHades->getCurrentEvent()->getCategory(catShowerGeantWire);
    if (!pCat) {
	pCat=pShowerDet->buildCategory(catShowerGeantWire);

	if (!pCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catShowerGeantWire, pCat, "Shower");
    }
    setInCat(pCat);

    pCat=gHades->getCurrentEvent()->getCategory(catShowerRawMatr);
    if (!pCat) {
	pCat=pShowerDet->buildCategory(catShowerRawMatr);

	if (!pCat) return kFALSE;
	else {
	    gHades->getCurrentEvent()->addCategory(catShowerRawMatr, pCat, "Shower");
	}
    }
    setOutCat(pCat);
	if(pCat) fShowerRawMatrIter=(HIterator*)pCat->MakeIterator("native");

    if(gHades->getEmbeddingMode()>0)
    {   // we need to get the HShowerCal catgeory
        // in addition
	pCat=gHades->getCurrentEvent()->getCategory(catShowerCal);
	if (!pCat) {
            Error("init()","No catShowerCal in input ... needed for embedding!");
	    return kFALSE;
	}
	if(pCat) fShowerCalIter= (HIterator*)pCat->MakeIterator("native");
    }

    pCat=gHades->getCurrentEvent()->getCategory(catShowerTrack);
    if (!pCat) {
	pCat=pShowerDet->buildCategory(catShowerTrack);

	if (!pCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catShowerTrack, pCat, "Shower");
    }

    setTrackCat(pCat);
    fTrackIter = (HIterator*)getTrackCat()->MakeIterator("native");

    //------------------------------------------
    // getting sim categories
    fGeantKineCat = (HLinearCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
    if(!fGeantKineCat){
	Error("init()","Received Zero pointer for catGeantKine!");
        return kFALSE;
    }
    fGeantShowerCat = (HLinearCategory*)gHades->getCurrentEvent()->getCategory(catShowerGeantRaw);
    if(!fGeantShowerCat){
	Error("init()","Received Zero pointer for catShowerGeantRaw!");
        return kFALSE;
    }
    fGeantTofCat = (HLinearCategory*)gHades->getCurrentEvent()->getCategory(catTofGeantRaw);
    if(!fGeantTofCat){
	Warning("init()","Received Zero pointer for catTofGeantRaw!");
    }
    fGeantRpcCat = (HLinearCategory*)gHades->getCurrentEvent()->getCategory(catRpcGeantRaw);
    if(!fGeantRpcCat){
	Warning("init()","Received Zero pointer for catRpcGeantRaw!");
    }

    if (!fGeantRpcCat && !fGeantTofCat) return kFALSE;

    if (fGeantRpcCat) isRpc = kTRUE;
    //------------------------------------------


    return HShowerDigitizer::init();
}

HShowerPadDigitizer& HShowerPadDigitizer::operator=(HShowerPadDigitizer &c) {
// It should have been done
  return c;
}

Int_t HShowerPadDigitizer::execute()
{

    trackMap.clear();    // filled in digitize()->digiPads()->updatePad()

    if(gHades->getEmbeddingMode() > 0 && gHades->getEmbeddingDebug()!=1)
    {
	//---------------------------------------------------------------
	// loop over ShowerCal category and create the
	// corresponding HShowerTrack for the real data objects.

	HLocation loc;
	loc.set(4,0,0,0);
	HShowerCal *pCal;
	fShowerCalIter->Reset();
        HShowerRaw pRaw;
	while ((pCal = (HShowerCal *)fShowerCalIter->Next()) != 0)
	{
	    loc[0] = pCal->getSector();
	    loc[1] = pCal->getModule();
	    loc[2] = pCal->getRow();
	    loc[3] = pCal->getCol();
	    pRaw.setSector(loc[0]);
            pRaw.setModule(loc[1]);
            pRaw.setRow(loc[2]);
            pRaw.setCol(loc[3]);

	    //------------------------------------------------------------
            // add pads to track map
	    Int_t addTr = pCal->getAddress();
	    vector<Int_t> v;
	    v.push_back(gHades->getEmbeddingRealTrackId());
	    trackMap.insert( make_pair( addTr, v ) );
            //------------------------------------------------------------
	}
    }
    //---------------------------------------------------------------


    //---------------------------------------------------------------
    TObject *pHit;

    lNrEvent++;
    fIter->Reset();
    while((pHit = fIter->Next()))
    {
	   digitize(pHit);
    }
    //---------------------------------------------------------------



    //---------------------------------------------------------------
    // create HShowerTrack object for each unique track of the fired pad
    HShowerTrack*  pShowerTrack;
    HLocation loc;
    Int_t s,m,row,col;

    vector<Int_t> check;
    for( map<Int_t, vector<Int_t> >::iterator iter = trackMap.begin(); iter != trackMap.end(); ++iter)
    {

	vector<Int_t>& list = iter ->second;
	Int_t nAddress      = iter ->first;

	s   =  nAddress                       /100000;
	m   = (nAddress - 100000*s)           / 10000;
	row = (nAddress - 100000*s - 10000*m) /   100;
	col = (nAddress - 100000*s - 10000*m - 100*row);

	loc.set(4,s,m,row,col);

        Int_t n = list.size();

	std::sort( list.begin(), list.end() ); // sort increasing order : real,primary,secondary with TOF,secondary with no TOF

	check.clear();
	for( Int_t i = 0; i < n; ++ i ) {
	    Int_t nTrack  = list[i];
	    if(     nTrack > 100000000) { nTrack -= 100000000; }     // secondary with no TOF
	    else if(nTrack >     10000) { nTrack -=     10000; }     // secondary with TOF
            else if(nTrack >         0) {                      }     // primary
	    else if(nTrack < 0)         {                      }     // real
	    if (i > 0 && find( check.begin(), check.end(), nTrack ) != check.end() ){
		// track exists already .... nothing todo
	    } else {
                // new track ... create HShowerTrack
		check.push_back(nTrack);

		pShowerTrack = (HShowerTrack*)((HLinearCategory*)getTrackCat())->getNewSlot(loc);

		if (pShowerTrack) {
		    pShowerTrack = new(pShowerTrack) HShowerTrack;
		    pShowerTrack->setAddress(nAddress);
		    pShowerTrack->setTrack(nTrack);
		}
	    }
	}
    }
    //---------------------------------------------------------------


    //---------------------------------------------------------------
    // gain and efficiency
    Double_t fQ;
    HShowerRawMatr *pRawMatr;
    fShowerRawMatrIter->Reset();
    while((pRawMatr = (HShowerRawMatr*)fShowerRawMatrIter->Next()))
    {
	   // calculate gain
	   fQ = gainCharge(pRawMatr);
	   pRawMatr->setCharge(fQ);

	   // effciency model good for post1 and post2
	   if(!checkEfficiency(pRawMatr)) {
	      pRawMatr->setCharge(0.0);
	   }
	}
    //---------------------------------------------------------------

    sort();   // this is used to call the track sort() in pad digitizer

    return 0;
}


Bool_t HShowerPadDigitizer::digitize(TObject *pHit) {
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HShowerPadDigitizer::execute");
  gDebuger->message("Hit  cat points to %p",pHit);
#endif

  HShowerGeantWire *pGeantWire = (HShowerGeantWire*) pHit;
  if (pGeantWire) {
    digiPads(pGeantWire);
  }

#if DEBUG_LEVEL>2
  gDebuger->leaveFunc("HShowerPadDigitizer::execute");
#endif

  return (pGeantWire) ? kTRUE : kFALSE;
}

Int_t HShowerPadDigitizer::sort() {
//    if (((HLinearCategory*)getTrackCat())->IsSortable())
    ((HLinearCategory*)getTrackCat())->sort();
    return 0;
}

Int_t HShowerPadDigitizer::findFirstHitInShower(Int_t trackID)
{
    //-------------------------------------------------
    // Used to suppress the secondaries created in the
    // SHOWER itself.
    //        0 = realistic (secondaries included)
    //        1 primary particle is stored
    //        2 the first track number entering the SHOWER in SAME SECTOR is stored
    //        3 the first track number entering the TOFINO in SAME SECTOR is stored
    //          or the primary track if no TOFINO was found
    //        4 (default) the first track number entering the TOFINO in SAME SECTOR is stored
    //          or the first track in SHOWER if no TOFINO was found


    Int_t numTrack = trackID;

    if(numTrack <= 0) return numTrack; // nothing to do for negative track numbers

    //--------------------------------------
    // case 0  : in = out
    if(modeTrack == 0) { return numTrack; }

    HGeantShower *poldShower;
    Int_t first = 0;
    Int_t parent= 0;

    HGeantKine* kine = (HGeantKine*)fGeantKineCat->getObject(numTrack - 1);
    if(kine){
        parent = kine->getParentTrack();

	if( parent == 0) {  return numTrack; } // nothing todo

	first = kine->getFirstShowerHit();
	if(first != -1){
	    poldShower = (HGeantShower*)fGeantShowerCat->getObject(first);
	} else {
	    Error("findFirstHitInShower()","No first shower hit!");
	    return numTrack;
	}

    } else {
	Error("findFirstHitInShower()","Received Zero pointer for kine!");
	return numTrack;
    }
    if(numTrack != poldShower->getTrack()){
	Error("findFirstHitInShower()","First shower hit not same trackID!");
	return numTrack;
    }

    //--------------------------------------------------------
    // return the track number for
    // the selected option modeTrack


    Int_t s = poldShower->getSector();

    //--------------------------------------
    // case 1  : in -> primary
    if(modeTrack == 1)
    {   // return track number of primary particle
	// of the given track

	kine = HGeantKine::getPrimary(numTrack,fGeantKineCat);
	return kine->getTrack();
    }

    //--------------------------------------
    // case 2 and 3  : entering SHOWER/TOF

    Int_t tempTrack = numTrack;
    first           = 0;

    //--------------------------------------
    // case 2  : entering SHOWER
    if(modeTrack == 2)
    {
	while((kine = kine->getParent(tempTrack,fGeantKineCat)) != 0)
	{
	    first = kine->getFirstShowerHit();
	    if(first != -1)
	    {
		// track is still in SHOWER

		HGeantShower* gshower = (HGeantShower*)fGeantShowerCat->getObject(first);
		Int_t s1 = gshower->getSector();

		if(s == s1)
		{   // check only sector
		    tempTrack = kine->getTrack();
		} else {
		    // track has no SHOWER hit any longer
		    // which fulfills the condition
		    break;
		}
	    } else {
		// track has no SHOWER hit any longer,
		// so the previous object was the one we
		// searched for
		break;
	    }
	}
        return tempTrack;
    }
    //--------------------------------------

    //--------------------------------------
    // case 3  : entering TOFINO
    if(modeTrack >= 3)
    {
	Bool_t foundTof  = kFALSE;
	Int_t tempTrack2 = tempTrack;
	do {
	    first = kine->getFirstTofHit();
	    if(first != -1)
	    { // we are in TOF
		HGeantTof* gtof = (HGeantTof*)fGeantTofCat->getObject(first);
		Int_t s1 = gtof->getSector();
		Int_t m  = gtof->getModule();

		if(s == s1 && m > 21 )
		{   // check only sector + TOFINO
		    foundTof  = kTRUE;
		    tempTrack = tempTrack2;
		}
	    }
	    tempTrack2 = kine->getParentTrack();

	} while( tempTrack2 > 0 && (kine = (HGeantKine*)fGeantKineCat->getObject(tempTrack2 - 1)) != 0);

	if(foundTof) { tempTrack += 10000; }
        else {
	    kine = (HGeantKine*)fGeantKineCat->getObject(numTrack - 1);


	    if( modeTrack == 3 ){
		// store primaries if no TOFino was found
		kine = HGeantKine::getPrimary(numTrack,fGeantKineCat);
		tempTrack = kine->getTrack() + 100000000;
	    } else if (modeTrack == 4){
        	//--------------------------------------
                // recover first particle entering SHOWER
		while((kine = kine->getParent(tempTrack,fGeantKineCat)) != 0)
		{
		    first = kine->getFirstShowerHit();
		    if(first != -1)
		    {
			// track is still in SHOWER
			HGeantShower* gshower = (HGeantShower*)fGeantShowerCat->getObject(first);
			Int_t s1 = gshower->getSector();

			if(s == s1)
			{   // check only sector
			    tempTrack = kine->getTrack();
			} else {
			    // track has no SHOWER hit any longer
			    // which fulfills the condition
			    break;
			}
		    } else {
			// track has no SHOWER hit any longer,
			// so the previous object was the one we
			// searched for
			break;
		    }
		}
		tempTrack += 100000000;
		//--------------------------------------
	    }
	}
    }
    //--------------------------------------

    return tempTrack;
}

Int_t HShowerPadDigitizer::findFirstHitInRpc(Int_t trackID)
{
    //-------------------------------------------------
    // Used to suppress the secondaries created in the
    // SHOWER itself.
    //        0 = realistic (secondaries included)
    //        1 primary particle is stored
    //        2 the first track number entering the SHOWER in SAME SECTOR is stored
    //        3 the first track number entering the RPC in SAME SECTOR is stored
    //          or the primary track if no RPC was found
    //        4 (default) the first track number entering the RPC in SAME SECTOR is stored
    //          or the first track in SHOWER if no RPC was found


    Int_t numTrack = trackID;

    if(numTrack <= 0) return numTrack; // nothing to do for negative track numbers

    //--------------------------------------
    // case 0  : in = out
    if(modeTrack == 0) { return numTrack; }

    HGeantShower *poldShower;
    Int_t first = 0;
    Int_t parent= 0;

    HGeantKine* kine = (HGeantKine*)fGeantKineCat->getObject(numTrack - 1);
    if(kine){
        parent = kine->getParentTrack();

	if( parent == 0) {  return numTrack; } // nothing todo

	first = kine->getFirstShowerHit();
	if(first != -1){
	    poldShower = (HGeantShower*)fGeantShowerCat->getObject(first);
	} else {
	    Error("findFirstHitInRpc()","No first shower hit!");
	    return numTrack;
	}

    } else {
	Error("findFirstHitInRpc()","Received Zero pointer for kine!");
	return numTrack;
    }
    if(numTrack != poldShower->getTrack()){
	Error("findFirstHitInRpc()","First shower hit not same trackID!");
	return numTrack;
    }

    //--------------------------------------------------------
    // return the track number for
    // the selected option modeTrack


    Int_t s = poldShower->getSector();

    //--------------------------------------
    // case 1  : in -> primary
    if(modeTrack == 1)
    {   // return track number of primary particle
	// of the given track

	kine = HGeantKine::getPrimary(numTrack,fGeantKineCat);
	return kine->getTrack();
    }

    //--------------------------------------
    // case 2 and 3  : entering SHOWER/RPC

    Int_t tempTrack = numTrack;
    first           = 0;

    //--------------------------------------
    // case 2  : entering SHOWER
    if(modeTrack == 2)
    {
	while((kine = kine->getParent(tempTrack,fGeantKineCat)) != 0)
	{
	    first = kine->getFirstShowerHit();
	    if(first != -1)
	    {
		// track is still in SHOWER

		HGeantShower* gshower = (HGeantShower*)fGeantShowerCat->getObject(first);
		Int_t s1 = gshower->getSector();

		if(s == s1)
		{   // check only sector
		    tempTrack = kine->getTrack();
		} else {
		    // track has no SHOWER hit any longer
		    // which fulfills the condition
		    break;
		}
	    } else {
		// track has no SHOWER hit any longer,
		// so the previous object was the one we
		// searched for
		break;
	    }
	}
        return tempTrack;
    }
    //--------------------------------------

    //--------------------------------------
    // case 3  : entering RPC
    if(modeTrack >= 3)
    {
	Bool_t foundRPC  = kFALSE;
	Int_t tempTrack2 = tempTrack;
	do {
	    first = kine->getFirstRpcHit();
	    if(first != -1)
	    { // we are in TOF
		HGeantRpc* grpc = (HGeantRpc*)fGeantRpcCat->getObject(first);
		Int_t s1 = grpc->getSector();

		if(s == s1)
		{   // check only sector + RPC
		    foundRPC  = kTRUE;
		    tempTrack = tempTrack2;
		}
	    }
	    tempTrack2 = kine->getParentTrack();

	} while( tempTrack2 > 0 && (kine = (HGeantKine*)fGeantKineCat->getObject(tempTrack2 - 1)) != 0);

	if(foundRPC) { tempTrack += 10000; }
        else {
	    kine = (HGeantKine*)fGeantKineCat->getObject(numTrack - 1);


	    if( modeTrack == 3 ){
		// store primaries if no RPC was found
		kine = HGeantKine::getPrimary(numTrack,fGeantKineCat);
		tempTrack = kine->getTrack() + 100000000;
	    } else if (modeTrack == 4){
        	//--------------------------------------
                // recover first particle entering SHOWER
		while((kine = kine->getParent(tempTrack,fGeantKineCat)) != 0)
		{
		    first = kine->getFirstShowerHit();
		    if(first != -1)
		    {
			// track is still in SHOWER
			HGeantShower* gshower = (HGeantShower*)fGeantShowerCat->getObject(first);
			Int_t s1 = gshower->getSector();

			if(s == s1)
			{   // check only sector
			    tempTrack = kine->getTrack();
			} else {
			    // track has no SHOWER hit any longer
			    // which fulfills the condition
			    break;
			}
		    } else {
			// track has no SHOWER hit any longer,
			// so the previous object was the one we
			// searched for
			break;
		    }
		}
		tempTrack += 100000000;
		//--------------------------------------
	    }
	}
    }
    //--------------------------------------

    return tempTrack;
}

Int_t HShowerPadDigitizer::findFirstHit(Int_t trackID)
{
    if(isRpc) {return findFirstHitInRpc   (trackID);}
    else      {return findFirstHitInShower(trackID);}
}


void HShowerPadDigitizer::updatePad(HShowerPad *pPad, Float_t fIndQ, Int_t nSect, Int_t nMod, Int_t nTrack)
{
   Int_t nRow, nCol;
   HShowerRaw *pShowerRaw=NULL;
   HLocation loc;
   Float_t fUpdateThreshold = pDigiPar->getUpdatePadThreshold();

   pPad->getPadPos(&nRow, &nCol);
   loc.set(4, nSect, nMod, nRow, nCol);

   pShowerRaw = (HShowerRaw*)((HMatrixCategory*)getOutCat())->getObject(loc);
   if (!pShowerRaw)
   {
      pShowerRaw = (HShowerRaw*)((HMatrixCategory*)getOutCat())->getSlot(loc);
      if (pShowerRaw != NULL)
      {
        pShowerRaw = new(pShowerRaw) HShowerRaw;
        pShowerRaw->setSector(loc[0]);
        pShowerRaw->setModule(loc[1]);
        pShowerRaw->setRow(loc[2]);
        pShowerRaw->setCol(loc[3]);
      }
   }

    if(pShowerRaw)
    {
        pShowerRaw->addCharge(fIndQ);

	if(pShowerRaw->getCharge() >= fUpdateThreshold) {

	    Int_t tempTrack = findFirstHit(nTrack);
	    //------------------------------------------------------------
	    Int_t addTr   = pShowerRaw->getAddress();
	    map<Int_t,vector<Int_t> >::iterator pos  = trackMap.find(addTr);
	    if( pos == trackMap.end() ) {  // address not yet registered
		vector<Int_t> v;
		v.push_back(tempTrack);
		trackMap.insert( make_pair( addTr, v ) );
	    } else {                       // add track to the list of the pad
		(pos->second).push_back(tempTrack);
	    }
	    //------------------------------------------------------------

	}
    }
}

Float_t HShowerPadDigitizer::calcCharge(Float_t charge, Float_t dist,
                Float_t Xd, Float_t Yd, Float_t Xu, Float_t Yu) {
//charge density matrix is calculated

const Float_t twoPI = 6.28318530718;

return ((charge/twoPI)*(
                      atan(Xd*Yd/(dist*sqrt(dist*dist+Xd*Xd+Yd*Yd))) -
                      atan(Xd*Yu/(dist*sqrt(dist*dist+Xd*Xd+Yu*Yu))) +
                      atan(Xu*Yu/(dist*sqrt(dist*dist+Xu*Xu+Yu*Yu))) -
                      atan(Xu*Yd/(dist*sqrt(dist*dist+Xu*Xu+Yd*Yd)))));
}

/* ************************************************************************** */

void HShowerPadDigitizer::digiPads(HShowerGeantWire* pWireHit)
{
Float_t fX, fY, fQ;
Float_t factor;
Float_t sum;
Int_t   nPadX, nPadY, nPadRange;
Float_t pChargeTable[MAX_PADS_DIST];
Int_t     iMaxInArray, m;
HShowerPad *pTmpPad;
HShowerPad *pCPad;
Float_t fPadThreshold    = pDigiPar->getPadThreshold();
Float_t fUpdateThreshold = pDigiPar->getUpdatePadThreshold();
HShowerPadTab *pPadParam = pGeometry->getPadParam(pWireHit->getModule());

    // don't calculate if charge belowe threshold
    if((fQ = pWireHit->getCharge()) < fUpdateThreshold)
        return;
        
    pWireHit->getXY(&fX, &fY);

            //row = Y,col = X
    if((pCPad = pPadParam->getPad(fX, fY)) == NULL)
        return;

    pCPad->getPadPos(&nPadY, &nPadX);  //row = Y; col = X
    iMaxInArray = MAX_PADS_DIST;
    nPadRange   = (MAX_PADS_DIST - 1) / 2;

    if(numericalCalc(pCPad, pWireHit, pChargeTable) != 0)
        return;

        // calculate sum for the first renormalization
    sum = 0.0f;
    for(m = 0; m < iMaxInArray; m++)
        sum += pChargeTable[m];

    if(sum <= 0.0f)
        return;

    factor = fQ / sum;
    for(m = 0; m < iMaxInArray; m++)
    {
        pChargeTable[m] *= factor;
    }

        // calculate sum for a renormalization
    sum = 0.0f;
    for(m = 0; m < iMaxInArray; m++)
    {
        if(pChargeTable[m] >= fPadThreshold)
            sum += pChargeTable[m];
        else
            pChargeTable[m] = 0.0f;
    }

        // if sum is below threshold put all charge to the central pad
    if(sum < fPadThreshold)
    {
        for(m = 0; m < iMaxInArray; m++)
            pChargeTable[m] = 0.0f;

        pChargeTable[nPadRange] = fQ;
    }
    else
    {
            // renormalization: sum of the charge on the pads
            //                  must be equal to the charge on the wire
        factor = fQ / sum;

        for(m = 0; m < iMaxInArray; m++)
        {
            if(pChargeTable[m] > 0.0f)
                pChargeTable[m] *= factor;
        }
    }

    for(m = 0; m < iMaxInArray; m++)
    {
        pTmpPad = pPadParam->getPad(nPadY, nPadX + m - nPadRange);
        if(((m == nPadRange) || (pChargeTable[m] >= fPadThreshold))
                && (pTmpPad != NULL) && (pTmpPad->getPadFlag()))
        {
            updatePad(pTmpPad, pChargeTable[m], pWireHit->getSector(),
                        pWireHit->getModule(), pWireHit->getTrack());
        }
    }
}
//---------------------------------------------------------
Double_t HShowerPadDigitizer::gainCharge(HShowerRawMatr *pRawMatr) 
{
// gain charge in simulation

  return pRawMatr->getCharge()*pDigiPar->getGain(pRawMatr->getSector(),
                                                 pRawMatr->getModule());
}

//---------------------------------------------------------
Bool_t HShowerPadDigitizer::checkEfficiency(HShowerRawMatr *pRawMatr) 
{
// check efficiency 
// model good for post converters

// if pre-converter
if(pRawMatr->getModule() == 0) return kTRUE;

Double_t threshold = pDigiPar->getThreshold(pRawMatr->getSector(),pRawMatr->getModule());

   if(pRawMatr->getCharge() > threshold) return kTRUE;
   else return kFALSE;
}

/* ************************************************************************** */

Int_t HShowerPadDigitizer::calcLimit(Float_t fCor, Int_t nMatrixRange,
                                                            Float_t fBoxSize)
{
//charge density matrix is delimited to the sensitive area which covers fired pads


  Int_t k, nLimit;

  k = (Int_t)((fabs(fCor) + 0.5*fBoxSize)/fBoxSize);

  if (fCor < 0 )
     nLimit = (k>nMatrixRange) ? 0 : nMatrixRange - k;
  else
     nLimit = (k>nMatrixRange) ? 2*nMatrixRange : nMatrixRange + k;

  return nLimit;
}


void HShowerPadDigitizer::moveCoord(HShowerPad *pPad,Float_t distWire, Float_t fDx, Float_t fDy,
          Float_t *corXld, Float_t *corYld, Float_t *corXlu, Float_t *corYlu,
          Float_t *corXrd, Float_t *corYrd, Float_t *corXru, Float_t *corYru) {
// moves charge density matrix to the place where is placed induced charge on the sense wire

  *corXld = pPad->getXld() - fDx;
// *corYld = pPad->getYld() - fDy;
  *corYld =  fDy - fDy - distWire;
  *corXlu = pPad->getXlu() - fDx;
//  *corYlu = pPad->getYlu() - fDy;
  *corYlu = fDy - fDy + distWire;
  *corXrd = pPad->getXrd() - fDx;
//  *corYrd = pPad->getYrd() - fDy;
  *corYrd = fDy - fDy - distWire;
  *corXru = pPad->getXru() - fDx;
//  *corYru = pPad->getYru() - fDy;
  *corYru = fDy - fDy + distWire;
}


void HShowerPadDigitizer::analyticCalc(HShowerPad *pPad, HShowerGeantWire* pWireHit)
{
// this function calculates charge induced on pad analytically
// it is useful only for square or rectangular pads
  Float_t fX, fY;
  Float_t fIndQ;

  Float_t corXld, corYld, corXlu, corYlu;
  Float_t corXrd, corYrd, corXru, corYru;
  pWireHit->getXY(&fX, &fY);

  Int_t nModule = pWireHit->getModule();

  HShowerWireTab *pfWire = pGeometry->getWireTab(nModule);
  Float_t distWire  = pfWire->getDistWire();

//  cout <<  " distWire " << distWire << endl;



  moveCoord(pPad, distWire, fX, fY, &corXld, &corYld, &corXlu, &corYlu,
                                &corXrd, &corYrd, &corXru, &corYru);

  fIndQ =  calcCharge(pWireHit->getCharge(),
     pDigiPar->getPlaneDist(),
     corXld, corYld, corXru, corYru);

  updatePad(pPad, fIndQ, pWireHit->getSector(),
                         pWireHit->getModule(), pWireHit->getTrack());
}

//------------------------------------------------------------------------------

Int_t HShowerPadDigitizer::numericalCalc(HShowerPad *pPad,
            HShowerGeantWire* pWireHit, Float_t pfValues[MAX_PADS_DIST])
{
Float_t       fAl, fAr, fBl, fBr;
Float_t       fXd, fXu, fYd, fYu;
Float_t       fXmin, fXmax;
Float_t       fYmin, fYmax;
Float_t       fWireX, fWireY;
Float_t       fY, fX, fQ;
Int_t         i;
HShowerFrameCorner *pCorner;

    if((pPad == NULL) || (pPad->getPadFlag() == 0))
        return -1;

  Int_t iModule                 = pWireHit->getModule();
  HShowerFrame *pFrame          = pGeometry->getFrame(iModule);
  Float_t fDistWire             = pGeometry->getWireTab(iModule)->getDistWire();
  Int_t iMatrixRange            = pDigiPar->getMatrixRange();
  Float_t fBoxSize              = pDigiPar->getBoxSize();
  const Float_t* pfChargeMatrix = pDigiPar->getChargeMatrix();

        // left border
    if((pPad->getPadFlag() == 2) && (pPad->getXld() < 0.0))
    {
        pCorner = pFrame->getCorner(0);
        fXd = pCorner->getX();
        fYd = pCorner->getY();

        pCorner = pFrame->getCorner(1);
        fXu = pCorner->getX();
        fYu = pCorner->getY();
    }
    else
    {
        fXd = pPad->getXld();
        fYd = pPad->getYld();
        fXu = pPad->getXlu();
        fYu = pPad->getYlu();
    }

    fAl = (fXu - fXd) / (fYu - fYd);
    fBl = fXu - fAl * fYu;

        // right border
    if((pPad->getPadFlag() == 2) && (pPad->getXld() >= 0.0))
    {
        pCorner = pFrame->getCorner(3);
        fXd = pCorner->getX();
        fYd = pCorner->getY();

        pCorner = pFrame->getCorner(2);
        fXu = pCorner->getX();
        fYu = pCorner->getY();
    }
    else
    {
        fXd = pPad->getXrd();
        fYd = pPad->getYrd();
        fXu = pPad->getXru();
        fYu = pPad->getYru();
    }

    fAr = (fXu - fXd) / (fYu - fYd);
    fBr = fXu - fAr * fYu;

        // range of the charge matrix
    pWireHit->getXY(&fWireX, &fWireY);
    fXmin = fWireX - iMatrixRange * fBoxSize;
    fXmax = fWireX + iMatrixRange * fBoxSize;
    fYmin = fWireY - fDistWire + 0.5 * fBoxSize;
    fYmax = fWireY + fDistWire - 0.5 * fBoxSize;

    memset(pfValues, 0, MAX_PADS_DIST * sizeof(Float_t));

    for(fY = fYmin; fY <= fYmax; fY += fBoxSize)
    {
            // left border
        if((fX = fAl * fY + fBl) >= fXmin && (fWireX-fX) >= 0.0)
        {
            i = (int)((fX - fXmin) / fBoxSize);
	    if(i>2*iMatrixRange||i<0)
	    {
			cout << " left border fWireX-fX " << fWireX-fX <<  endl;
			cout << " fXd,fYd,fXu,fYu " << pPad->getXld() <<","<< pPad->getYld() << ","<< pPad->getXlu() << ","<< pPad->getYlu() <<  endl;
		Error("numericalCalc()","left border matrix : max %i, ind %i, fX %7.4f, fXmin %7.4f, fWireX %7.4f,fBoxSize %5.3f, PadNr %i",2*iMatrixRange,i,fX,fXmin,fWireX,fBoxSize,pPad->getPadNr());
		continue;
	    }
	    pfValues[0] += pfChargeMatrix[i];
            fQ = pfChargeMatrix[i];
        }
        else
            fQ = 0.0f;

            // right border
        if((fX = fAr * fY + fBr) <= fXmax && (fWireX-fX) <= 0.0)
        {
            i = (int)((fX - fXmin) / fBoxSize);
 	    if(i>2*iMatrixRange||i<0)
	    {
		cout << " right border fWireX-fX " << fWireX-fX <<  endl;
		cout << " fXd,fYd,fXu,fYu " << pPad->getXrd() <<","<< pPad->getYrd() << ","<< pPad->getXru() << ","<< pPad->getYru() <<  endl;

		Error("numericalCalc()","right border matrix : max %i, ind %i, fX %7.4f, fXmin %7.4f, fWireX %7.4f, fBoxSize %5.3f, PadNr %i",2*iMatrixRange,i,fX,fXmin,fWireX,fBoxSize,pPad->getPadNr());
		continue;
	    }
	    	pfValues[1] += pfChargeMatrix[i] - fQ;
            pfValues[2] += pfChargeMatrix[2 * iMatrixRange] - pfChargeMatrix[i];
        }
        else
            pfValues[1] += pfChargeMatrix[2 * iMatrixRange] - fQ;
    }

    return 0;
}
