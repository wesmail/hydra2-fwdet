//////////////////////////////////////////////////////////////////////////////
//
// @(#)hydraTrans/richNew:$Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichParAsciiFileIo
//
//  For plain i/o ascii. Used only partially in RICH.
//
//////////////////////////////////////////////////////////////////////////////


#include "hdetparasciifileio.h"
#include "hparset.h"
#include "hrichcalpar.h"
#include "hrichmappingpar.h"
#include "hrichparasciifileio.h"
#include "hrichthresholdpar.h"

#include <fstream>

using namespace std;

ClassImp(HRichParAsciiFileIo)

HRichParAsciiFileIo::HRichParAsciiFileIo(fstream* f)
   : HDetParAsciiFileIo(f)
{
   fName = "HRichParIo";
}

Bool_t HRichParAsciiFileIo::init(HParSet* pPar, Int_t* set)
{
   // calls the appropriate read function for the container
   const Text_t* name = pPar->GetName();
   if (NULL != pFile) {
      if (0 == strncmp(name, "RichCalPar", strlen("RichCalPar"))) {
         return readFile<HRichCalPar>((HRichCalPar*)pPar);
      }
      if (0 == strncmp(name, "RichMappingParameters", strlen("RichMappingParameters"))) {
         return readFile<HRichMappingPar>((HRichMappingPar*)pPar);
      }
      if (0 == strncmp(name, "RichThresholdParameters", strlen("RichThresholdParameters"))) {
         return readFile<HRichThresholdPar>((HRichThresholdPar*)pPar);
      }
   }
   Error("init", "Initialization of %s not possible from ASCII file!", name);
   return kFALSE;

}

Int_t HRichParAsciiFileIo::write(HParSet* pPar)
{
   // calls the appropriate write function for the container

   const Text_t* name = pPar->GetName();
   if (NULL != pFile) {
      if (0 == strncmp(name, "RichCalPar", strlen("RichCalPar"))) {
         return writeFile<HRichCalPar>((HRichCalPar*)pPar);
      }
      if (0 == strncmp(name, "RichMappingParameters", strlen("RichMappingParameters"))) {
         return writeFile<HRichMappingPar>((HRichMappingPar*)pPar);
      }
      if (0 == strncmp(name, "RichThresholdParameters", strlen("RichThresholdParameters"))) {
         return writeFile<HRichThresholdPar>((HRichThresholdPar*)pPar);
      }
   }
   Error("write", "%s could not be written to ASCII file", name);
   return kFALSE;
}
