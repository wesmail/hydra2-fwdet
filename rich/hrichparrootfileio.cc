//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichParRootFileIo
//
//  I/O for root files.
//
//////////////////////////////////////////////////////////////////////////////


#include "hparrootfileio.h"
#include "hparset.h"
#include "hrichparrootfileio.h"
#include "richdef.h"

using namespace std;


ClassImp(HRichParRootFileIo)

HRichParRootFileIo::HRichParRootFileIo(HParRootFile* f)
   : HDetParRootFileIo(f)
{
   // constructor sets the name of the detector I/O "HRichParIo"
   fName = "HRichParIo";
   initModules = new TArrayI(RICH_MAX_SECTORS);
}

HRichParRootFileIo::~HRichParRootFileIo()
{
   if (NULL != modulesFound) {
      delete modulesFound;
      modulesFound = NULL;
   }
   if (NULL != initModules) {
      delete initModules;
      initModules = NULL;
   }
}

Bool_t
HRichParRootFileIo::init(HParSet* pPar, Int_t* set)
{
   // initializes a container called by name, but only the modules
   // defined in the array 'set'
   // calls the special read function for this container
   // If it is called the first time it reads the setup found in the file

   if (kFALSE == isActiv) {
      readModules("Rich");
   }
   const Text_t* name = pPar->GetName();
   if (NULL != pFile) {
      if (0 == strncmp(name, "RichGeometryParameters", strlen("RichGeometryParameters"))) {
         return read(pPar);
      }
      if (0 == strncmp(name, "RichCalPar", strlen("RichCalPar"))) {
         return read(pPar);
      }
      if (0 == strncmp(name, "RichMappingParameters", strlen("RichMappingParameters"))) {
         return read(pPar);
      }
   }
   Error("init", "initialization of %s not possible from ROOT file!", name);
   return kFALSE;
}
