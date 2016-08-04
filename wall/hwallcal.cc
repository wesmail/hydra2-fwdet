// File: hwallcal.cc
// Last update: 21.04.06 --- B.Spruck
// Modified for Wall by M.Golubeva 01.11.2006

#include "hwallcal.h"

ClassImp(HWallCal)

void HWallCal::clear(void)
{
  nHits=0;// This is important!
  cell=-1;
  time1=-200;
  adc1=-200;
  time2=-200;
  adc2=-200;
  time3=-200;
  adc3=-200;
  time4=-200;
  adc4=-200;
}

Float_t HWallCal::getTime(const Int_t n) const
{
  // Returns the time of the nth hit (1<=n<=4)
  switch (n) {
    case 1: return time1;
    case 2: return time2;
    case 3: return time3;
    case 4: return time4;
    default:
      Error("getTime()","hit number: %i out of range [1,4]",n);
      return -1;
  }
}

Float_t HWallCal::getAdc(const Int_t n) const
{
   // Returns the adc of the nth hit (1<=n<=4)
   switch (n) {
     case 1: return adc1;
     case 2: return adc2;
     case 3: return adc3;
     case 4: return adc4;
     default:
       Error("getAdc()","hit number: %i out of range [1,4]",n);
       return -1;
   }
}

void HWallCal::getTimeAndAdc(const Int_t n, Float_t& time, Float_t& adc )
{
  // Returns the time and adc of the nth hit (1<=n<=4)
  switch (n) {
    case 1:
      time=time1;
      adc=adc1;
      return;
    case 2:
      time=time2;
      adc=adc2;
      return;
    case 3:
      time=time3;
      adc=adc3;
      return;
    case 4:
      time=time4;
      adc=adc4;
      return;
    default:
      Error("getTime()","hit number: %i out of range [1,4]",n);
      time=adc=-500;
      return;
  }
}

Bool_t HWallCal::setTimeAdc(const Float_t time,const Float_t adc)
{
  // Stores the given time and adc in the next data element time* and adc*
  // and sets the multiplicity.
  // Return kFALSE if 4 hits are already stored.
  switch (nHits) {
    case 0:
      time1 = time;
      adc1 = adc;
      nHits++;
      break;
    case 1:
      time2 = time;
      adc2 = adc;
      nHits++;
      break;
    case 2:
      time3 = time;
      adc3 = adc;
      nHits++;
      break;
    case 3:
      time4 = time;
      adc4 = adc;
      nHits++;
      break;
    default:
      if( nHits>=4) nHits++;// continue counting Hits ...
      return kFALSE;
  }
  return kTRUE;
}

