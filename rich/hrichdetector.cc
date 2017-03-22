//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichDetector
//
//
//
//////////////////////////////////////////////////////////////////////////////


#include "TClass.h"

#include "hades.h"
#include "hcategory.h"
#include "hdetpario.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hparasciifileio.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hrichanalysis.h"
#include "hrichdetector.h"
#include "hrichparasciifileio.h"
#include "hrichparrootfileio.h"
#include "htaskset.h"
#include "richdef.h"

#include <string.h>

using namespace std;

ClassImp(HRichDetector)

HRichDetector::HRichDetector(void) : HDetector("Rich", "The RICH detector")
{

   maxModules = 1;
   modules    = new TArrayI(RICH_MAX_SECTORS);

}

HTask* HRichDetector::buildTask(const Text_t*   name,
                         const Option_t* opt)
{
   if (NULL != name && 0 == strcmp(name, "default")) {
      name = NULL;
   }

   HTaskSet* pDetTasks = new HTaskSet("Rich", "List of Rich tasks");

   if (NULL == name) {
      //default list of tasks
      pDetTasks->add(new HRichAnalysis("rich.ana", "Ring finder", kFALSE));
      return pDetTasks;
   }

   return HDetector::buildTask(name, opt);
}

HCategory* HRichDetector::buildLinearCat(const Text_t* classname,
                              const Int_t size)
{
   HLinearCategory* category = NULL;

   category = new HLinearCategory(classname, size);
   return category;
}

HCategory* HRichDetector::buildMatrixCat(const Text_t* classname,
                              const Float_t fillRate)
{
   Int_t            sizes[3];
   HMatrixCategory* category = NULL;

   sizes[0] = RICH_MAX_SECTORS;
   sizes[1] = RICH700_MAX_ROWS;
   sizes[2] = RICH700_MAX_COLS;

   category = new HMatrixCategory(classname, 3, sizes, fillRate);
   return category;
}

HCategory* HRichDetector::buildSimMatrixCat(const Text_t* classname,
                                 const Float_t fillRate,
                                 const Int_t   size)
{

   HMatrixCategory *category = NULL;
   Int_t ini[2];

   ini[0] = RICH_MAX_SECTORS;
   ini[1] = size;

   category = new HMatrixCategory(classname, 2, ini, fillRate);
   return category;
}

HCategory* HRichDetector::buildCategory(Cat_t cat)
{
   switch (cat) {
      case catRichCal             :
         return buildMatrixCat("HRichCal", 1);
      case catRichHit             :
         return buildLinearCat("HRichHit");
      case catRichTrack           :
         return buildLinearCat("HRichTrack");
      case catRichDirClus         :
         return buildMatrixCat("HRichDirClus", 0.5);
      case catRichHitHdr          :
         return buildLinearCat("HRichHitHeader");
      default:
         return NULL;
   }
}

void HRichDetector::activateParIo(HParIo* io)
{
// Initialization according to Ilse's scheme
// activates the input/output class for the parameters
// needed by the Rich


   if (0 == strncmp(io->IsA()->GetName(), "HOraIo", strlen("HOraIo"))) {
      io->setDetParIo("HRichParIo");
      return;
   }

   if (0 == strncmp(io->IsA()->GetName(), "HParRootFileIo", strlen("HParRootFileIo"))) {
      HRichParRootFileIo* p = NULL;
      p = new HRichParRootFileIo(static_cast<HParRootFileIo*>(io)->getParRootFile());
      io->setDetParIo(p);
   }

   if (0 == strncmp(io->IsA()->GetName(), "HParAsciiFileIo", strlen("HParAsciiFileIo"))) {
      HRichParAsciiFileIo* p = NULL;
      p = new HRichParAsciiFileIo(static_cast<HParAsciiFileIo*>(io)->getFile());
      io->setDetParIo(p);
   }

}

Bool_t HRichDetector::write(HParIo* output)
{
// Writes the Rich setup to output

   HDetParIo* out = NULL;
   out  = output->getDetParIo("HRichParIo");
   if (NULL != out)
      return out->write(this);
   return kFALSE;
}
