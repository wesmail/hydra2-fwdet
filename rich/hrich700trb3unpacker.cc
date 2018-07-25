//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HRich700Trb3Unpacker
//
//  Class for unpacking TRB3 data and filling
//    the Start2 Raw category
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
#include "hrichdetector.h"
#include "hrich700raw.h"
#include "hrichcal.h"

#include "hrich700trb3unpacker.h"
#include "hrich700trb3lookup.h"
#include "hrich700thresholdpar.h"
#include "hrich700digipar.h"



#include "htrbnetunpacker.h"
#include "richdef.h"


#include <iostream>


using namespace std;

ClassImp(HRich700Trb3Unpacker)


// time over threshold cut, later as anaylsis parameter?
#define RICH700_MAX_TOT 			100

//#define RICH700_NO_TIME_CUT 1


Bool_t HRich700Trb3Unpacker::fHasPrintedTDC = kFALSE;

HRich700Trb3Unpacker::HRich700Trb3Unpacker(UInt_t id) : HTrb3TdcUnpacker(id) {
    // constructor

    setHUBId(0x9000); // TODO: how to configure the hub and cts ids here?
    setCTSId(0xC001);
    fCalCat=NULL;
    pRawCat = NULL;
    fTimeRef = kTRUE;
    fLookup  = NULL;
    fThresholds = NULL;
}

Bool_t HRich700Trb3Unpacker::init(void)
{
    // creates the raw category and gets the pointer to the TRB3 lookup table

    HRichDetector* det = (HRichDetector*)gHades->getSetup()->getDetector("Rich");
    if (!det) {
	Error("init", "No Rich Detector found.");
	return kFALSE;
    }

    pRawCat = gHades->getCurrentEvent()->getCategory(catRich700Raw);
    if (NULL == pRawCat) {
	pRawCat = det->buildCategory(catRich700Raw);
	if (NULL == pRawCat) {
	    Error("init", "Pointer to HRich700Raw category is NULL");
	    return kFALSE;
	} else {
	    gHades->getCurrentEvent()->addCategory(catRich700Raw, pRawCat,"Rich");
	}
    }

    fCalCat = gHades->getCurrentEvent()->getCategory(catRichCal);
    if (NULL == fCalCat) {
	fCalCat = det->buildCategory(catRichCal);
	if (NULL == fCalCat) {
	    Error("init", "Pointer to HRichCal category is NULL");
	    return kFALSE;
	} else {
	    gHades->getCurrentEvent()->addCategory(catRichCal, fCalCat, "Rich");
	}
    }

    fRawLoc.set(2, 0, 0);
    fCalLoc.set(3, 0, 0,0);

    //
    fLookup = (HRich700Trb3Lookup*)(gHades->getRuntimeDb()->getContainer("Rich700Trb3Lookup"));
    if (!fLookup) {
	Error("init", "No Pointer to parameter container Rich700Trb3Lookup.");
	return kFALSE;
    }



    // here second parameter to supress hits outside time and tot cuts per channel!
    fThresholds = (HRich700ThresholdPar*)(gHades->getRuntimeDb()->getContainer("Rich700ThresholdPar"));
    if (!fThresholds) {
	Error("init", "No Pointer to parameter container Rich700ThresholdPar.");
	return kFALSE;
    }

    fDigiPar = (HRich700DigiPar*)(gHades->getRuntimeDb()->getContainer("Rich700DigiPar"));
    if (!fDigiPar) {
	Error("init", "No Pointer to parameter container Rich700DigiPar.");
	return kFALSE;
    }
    ////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////



    if (NULL == trbNetUnpacker) {
	if (gHades->getDataSource()) {
	    HDataSource* source = gHades->getDataSource();
	    if (source->InheritsFrom("HldSource")) {
		trbNetUnpacker = ((HldSource *)gHades->getDataSource())->getTrbNetUnpacker();
	    } else {
		Warning("init", "DataSource not inherited from HldSource! trbNetUnpacker == 0 ");
	    }
	} else {
	    Warning("init", "Could not retrieve DataSource! trbNetUnpacker == 0 ");
	}
    }
    if (!trbNetUnpacker->init()) {
	Error("init", "Failed to initialize HTrbNetUnpacker!");
	return kFALSE;
    }


    return kTRUE;
}

