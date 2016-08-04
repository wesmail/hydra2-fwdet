//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////
//  HPionTrackerParRootFileIo
//
//  Class for PionTracker parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
////////////////////////////////////////////////////////////////////

using namespace std;
#include "hpiontrackerparrootfileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hpiontrackerdetector.h"
#include "hparset.h"
#include "hpiontrackertrb3lookup.h"
#include "hpiontrackercalpar.h"
#include <iostream>
#include <iomanip>

ClassImp(HPionTrackerParRootFileIo)

HPionTrackerParRootFileIo::HPionTrackerParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "HPionTrackerParIo"
  fName = "HPionTrackerParIo";
  Int_t n = gHades->getSetup()->getDetector("PionTracker")->getMaxModules();
  initModules = new TArrayI(n);
}

HPionTrackerParRootFileIo::~HPionTrackerParRootFileIo() {
  // destructor
  if (modulesFound) {
    delete modulesFound;
    modulesFound = 0;
  }
}

Bool_t HPionTrackerParRootFileIo::init(HParSet* pPar, Int_t* set) {
  // Initializes a container called by name, but only the modules defined
  //   in the array "set".
  // Calls the special read function for this container.
  // If it is called the first time, it reads the setup found in the file.
  if (!isActiv) readModules("PionTracker");
  const Text_t* name = pPar->GetName();
  if (NULL != pFile) {
    if (0 == strncmp(name, "PionTrackerTrb3Lookup", strlen("PionTrackerTrb3Lookup"))) {
      return HDetParRootFileIo::read(pPar);
    }
    if (0 == strncmp(name, "PionTrackerCalPar", strlen("PionTrackerCalPar"))) {
      return read((HPionTrackerCalPar*)pPar, set);
    }
  }
  Error("init(HParSet*,Int_t*)","Initialization of %s not possible from ROOT file",name);
  return kFALSE;
}

Bool_t HPionTrackerParRootFileIo::read(HPionTrackerCalPar* pPar, Int_t* set) {
  // Reads and fills the container "PionTrackerCalPar"
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
  HPionTrackerCalPar* rPar = (HPionTrackerCalPar*)findContainer(name, version);
  Bool_t allFound = kTRUE;
  initModules->Reset();
  for (Int_t m = 0; m < pPar->getSize(); m++) {
    if (set[m]) {
      HPionTrackerCalParMod& r = (*rPar)[m];
      HPionTrackerCalParMod& p = (*pPar)[m];
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
  delete rPar;
  return allFound;
}
