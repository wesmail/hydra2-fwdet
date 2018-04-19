//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
// *-- Author  : JAM
// *-- Revised : Joern Adamczewski-Musch <j.adamczewski@gsi.de> 2017
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700PixelThreshold
//    Contains trb3 time thresholds for single pixel of rich700
//    Defined after example of hrichcalparcell JAM Nov2017
//
//////////////////////////////////////////////////////////////////////////////



#include "hrich700pixelthreshold.h"

ClassImp(HRich700PixelThreshold)



HRich700PixelThreshold::HRich700PixelThreshold()
{
   reset();
}

HRich700PixelThreshold::HRich700PixelThreshold(Double_t tmin, Double_t tmax, Double_t totmin, Double_t totmax, Short_t flag)
{
   setParams(tmin, tmax, totmin, totmax, flag);
}

inline Int_t HRich700PixelThreshold::getNLocationIndex()
{
   return 2;
}

inline Int_t HRich700PixelThreshold::getLocationIndex(Int_t i)
{
   switch (i) {
      case 0 :
         return (getPMT() -1);    // real pmt id begins with 1, array begins with 0
         break;
      case 1 :
         return (getPixel() -1);  // real pixel id begins with 1, array begins with 0
         break;
      default:
         return -1;
         break;
   }
   return -1;
}
