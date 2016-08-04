/////////////////////////////////////////////////////////////
//
//  HTrb3TdcUnpacker
//
//  This is class to unpack and calibrate data of single FPGA TDC
//  It should be similar to functionality of hadaq::TdcProcessor from 'stream' framework
//
/////////////////////////////////////////////////////////////

#ifndef HTRB3TDCUNPACKER_H
#define HTRB3TDCUNPACKER_H

#include "TObject.h"
#include <vector>

class HTrb3Unpacker;
class HTrb3CalparTdc;

class HTrb3TdcUnpacker : public TObject  {

public:

   struct ChannelRec {
      Double_t rising_tm[10];
      Double_t falling_tm[10];
      UInt_t   rising_mult;
      UInt_t   falling_mult;

      ChannelRec() :
         rising_mult(0),
         falling_mult(0) {}

      void clear()
      {
         rising_mult  = 0;
         falling_mult = 0;
      }

      void addHit(Bool_t rising, Double_t tm)
      {
         if (rising) {
            if (rising_mult < 10)
               rising_tm[rising_mult++] = tm;
         } else {
            if (falling_mult < 10)
               falling_tm[falling_mult++] = tm;
         }
      }

      Double_t getHit(Bool_t rising, unsigned cnt)
      {
         return rising ?
            (cnt<rising_mult ? rising_tm[cnt] : 0.) :
            (cnt<falling_mult ? falling_tm[cnt] : 0.);
      }

      Double_t getLastHit(Bool_t rising)
      {
         return rising ?
            (rising_mult>0 ? rising_tm[rising_mult-1] : 0.) :
            (falling_mult>0 ? falling_tm[falling_mult-1] : 0.);
      }

      void substractRefTime(Double_t reftm)
      {
         for (unsigned n=0;n<rising_mult;n++)
            rising_tm[n] -= reftm;

         for (unsigned n=0;n<falling_mult;n++)
            falling_tm[n] -= reftm;
      }
   };

protected:
   HTrb3Unpacker* fUnpacker;    //! pointer on unpacker
   UInt_t fTdcId;               //! TRB address, placed in sub-sub event header
   HTrb3CalparTdc* tdcpar;      //! pointer to TDC calibration parameters
   std::vector<ChannelRec> fCh; //!

   Bool_t isQuietMode() const;

   Int_t getSubEvtId() const;
   Int_t getEventSeqNumber();

 public:
   HTrb3TdcUnpacker(UInt_t id=0, HTrb3CalparTdc* tdcpar=0);
   virtual ~HTrb3TdcUnpacker() {}

   UInt_t getTrbAddr() const { return fTdcId; }

   void setParent(HTrb3Unpacker* prnt) { fUnpacker = prnt; }
   
   UInt_t numChannels() const { return fCh.size(); }
   ChannelRec& getCh(unsigned n) { return fCh[n]; }

   void   clearHits();
   void   scanTdcData(UInt_t* data, UInt_t datalen);
   Bool_t correctRefTimeCh(UInt_t ch = 0);


   ClassDef(HTrb3TdcUnpacker,0); // Unpacker for TRB3 FPGA TDC
};

#endif /* !HTRB3TDCUNPACKER_H */


