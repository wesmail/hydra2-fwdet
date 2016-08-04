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
#include "hdatasource.h"
#include "hdebug.h"
#include "hevent.h"
#include "heventheader.h"
#include "hlatchunpacker.h"
#include "hldsubevt.h"
#include "hstartdef.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>

#define TEST 0

using namespace std;

ClassImp(HLatchUnpacker)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////
//
// HLatchUnpacker
//
// Unpacker used to read the LATCH data from .hld files
// Sets EventHeader::fTBit.
//
////////////////////////////////////////////////////////////////////

HLatchUnpacker::HLatchUnpacker(Int_t id)
{
   // Constructor
   // Input: subevent id
   subEvtId = id;
}

Int_t HLatchUnpacker::execute()
{


   // Unpacks the latch data and sets EventHeader::fTBit
#if DEBUG_LEVEL>4
   gDebuger->enterFunc("HLatchUnpacker::execute\n");
#endif

   UInt_t* data       = 0;
   UInt_t* end        = 0;

   UInt_t uTrbNetAdress;        // TrbNetAdress
   UInt_t uSubBlockSize;        // BlockSize of SubSubEvent
   UInt_t uSubBlockSizeTest;    // BlockSize of SubSubEvent
   UInt_t uSubBlockSizeScalers; // BlockSize of Scaler words
   UInt_t trbExtensionSize;     // number of TRB extension words


   if (pSubEvt) {

      trbExtensionSize = 0;

      data = pSubEvt->getData();
      end  = pSubEvt->getEnd();

#if DEBUG_LEVEL>4
      printf("Next SubEvt. data between: %p and %p\n", data, end);
      pSubEvt->dumpIt();
#endif




      // loop over data words in Subevent -> standard HUB word (SubSubEvent)
      while (data != end) {


#if DEBUG_LEVEL>2
         printf("standard HUB word: %08x\n", *data);
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
            printf("first data header: %08x (%p)\n", *data, data);
#endif

            uSubBlockSizeTest = *data & 0xFF;

            if (uSubBlockSize != uSubBlockSizeTest) {
               Error("execute", "check of SubBlockSize failed : %04x != %04x", uSubBlockSize, *(data + 1) & 0xFF);
               return -1;
            }

            // second header of data (31-16 data version, 15-0 number of extension words)
            ++data;

#if DEBUG_LEVEL>2
            printf("second data header: %08x (%p)\n", *data, data);
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


            // Latches
            ++data;

#if DEBUG_LEVEL>2
            printf("Latch data word: %08x (%p)\n", *data, data);
#endif

            fillLatch(data);


            // empty word
            ++data;


            // scaler information ( for MDC calibration trigger ) -> skip data words
            // scaler information is read out in htboxunpacker
            // no HEADER information about scaler event (its written every second)
            data += uSubBlockSizeScalers ;

         }//end CTS TRB

         ++data;
      }
   }


#if DEBUG_LEVEL>4
   gDebuger->leaveFunc("HLatchUnpacker::execute\n");
#endif

   return 1;
}


UInt_t* HLatchUnpacker::fillLatch(UInt_t* data)
{

   //Function for filling latch structure. (one 16 bit data word)
   //returns pointer to the next data word  in subevt.

   UInt_t latchData = *data & 0xFFFFFFFF;

   gHades->getCurrentEvent()->getHeader()->setTBit(latchData);

   return data;
}
