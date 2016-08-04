//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Martin Jurkovic <martin.jurkovic@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichUnpacker
//
//
//////////////////////////////////////////////////////////////////////////////


#include "TRandom.h"

#include "hades.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "hdetector.h"
#include "hevent.h"
#include "heventheader.h"
#include "hldsource.h"
#include "hldsubevt.h"
#include "hlocation.h"
#include "hrichcal.h"
#include "hrichcalsim.h"
#include "hrichdetector.h"
#include "hrichmappingpar.h"
#include "hrichunpacker.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "richdef.h"

#include <iostream>

ClassImp(HRichUnpacker)

const UInt_t HRichUnpacker::kHubDebugId = 0x5555;
const UInt_t HRichUnpacker::kADCZero    = 0x2000;

HRichUnpacker::HRichUnpacker(Int_t richId,
                             Int_t strtEvt)
   : HldUnpack()
{

   fRichId          = richId;
   fStartEvt        = strtEvt;
   fSecMisMatchCntr = 0;
   fEventNr         = -1;

   fpCalCat         = NULL;
   fpMapPar         = NULL;

}

Bool_t HRichUnpacker::init(void)
{
// Initialize input parameter containers and build output categories

   HRichDetector* pRichDet = static_cast<HRichDetector*>(gHades->getSetup()->getDetector("Rich"));

   if (NULL == trbNetUnpacker) {
      if (gHades->getDataSource()) {
         HDataSource* source = gHades->getDataSource();
         if (source->InheritsFrom("HldSource")) {
            trbNetUnpacker = ((HldSource *)gHades->getDataSource())->getTrbNetUnpacker();
         } else {
            Warning("init", "DataSource not inherited from HldSource! trbNetUnpacker == 0 ");
         }
      } else {
         Warning("init", "Could not retrieve Datasource! trbNetUnpacker == 0 ");
      }
   }


// Output categories
   fpCalCat = gHades->getCurrentEvent()->getCategory(catRichCal);
   if (NULL == fpCalCat) {
      if (0 == gHades->getEmbeddingMode()) {
         fpCalCat = pRichDet->buildCategory(catRichCal);
      } else {
         fpCalCat = pRichDet->buildMatrixCat("HRichCalSim", 1);
      }
      if (NULL == fpCalCat) {
         Error("init", "Can not build HRichCal category.");
         return kFALSE;
      } else {
         gHades->getCurrentEvent()->addCategory(catRichCal, fpCalCat, "Rich");
      }
   }

// Input parameter containers
   HRuntimeDb* rtdb = gHades->getRuntimeDb();
   fpMapPar = static_cast<HRichMappingPar*>(rtdb->getContainer("RichMappingParameters"));

   return kTRUE;
}

