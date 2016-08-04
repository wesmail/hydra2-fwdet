//////////////////////////////////////////////////////////////////////////////
//
// $Id:
//
//*-- Author : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HStart2HitFPar
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hparamlist.h"
#include "hstart2hitfpar.h"

#include <iostream>

using namespace std;

ClassImp(HStart2HitFPar)
/*
HStart2HitFPar::HStart2HitFPar(const HStart2HitFPar& source)
{
   Error("HStart2HitFPar", "HStart2HitFPar object can not be initialized with values of another object!");
}

HStart2HitFPar& HStart2HitFPar::operator=(const HStart2HitFPar& source)
{
   if (this != &source) {
      Error("operator=", "HStart2HitFPar object can not be assigned!");
   }
   return *this;
}
*/
void HStart2HitFPar::clear()
{
   fModules = 0;
   fStrips  = 0;
   fMeanTime.Reset();
   fWidth.Reset();
}

void HStart2HitFPar::printParams()
{

   if (fMeanTime.GetSize() != fWidth.GetSize()) {
      Error("printParams", "Arrays differ in size");
      return;
   }
   if (0 == fStrips) {
      Error("printParams", "Number of strips is 0, can not proceed...");
      return;
   }

   cout << endl << endl;
   cout << "Start2 Hit Finder Parameters" << endl;
   cout << "Number of modules: " << fModules << endl;
   cout << "Number of strips:  " << fStrips << endl;
   for (Int_t i = 0; i < fMeanTime.GetSize(); ++i) {
      cout.width(4);
      cout << i % fStrips; // module
      cout.width(4);
      cout << (Int_t)i / fStrips; // strip
      cout.width(10);
      cout << fMeanTime[i];
      cout.width(10);
      cout << fWidth[i];
      cout << endl;
   }
   cout << endl << endl;

}

void HStart2HitFPar::putParams(HParamList* l)
{
   if (!l) return;
   l->add("fModules",  fModules);
   l->add("fStrips",   fStrips);
   l->add("fMeanTime", fMeanTime);
   l->add("fWidth",    fWidth);
}

Bool_t HStart2HitFPar::getParams(HParamList* l)
{
   if (!l) return kFALSE;
   if (!l->fill("fModules",  &fModules))  return kFALSE;
   if (!l->fill("fStrips",   &fStrips))   return kFALSE;
   if (!l->fill("fMeanTime", &fMeanTime)) return kFALSE;
   if (!l->fill("fWidth",    &fWidth))    return kFALSE;

   if (fMeanTime.GetSize() != fWidth.GetSize() ||
       fMeanTime.GetSize() != (fModules * fStrips)) {
      Error("getParams", "Wrong array sizes: fMeanTime = %i, fWidth = %i, should be = %i",
            fMeanTime.GetSize(), fWidth.GetSize(), (fModules * fStrips));
      return kFALSE;
   }

   return kTRUE;
}
