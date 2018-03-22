//*--- Author: Vladimir Pechenov
//*--- Modified: Vladimir Pechenov 05/02/2005

using namespace std;
#include "hmdcwirefit.h"
#include <iostream>
#include <iomanip>

#include "TBuffer.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HMdcWireFit
//
// Container class keeps one wire fit information for debuging and tuning.
//
// void clear()
//        Default setting of the data members.
//
// void setAddress(Char_t s,Char_t m,Char_t l,Short_t c)
// void setSec(Char_t s)
// void setMod(Char_t m)
// void setLayer(Char_t l)
// void setCell(Short_t c)
//                Set wire address.
// void setTimeNum(Char_t t)
//                Set drift time index (t=1 for time1 in HMdcCal1 and
//                                      t=2 for time2 in HMdcCal1).
// void setTdcTime(Float_t v)
//                Set measured time minus time of wire offset.
// void setDriftTime(Float_t v)
//                Set calculated drift time.
// void setFullTime(Float_t v)
//                Set calculated drift time + time offset.
// void setDev(Float_t v)
//                Set deviation (full time minus tdc time).
// void setWeight(Float_t v)
//                Set wire weight in fit.
// void setMinDist(Float_t v)
//                Set minimal distance from track to wire.
// void setAlpha(Float_t v)
//                Set impact angle of track in cell system.
// void setIsInCell(Bool_t v)
//                v=kFALSE if track don't cross cell.
// void setTdcTimeErr(Float_t v)
//                Set error of drift time.
//
// void getAddress(Int_t& s,Int_t& m,Int_t& l,Int_t& c)
// Char_t  getSec(void)
// Char_t  getMod(void)
// Char_t  getLayer(void)
// Short_t getCell(void)
//                Get wire address.
// Char_t  getTimeNum(void)
//               Get drift time index (return 1 for time1 in HMdcCal1,
//                                     return 2 for time2 in HMdcCal1).
// Float_t getTdcTime(void)
//                Get measured time minus time of wire offset.
// Float_t getDriftTime(void)
//                Get calculated drift time.
// Float_t getFullTime(void)
//                Get calculated drift time + time offset.
// Float_t getTimeOffset
//                Get time offset ( =getFullTime()-getDriftTime())
// Float_t getDev(void)
//                Get deviation (full time minus tdc time).
// Float_t getWeight(void)
//                Get wire weight in fit.
// Float_t getMinDist(void)
//                Get minimal distance from track to wire.
// Float_t getAlpha(void)
//                Get impact angle of track in cell system.
// Bool_t  getIsInCell(void)
//                Return kTRUE if track cross cell,
//                retrn kFALSE if track don't cross cell.
// Float_t getTdcTimeErr(void)
//                Get error of drift time.
// Float_t getChi2(void)
//                Get chi2 ( =getDev()/getTdcTimeErr() )
//
// Bool_t isGeant()
//                Return kFALSE
// void print()
//                Print container
//
//////////////////////////////////////////////////////////////////////////////


ClassImp(HMdcWireFit)

void HMdcWireFit::print() {
  // Print this container
  printf("S%iM%iL%i C%3i T%i:",sec+1,mod+1,layer+1,cell+1,timeNum);
  printf(" %5.2fmm %5.1fdeg. tof=%5.1f",minDist,alpha,fullTime-driftTime);
  printf("%c drTm=%5.1f fTm=%5.1f",(isInCell) ? ' ':'O',driftTime,fullTime);
  printf(" TDC=%5.1f dev=%6.1f WT=%g\n",getTdcTime(),dev,weight);
}

void HMdcWireFit::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcWireFit.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> sec;
      R__b >> mod;
      R__b >> layer;
      R__b >> cell;
      R__b >> timeNum;
      R__b >> tdcTimeCal1;
      R__b >> tdcTime;
      R__b >> dev;
      R__b >> weight;
      R__b >> driftTime;
      R__b >> fullTime;
      R__b >> minDist;
      R__b >> alpha;
      R__b >> isInCell;
      R__b >> tdcTimeErr;
      R__b >> isUsedInFit;
      if(R__v > 1) {
        R__b >> tot;
        R__b >> cellPath;
      } else {
        tot      = 0.;
        cellPath = -1.;
      }
      R__b.CheckByteCount(R__s, R__c, HMdcWireFit::IsA());
   } else {
      R__c = R__b.WriteVersion(HMdcWireFit::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << sec;
      R__b << mod;
      R__b << layer;
      R__b << cell;
      R__b << timeNum;
      R__b << tdcTimeCal1;
      R__b << tdcTime;
      R__b << dev;
      R__b << weight;
      R__b << driftTime;
      R__b << fullTime;
      R__b << minDist;
      R__b << alpha;
      R__b << isInCell;
      R__b << tdcTimeErr;
      R__b << isUsedInFit;
      R__b << tot;
      R__b << cellPath;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
