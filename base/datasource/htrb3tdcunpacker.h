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

#include "htrb3unpacker.h"
#include "htrb3calpar.h"

#include "TObject.h"
#include <vector>
#include <map>

class HTrb3CalparTdc;

class HTrb3TdcUnpacker : public HTrb3Unpacker  {

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

      Double_t getHit(Bool_t rising, UInt_t cnt)
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
         for (UInt_t n=0;n<rising_mult;n++)
            rising_tm[n] -= reftm;

         for (UInt_t n=0;n<falling_mult;n++)
            falling_tm[n] -= reftm;
      }
   };

   static const size_t maxchan = 65;
   struct TDC {
      UInt_t pSubEvtId;
      UInt_t fTdcId;
      UInt_t nChan;
      UInt_t fCalibr[2]; //!< temporary fine time calibration info from data stream
      Int_t fNcalibr; // counter of available fine time calibration values
      ChannelRec fCh[maxchan];

      TDC(size_t numchannels=maxchan) :
         pSubEvtId(0),
         fTdcId(0),
         nChan(numchannels), fNcalibr(0) {}

      void clear()
      {
         for (UInt_t i = 0; i < nChan; ++i)
             fCh[i].clear();
      }

      UInt_t getTrbAddr() const { return fTdcId; }
      UInt_t numChannels() const { return nChan; }
      ChannelRec& getCh(UInt_t n) { return fCh[n]; }

      Int_t correctRefTimeCh(UInt_t refch = 0)
      {
         // calculates all times to first hit in channel 0
         if (refch>=numChannels()) return kFALSE;
         if (fCh[refch].rising_mult<=0) return kFALSE;
         Double_t reftm = fCh[refch].rising_tm[0];
         for (UInt_t ch=0;ch<numChannels();ch++)
            fCh[ch].substractRefTime(reftm);
         return kTRUE;
      }
   };

protected:
   HTrb3Calpar* calpar;         //! TDC calibration parameters
   std::map<UInt_t, HTrb3CalparTdc*> fCalpars;
   std::vector<TDC> fTDCs;      //!
   std::map<UInt_t, Int_t> fTDCsMap; //!
   Int_t nCalSrc;               //! 0 -auto, 1 - no calibration, 2 - calpar, 3 - DAQ

   UInt_t fMinAddress; //!< for check of address range when autoregistration of tdcs
   UInt_t fMaxAddress; //!< for check of address range when autoregistration of tdcs

   Int_t getEventSeqNumber();

   void addCalpar(UInt_t id1, HTrb3CalparTdc*);

 public:
   HTrb3TdcUnpacker(UInt_t id=0);
   virtual ~HTrb3TdcUnpacker();

   virtual Bool_t reinit();

   UInt_t numTDC() const { return fTDCs.size(); }
   TDC * getTDC(UInt_t n) { return &(fTDCs[n]); }

   /** preassign tdc of expected trbnet address. returns index in map*/
   Int_t addTDC(UInt_t trbaddr, size_t numchannels=maxchan);

   void setCalibrationSource(UInt_t s) { nCalSrc = s; }

   Bool_t scanTdcData(UInt_t trbaddr, UInt_t* data, UInt_t datalen);
   Bool_t correctRefTimeCh(UInt_t ch = 0);

   void setMinAddress(UInt_t trbnetaddress) { fMinAddress=trbnetaddress; }
   void setMaxAddress(UInt_t trbnetaddress) { fMaxAddress=trbnetaddress; }

protected:
   void clearAll();
   virtual Bool_t decodeData(UInt_t trbaddr, UInt_t n, UInt_t * data);

   ClassDef(HTrb3TdcUnpacker,0); // Unpacker for TRB3 FPGA TDC
};

#endif /* !HTRB3TDCUNPACKER_H */


