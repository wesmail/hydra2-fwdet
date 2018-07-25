//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HEmcTrb3Unpacker
//
//  Class for unpacking TRB3 data and filling
//    the Emc Raw category
//
//  Basic subevent decoding and eventual TDC corrections are
//  performed by decode function in the base class
//  see: /base/datasource/htrb3unpacker.h
//
/////////////////////////////////////////////////////////////

#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "heventheader.h"
#include "hldsource.h"
#include "hldsubevt.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hemcdetector.h"
#include "hemcraw.h"
#include "hemctrb3lookup.h"
#include "hemctrb3unpacker.h"
#include "emcdef.h"
#include "htrbnetunpacker.h"

#include <iostream>

using namespace std;

ClassImp(HEmcTrb3Unpacker)

// JAM: this little switch will enable that we also take single leading edges into account
//#define EMC_USE_LEADING_WITHOUT_TRAILING_EDGES 1

// JAM: this little switch will enable that we also take single trailing edges into account
//#define EMC_USE_TRAILING_WITHOUT_LEADING_EDGES 1

// switch on filtering of tdc messages with first time window as defined below
#define EMC_USE_TIME_WINDOW 1

// first time window to filter out some unusable hits. units are nanoseconds relative to trigger (channel 0)
#define EMC_TIME_MIN -500.0
#define EMC_TIME_MAX  500.0
// TODO: optionally put this into some parameter container.
Bool_t HEmcTrb3Unpacker::fHasPrintedTDC = kFALSE;

HEmcTrb3Unpacker::HEmcTrb3Unpacker(UInt_t id) :
        HTrb3TdcUnpacker(id), fLookup(0), fTimeRef(kTRUE) ,fTimeShift(650.) {
	// constructor
	pRawCat = NULL;
}

Bool_t HEmcTrb3Unpacker::init(void) {
    // creates the raw category and gets the pointer to the TRB3 lookup table

    HEmcDetector* det = (HEmcDetector*) gHades->getSetup()->getDetector("Emc");
    if (!det) {
        Error("init", "No EMC Detector found.");
        return kFALSE;
    }
    pRawCat = det->buildCategory(catEmcRaw);
    if (!pRawCat)
        return kFALSE;

    fLoc.set(2, 0, 0);

    fLookup = (HEmcTrb3Lookup*) (gHades->getRuntimeDb()->getContainer("EmcTrb3Lookup"));
    if (!fLookup) {
        Error("init", "No Pointer to parameter container EmcTrb3Lookup.");
        return kFALSE;
    }

    if (NULL == trbNetUnpacker) {
        if (gHades->getDataSource()) {
            HDataSource* source = gHades->getDataSource();
            if (source->InheritsFrom("HldSource")) {
                trbNetUnpacker =
                        ((HldSource *) gHades->getDataSource())->getTrbNetUnpacker();
            } else {
                Warning("init",
                        "DataSource not inherited from HldSource! trbNetUnpacker == 0 ");
            }
        } else {
            Warning("init",
                    "Could not retrieve DataSource! trbNetUnpacker == 0 ");
        }
    }

    if (!trbNetUnpacker->init()) {
        Error("init()", "Failed to initialize HTrbNetUnpacker!");
        return kFALSE;
    }

    return kTRUE;
}

Bool_t HEmcTrb3Unpacker::reinit(void)
{
    // Called in the beginning of each run, used here to initialize TDC unpacker
    // if TDCs processors are not yet created, use parameter to instantiate them
    // if auto register TDC feature is used in setup of unpackers (by setting
    // setMinAddress()+setMaxAddress() manually)

    if (numTDC() == 0 )  // configure the tdcs only the first time we come here (do not if autoregister is used):
    {
	if (fMinAddress == 0 && fMaxAddress == 0)
	{
	    // here evaluate which subevents are configured in lookup table:
	    Int_t numTDC = fLookup->getSize();
	    Int_t offset = fLookup->getArrayOffset() ;
	    for (Int_t slot = 0; slot < numTDC; ++slot) {

		Int_t trbnetaddress = offset+slot;

		HEmcTrb3LookupBoard* tdc = fLookup->getBoard(trbnetaddress);
		if(tdc){
		    Int_t nChan = tdc->getSize();
		    if (trbnetaddress) {
			Int_t tindex = addTDC(trbnetaddress,nChan);
			Info("reinit", "Added TDC 0x%04x with %d channels from HEmcTrb3Lookup to map index %d",
			     trbnetaddress, nChan,tindex);
		    }
		}
	    }

	    // set the expected range for the automatic adding of TDC structures:
	    setMinAddress(fLookup->getArrayOffset());
	    setMaxAddress(fLookup->getArrayOffset() + fLookup->getSize());
	    fUseTDCFromLookup = kTRUE; // do not use auto register if set in
	    fHasPrintedTDC = kTRUE;

	} else {
            Info("reinit", "TDCs will be added in auto register mode between min address 0x%04x and max address 0x%04x!",fMinAddress,fMaxAddress);
	}
    }
    // we do not call reinit of superclass, since we do not use tdc calibration parameters in hydra anymore!
    return kTRUE;
}