Bool_t HRich700Trb3Unpacker::reinit(void) {
    // Called in the beginning of each run, used here to initialize TDC unpacker
    // if TDCs processors are not yet created, use parameter to instantiate them
    // if auto register TDC feature is used in setup of unpackers (by setting
    // setMinAddress()+setMaxAddress() manually)

    if (numTDC() == 0) // configure the tdcs only the first time we come here:
    {
	if (fMinAddress == 0 && fMaxAddress == 0)
	{
	    // here evaluate which subevents are configured in lookup table:
	    Int_t numslots = fLookup->getSize();
	    fLookup->resetTdcIterator();
	    for (Int_t slot = 0; slot < numslots; ++slot) {
		Int_t trbnetaddress = fLookup->getNextTdcAddress();
		if (trbnetaddress) {
		    Int_t tindex=addTDC(trbnetaddress, RICH700_MAX_TDCCHANNELS+1);
		    if(!fHasPrintedTDC) Info("reinit", "Added TDC 0x%04x with %d channels from HRich700Trb3Lookup to map index %d",
					     trbnetaddress, RICH700_MAX_TDCCHANNELS+1,tindex);
		}
	    }

	    // here set the expected range for the automatic adding of TDC structures:
	    setMinAddress(fLookup->getArrayOffset());
	    setMaxAddress(fLookup->getArrayOffset() + fLookup->getSize());
	    nCalSrc=3; // just in case this parameter will be used someday
	    fUseTDCFromLookup = kTRUE; // do not use auto register if set in
	    fHasPrintedTDC = kTRUE;
	} else {
	    Info("reinit", "TDCs will be added in auto register mode between min address 0x%04x and max address 0x%04x!",fMinAddress,fMaxAddress);
	}
    }
    // we do not call reinit of superclass, since we do not use tdc calibration parameters in hydra anymore!
    return kTRUE;
}


