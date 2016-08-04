//*-- Author : Walter Karig
//*-- Modified : 08/07/2010 by M. Weber
//*-- Modified : 03/05/2005 by Ilse Koenig
//*-- Modified : 16/09/2003 by T.Wojcik
//*-- Modified : 24/11/2002 by Dusan Zovinec
//*-- Modified : 18/12/2001 by Ilse Koenig
//*-- Modified : 29/11/2000 by Ilse Koenig
//*-- Modified : 13/11/2000 by M. Golubeva
//*-- Modified : 05/06/98 by Manuel Sanchez


#include "hades.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "hdebug.h"
#include "hdetector.h"
#include "hevent.h"
#include "heventheader.h"
#include "hldsubevt.h"
#include "hspectrometer.h"
#include "htboxchan.h"
#include "htboxunpacker.h"

using namespace std;

ClassImp(HTBoxUnpacker)

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////
//
// HTBoxUnpacker
//
// Unpacker used to read Scalers data from .hld files
// Fills the TBox category.
//
//////////////////////////////////////////////////////////////////

HTBoxUnpacker::HTBoxUnpacker(Int_t id)
{
   fSubEvtId = id;
   fCat      = NULL;

   fLoc.set(1, 0);
}

Bool_t HTBoxUnpacker::init(void)
{

   // Creates the TBox category for the scalers, if not yet existing

   fCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catTBoxChan);
   if (NULL == fCat) {
      fCat = gHades->getSetup()->getDetector("TBox")->buildCategory(catTBoxChan);
      if (NULL != fCat) {
         gHades->getCurrentEvent()->addCategory(catTBoxChan, fCat, "TBox");
      } else {
         Error("init", "Can not build output category");
         return kFALSE;
      }
   }
   return kTRUE;
}

Int_t HTBoxUnpacker::execute()
{

   // Unpacks the scaler data and fills the TBox data category


#if DEBUG_LEVEL>4
   gDebuger->enterFunc("HTBoxUnpacker::execute\n");
#endif

   // scalers are written every second (decoupled from calibration
   //if (!(gHades->isCalibration())) {  // no calibration event, nothing to be done for scalers
   //  return 1;
   //}

   UInt_t* data       = 0;
   UInt_t* end        = 0;

   UInt_t uTrbNetAdress;        // TrbNetAdress
   UInt_t uSubBlockSize;        // BlockSize of SubSubEvent
   UInt_t uSubBlockSizeTest;    // BlockSize of SubSubEvent
   UInt_t uSubBlockSizeScalers; // BlockSize of Scaler words
   UInt_t trbExtensionSize;     // number of TRB extension words

   HTBoxChan* ptboxchan = NULL;

   if (pSubEvt) {

      trbExtensionSize = 0;

      data = pSubEvt->getData();
      end  = pSubEvt->getEnd();

#if DEBUG_LEVEL>4
      Info("execute", "TBOX: Next SubEvt. data between: %p and %p\n", data, end);
      pSubEvt->dumpIt();
#endif


      // loop over data words in Subevent -> standard HUB word (SubSubEvent)
      while (data != end) {


#if DEBUG_LEVEL>2
         Info("execute", "standard HUB word: %08x\n", *data);
#endif

         uSubBlockSize = (*data >> 16) & 0xFF;     // No. of the data words from one TRB board
         uSubBlockSizeScalers = uSubBlockSize - 4;
         uTrbNetAdress = *data       & 0xFFFF;     // TRB net Adddress

         // if no CTS TRB -> skip data
         if (uTrbNetAdress > 0x00FF) {
            data += uSubBlockSize;
         } else {


            // first header of data (TRB header -> not used, sub block size)
            // control the SubSubEvent size
            ++data;

#if DEBUG_LEVEL>2
            Info("execute", "first data header: %08x (%p)\n", *data, data);
#endif

            uSubBlockSizeTest = *data & 0xFF;

            if (uSubBlockSize != uSubBlockSizeTest) {
               Error("execute", "check of SubBlockSize failed : %04x != %04x", uSubBlockSize, *(data + 1) & 0xFF);
               return -1;
            }

            // second header of data (31-16 data version, 15-0 number of extension words)
            ++data;

#if DEBUG_LEVEL>2
            Info("execute", "second data header: %08x (%p)\n", *data, data);
#endif

            trbExtensionSize = *data       & 0xFFFF;

            // loop over TRB extensions
            if (trbExtensionSize != 0) {
               for (UInt_t ii = 0; ii < trbExtensionSize; ii++) {
                  ++data;
                  if (trbExtensionSize < 128) {
                  } else {
                     Error("execute", "too many TRB extension words (maximum = 128)");
                     return -1;
                  }
               }
            }



            // Latches (skip this data word)
            ++data;

#if DEBUG_LEVEL>2
            Info("execute", "Latch data word: %08x (%p)\n", *data, data);
#endif

            // empty data word
            ++data;


            // scaler information
            for (UInt_t i = 0; i < uSubBlockSizeScalers ; i++) {

               ++data;


               //Function for filling TBoxChan category for given number of scalers

               if (NULL != fCat) {
                  ptboxchan = (HTBoxChan*)fCat->getNewSlot(fLoc);
                  if (NULL != ptboxchan) {
                     ptboxchan = new(ptboxchan) HTBoxChan;
		     ptboxchan->setScalerData(i, *data);
		     if(i==0){ // time in spill should be set in the eventheader
                         Int_t time_ns =   *data * 5; // time in ns
                         gHades->getCurrentEvent()->getHeader()->setTimeInSpill(time_ns);
		     }
                  } else {
                     Error("fillScalers", "Can not get new slot for HTBoxChan");
                     continue;
                  }
               }

#if DEBUG_LEVEL>2
               Info("execute", "Scaler data word %i:  %08x (%p)\n", i, *data, data);
#endif


            }
         }//end CTS TRB

         ++data;
      }
   }


#if DEBUG_LEVEL>4
   gDebuger->leaveFunc("HTBoxUnpacker::execute\n");
#endif

   return 1;
}
