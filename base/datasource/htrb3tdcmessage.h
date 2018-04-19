#ifndef HTRB3TDCMESSAGE_H
#define HTRB3TDCMESSAGE_H

#include "Rtypes.h"

#include <stdint.h>
#include <stdio.h>

enum TdcMessageKind {
   tdckind_Reserved = 0x00000000,
   tdckind_Header   = 0x20000000,
   tdckind_Debug    = 0x40000000,
   tdckind_Epoch    = 0x60000000,
   tdckind_Mask     = 0xe0000000,
   tdckind_Hit      = 0x80000000,
   tdckind_Hit1     = 0xa0000000,
   tdckind_Hit2     = 0xc0000000,
   //tdckind_Hit3     = 0xe0000000 // now used for calib message
   tdckind_Calib    = 0xe0000000 // calibrated fine times
};

enum TdcConstants {
   MaxNumTdcChannels = 65
};

/** TdcMessage is wrapper for data, produced by FPGA-TDC
 * struct is used to avoid any potential overhead */

struct HTrb3TdcMessage {
   protected:
      UInt_t   fData;

      static UInt_t gFineMinValue;
      static UInt_t gFineMaxValue;

   public:

      HTrb3TdcMessage() : fData(0) {}

      HTrb3TdcMessage(UInt_t d) : fData(d) {}

      void assign(UInt_t d) { fData=d; }

      /** Returns kind of the message
       * If used for the hit message, four different values can be returned */
      UInt_t getKind() const { return fData & tdckind_Mask; }

      /** uncalibrated raw hit. may use extra information from preceding CalibMsg for fine time calibration*/
      Bool_t isHitMsg() const { return getKind() == tdckind_Hit; }

      /** calibrated format with replacing original fine counter*/
      Bool_t isHit1Msg() const { return getKind() == tdckind_Hit1;}

      /** not used yet*/
      Bool_t isHit2Msg() const { return getKind() == tdckind_Hit1;}

      Bool_t isEpochMsg() const { return getKind() == tdckind_Epoch; }
      Bool_t isDebugMsg() const { return getKind() == tdckind_Debug; }
      Bool_t isHeaderMsg() const { return getKind() == tdckind_Header; }
      Bool_t isReservedMsg() const { return getKind() == tdckind_Reserved; }

      /** extra calibration message: contains 1 or 2 calibrated fine time information*/
      Bool_t isCalibMsg() const { return getKind() == tdckind_Calib; }

      // methods for epoch

      /** Return Epoch for epoch marker, 28 bit */
      UInt_t getEpochValue() const { return fData & 0xFFFFFFF; }
      /** Get reserved bit for epoch, 1 bit */
      UInt_t getEpochRes() const { return (fData >> 28) & 0x1; }

      // methods for hit

      /** Returns hit channel ID */
      UInt_t getHitChannel() const { return (fData >> 22) & 0x7F; }

      /** Returns hit coarse time counter, 11 bit */
      UInt_t getHitTmCoarse() const { return fData & 0x7FF; }

      /** Returns hit fine time counter, 10 bit */
      UInt_t getHitTmFine() const { return (fData >> 12) & 0x3FF; }

      /** Returns time stamp, which is simple combination coarse and fine counter */
      UInt_t getHitTmStamp() const { return (getHitTmCoarse() << 10) | getHitTmFine(); }

      /** Returns hit edge 1 - rising, 0 - falling */
      UInt_t getHitEdge() const {  return (fData >> 11) & 0x1; }

      Bool_t isHitRisingEdge() const { return getHitEdge() == 0x1; }
      Bool_t isHitFallingEdge() const { return getHitEdge() == 0x0; }

      /** Returns hit reserved value, 2 bits */
      UInt_t getHitReserved() const { return (fData >> 29) & 0x3; }

      // methods for header

      /** Return error bits of header message */
      UInt_t getHeaderErr() const { return fData & 0xFFFF; }

      /** Return reserved bits of header message */
      UInt_t getHeaderRes() const { return (fData >> 16) & 0x1FFF; }

      /** Return first value in calibration message */
      UInt_t getCalibFirst() const { return (fData  & 0x3fff); }

      /** Return second value in calibration message */
      UInt_t getCalibSecond() const { return ((fData >> 14) & 0x3fff);}


      void print(Double_t tm = -1.);

      static Double_t coarseUnit() { return 5e-9; }

      static Double_t simpleFineCalibr(UInt_t fine)
      {
         if (fine<=gFineMinValue) return 0.;
         if (fine>=gFineMaxValue) return coarseUnit();
         return (coarseUnit() * (fine - gFineMinValue)) / (gFineMaxValue - gFineMinValue);
      }

      /** Method set static limits, which are used for simple interpolation of time for fine counter */
      static void setFineLimits(UInt_t min, UInt_t max)
      {
         gFineMinValue = min;
         gFineMaxValue = max;
      }

//      ClassDef(HTrb3TdcMessage, 0)
};


#endif