Int_t HRich700Trb3Unpacker::execute(void) {

    if (gHades->isCalibration()) {
	//calibration event
	return 1;
    }
    if (gHades->getCurrentEvent()->getHeader()->getId() == 0xe) {
	//scaler event
	return 1;
    }

    //if (debugFlag > 0) Warning("execute", "JJJJJ no calibration and no header event...");

    // if there is no data, do not try to analyze it
    // pSubEvt - make sure that there is something for decoding
    if (!pSubEvt) return 1;

    //if (debugFlag > 0) Warning("execute", "JJJJJ has subevent...");

    Int_t nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

    // decodes the subevent and fill arrays, see: htrb3unpacker.h
    if (!decode()) {
	Error("decode", "subsubevent decoding failed!!! Evt Nr : %i SubEvtId: %x", nEvt, subEvtId);
	return -1;
    }


    // correct for reference time here!
    if (fTimeRef) {

#ifdef USE_FILLED_TDC
	for (UInt_t n=0;n<numFilledTDC();n++) {
            HTrb3TdcUnpacker::TDC* tdc = getFilledTDC(n);
#else
	for (UInt_t n=0;n<numActiveTDC();n++) {
            HTrb3TdcUnpacker::TDC* tdc = getActiveTDC(n);
#endif
	    if (debugFlag > 0) Warning("execute", "JJJ try to correct reftime for tdc %04x, tdcindex:%d  Evt Nr : %i SubEvtId: %x",
				       (UInt_t) tdc->getTrbAddr(), n, nEvt, subEvtId);
	    // JAM todo: may configure other reference channel than 0 here?
	    if (!tdc->correctRefTimeCh(REFCHAN)) {
		if (debugFlag > 0) Warning("execute", "time-correction for tdc %04x failed!!! Evt Nr : %i SubEvtId: %x",
					   (UInt_t) tdc->getTrbAddr(), nEvt, subEvtId);
	    }
	}
    }

    if (debugFlag > 0) Warning("execute", "JJJJJ has cleared hits.");
    // now loop over all known tdcs

#ifdef USE_FILLED_TDC
    for (UInt_t ntdc=0;ntdc<numFilledTDC();ntdc++) {
	HTrb3TdcUnpacker::TDC* tdc = getFilledTDC(ntdc);
#else
    for (UInt_t ntdc=0;ntdc<numActiveTDC();ntdc++) {
	HTrb3TdcUnpacker::TDC* tdc = getActiveTDC(ntdc);
#endif

	// check the fLookup table
	HRich700Trb3LookupTdc *board = fLookup->getTdc(tdc->getTrbAddr());

	if (!board) {
	    Warning("execute", "Evt Nr : %i  SubEvId: %x (%i) unpacked but TDC Board 0x%x not in lookup table",
		    nEvt, getSubEvtId(), getSubEvtId(), tdc->getTrbAddr());
	    continue;
	}

	if (debugFlag > 0) Warning("execute", "JJJJJ Evt Nr : %i  SubEvId: %x (%i) unpacking TDC Board 0x%x...",
				   nEvt, getSubEvtId(), getSubEvtId(), tdc->getTrbAddr());

	// fill the raw category for RICH700 detector
#ifdef USE_FILLED_TDC
	for (UInt_t j = 0; j < tdc->chanList.size(); j++) {
            UInt_t i = tdc->chanList[j];
	    HTrb3TdcUnpacker::ChannelRec& theRecord = tdc->getCh(i);
#else
	for (UInt_t i = 0; i < tdc->numChannels(); i++) {
	    if(REFCHAN == i) continue;
	    HTrb3TdcUnpacker::ChannelRec& theRecord = tdc->getCh(i);
	    if(!theRecord.hasData) continue;
#endif

	    // first check that tdc channel exists in lookup table at all
	    HRich700Trb3LookupChan *chan = board->getChannel(i);
	    if (chan==0) {
		continue;
	    }


	    // than check that address in lookup-table is not empty
	    chan->getAddress(fRawLoc[0], fRawLoc[1]); // pmt, pixel
	    if (fRawLoc[0] < 0) {
		Warning("execute", "Skip  pmt=%i, pixel=%i", fRawLoc[0], fRawLoc[1] );
		continue;
	    }
	    // now the location identifier is ready. We use it to retrieve data element when we accept a hit.

	    if (debugFlag > 1) Warning("execute", "JJJJ Processing catRich700Raw Slot for  pmt=%i, pixel=%i", fRawLoc[0], fRawLoc[1] );


	    // JAM here get pixel thresholds from parameter container:
	    Double_t tmin=0, tmax=1.0e-6, totmin=1e-9, totmax=1e-8; // units here seconds like in the trb3 unpacker messages!
	    HRich700PixelThreshold* pthres =fThresholds->getObject(fRawLoc);
	    if(pthres)
	    {
		if(pthres->getFlag() !=0)
		{
		    if (debugFlag > 0) Warning("execute", "Skipping invalid pixel - pmt=%i, pixel=%ie",
					       fRawLoc[0], fRawLoc[1]);
		    continue;
		}

		tmin=pthres->getT_Min(); // use  units ns from parameter container.
		tmax=pthres->getT_Max();
		totmin=pthres->getTot_Min();
		totmax=pthres->getTot_Max();
	    }
	    else
	    {
		Error("execute()", "Can't get threshold parameter for  pmt=%i, pixel=%i, using precompileddefaults!", fRawLoc[0], fRawLoc[1]);
	    }



	    // JAM preliminary unpacker ----------
	    // todo: dynamically find out corresponding leading/trailing edges in map?
	    // todo: handling of single leading/trailing edges?
	    if (debugFlag > 1) Warning("execute", "JJJJ Record of channel %d has multiplicities -  rising:%d falling:%d \n",
				       i, (Int_t)theRecord.rising_mult, (Int_t) theRecord.falling_mult);

	    Int_t loc[3] = {-1,-1,-1};
	    fDigiPar->pmtIdPixelToColRowSec(fRawLoc[0],fRawLoc[1],loc[0],loc[1],loc[2],kFALSE);
	    if(loc[0]<0) continue;

	    UInt_t lix=0;
	    for(lix=0; lix<theRecord.rising_mult &&  lix<theRecord.falling_mult; ++lix)
	    {



		Double_t tm0 = theRecord.rising_tm[lix]*1e9;
		if (debugFlag > 0) Warning("execute", "JJJJ current hit leading: tm0:%e", tm0);
#ifndef RICH700_NO_TIME_CUT
		if((tm0<tmin) || (tm0>tmax)) {

		    if (debugFlag > 0) Warning("execute", "JJJJ Rejecting leading hit outside tmin:%e or tmax:%e", tmin, tmax);
		    continue; // leading edge outside the time window
		}
#endif
		// now look for corresponding trailing edge:
		Double_t tm1 = -1.0;
		if(lix<theRecord.falling_mult)
		    tm1=theRecord.falling_tm[lix]*1e9;
		if (debugFlag > 0) Warning("execute", "JJJJ current hit falling: tm1:%e", tm1);
#ifndef RICH700_NO_TIME_CUT
		if((tm1>=tmin) && (tm1 <= tmax)) // trailing edge also inside time window
#endif
		{
		    // TODO: here filter out hits that do not fulfill the time threshold parameter


		    Double_t tot=tm1-tm0;
#ifndef RICH700_NO_TIME_CUT
		    if((tot>=totmin) && (tot<totmax))
#endif
		    {
			/////// ACCEPTED HITS ARE HERE //////////////////////////////////
			//

			// regular case with leading/trailing edges of ChannelRec in order
			// note that flag 0b11 is consistent with test beam monitoring code
			if(addRawHit(tm0,tm1,tot,loc[0],loc[1],loc[2],0b11)!=0)  continue;

			// JAM2018 to do: fill cal category with hit information, probably later evaluate charge from tot or something
			// for the moment each hit with leading and trailing edge within time windows adds charge value 1.0
			if(addCalHitCharge(loc[0],loc[1],loc[2],1.0)!=0)         continue;

			////////// END ACCEPTED HITS
		    }



		    ////////////////////////////////////////////////////////////
		    // do we want to keep hits that are not within tot condition?
		    else
		    {
			if (debugFlag > 0) Warning("execute", "JJJJ Rejecting ToT hit outside totmin:%e or totmax:%e",
						   totmin, totmax);
			//    			   addRawHit(tm0,-1,-1,0b10);
			//    			   addRawHit(-1,tm1,-1,0b01);
		    }
		    ///////////////////////////////////////////////////////
		} // trailing edge inside

		// do we want to keep hits that are not within tot condition?
		else
		{
		    if (debugFlag > 0) Warning("execute", "JJJJ Rejecting trailing hit outside tmin:%e or tmax:%e",
					       tmin, tmax);

		    //    		   // no corresponding trailing edge found
		    //    		   addRawHit(tm0,-1,-1,0b10);
		}


	    } // for lix

	    ////////////// optional?
	    //       // now check for single trailing edges?
	    //       for (UInt_t tix=lix; tix< theRecord.falling_mult; ++tix)
	    //       {
	    //    	   Double_t tm1=theRecord.falling_tm[tix]*1e9;
	    //    	   addRawHit((-1,tm1,-1,0b01);
	    //       }
	    //////////////////////


	} // loop over TDC channels
    } // loop over TDC


    // TODO: ??? for more elaborate treatment we may loop here again over the rich700raw category and convert any hits to the standard cal category.
    // there is no other task involved, so trb3 unpacker has to do this
    // we still keep rich700raw category in framework for monitoring purposes!
    // currently, we fill cal category together with raw category and



    return 1; // kTRUE
}



//////////////////// Some helper functions below: /////////////////////////////



Int_t HRich700Trb3Unpacker::addRawHit(Double_t t_leading, Double_t t_trailing,
				      Double_t tot,Int_t sec,Int_t col,Int_t row,UInt_t flag) {

    HRich700Raw* raw =  static_cast<HRich700Raw*> (pRawCat->getObject(fRawLoc));
    if (!raw) {
	raw = (HRich700Raw *) pRawCat->getSlot(fRawLoc);
	if (raw) {
	    raw = new (raw) HRich700Raw;
	    raw->setAddress(fRawLoc[0], fRawLoc[1],sec,col,row);
	} else {

	    if (debugFlag > 0)
		Warning("addRawHit()", "Can't get slot pmt=%i, pixel=%i",
			fRawLoc[0], fRawLoc[1]);
	    return -1;
	}
    }
    raw->addHit(t_leading, t_trailing, tot, 0b11);
    if (debugFlag > 1)
	Warning("addRawHit",
		"JJJJ ADDING hit for pmt:%d pixel:%d tm0:%e tm1:%e tot:%e",
		fRawLoc[0], fRawLoc[1], t_leading, t_trailing, tot);
    return 0;
}


Int_t HRich700Trb3Unpacker::addCalHitCharge(Int_t sector, Int_t col, Int_t row,
					    Float_t charge) {
    fCalLoc[0] = sector;
    fCalLoc[1] = row;
    fCalLoc[2] = col;

    // from old rich unpacker:
    //	  loc.setOffset(col);
    //	  loc.setIndex(1, row);
    //	  loc.setIndex(0, fDataWord.sector);
    //////////////


    if (debugFlag > 1)
	Warning("addCalHitCharge",
		"JJJJ ADDING cal hit for sector:%d row:%d col:%d ",
		fCalLoc[0], fCalLoc[1],  fCalLoc[2]);


    HRichCal* cal = static_cast<HRichCal*>(fCalCat->getObject(fCalLoc));
    if (NULL == cal) {
	cal = static_cast<HRichCal*>(fCalCat->getSlot(fCalLoc));
	if (NULL != cal) {
	    // fill empty slot with new data instance:
	    cal = new (cal) HRichCal;
	    cal->setEventNr(gHades->getCurrentEvent()->getHeader()->getEventSeqNumber());
	    cal->setSector(fCalLoc[0]);
	    cal->setRow(fCalLoc[1]);
	    cal->setCol(fCalLoc[2]);
	} else {
	    if (debugFlag > 0)
		Warning("addCalHit()",
			"Can't get slot sector=%i, row=%i, col=%i", fCalLoc[0],
			fCalLoc[1], fCalLoc[2]);
	    return -1;
	}
    }
    // now increment the "charge" field by another hit:
    cal->addCharge(charge);
    return 0;
}



