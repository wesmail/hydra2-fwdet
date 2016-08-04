//*-- AUTHOR : Rainer Schicker
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////
//  HTBoxDetector
//
//  class for detector TBox
//  stores the setup in an array of 1 module;
//////////////////////////////////////////////////////////////////

#include "hades.h"
#include "hcategory.h"
#include "hdetpario.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hparasciifileio.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hruntimedb.h"
#include "hstartdef.h"
#include "htboxdetector.h"

#include <stdio.h>

ClassImp(HTBoxDetector)

HTBoxDetector::HTBoxDetector()
{
   // constructor
   fName = "TBox";
   maxSectors = -1;
   maxModules = 1;
   maxComponents = 8;
   modules = new TArrayI(maxModules);
}


HTBoxDetector::HTBoxDetector(Int_t mods, Int_t chan)
{
   // constructor
   fName = "TBox";
   maxSectors = -1;
   maxModules = mods;
   maxComponents = chan;
   modules = new TArrayI(maxModules);
}


HTBoxDetector::~HTBoxDetector()
{
   // destructor
   if (modules) delete modules;
   modules = 0;
}


Bool_t HTBoxDetector::init(void)
{
   // nothing to be done
   return kTRUE;
}

HCategory* HTBoxDetector::buildMatrixCategory(const Text_t* className)
{
   Int_t maxMod = getMaxModInSetup();
   if (maxMod == 0) return 0;
   Int_t* sizes = new Int_t[2]; // 2 levels
   sizes[0] = maxMod;
   sizes[1] = maxComponents;
   HMatrixCategory* category = new HMatrixCategory(className, 2, sizes, 1.0);
   delete [] sizes;
   return category;
}


HCategory* HTBoxDetector::buildLinearCategory(const Text_t* className)
{
   Int_t size = getMaxModInSetup() * maxComponents;
   if (size) {
      HLinearCategory* category = new HLinearCategory(className, size);
      return category;
   }
   printf("TBox linear category could not be built\n");
   return NULL;
}


HCategory* HTBoxDetector::buildCategory(Cat_t cat)
{
   // builds the default categories (here MatrixCategories)
   switch (cat) {
      case catTBoxChan    :
         return buildLinearCategory("HTBoxChan");
      default :
         return NULL;
   }
}

Int_t HTBoxDetector::getMaxModInSetup(void)
{
   Int_t maxMod = -1;
   for (Int_t i = 0; i < maxModules; i++) {
      if (modules->At(i)) maxMod = (i > maxMod) ? i : maxMod;
   }
   maxMod++;
   return maxMod;
}
