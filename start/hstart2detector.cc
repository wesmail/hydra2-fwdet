//*-- AUTHOR : Rainer Schicker
//*-- modified : 20/07/2003 by T.Wojcik
//*-- modified : 18/12/2001 by I.Koenig
//*-- modified : 02/03/2000 by I.Koenig

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////
//
//  HStart2Detector
//
//  Class for detector "Start2" using the TRB for readout
//  stores the setup in a linear array of maximum number
//  of modules defined in the hstartdef.h definition file.
//  At the moment:
//    - 1st module: start detector
//    - 2nd module: veto detector ????
//    - 3rd module: halo detector ????
//////////////////////////////////////////////////////////////////

#include "TClass.h"

#include "hades.h"
#include "hcategory.h"
#include "hdetpario.h"
#include "hevent.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hparasciifileio.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hstart2detector.h"
#include "hstartparasciifileio.h"
#include "hstartparrootfileio.h"

using namespace std;

ClassImp(HStart2Detector)

HStart2Detector::HStart2Detector()
{
   // constructor
   fName         = "Start";
   maxSectors    = START2_MAX_SECTORS;
   maxModules    = START2_MAX_MODULES;
   maxComponents = START2_MAX_COMPONENTS;
   modules       = new TArrayI(maxModules);

   for (Int_t i = 0; i < maxModules; i++) {
      modules->AddAt(0, i);
   }
}


HStart2Detector::~HStart2Detector()
{
   // destructor
   if (NULL != modules) {
      delete modules;
      modules = NULL;
   }
}


Bool_t HStart2Detector::init(void)
{
   // nothing to be done
   return kTRUE;
}


void HStart2Detector::activateParIo(HParIo* io)
{
   // activates the input/output class for the parameters
   // needed by the Start2
   if (strcmp(io->IsA()->GetName(), "HParOraIo") == 0) {
      io->setDetParIo("HStartParIo");
      return;
   }
   if (strcmp(io->IsA()->GetName(), "HParRootFileIo") == 0) {
      HStartParRootFileIo* p = new HStartParRootFileIo(((HParRootFileIo*)io)->getParRootFile());
      io->setDetParIo(p);
   }
   if (strcmp(io->IsA()->GetName(), "HParAsciiFileIo") == 0) {
      HStartParAsciiFileIo* p = new HStartParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
      io->setDetParIo(p);
   }
}


Bool_t HStart2Detector::write(HParIo* output)
{
   // writes the Start2 setup to output
   HDetParIo* out = output->getDetParIo("HStartParIo");
   if (out) return out->write(this);
   return kFALSE;
}


HCategory* HStart2Detector::buildMatrixCategory(const Text_t* className, Float_t fillRate)
{
   Int_t maxMod = getMaxModInSetup();
   if (maxMod == 0) return 0;
   Int_t* sizes = new Int_t[2]; // 2 levels
   sizes[0] = maxMod;
   sizes[1] = maxComponents;
   HMatrixCategory* category = new HMatrixCategory(className, 2, sizes, fillRate);
   delete [] sizes;
   return category;
}


HCategory* HStart2Detector::buildLinearCategory(const Text_t* className)
{
   Int_t size = getMaxModInSetup() * maxComponents;
   if (size) {
      HLinearCategory* category = new HLinearCategory(className, size);
      return category;
   }
   return 0;
}


HCategory* HStart2Detector::buildCategory(Cat_t cat)
{
   // gets the category if existing
   // builts and adds if not existing
   // returns the pointer to the category or zero
   HCategory *pcat;
   pcat = gHades->getCurrentEvent()->getCategory(cat);
   if (pcat) return(pcat);  // already existing
   switch (cat) {
      case catStart2Raw:
         pcat = buildMatrixCategory("HStart2Raw", 0.5);
         break;
      case catStart2Cal:
         pcat = buildMatrixCategory("HStart2Cal", 0.5);
         break;
      case catStart2Hit:
         pcat = buildLinearCategory("HStart2Hit");
         break;
      default:
         return NULL;
   }
   if (pcat) gHades->getCurrentEvent()->addCategory(cat, pcat, "Start");
   return(pcat);
}


Int_t HStart2Detector::getMaxModInSetup(void)
{
   Int_t maxMod = -1;
   for (Int_t i = 0; i < maxModules; i++) {
      if (modules->At(i)) maxMod = (i > maxMod) ? i : maxMod;
   }
   maxMod++;
   return maxMod;
}





