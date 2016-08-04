//*-- AUTHOR : Jerzy Pietraszko, Ilse Koenig 
//*-- Created : 03/02/2006
// Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HWallTrbUnpacker
//
//  Class for unpacking TRB data and filling 
//	the Forward Wall Raw category
//  Base subevent decoding is performed by decode function
//  see: htrbbaseunpacker.h 
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hwalltrbunpacker.h"
#include "walldef.h"
#include "htrblookup.h"
#include "hwallraw.h"
//#include "hwallrefhitpar.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

ClassImp (HWallTrbUnpacker)

HWallTrbUnpacker::HWallTrbUnpacker (UInt_t id):HTrbBaseUnpacker (id) {
    // constructor
    pRawCat = NULL;
    debugWallFlag=0;
    doRefTimeCorr=0;
    noTimeRefCorr=false;
}

Bool_t HWallTrbUnpacker::init (void) {

    // creates the raw category and gets the pointer to the lookup table
    pRawCat = gHades->getCurrentEvent ()->getCategory (catWallRaw);
    if (!pRawCat) {
	pRawCat = gHades->getSetup ()->getDetector ("Wall")->buildCategory (catWallRaw);
	if (!pRawCat)
	    return kFALSE;
	gHades->getCurrentEvent ()->addCategory (catWallRaw, pRawCat, "Wall");
    }
    loc.set (1, 0);
    lookup = (HTrbLookup *) (gHades->getRuntimeDb ()->getContainer ("TrbLookup"));
    if (!lookup) return kFALSE;

    return kTRUE;
}


Int_t HWallTrbUnpacker::execute (void)
{

    HWallRaw *pRaw = 0;	// pointer to Raw category

    if (gHades->isCalibration ()) {
	//calibration event
	return 1;
    }//if (gHades->isCalibration ())

    // decode subevent data
    if (pSubEvt) {  // pSubEvt - make sure that there is something for decoding

	// decode the subevent and fill arrays, see: htrbbaseunpacker.h
	if(!decode()) return(-1);

	if(!noTimeRefCorr){
	    switch(trbDataVer){
	    case 0:// old
	    case 2:// Wall/Veto
		// RPC style
		correctRefTimeCh31();
		break;
	    case 3:
		// new scheme for high rates
		correctRefTimeCh127();
		break;
		//         case 4:
		// W.K. electronics
		//            correctRefTimeWKStyle();
		//            break;
	    default:
		// do nothing
		break;
	    }
	}

	// get the part of the lookup table which belongs to the subevent ID
	// in order to check which TRBchannel belongs to the WallDetector

	///////////////////////////////
	//
	//  Be Carefull !! I am changing SubEvent ID
	//  Should not be a case !! Only for tests
	//  My test HLD data are with SubEventId = 545
	//  In Hydra Allowed SubEvents Ids for TRB starts from 800...
	//
	//////////////////////////////


	HTrbLookupBoard *board = lookup->getBoard (subEvtId);
	if (!board) {
	    // Problem: unpacker for specific SubId was called but for this SubId
	    // no channel in lookup table axist.
	    //
	    Warning ("execute", "SubEvent: %i unpacked but channel in Lookup table does not exist", subEvtId);
	}//if (!board)

	//fill RAW category for Forward Wall detector
	for (Int_t i = 0; i < 128; i++) {
	    if (trbLeadingMult[i] < 1) continue; //Leading Time data for this channel exist
	    // fill wall raw category
	    HTrbLookupChan *chan = board->getChannel (i);
	    if (chan && 'W' == (Char_t) (chan->getDetector ())) {	//Channel belongs to Wall det
		loc[0] = (Int_t)chan->getCell();
		pRaw = (HWallRaw *) pRawCat->getObject (loc);

		if (!pRaw) {
		    pRaw = (HWallRaw *) pRawCat->getSlot (loc);
		    if (pRaw) {
			pRaw = new (pRaw) HWallRaw;
			pRaw->setCell ((Int_t) chan->getCell ());
			for(Int_t chmult=0; chmult< trbLeadingMult[i]; chmult++){
			    pRaw->fill( trbLeadingTime[i][chmult], trbADC[i][chmult]);
			}
		    }//if (pRaw)
		    else {
			Error ("execute()", "Can't get slot");
			cerr << "Can't get slot: Cell " <<(Int_t)chan->getCell() << endl;
			return -1;
		    }
		}//if (!pRaw)
		else {
		    Error ("execute()", "Slot already exists! This should not happen with TRB board data.");
		    cerr << "Slot already exists: Cell " <<(Int_t)chan->getCell() << endl;
		}

		// fill time info for channel: mod, fiber
		// btw: HHodoRaw will care itself if there are more hits and
		// handle this cases correctly by itself --- B.S.
		// todo: convert invalid time from unpacker -> invalid time in hodo class ... all negative are invalid?


	    }//if (chan && 'W' == (Char_t) (chan->getDetector ()))
	}// for(Int_t i=0; i<128; i++)

    }//if(pSubEvt)

    return 1;

}
