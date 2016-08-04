#ifndef HDETPARROOTFILEIO_H
#define HDETPARROOTFILEIO_H

#include "hdetpario.h"
#include "TFile.h"
#include "TArrayI.h"

class HParRootFile;
class HDetector;
class HParSet;
class HDetGeomPar;

class HDetParRootFileIo : public HDetParIo {
protected:
  HParRootFile* pFile;    // pointer to parameter ROOT file
  TArrayI* modulesFound;  // array of modules found in the Root file
  TArrayI* initModules;   // array of modules initialized from Root file
  Bool_t isActiv;         // flag is set kTRUE by function readModules()
public:
  HDetParRootFileIo(HParRootFile* f);
  virtual ~HDetParRootFileIo();
  virtual Bool_t read(HParSet*);
  Int_t write(HParSet*);
  Bool_t write(HDetector*);
  void printInfo(const Text_t* msg);
  void resetActivFlag(Bool_t f=kFALSE) {isActiv=f;}
  Bool_t read(HDetGeomPar*,Int_t*);
protected:
  Int_t readModules(const Text_t* detName);
  Int_t findInputVersion(const Text_t* contName);
  Int_t getMaxVersion(const Text_t* contName);
  TObject* findContainer(const Text_t* contName, Int_t version);

  ClassDef(HDetParRootFileIo,0) // detector base class for parameter I/O from ROOT file
};

#endif  /* !HDETPARROOTFILEIO_H */

