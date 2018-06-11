//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HStart2Trb3Unpacker
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
#include "hstart2detector.h"
#include "hstart2raw.h"
#include "hstart2trb3lookup.h"
#include "hstart2trb3calpar.h"
#include "hstart2trb3unpacker.h"
#include "htrbnetunpacker.h"

#include <iostream>


using namespace std;

ClassImp(HStart2Trb3Unpacker)

HStart2Trb3Unpacker::HStart2Trb3Unpacker(UInt_t id) : HTrb3TdcUnpacker(id) {
  // constructor
  pRawCat = NULL;
  timeRef = kTRUE;
  lookup  = NULL;
}

Bool_t HStart2Trb3Unpacker::init(void) {
  // creates the raw category and gets the pointer to the TRB3 lookup table

   HStart2Detector* det = (HStart2Detector*)gHades->getSetup()->getDetector("Start");
   if (!det) {
      Error("init", "No Start Detector found.");
      return kFALSE;
   }
  pRawCat = det->buildCategory(catStart2Raw);
  if (!pRawCat) return kFALSE;

  loc.set(2, 0, 0);

  lookup = (HStart2Trb3Lookup*)(gHades->getRuntimeDb()->getContainer("Start2Trb3Lookup"));
  if (!lookup) {
     Error("init", "No Pointer to parameter container Start2Trb3Lookup.");
     return kFALSE;
  }

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

Bool_t HStart2Trb3Unpacker::reinit(void)
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
	    Int_t numTDC = lookup->getSize();
	    Int_t offset = lookup->getArrayOffset() ;

	    for (Int_t slot = 0; slot < numTDC; ++slot) {
		Int_t trbnetaddress = offset+slot;
		HStart2Trb3LookupTdc* tdc = lookup->getTdc(trbnetaddress);
		if(tdc){
		    Int_t nChan = tdc->getSize();
		    if (trbnetaddress) {
			Int_t tindex = addTDC(trbnetaddress,nChan);
			Info("reinit", "Added TDC 0x%04x with %d channels from HStart2Trb3Lookup to map index %d",
			     trbnetaddress, nChan,tindex);
		    }
		}
	    }

	    // set the expected range for the automatic adding of TDC structures:
	    setMinAddress(lookup->getArrayOffset());
	    setMaxAddress(lookup->getArrayOffset() + lookup->getSize());
	    fUseTDCFromLookup = kTRUE; // do not use auto register if set in
	} else {
            Info("reinit", "TDCs will be added in auto register mode between min address 0x%04x and max address 0x%04x!",fMinAddress,fMaxAddress);
	}
    }
    // we do not call reinit of superclass, since we do not use tdc calibration parameters in hydra anymore!
    return kTRUE;
}

Int_t HStart2Trb3Unpacker::execute(void) {

  HStart2Raw *pRaw = 0;  // pointer to Raw category
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
  if (!pSubEvt) return 1;

  Int_t nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

  // decodes the subevent and fill arrays, see: htrb3unpacker.h
  if (!decode()) {
     Error("decode", "subsubevent decoding failed!!! Evt Nr : %i SubEvtId: %x", nEvt, subEvtId);
     return -1;
  }

  // correct for reference time here!
  // we don't check for proper time reference correction
  // to avoid event skipping
  // this must be done better in future but now is a good workaround

  if (timeRef) {
     correctRefTimeCh(0);
  }

  for (UInt_t ntdc=0;ntdc<numTDC();ntdc++) {
     TDC* tdc = getTDC(ntdc);

    // check the lookup table
    HStart2Trb3LookupTdc *board = lookup->getTdc(tdc->getTrbAddr());

    if (!board) {
       if (debugFlag > 0) Warning("execute", "Evt Nr : %i  SubEvId: %x (%i) unpacked but TDC Board 0x%x not in lookup table",
	                          nEvt, getSubEvtId(), getSubEvtId(), tdc->getTrbAddr());
       continue;
    }

    // fill the raw category for Start detector
    for (UInt_t i = 1; i < tdc->numChannels()-1; i++) {

       // first check that tdc channel exists in lookup table at all
       HStart2Trb3LookupChan *chan = board->getChannel(i);
       if (chan==0) continue;

       // than check that address in lookup-table is not empty
       chan->getAddress(loc[0], loc[1]);
       if (loc[0] < 0) continue;
       HTrb3TdcUnpacker::ChannelRec& rec0 = tdc->getCh(i);
       // ignore channels without rising hits
       if (rec0.rising_mult < 1) continue;

       HTrb3TdcUnpacker::ChannelRec& rec1 = tdc->getCh(i+1);

       // from here we got access to TDC channel and should map it to Start detector cell
       pRaw = (HStart2Raw*)pRawCat->getObject(loc);
       if (!pRaw) {
          pRaw = (HStart2Raw *)pRawCat->getSlot(loc);
          if (pRaw) {
             pRaw = new(pRaw) HStart2Raw;
             pRaw->setAddress(loc[0], loc[1]);
          } else {
             Error("execute()", "Can't get slot mod=%i, cell=%i", loc[0], loc[1]);
             return -1;
          }
       } else {
          Error("execute", "Slot already exists for mod=%i, cell=%i", loc[0], loc[1]);
          return -1;
       }
       // fill ...
       for (UInt_t chmult = 0; chmult < rec0.rising_mult; chmult++) {

          Double_t tm0 = rec0.rising_tm[chmult]*1e9;

          if (debugFlag > 0)
             printf("--filling data TDC:%04x ch:%2d loc0:%2d loc1:%2d xtime: %6.3f\n",
                   tdc->getTrbAddr(), i, loc[0], loc[1], tm0);

          Double_t adc = -1000;

          for (UInt_t n1=0; n1<rec1.rising_mult; n1++) {
             Double_t tm1 = rec1.rising_tm[n1]*1e9;
             if (tm1<=tm0) continue;
             if ((adc<0.) || (adc>tm1-tm0)) adc = tm1-tm0;
          }

          pRaw->setTimeAndWidth(tm0, adc);
      }
    } // loop over TDC channels
  } // loop over TDC
  return 1;
}