Int_t HEmcTrb3Unpacker::execute(void) {
    if (gHades->isCalibration()) {
        //calibration event
        return 1;
    }

    if (gHades->getCurrentEvent()->getHeader()->getId() == 0xe) {
        //scaler event
        return 1;
    }

    // if there is no data, do not try to analyze it
    // pSubEvt - make sure that there is something for decoding
    if (!pSubEvt)
        return 1;

    Int_t nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

    // decodes the subevent and fill arrays, see: htrb3unpacker.h
    if (!decode()) {
        Error("decode",
                "subsubevent decoding failed!!! Evt Nr : %i SubEvtId: %x", nEvt,
                subEvtId);
        return -1;
    }

    // correct for reference time here!
    // we don't check for proper time reference correction
    // to avoid event skipping
    // this must be done better in future but now is a good workaround

    if (fTimeRef) {
        correctRefTimeCh(REFCHAN);
    }

    const Double_t timeUnit=1e9;

    for (UInt_t ntdc = 0; ntdc < numActiveTDC(); ntdc++) {
        HTrb3TdcUnpacker::TDC* tdc = getActiveTDC(ntdc);

        // check the lookup table
        HEmcTrb3LookupBoard *board = fLookup->getBoard(tdc->getTrbAddr());
        if (!board) {
            if (debugFlag > 0)
                Warning("execute",
                        "Evt Nr : %i  SubEvId: %x (%i) unpacked but TDC Board 0x%x not in lookup table",
                        nEvt, getSubEvtId(), getSubEvtId(), tdc->getTrbAddr());
            continue;
        }

	// fill the raw category for EMC detector
        for (UInt_t i = 0; i < tdc->numChannels(); i++) {
            if(REFCHAN == i) continue;

	    HTrb3TdcUnpacker::ChannelRec& theRecord = tdc->getCh(i);

// ignore channels without rising hits already here
#ifndef EMC_USE_TRAILING_WITHOUT_LEADING_EDGES
            if (theRecord.rising_mult < 1)
                continue;
#endif
            HEmcTrb3LookupChan *chan = board->getChannel(i);
            if (chan == 0) {
                Warning("execute", "Missing channel %u in lookup table", i);
                continue;
            }
            chan->getAddress(fLoc[0], fLoc[1]);

	    // if channel not connected in lookup-table, ignore it
	    if (fLoc[0] < 0)  { // empty slot default is -1
		continue;
	    }

	    // exclude all not defined and broken channels
            if (!chan->isDefinedChannel() || chan->isBrokenChannel())
                continue;


	    Bool_t fastchannel = chan->isFastChannel();
            if(fastchannel && chan->isSlowChannel())
            {
                Warning("execute", "NEVER COME HERE - channel %u has both fast and slow property! skip it..", i);
                    continue;
            }

#ifdef EMC_USE_TIME_WINDOW
            // later we may get this from parameter container or other setup file. For the moment  fix it
            Double_t tmin = EMC_TIME_MIN;
            Double_t tmax = EMC_TIME_MAX;
#endif
            UInt_t lix = 0;
            for (lix = 0; lix < theRecord.rising_mult; ++lix) {
                Double_t tm0 = theRecord.rising_tm[lix] * timeUnit;
                if (debugFlag > 0)
                    Warning("execute", "JJJJ current hit leading: tm0:%e, apply time shift:%e", tm0, fTimeShift);
                tm0 +=fTimeShift;

#ifdef EMC_USE_TIME_WINDOW
                if ((tm0 < tmin) || (tm0 > tmax)) {
                    if (debugFlag > 0)
                        Warning("execute",
                                "JJJJ Rejecting leading hit outside tmin:%e or tmax:%e",
                                tmin, tmax);
                    continue; // leading edge outside the time window
                }

#endif
                // now look for corresponding trailing edge:
                if (lix < theRecord.falling_mult) {
                    Double_t tm1 = theRecord.falling_tm[lix] * timeUnit;
                    if (debugFlag > 0)
                        Warning("execute", "JJJJ current hit falling: tm1:%e, apply time shift:%e",
                                tm1, fTimeShift);
                    tm1 +=fTimeShift;


#ifdef EMC_USE_TIME_WINDOW
                    if ((tm1 < tmin) || (tm1 > tmax)) {
                        if (debugFlag > 0)
                            Warning("execute",
                                    "JJJJ Rejecting trailing hit outside tmin:%e or tmax:%e",
                                    tmin, tmax);

#ifdef EMC_USE_LEADING_WITHOUT_TRAILING_EDGES
                        tm1=0.0; // still we take incomplete leading edge
#else
                        continue; // skip both leading and trailing
#endif
                    } // if ((tm1 < tmin) || (tm1 > tmax))


#endif
                    //
                    // TODO: optional filter out hits that do not fulfill the time threshold parameter
                    //Double_t tot=tm1-tm0;
                    // however, this can be also done in subsequent calibrator object

/////// ACCEPTED HITS ARE HERE //////////////////////////////////
                    // regular case with leading/trailing edges of ChannelRec in order
                    if (addRawHit(tm0, tm1, fastchannel) != 0)
                        continue; // we will skip this if this channel is not mapped to detector cell

////////// END ACCEPTED HITS/////////////////////////////////////
                } // if lix<theRecord.falling_mul
                else {
#ifdef EMC_USE_LEADING_WITHOUT_TRAILING_EDGES
                    // here we treat hist with only leading edges, no corresponding trailing edge
                    // such hit will deliver tot width of -1
                    if(addRawHit(tm0, 0.0, fastchannel)!=0)
                    continue;
#endif
                }
            } // for lix

////////////// optional now check for single trailing edges?
#ifdef EMC_USE_TRAILING_WITHOUT_LEADING_EDGES

            for (UInt_t tix=lix; tix< theRecord.falling_mult; ++tix)
            {
                Double_t tm1=theRecord.falling_tm[tix] * timeUnit;

#ifdef EMC_USE_TIME_WINDOW
                    if ((tm1 < tmin) || (tm1 > tmax)) {

                        if (debugFlag > 0)
                            Warning("execute",
                                    "JJJJ Rejecting trailing hit outside tmin:%e or tmax:%e",
                                    tmin, tmax);
			continue;
#endif

                if(addRawHit(0.0, tm1, fastchannel)!=0)
		    continue;
            }
             ////////////////////
#endif
/////////////// end single trailing edges


        } // loop over TDC channels
    } // loop over TDC
    return 1;
}

Int_t HEmcTrb3Unpacker::addRawHit(Double_t t_leading, Double_t t_trailing,
        Bool_t isfastchannel) {

    // from here we got access to TDC channel and should map it to EMC channel
    HEmcRaw* raw = (HEmcRaw*) pRawCat->getObject(fLoc);
    if (!raw) {
        raw = (HEmcRaw *) pRawCat->getSlot(fLoc);
        if (raw) {
	    raw = new (raw) HEmcRaw;
	    Char_t row,col;
	    HEmcDetector::getRowCol(fLoc[1],row,col);
	    raw->setAddress(fLoc[0], fLoc[1],row,col);
        } else {
            if (debugFlag > 0)
                Warning("addRawHit()", "Can't get slot mod=%i, chan=%i",
                        fLoc[0], fLoc[1]);
            return -1;
        }
    }

    isfastchannel ?
            raw->addFastHit(t_leading, t_trailing) :
            raw->addSlowHit(t_leading, t_trailing);
    if (debugFlag > 1)
        Warning("addRawHit",
                "JJJJ ADDING %s hit for mod:%d chan:%d tm0:%e tm1:%e",
                (isfastchannel ? "fast" : "slow"), fLoc[0], fLoc[1], t_leading,
                t_trailing);
    return 0;
}
