//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HEmcParRootFileIo
//
//  Class for EMC parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hemcparrootfileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hemcdetector.h"
#include "hemctrb3lookup.h"
#include "hemccalpar.h"
#include "hemcgeompar.h"
#include <iostream> 
#include <iomanip>

ClassImp(HEmcParRootFileIo)

HEmcParRootFileIo::HEmcParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "HEmcParIo"
  fName="HEmcParIo";
  HDetector* det=gHades->getSetup()->getDetector("Emc");
  Int_t n = det->getMaxSectors();
  initModules=new TArrayI(n);
}

HEmcParRootFileIo::~HEmcParRootFileIo(void) {
  // destructor
  if (initModules) {
    delete initModules;
    initModules=0;
  }
}

Bool_t HEmcParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // initializes a container called by name, but only the modules
  // defined in the array 'set'
  // calls the special read function for this container
  // If it is called the first time it reads the setup found in the file
  if (!isActiv) readModules("Emc");
  const Text_t* name=pPar->GetName();
  if (pFile) {
    if (0 == strncmp(name, "EmcTrb3Lookup", strlen("EmcTrb3Lookup"))) {
      return HDetParRootFileIo::read((HEmcTrb3Lookup*)pPar);
    }
    if (0 == strncmp(name, "EmcCalPar", strlen("EmcCalPar"))) {
      return read((HEmcCalPar*)pPar,set);
    }
    if (0 == strncmp(name,"EmcGeomPar", strlen("EmcGeomPar"))) {
      return HDetParRootFileIo::read((HEmcGeomPar*)pPar,set);
    }
  }
  Error("init(HParSet*,Int_t*)","Initialization of %s not possible from ASCII file",name);
  return kFALSE;
}

Bool_t HEmcParRootFileIo::read(HEmcCalPar* pPar, Int_t* set) {
  // Reads and fills the container "EmcCalPar"
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
  HEmcCalPar* rPar = (HEmcCalPar*)findContainer(name, version);
  Bool_t allFound = kTRUE;
  initModules->Reset();
  for (Int_t m = 0; m < pPar->getSize(); m++) {
    if (set[m]) {
      HEmcCalParSec& r = (*rPar)[m];
      HEmcCalParSec& p = (*pPar)[m];
      Int_t pSecSize = p.getSize();
      if (pSecSize && r.getSize() == pSecSize) {
        for (Int_t c = 0; c < pSecSize; c++) p[c].fill(r[c]);
        initModules->AddAt(m + 1, m);
      } else allFound = kFALSE;
    }
  }
  if (allFound) {
    pPar->setInputVersion(version, inputNumber);
    pPar->setChanged();
    cout << name << " initialized from ROOT file" << endl;
  }
  delete rPar;
  return allFound;
}
