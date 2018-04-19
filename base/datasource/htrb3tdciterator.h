#ifndef HTRB3TDCITERATOR_H
#define HTRB3TDCITERATOR_H

#include "htrb3tdcmessage.h"

class HTrb3TdcIterator {
   protected:

      enum { DummyEpoch = 0xffffffff };

      UInt_t*  fBuf;        //! pointer on raw data
      UInt_t   fBuflen;     //! length of raw data
      Bool_t       fSwapped;    //! true if raw data are swapped

      HTrb3TdcMessage  fMsg;  //! current message
      UInt_t  fCurEpoch;    //! current epoch

      // base::LocalStampConverter  fConv;   //! use to covert time stamps in seconds

   public:

      HTrb3TdcIterator() :
         fBuf(0),
         fBuflen(0),
         fSwapped(false),
         fMsg(),
         fCurEpoch(DummyEpoch)
         //fConv()
      {
         // we have 11 bits for coarse stamp and 32 bits for epoch
         // each bin is 5 ns
         // fConv.SetTimeSystem(11+32, HTrb3TdcMessage::coarseUnit());
      }

      void assign(UInt_t* buf, UInt_t len, Bool_t swapped = true)
      {
         fBuf = buf;
         fBuflen = len;
         fSwapped = swapped;
         fMsg.assign(0);

         if (fBuflen == 0) fBuf = 0;
         fCurEpoch = DummyEpoch;
      }

//      void assign(hadaq::RawSubevent* subev, UInt_t indx, UInt_t datalen)
//      {
//         if (subev!=0)
//            assign(subev->GetDataPtr(indx), datalen, subev->IsSwapped());
//      }

      /** One should call method to set current reference epoch */
      void setRefEpoch(UInt_t epoch)
      {
         //fConv.MoveRef(((uint64_t) epoch) << 11);
      }

      Bool_t next()
      {
         if (fBuf==0) return kFALSE;

         if (fSwapped)
            fMsg.assign((((UChar_t *) fBuf)[0] << 24) | (((UChar_t *) fBuf)[1] << 16) | (((UChar_t *) fBuf)[2] << 8) | (((UChar_t *) fBuf)[3]));
         else
            fMsg.assign(*fBuf);

         if (fMsg.isEpochMsg()) fCurEpoch = fMsg.getEpochValue();

         fBuf++;
         if (--fBuflen == 0) fBuf = 0;

         return kTRUE;
      }

      /** Returns 39-bit value, which combines epoch and coarse counter.
       * Time bin is 5 ns  */
      uint64_t getMsgStamp() const
      { return (isCurEpoch() ? ((uint64_t) fCurEpoch) << 11 : 0) | (fMsg.isHitMsg() ? fMsg.getHitTmCoarse() : 0); }

      inline Double_t getMsgTimeCoarse() const
      { return getMsgStamp() * HTrb3TdcMessage::coarseUnit();  /*fConv.ToSeconds(getMsgStamp()) */; }

      inline Double_t getMsgTimeFine() const
      { return fMsg.isHitMsg() ? HTrb3TdcMessage::simpleFineCalibr(fMsg.getHitTmFine()) : 0.; }

      HTrb3TdcMessage& msg() { return fMsg; }

      /** Returns true, if current epoch was assigned */
      Bool_t isCurEpoch() const { return fCurEpoch != DummyEpoch; }

      /** Clear current epoch value */
      void clearCurEpoch() { fCurEpoch = DummyEpoch; }

      /** Return value of current epoch */
      UInt_t getCurEpoch() const { return fCurEpoch; }

      void printmsg()
      {
         Double_t tm = -1.;
         if (msg().isHitMsg() || msg().isEpochMsg())
            tm = getMsgTimeCoarse() - getMsgTimeFine();
         msg().print(tm);
      }

//   ClassDef(HTrb3TdcIterator,0);
};

#endif
