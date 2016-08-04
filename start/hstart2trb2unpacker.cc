//*-- AUTHOR : Jerzy Pietraszko, Ilse Koenig
//*-- Created : 04/12/2009

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HStartTrb2Unpacker
//
//  Class for unpacking TRB data and filling
//    the Start2 Raw category
//
//  Basic subevent decoding and eventual TDC corrections are
//  performed by decode function in the base class
//  see: /base/datasource/htrb2unpacker.h
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
#include "hstart2trb2lookup.h"
#include "hstart2trb2unpacker.h"
#include "hstartdef.h"
#include "htrb2correction.h"
#include "htrbnetaddressmapping.h"
#include "htrbnetunpacker.h"

#include <iostream>

using namespace std;

ClassImp(HStart2Trb2Unpacker)

HStart2Trb2Unpacker::HStart2Trb2Unpacker(UInt_t id): HTrb2Unpacker(id)
{
   // constructor
   pRawCat = NULL;
   timeRef = kTRUE;
}

Bool_t HStart2Trb2Unpacker::init(void)
{
   // creates the raw category and gets the pointer to the TRB2 lookup table

   HStart2Detector* det = (HStart2Detector*)gHades->getSetup()->getDetector("Start");
   if (!det) {
      Error("init", "No Start Detector found.");
      return kFALSE;
   }
   pRawCat = det->buildCategory(catStart2Raw);
   loc.set(2, 0, 0);

   lookup = (HStart2Trb2Lookup*)(gHades->getRuntimeDb()->getContainer("Start2Trb2Lookup"));
   if (!lookup) {
      Error("init", "No Pointer to parameter container Start2Trb2Lookup.");
      return kFALSE;
   }

   // control histograms ??

   if (correctINL) {
      trbaddressmap = (HTrbnetAddressMapping*)(gHades->getRuntimeDb()->getContainer("TrbnetAddressMapping"));
      if (!trbaddressmap) {
         Error("init", "No Pointer to parameter container TrbnetAddressMapping.");
         return kFALSE;
      }
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

Bool_t HStart2Trb2Unpacker::finalize(void)
{
   // fills the control histograms
   if (!quietMode && debugFlag > 0) {
      Info("finalize", "No control histograms implemented");
   }
   return kTRUE;
}

Int_t HStart2Trb2Unpacker::execute(void)
{

   HStart2Raw *pRaw = 0;  // pointer to Raw category
   Int_t nEvt = 0;        // Evt Seq. Nb.
   if (gHades->isCalibration()) {
      //calibration event
      return 1;
   }

   if (gHades->getCurrentEvent()->getHeader()->getId() == 0xe) {
      //scaler event
      return 1;
   }


   // decode subevent data
   if (pSubEvt) {        // pSubEvt - make sure that there is something for decoding
      nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

      // decodes the subevent and fill arrays, see: htrb2unpacker.h
      // decode() return values:
      //    100  - end of subevent, stop decoding by purpose
      //     10  - CTS information -> go to next subsubevt
      //      0  - error, decoding failed
      //      1  - data  decode
      // decode all subsubevents until subevent is finished - even if there is an error
      Int_t DecodeOutput = 1;
      while (1) {
         DecodeOutput = decode();
         // go to next TRB
         uStartPosition = uStartPosition + uSubBlockSize + 1; //start TRB decoding at the correct startposition
         if (DecodeOutput == 0) { //stop decoding of current TRB and go to next
            Error("decode", "subsubevent decoding failed!!! Evt Nr : %i SubEvtId: %x", nEvt, subEvtId);
            break;
         }
         if (DecodeOutput == 10) { // CTS information -> do not fill structure
            if (debugFlag > 0) Info("decode", "CTS info -> DecodeOutput : %i", DecodeOutput);
            continue;
         }
         if (DecodeOutput == 100) { //End of subevebt is reached
            uStartPosition = 0;
            if (debugFlag > 0) Info("decode", "DecodeOutput : %i", DecodeOutput);
            break;
         }
         // if(!decode()) return(-1); <--- for old data structure

         // correct for reference time here!
         // high resolution mode (8th channel)
         // low resolution mode (32th channel)
         if (timeRef) {
            // if (highResModeOn) correctRefTimeCh(8);
            if (highResModeOn) correctRefTimeStartDet23();
            else correctRefTimeCh(32);
         }

         // check the lookup table
         HStart2Trb2LookupBoard *board = lookup->getBoard(uTrbNetAdress);
         if (!board) {
            Warning("execute", "Evt Nr : %i  TrbNetAdress: %x (%i) unpacked but TRB Board not in lookup table", nEvt, uTrbNetAdress, uTrbNetAdress);
	    //return 1;
            continue;
         }

         // fill the raw category for Start detector
         for (Int_t i = 0; i < board->getSize(); i++) {
            if (trbLeadingMult[i] < 1) continue;         //Leading Time data for this channel exist
            // MAYBE: && trbTrailingMult[i] < 1
            // I would like to take also trailing data... f.i. debugging, if signal is inverted
            // But this I wont get from the trbunpacker? (just a guess)
            HStart2Trb2LookupChan *chan = board->getChannel(i);
            // if-cond for "chan belongs to start detector" ????
            chan->getAddress(loc[0], loc[1]);
            if (loc[0] >= 0) {
               pRaw = (HStart2Raw*)pRawCat->getObject(loc);
               if (!pRaw) {
                  pRaw = (HStart2Raw *)pRawCat->getSlot(loc);
                  if (pRaw) {
                     pRaw = new(pRaw) HStart2Raw;
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
               Int_t m;
               m = trbLeadingMult[i];
               if (m > 10) {
                  m = 10; // no more than 10 Times stored
               }

               for (Int_t chmult = 0; chmult < m; chmult++) {
                  if (debugFlag > 0) {
                     cout << "--filling data" << " time: " << trbLeadingTime[i][chmult] << endl;
                  }
                  pRaw->setTimeAndWidth(trbLeadingTime[i][chmult], trbADC[i][chmult]);
                  if (debugFlag > 0 && chmult < 4) {
                     cout << "--get data from raw" << " time: " << pRaw->getTime(chmult + 1) << endl;
                  }
               }
            } // if loc[0]>0
         } // loop over channels
      } // while loop
   }  //if(pSubEvt)
   return 1;
}

