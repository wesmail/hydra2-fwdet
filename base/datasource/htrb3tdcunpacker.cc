//*-- AUTHOR Sergey Linev
//*-- Modified : 17/04/2014 by I. Koenig
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HTrb3TdcUnpacker
//
//  This is class to unpack and calibrate data of single FPGA TDC
//  It should be similar to functionality of hadaq::TdcProcessor from 'stream' framework
//
/////////////////////////////////////////////////////////////


#include "htrb3tdcunpacker.h"

#include "TString.h"

#include "htrb3tdciterator.h"
#include "htrb3tdcmessage.h"
#include "htrb3unpacker.h"
#include "htrb3calpar.h"
#include "hades.h"
#include "hevent.h"
#include "heventheader.h"

HTrb3TdcUnpacker::HTrb3TdcUnpacker(UInt_t id, HTrb3CalparTdc* tpar) :
   TObject(),
   fUnpacker(0),
   fTdcId(id),
   fCh()
{
   tdcpar = tpar;
   if (tdcpar)
   {
     Int_t numch = tdcpar->getNChannels();
     // allocate at once all channels
     fCh.resize(numch);

   }
}

void HTrb3TdcUnpacker::clearHits()
{
   for (UInt_t nch=0;nch<fCh.size();nch++) fCh[nch].clear();
}

Bool_t HTrb3TdcUnpacker::isQuietMode() const
{
   return fUnpacker ? fUnpacker->isQuietMode() : kFALSE;
}

Int_t HTrb3TdcUnpacker::getSubEvtId() const
{
   return fUnpacker ? fUnpacker->getSubEvtId() : 0;
}

Int_t HTrb3TdcUnpacker::getEventSeqNumber()
{
   if (gHades == 0) return 0;
   if (gHades->getCurrentEvent() == 0) return 0;
   if (gHades->getCurrentEvent()->getHeader() == 0) return 0;
   return gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
}


void HTrb3TdcUnpacker::scanTdcData(UInt_t* data, UInt_t datalen)
{
   HTrb3TdcIterator iter;

   iter.assign((uint32_t*) data, datalen, false);

   HTrb3TdcMessage& msg = iter.msg();

   UInt_t cnt(0);
//   Bool_t iserr(false);
//   UInt_t first_epoch(0);
//   bool isfirstepoch(false);

   while (iter.next()) {

//      msg.print();

      //if ((cnt==0) && !msg.isHeaderMsg()) iserr = true;

      cnt++;

/*      if ((cnt==2) && msg.isEpochMsg()) {
         isfirstepoch = true;
         first_epoch = msg.getEpochValue();
      }
*/

      if (msg.isHitMsg()) {
         UInt_t chid = msg.getHitChannel();
         UInt_t fine = msg.getHitTmFine();

         //UInt_t coarse = msg.getHitTmCoarse();

         Bool_t isrising = msg.isHitRisingEdge();

         Double_t localtm = iter.getMsgTimeCoarse();

         if (!iter.isCurEpoch()) {
            // one expects epoch before each hit message, if not data are corrupted and we can ignore it
            if (!isQuietMode())
               fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Missing epoch for hit from channel %u\n",
                      getEventSeqNumber(), (unsigned) getSubEvtId(), (unsigned) getTrbAddr(), chid);
           // iserr = true;
            continue;
         }

//         if (IsEveryEpoch())
//            iter.clearCurEpoch();

         if (chid >= numChannels()) {
            if (!isQuietMode())
               fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x TDC Channel number problem %u\n",
                     getEventSeqNumber(), (unsigned) getSubEvtId(), (unsigned) getTrbAddr(), chid);
            //iserr = true;
            continue;
         }

         if (fine == 0x3FF) {
            // special case - missing hit, just ignore such value
            if (!isQuietMode())
               fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Missing hit in channel %u fine counter is %x\n",
                        getEventSeqNumber(), (unsigned) getSubEvtId(), (unsigned) getTrbAddr(), chid, fine);
            continue;
         }

         if (fine >= (UInt_t)tdcpar->getNBinsPerChannel()) {
            if (!isQuietMode())
               fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Fine counter %u out of allowed range 0..%u in channel %u\n",
                      getEventSeqNumber(), (unsigned) getSubEvtId(), (unsigned) getTrbAddr(),
                      fine, tdcpar->getNBinsPerChannel(), chid);
            //iserr = true;
            continue;
         }

         ChannelRec& rec = fCh[chid];
         if (isrising) {
            localtm -= tdcpar->getRisingPar(chid,fine) / 1.e12;
         } else {
            localtm -= tdcpar->getFallingPar(chid,fine) / 1.e12;
         }

         rec.addHit(isrising, localtm);

         // if (!isQuietMode())
         //    printf("TDC Channel %2d time %12.9f\n", chid, localtm);

         continue;
      }

      // process other messages kinds

      switch (msg.getKind()) {
        case tdckind_Reserved:
           break;
        case tdckind_Header: {
           UInt_t errbits = msg.getHeaderErr();
           if (errbits && !isQuietMode())
              fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x found error bits: 0x%x\n",
                      getEventSeqNumber(), (unsigned) getSubEvtId(), (unsigned) getTrbAddr(), errbits);
           break;
        }
        case tdckind_Debug:
           break;
        case tdckind_Epoch:
           break;
        default:
           if (!isQuietMode())
              fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Unknown bits 0x%x in header\n",
                    getEventSeqNumber(), (unsigned) getSubEvtId(), (unsigned) getTrbAddr(), msg.getKind());
           break;
      }
   }

//   if (isfirstepoch && !iserr)
//      iter.setRefEpoch(first_epoch);
}

Bool_t HTrb3TdcUnpacker::correctRefTimeCh(UInt_t refch)
{
   // calculates all times to first hit in channel 0
   if (refch>=numChannels()) return kFALSE;

   if (fCh[refch].rising_mult<=0) return kFALSE;

   Double_t reftm = fCh[refch].rising_tm[0];

   for (UInt_t ch=0;ch<numChannels();ch++)
      fCh[ch].substractRefTime(reftm);

   return kTRUE;
}

