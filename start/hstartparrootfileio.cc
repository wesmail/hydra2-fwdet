//*-- AUTHOR : Ilse Koenig
//*-- Modified : 03/12/2009 by I.Koenig

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HStartParRootFileIo
//
//  Class for Start parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////

#include "hades.h"
#include "hparrootfileio.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hstart2calpar.h"
#include "hstart2detector.h"
#include "hstart2trb2lookup.h"
#include "hstart2geompar.h"
#include "hstart2trb3lookup.h"
#include "hstart2trb3calpar.h"
#include "hstartparrootfileio.h"

#include <iostream>

using namespace std;

ClassImp(HStartParRootFileIo)

HStartParRootFileIo::HStartParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f)
{
   // constructor sets the name of the detector I/O "HStartParIo"
   fName = "HStartParIo";
   Int_t n = gHades->getSetup()->getDetector("Start")->getMaxModules();
   initModules = new TArrayI(n);
}


HStartParRootFileIo::~HStartParRootFileIo()
{
   // destructor
   if (modulesFound) {
      delete modulesFound;
      modulesFound = 0;
   }
   if (initModules) {
      delete initModules;
      initModules = 0;
   }
}

Bool_t HStartParRootFileIo::init(HParSet* pPar, Int_t* set)
{
   // Initializes a container called by name, but only the modules defined
   //   in the array 'set.
   // Calls the special read function for this container.
   // If it is called the first time, it reads the setup found in the file.
   if (!isActiv) readModules("Start");
   const Text_t* name = pPar->GetName();
   if (pFile) {
      if (strcmp(name, "Start2Trb2Lookup") == 0) return HDetParRootFileIo::read(pPar);
      if (strcmp(name, "Start2Calpar") == 0)     return read((HStart2Calpar*)pPar, set);
      if (strcmp(name, "Start2GeomPar")==0)
           return HDetParRootFileIo::read((HStart2GeomPar*)pPar,set);
      if (strcmp(name, "Start2Trb3Lookup") == 0) return HDetParRootFileIo::read(pPar);
      if (strcmp(name, "Start2Trb3Calpar") == 0) return HDetParRootFileIo::read(pPar);
   }
   cerr << "initialization of " << name << " not possible from ROOT file!" << endl;
   return kFALSE;
}

Bool_t HStartParRootFileIo::read(HStart2Calpar* pPar, Int_t* set)
{
   // Reads and fills the container "Start2CalPar"
   Text_t* name = (Char_t*)pPar->GetName();
   Int_t version = findInputVersion(name);
   if (version <= 0) {
      pPar->setInputVersion(-1, inputNumber);
      return kFALSE;    // not in ROOT file
   }
   if (pPar->getInputVersion(inputNumber) == version
       && pPar->getInputVersion(inputNumber) != -1) return kTRUE;
   // needs reinitialization
   pPar->clear();
   HStart2Calpar* rStart = (HStart2Calpar*)findContainer(name, version);
   Bool_t allFound = kTRUE;
   initModules->Reset();
   for (Int_t m = 0; m < pPar->getSize(); m++) {
      if (set[m]) {
         HStart2CalparMod& r = (*rStart)[m];
         HStart2CalparMod& p = (*pPar)[m];
         Int_t pModSize = p.getSize();
         if (pModSize && r.getSize() == pModSize) {
            for (Int_t c = 0; c < pModSize; c++) p[c].fill(r[c]);
            initModules->AddAt(m + 1, m);
         } else allFound = kFALSE;
      }
   }
   if (allFound) {
      pPar->setInputVersion(version, inputNumber);
      pPar->setChanged();
      cout << name << " initialized from ROOT file" << endl;
   }
   delete rStart;
   return allFound;
}

