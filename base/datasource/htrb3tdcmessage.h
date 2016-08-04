#ifndef HTRB3TDCMESSAGE_H
#define HTRB3TDCMESSAGE_H

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
   tdckind_Hit3     = 0xe0000000
};

enum TdcConstants {
   MaxNumTdcChannels = 65
};

/** TdcMessage is wrapper for data, produced by FPGA-TDC
 * struct is used to avoid any potential overhead */

struct HTrb3TdcMessage {
   protected:
      uint32_t   fData;

      static unsigned gFineMinValue;
      static unsigned gFineMaxValue;

   public:

      HTrb3TdcMessage() : fData(0) {}

      HTrb3TdcMessage(uint32_t d) : fData(d) {}

      void assign(uint32_t d) { fData=d; }

      /** Returns kind of the message
       * If used for the hit message, four different values can be returned */
      uint32_t getKind() const { return fData & tdckind_Mask; }

      bool isHitMsg() const { return fData & tdckind_Hit; }

      bool isEpochMsg() const { return getKind() == tdckind_Epoch; }
      bool isDebugMsg() const { return getKind() == tdckind_Debug; }
      bool isHeaderMsg() const { return getKind() == tdckind_Header; }
      bool isReservedMsg() const { return getKind() == tdckind_Reserved; }

      // methods for epoch

      /** Return Epoch for epoch marker, 28 bit */
      uint32_t getEpochValue() const { return fData & 0xFFFFFFF; }
      /** Get reserved bit for epoch, 1 bit */
      uint32_t getEpochRes() const { return (fData >> 28) & 0x1; }

      // methods for hit

      /** Returns hit channel ID */
      uint32_t getHitChannel() const { return (fData >> 22) & 0x7F; }

      /** Returns hit coarse time counter, 11 bit */
      uint32_t getHitTmCoarse() const { return fData & 0x7FF; }

      /** Returns hit fine time counter, 10 bit */
      uint32_t getHitTmFine() const { return (fData >> 12) & 0x3FF; }

      /** Returns time stamp, which is simple combination coarse and fine counter */
      uint32_t getHitTmStamp() const { return (getHitTmCoarse() << 10) | getHitTmFine(); }

      /** Returns hit edge 1 - rising, 0 - falling */
      uint32_t getHitEdge() const {  return (fData >> 11) & 0x1; }

      bool isHitRisingEdge() const { return getHitEdge() == 0x1; }
      bool isHitFallingEdge() const { return getHitEdge() == 0x0; }

      /** Returns hit reserved value, 2 bits */
      uint32_t getHitReserved() const { return (fData >> 29) & 0x3; }

      // methods for header

      /** Return error bits of header message */
      uint32_t getHeaderErr() const { return fData & 0xFFFF; }

      /** Return reserved bits of header message */
      uint32_t getHeaderRes() const { return (fData >> 16) & 0x1FFF; }

      void print(double tm = -1.);

      static double coarseUnit() { return 5e-9; }

      static double simpleFineCalibr(unsigned fine)
      {
         if (fine<=gFineMinValue) return 0.;
         if (fine>=gFineMaxValue) return coarseUnit();
         return (coarseUnit() * (fine - gFineMinValue)) / (gFineMaxValue - gFineMinValue);
      }

      /** Method set static limits, which are used for simple interpolation of time for fine counter */
      static void setFineLimits(unsigned min, unsigned max)
      {
         gFineMinValue = min;
         gFineMaxValue = max;
      }

//      ClassDef(HTrb3TdcMessage, 0)
};


#endif
