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

#include "hrich700trb3lookup.h"
#include "hrich700thresholdpar.h"
#include "hrich700geompar.h"


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
      if (0 == strncmp(name, "Rich700Trb3Lookup", strlen("Rich700Trb3Lookup"))) {
	  return readFile<HRich700Trb3Lookup>((HRich700Trb3Lookup*)pPar);
      }
      if (0 == strncmp(name, "Rich700ThresholdPar", strlen("Rich700ThresholdPar"))) {
	  return readFile<HRich700ThresholdPar>((HRich700ThresholdPar*)pPar);
      }
      if (0 == strncmp(name,"Rich700GeomPar", strlen("Rich700GeomPar"))){
	  return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
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
      if (0 == strncmp(name, "Rich700Trb3Lookup", strlen("Rich700Trb3Lookup"))) {
    	  return writeFile<HRich700Trb3Lookup>((HRich700Trb3Lookup*)pPar);
      }

      if (0 == strncmp(name, "Rich700ThresholdPar", strlen("Rich700ThresholdPar"))) {
	  return writeFile<HRich700ThresholdPar>((HRich700ThresholdPar*)pPar);
      }

      if (0 == strncmp(name,"Rich700GeomPar", strlen("Rich700GeomPar"))){
	  return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
      }
   }
   Error("write", "%s could not be written to ASCII file", name);
   return kFALSE;
}
