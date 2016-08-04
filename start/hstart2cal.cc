//*-- Modified : 03.12.2009 Ilse Koenig

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HStart2Cal
//
//  This class contains START2 detector Cal data
//
//  Containing calibrated Time and width, up to 4 hits, same as on
//  raw Level.
//
/////////////////////////////////////////////////////////////

#include "hstart2cal.h"

ClassImp(HStart2Cal)

Float_t HStart2Cal::getTime(const Int_t n) const
{
   // Returns the time of the nth hit (1<=n<=4)
   switch (n) {
      case 1:
         return fTime1;
      case 2:
         return fTime2;
      case 3:
         return fTime3;
      case 4:
         return fTime4;
      default:
         Error("getTime", "hit number: %i out of range [1,4]", n);
         return -1.;
   }
}

Float_t HStart2Cal::getWidth(const Int_t n) const
{
   // Returns the width of the nth hit (1<=n<=4)
   switch (n) {
      case 1:
         return fWidth1;
      case 2:
         return fWidth2;
      case 3:
         return fWidth3;
      case 4:
         return fWidth4;
      default:
         Error("getWidth", "hit number: %i out of range [1,4]", n);
         return -1.;
   }
}

void HStart2Cal::getTimeAndWidth(const Int_t n, Float_t& time, Float_t& width)
{
   // Returns the time and width of the nth hit (1<=n<=4)
   switch (n) {
      case 1:
         time = fTime1;
         width  = fWidth1;
         return;
      case 2:
         time = fTime2;
         width  = fWidth2;
         return;
      case 3:
         time = fTime3;
         width  = fWidth3;
         return;
      case 4:
         time = fTime4;
         width  = fWidth4;
         return;
      default:
         Error("getTimeAndWidth", "hit number: %i out of range [1,4]", n);
         time = -1.;
         width  = -1.;
         return;
   }
}

Bool_t HStart2Cal::setTimeAndWidth(const Float_t time, const Float_t width)
{
   // Stores the given time and width in the next data element time* and width*
   // and sets the multiplicity.
   // Returns kFALSE if 4 hits are already stored.
   switch (fMultiplicity) {
      case 0:
         fTime1 = time;
         fWidth1  = width;
         ++fMultiplicity;
         return kTRUE;
      case 1:
         fTime2 = time;
         fWidth2  = width;
         ++fMultiplicity;
         return kTRUE;
      case 2:
         fTime3 = time;
         fWidth3  = width;
         ++fMultiplicity;
         return kTRUE;
      case 3:
         fTime4 = time;
         fWidth4  = width;
         ++fMultiplicity;
         return kTRUE;
      default:
         if (fMultiplicity >= 4) {
            ++fMultiplicity; // continue counting Hits ...
         }
         return kFALSE;
   }
   return kTRUE;
}