Int_t HRichUnpacker::execute()
{

   UInt_t sourceSector = 10;
   Int_t  col          = 0;
   Int_t  row          = 0;
   DHeader dHeader;
   HLocation loc;
   loc.set(3, 0, 0, 0);
   fEventNr = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

   if (NULL == pSubEvt) {
      if (0 != gHades->getCurrentEvent()->getHeader()->getEventSeqNumber()) {
#if DEBUG_LEVEL > 0
         Warning("execute", "EvtId: 0x%x, Unpacker 0x%x ==> no data", gHades->getCurrentEvent()->getHeader()->getId(), fRichId);
#endif
      }
      return 1;
   }

   if (fEventNr >= fStartEvt) {

      // decode the header first
      for (UInt_t i = 0; i < pSubEvt->getDataLen(); i++) {
         // Check for pedding word first (skip)
         if (0xaaaaaaaa == pSubEvt->getData()[i]) {
            continue;
         }

         // decode subsub_event and its header
         dHeader.getEvent(pSubEvt->getData()[i]);
         if (0 == dHeader.getSize()) {
            // no data im subsubevent
            continue;
         }

         // check for debug info data type
         if (kHubDebugId == dHeader.getSource()) {
#if DEBUG_LEVEL > 2
            Info("execute", "Only debug info will follow, skiping.");
#endif
            decodeTrbNet((pSubEvt->getData() + i), fRichId);
            i += (dHeader.getSize());
            continue;
         }

         // After decoding and checking the header jump to the next data word
         i++;

         // Each data source (in this case ADC module) is uniquely assigned to a specific sector
         if ((dHeader.getSource() & 0xf) > 4 ||
             (dHeader.getSource() & 0xff00) != 0x3000) {
            Error("execute", "Wrong ADSM number: 0x%x", dHeader.getSource());
         }
         sourceSector = (dHeader.getSource() >> 4) & 0xf;

         // Read the sub-sub event
         for (UInt_t j = 0; j < dHeader.getSize(); j++, i++) {
            // Check for debug word (skip)
            if (0 != ((pSubEvt->getData()[i] >> 31) & 0x01)) {
#if DEBUG_LEVEL > 2
               Info("execute", "debug info %x", pSubEvt->getData()[i]);
#endif
               continue;
            }

            // Fill the data structure
            fDataWord.charge  = (pSubEvt->getData()[i] >> RICH_CHARGE_OFFSET)     & RICH_CHARGE_MASK;
            fDataWord.channel = (pSubEvt->getData()[i] >> RICH_CHANNEL_OFFSET)    & RICH_CHANNEL_MASK;
            fDataWord.apv     = (pSubEvt->getData()[i] >> RICH_APV_OFFSET)        & RICH_APV_MASK;
            fDataWord.adc     = (pSubEvt->getData()[i] >> RICH_ADC_OFFSET)        & RICH_ADC_MASK ;
            fDataWord.sector  = (pSubEvt->getData()[i] >> RICH_SECTOR_OFFSET)     & RICH_SECTOR_MASK;

            if (1 == fDataWord.channel % 2) {
               continue;
            }

            // Check if the HW address is in reasonable range
            if ((fDataWord.channel >= RICH_MAX_CHANNELS) ||
                (fDataWord.adc     >= RICH_MAX_ADCS)     ||
                (fDataWord.apv     >= RICH_MAX_APVS)     ||
                (fDataWord.sector  >= RICH_MAX_SECTORS)) {
               Error("execute", "Evt %i SubEvtId %x Source %x: Wrong address (sec,adc,apv,ch) = (%i,%i,%i,%i)",
                     fEventNr, fRichId, dHeader.getSource(), fDataWord.sector, fDataWord.adc, fDataWord.apv, fDataWord.channel);
               continue;
            }

            // Check sector consistency
            if (fDataWord.sector != sourceSector) {
               fSecMisMatchCntr++;
               Error("execute", "Evt %i SubEvtId %x Source %x: Unpacked sector %i differs from assigned sector %i (0x%x)",
                     fEventNr, fRichId, dHeader.getSource(), fDataWord.sector, sourceSector, pSubEvt->getData()[i]);
               continue;
            }

            // Check if the apv channel is connected to a pad and get SW coordinates (row,col).
            if (kFALSE == fpMapPar->getSWAddress(getAddress(fDataWord), row, col)) {
#if DEBUG_LEVEL > 4
               Error("execute", "Evt %i SubEvtId %x Source %x: addr: %x: APV channel (sec,adc,apv,ch) = (%i,%i,%i,%i) is unconnected.",
                     fEventNr, fRichId, dHeader.getSource(), getAddress(fDataWord), fDataWord.sector, fDataWord.adc, fDataWord.apv, fDataWord.channel);
               cerr << "printDataWord says:" << endl;
               printDataWord(fDataWord);
               cerr << "printMapping says:" << endl;
               printMapping(fDataWord);
#endif
               continue;
            }

            loc.setOffset(col);
            loc.setIndex(1, row);
            loc.setIndex(0, fDataWord.sector);

            // APV data are already calibrated. To the ADC value a random nb
            // [0;1] is added to account for the cut-off in the ADC
            Float_t charge = static_cast<Float_t>(fDataWord.charge - kADCZero) + gRandom->Rndm();
            HRichCal* pCell = NULL;
            if (charge > 0.0) {
               pCell = static_cast<HRichCal*>(fpCalCat->getSlot(loc));
               if (NULL != pCell) {
                  if (0 == gHades->getEmbeddingMode()) {
                     pCell = new(pCell) HRichCal(loc[0], loc[1], loc[2]);
                  } else {
                     pCell = new(pCell) HRichCalSim(loc[0], loc[1], loc[2]);
                  }
                  pCell->setCharge(charge);
                  pCell->setEventNr(fEventNr);
               } else {
                  Error("Execute", "getSlot(loc) returned NULL! (sec,row,col) = (%i,%i,%i), addr: 0x%x (%i,%i)", loc[0], loc[1], loc[2], getAddress(fDataWord), row, col);
               }
            }
         } // end of unpacking subsub event

         // rewind by one data word
         i--;

      } // end of unpacking sub event
   } // end of start event condition
   return 1;
}

Bool_t HRichUnpacker::finalize(void)
{
#if DEBUG_LEVEL > 1
   cerr << "SubEvtId:" << fRichId << " RICH: Number of evts processed: " << fEventNr  << endl;
   cerr << "SubEvtId:" << fRichId << " RICH: Sector mismatch errors:   " << fSecMisMatchCntr << endl;
#endif
   return kTRUE;
}

void HRichUnpacker::printDataWord(const DataWord & addr)
{
   cout << "*****************************************************************************" << endl;
   cout << "Event Nr: " << fEventNr << endl;
   cout << " SEC: "     << addr.sector
        << " ADC: "     << addr.adc
        << " APV: "     << addr.apv
        << " CH: "      << addr.channel
        << " CHRG: "    << addr.charge << endl;
}

void HRichUnpacker::printMapping(const DataWord & addr)
{
   Int_t row = -1;
   Int_t col = -1;

   cout << "Event Nr: "       << fEventNr << endl;
   cout << "isValidAddress: " << fpMapPar->getSWAddress(getAddress(addr), row, col) << endl;
   cout << "Col: "            << col << endl;
   cout << "Row: "            << row << endl;
   cout << "*****************************************************************************" << endl;
}


DHeader::DHeader()
{
   reset();
}

void DHeader::reset()
{
   fSize   = 0;
   fSource = 0;
}

void DHeader::getEvent(UInt_t word)
{
   fSize   = (word >> 16) & 0xffff;
   fSource = word & 0xffff;
}

void DHeader::dump()
{
   Info("dump", "%c size: 0x%04x  source: 0x%04x\n", '%', fSize, fSource);
}
