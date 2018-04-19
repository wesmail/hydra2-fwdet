#include "htrb3tdcmessage.h"

// JAM2018: adjusted min/max values for simple calibration according to hldprint
UInt_t HTrb3TdcMessage::gFineMinValue = 31; //20;
UInt_t HTrb3TdcMessage::gFineMaxValue = 491; //500;

void HTrb3TdcMessage::print(Double_t tm)
{
   switch (getKind()) {
      case tdckind_Reserved:
         printf("     tdc reserv 0x%08x\n", (UInt_t) fData);
         break;

      case tdckind_Header:
         printf("     tdc head   0x%08x\n", (UInt_t) fData);
         break;
      case tdckind_Debug:
         printf("     tdc debug  0x%08x\n", (UInt_t) fData);
         break;
      case tdckind_Epoch:
         printf("     tdc epoch  0x%08x", (UInt_t) fData);
         if (tm>=0) printf ("  tm:%9.2f", tm*1e9);
         printf("   epoch 0x%x\n", (UInt_t) getEpochValue());
         break;
      case tdckind_Hit:
      case tdckind_Hit1:
      case tdckind_Hit2:
         printf("     tdc hit    0x%08x", (UInt_t) fData);
         if (tm>=0) printf ("  tm:%9.2f", tm*1e9);
         printf("   ch %3u isrising:%u tc 0x%3x tf 0x%3x\n",
                 (UInt_t) getHitChannel(), (UInt_t)getHitEdge(),
                 (UInt_t)getHitTmCoarse(), (UInt_t)getHitTmFine());
         break;

      case tdckind_Calib:
    	  printf("     tdc calibr    0x%08x", (UInt_t) fData);
    	  printf("tdc calibr v1 0x%04x v2 0x%04x\n", (UInt_t) getCalibFirst(), (UInt_t) getCalibSecond());

      default:
         printf("     tdc unkn   0x%08x   kind %u\n", (UInt_t) fData, (UInt_t) getKind());
         break;
   }
}
