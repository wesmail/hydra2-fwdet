//*-- Modified : 03.12.2009 Ilse Koenig

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
//
//  HStart2Raw
//
//  Unpacked TRB raw data of the START detector using the TRB for readout
//
//  The class accepts up to 4 (START2_MAX_MULT) hits per channel, each with
//    time and width. If the number of hits exceeds 4, the hit counter
//    will be incremeted, but no data are stored for these hits.
//
/////////////////////////////////////////////////////////////////////////////

#include "hstart2raw.h"

ClassImp(HStart2Raw)

Float_t HStart2Raw::getTime(const Int_t n) const
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
         Error("getTime()", "hit number: %i out of range [1,4]", n);
         return -1.;
   }
}

Float_t HStart2Raw::getWidth(const Int_t n) const
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
         Error("getWidht()", "hit number: %i out of range [1,4]", n);
         return -1.;
   }
}

void HStart2Raw::getTimeAndWidth(const Int_t n, Float_t& time, Float_t& width)
{
   // Returns the time and width of the nth hit (1<=n<=4)
   switch (n) {
      case 1:
         time  = fTime1;
         width = fWidth1;
         return;
      case 2:
         time  = fTime2;
         width = fWidth2;
         return;
      case 3:
         time  = fTime3;
         width = fWidth3;
         return;
      case 4:
         time  = fTime4;
         width = fWidth4;
         return;
      default:
         Error("getTimeAndWidth", "hit number: %i out of range [1,4]", n);
         time  = -1.;
         width = -1.;
         return;
   }
}

Bool_t HStart2Raw::setTimeAndWidth(const Float_t time, const Float_t width)
{
   // Stores the given time and width in the next data element time* and width*
   // and sets the multiplicity.
   // Return kFALSE if 4 hits are already stored.

   switch (fMultiplicity) {
      case 0:
         fTime1  = time;
         fWidth1 = width;
         ++fMultiplicity;
         return kTRUE;
      case 1:
         fTime2  = time;
         fWidth2 = width;
         ++fMultiplicity;
         return kTRUE;
      case 2:
         fTime3  = time;
         fWidth3 = width;
         ++fMultiplicity;
         return kTRUE;
      case 3:
         fTime4  = time;
         fWidth4 = width;
         ++fMultiplicity;
         return kTRUE;
      default:
         if (fMultiplicity >= 4) {
            ++fMultiplicity;
         }
         return kFALSE;
   }
}
