//_HADES_CLASS_DESCRIPTION
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
// HEmcParAsciiFileIo
//
// Class for EMC parameter input/output from/into Ascii file
//
/////////////////////////////////////////////////////////////
                                                                                              
#include "hemcparasciifileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hparset.h"
#include "hemcdetector.h"
#include "hemctrb3lookup.h"
#include "hemccalpar.h"
#include "hemcgeompar.h"
    
ClassImp(HEmcParAsciiFileIo)
                                                                                              
HEmcParAsciiFileIo::HEmcParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f) {
  // constructor calls the base class constructor
  fName="HEmcParIo";
}
                                                                                              
Bool_t HEmcParAsciiFileIo::init(HParSet* pPar,Int_t* set) {
  // calls the appropriate read function for the container
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    if (0 == strncmp(name, "EmcTrb3Lookup", strlen("EmcTrb3Lookup"))) {
      return HDetParAsciiFileIo::readFile<HEmcTrb3Lookup>((HEmcTrb3Lookup*)pPar);
    }
    if (0 == strncmp(name, "EmcCalPar", strlen("EmcCalPar"))) {
      return read((HEmcCalPar*)pPar,set);
    }
    if (0 == strncmp(name,"EmcGeomPar", strlen("EmcGeomPar"))) {
      return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
    }
    Error("init(HParSet*,Int_t*)","Initialization of %s not possible from ASCII file",name);
    return kFALSE;
  }
  Error("init(HParSet*,Int_t*)","No input file open");
  return kFALSE;
}
                                                                                              
Int_t HEmcParAsciiFileIo::write(HParSet* pPar) {
  // calls the appropriate write function for the container
  if (pFile) {
    const  Text_t* name=pPar->GetName();
    if (0 == strncmp(name, "EmcTrb3Lookup", strlen("EmcTrb3Lookup"))) {
      return HDetParAsciiFileIo::writeFile<HEmcTrb3Lookup>((HEmcTrb3Lookup*)pPar);
    }
    if (0 == strncmp(name, "EmcCalPar", strlen("EmcCalPar"))) {
      return HDetParAsciiFileIo::writeFile<HEmcCalPar>((HEmcCalPar*)pPar);
    }
    if (0 == strncmp(name,"EmcGeomPar", strlen("EmcGeomPar"))) {
      return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
    }
    Error("write(HParSet*)","%s could not be written to ASCII file",name);
    return -1;
  }
  Error("write(HParSet*)","No output file open");
  return -1;
}
                                                                                              
template<class T> Bool_t HEmcParAsciiFileIo::read(T* pPar, Int_t* set) {
  // template function for all parameter containers
  // searches the container in the file, reads the data line by line and
  //   calles the member function readline(...) of the container class
  const Text_t* name = pPar->GetName();
  HDetector* det = gHades->getSetup()->getDetector("Emc");
  Int_t nSize = det->getMaxModules();
  if (!findContainer(name)) return kFALSE;
  pPar->clear();
  const Int_t maxbuf = 155;
  Text_t buf[maxbuf];
  while (!pFile->eof()) {
    pFile->getline(buf, maxbuf);
    if (buf[0] == '#') break;
    if (buf[0] != '/' && buf[0] != '\0') pPar->readline(buf, set);
  }
  Bool_t allFound = kTRUE;
  for (Int_t i = 0; i < nSize; i++) {
    if (set[i]) {
      if (set[i] == 999) set[i] = 1;
      else allFound = kFALSE;
    }
  }
  if (allFound) {
    pPar->setInputVersion(1, inputNumber);
    pPar->setChanged();
    Info("readFile", "%s initialized from Ascii file", pPar->GetName());
  }
  return allFound;
}


