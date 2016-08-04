//*-- AUTHOR : Ilse Koenig
//*-- Created : 17/01/2005
// Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HWallRaw
//
//  Class for the raw data of the Forward Wall
//
/////////////////////////////////////////////////////////////

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
//
// HHodoRaw
//
// Unpacked raw data of the Pion Hodoscopes
//
// The class accepts up to 4 hits per channel.
// The addresses of the fibers can be accessed via the inline functions
//     void setModule(const Int_t)
//     void setFiber(const Int_t)
//     void setAddress(const Int_t module,const Int_t fiber)
//     Int_t getModule(void) const
//     Int_t getFiber(void) const
//     void getAddress(Int_t& module,Int_t& fiber)
// and the number of hits with the inline function
//     Int_t getNHits(void) const
// The inline function clear() sets the data members to the following values:
//      nHits=0;
//      cell=time1=width1=time2=width2=time3=width3=time4=width4=-200;
//
/////////////////////////////////////////////////////////////////////////////

#include "hwallraw.h"

ClassImp(HWallRaw)
  Float_t HWallRaw::getTime(const Int_t n) const
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

Float_t HWallRaw::getWidth(const Int_t n) const
{
  // Returns the width of the nth hit (1<=n<=4)
  switch (n) {
    case 1: return width1;
    case 2: return width2;
    case 3: return width3;
    case 4: return width4;
    default:
      Error("getWidth()","hit number: %i out of range [1,4]",n);
      return -1;
  }
}

void HWallRaw::getTimeAndWidth(const Int_t n, Float_t& time, Float_t& width )
{
  // Returns the time and width of the nth hit (1<=n<=4)
  switch (n) {
    case 1:
      time=time1;
      width=width1;
      return;
    case 2:
      time=time2;
      width=width2;
      return;
    case 3:
      time=time3;
      width=width3;
      return;
    case 4:
      time=time4;
      width=width4;
      return;
    default:
      Error("getTime()","hit number: %i out of range [1,4]",n);
      time=width=-500;
      return;
   }
}

Bool_t HWallRaw::fill(const Float_t time,const Float_t width)
{
  // Stores the given time and width in the next data element time* and width*
  // and sets the multiplicity.
  // Return kFALSE if 4 hits are already stored.
  switch (nHits) {
    case 0:
      time1 = time;
      width1 = width;
      nHits++;
      break;
    case 1:
      time2 = time;
      width2 = width;
      nHits++;
      break;
    case 2:
      time3 = time;
      width3 = width;
      nHits++;
      break;
    case 3:
      time4 = time;
      width4 = width;
      nHits++;
      break;
    default:
      if( nHits>=4) nHits++;// continue counting Hits ...
      return kFALSE;
  }
  return kTRUE;
}

Bool_t HWallRaw::fill_lead(const Float_t time)
{
  // Stores the given time in the next data element time*
  // and sets the multiplicity.
  // Return kFALSE if 4 hits are already stored.
  switch (nHits) {
    case 0:
      time1 = time;
      nHits++;
      break;
    case 1:
      time2 = time;
      nHits++;
      break;
    case 2:
      time3 = time;
      nHits++;
      break;
    case 3:
      time4 = time;
      nHits++;
      break;
    default:
      if( nHits>=4) nHits++;// continue counting Hits ...
      return kFALSE;
  }
  return kTRUE;
}

Bool_t HWallRaw::fill_trail(const Float_t time)
{
  // Calculates the time between trailing and LAST(!) leading hit.
  // No other check if its really the right one,
  // i am depending on the TDC to deliver the right order
  // Return kFALSE if no leading yet or more than 4 Hits
  switch (nHits) {
    case 0:
      // no Leading
      return kFALSE;
    case 1:
      if(width1>=0) return kFALSE;// already a trailing filled in
      width1 = time-time1;
      break;
    case 2:
      if(width2>=0) return kFALSE;
      width2 = time-time2;
      break;
    case 3:
      if(width3>=0) return kFALSE;
      width3 = time-time3;
      break;
    case 4:
      if(width4>=0) return kFALSE;
      width4 = time-time4;
      break;
    default:
      return kFALSE;
  }
  return kTRUE;
}

