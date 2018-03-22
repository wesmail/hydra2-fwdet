#include "hlocateddataobject.h"
#include "TClass.h"
#include "TBuffer.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 9/11/1998

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////
//HLocatedDataObject
//
// This is a data object which also stores it's location in the event structure
// This location is retrieved in a standard way through the functions 
//getNLocationIndex() (returns the number of indexes needed to locate the
//data object) and getLocationIndex(Int_t n) (which returns the value of the
//index number "n").
/////////////////////////////////

ClassImp(HLocatedDataObject)

void HLocatedDataObject::Streamer(TBuffer &R__b)
{
   // Stream an object of class HLocatedDataObject.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      if(R__v < 2) {
         Version_t R__t = R__b.ReadVersion(); if (R__t) { }
      }
      TObject::Streamer(R__b);
   } else {
      R__b.WriteVersion(HLocatedDataObject::IsA());
      TObject::Streamer(R__b);
   }
}
