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

HEmcTrb3Unpacker::HEmcTrb3Unpacker(UInt_t id): HTrb3Unpacker(id) {
  // constructor
  pRawCat = NULL;
  timeRef = kTRUE;
}

Bool_t HEmcTrb3Unpacker::init(void) {
  // creates the raw category and gets the pointer to the TRB3 lookup table

  HEmcDetector* det = (HEmcDetector*)gHades->getSetup()->getDetector("Emc");
  if (!det) {
    Error("init", "No EMC Detector found.");
    return kFALSE;
  }
  pRawCat = det->buildCategory(catEmcRaw);
  if (!pRawCat) return kFALSE;

  loc.set(2, 0, 0);

  lookup = (HEmcTrb3Lookup*)(gHades->getRuntimeDb()->getContainer("EmcTrb3Lookup"));
  if (!lookup) {
    Error("init", "No Pointer to parameter container EmcTrb3Lookup.");
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
    Error("init()", "Failed to initialize HTrbNetUnpacker!");
    return kFALSE;
  }

  return kTRUE;
}

Int_t HEmcTrb3Unpacker::execute(void) {
  HEmcRaw *pRaw = 0;  // pointer to Raw category
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
  if (timeRef) {
     for (unsigned n=0;n<numTDC();n++) {
        if (!getTDC(n)->correctRefTimeCh(0)) {
           Error("decode", "time-correction for tdc %04x failed!!! Evt Nr : %i SubEvtId: %x", (unsigned) getTDC(n)->getTrbAddr(), nEvt, subEvtId);
           return -1;
        }
     }
  }

  for (unsigned ntdc=0;ntdc<numTDC();ntdc++) {
     HTrb3TdcUnpacker* tdc = getTDC(ntdc);

    // check the lookup table
    HEmcTrb3LookupBoard *board = lookup->getBoard(tdc->getTrbAddr());
    if (!board) {
       Warning("execute", "Evt Nr : %i  SubEvId: %x (%i) unpacked but TDC Board not in lookup table",
                         nEvt, getSubEvtId(), getSubEvtId());
        return 1;
    }

    // fill the raw category for EMC detector
    for (UInt_t i = 1; i < tdc->numChannels(); i++) {

        HTrb3TdcUnpacker::ChannelRec& rec = tdc->getCh(i);

        // ignore channels without rising hits
        if (rec.rising_mult < 1) continue;

        HEmcTrb3LookupChan *chan = board->getChannel(i);

        if (chan==0) {
           Warning("execute", "Missing channel %u in lookup table", i); 
           continue;
        }

        chan->getAddress(loc[0], loc[1]);
        // if channel not exist in lookup-table, ignore it
        if (loc[0] < 0) continue;

        // from here we got access to TDC channel and should map it to EMC channel
        pRaw = (HEmcRaw*)pRawCat->getObject(loc);
        if (!pRaw) {
           pRaw = (HEmcRaw *)pRawCat->getSlot(loc);
           if (pRaw) {
              pRaw = new(pRaw) HEmcRaw;
              pRaw->setAddress(loc[0], loc[1]);
           } else {
              Error("execute()", "Can't get slot mod=%i, chan=%i", loc[0], loc[1]);
              return -1;
           }
        } else {
           Error("execute()", "Slot already exists for mod=%i, chan=%i", loc[0], loc[1]);
           return -1;
        }
        // fill ...
        Int_t m = rec.rising_mult;
        if (m > 10) m = 10; // no more than 10 times stored

        for (Int_t chmult = 0; chmult < m; chmult++) {
           if (debugFlag > 0)
              printf("--filling data TDC:%04x ch:%2d loc0:%2d loc1:%2d xtime: %10.8f\n",
                    tdc->getTrbAddr(), i, loc[0], loc[1], rec.rising_tm[chmult]);

           pRaw->setTimeAndWidth(rec.rising_tm[chmult], 123 /*trbADC[i][chmult]*/);
       }
     } // loop over TDC channels
  } // loop over TDC
  return 1;
}

