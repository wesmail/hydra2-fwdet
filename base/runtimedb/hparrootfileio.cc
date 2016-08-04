//*-- AUTHOR : Ilse Koenig
//*-- Modified : 03/02/2000 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HParRootFileIo
//
// Interface class for parameter I/O from/to ROOT file
// derived from interface base class HParIo.
//
// It contains a pointer to the "connection class" of type HParRootFile
// inherited from TFile.  HParRootFile contains an object of type HRun
// which holds the list of container versions for an event file.
//
// It contains also a list of detector interface classes all having the common
// base type HDetParRootFileIo. Every detector has its own interface class.
//
// This class replaces the former class HParFileIo.
// 
//////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "TROOT.h"  
#include "hparrootfileio.h"
#include "hdetpario.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hrun.h"
#include <stdio.h>
#include <iostream> 
#include <iomanip>

ClassImp(HParRootFile)
ClassImp(HParRootFileIo)

HParRootFile::HParRootFile(const Text_t* fname,const Option_t* option,
                           const Text_t* ftitle, Int_t compress)
              : TFile(fname,option,ftitle,compress) {
  // constructor opens a ROOT file
  run=0;
}

HParRootFile::~HParRootFile() {
  // destructor
  if (run) delete run;
  run=0;
}

void HParRootFile::readVersions(HRun* currentRun) {
  // finds the current run containing the parameter container versions
  // in the ROOT file
  if (run) delete run;
  run=(HRun*)Get(((Char_t*)currentRun->GetName()));
}



HParRootFileIo::HParRootFileIo() {
  // constructor
  file=0;
}

HParRootFileIo::~HParRootFileIo() {
  // destructor closes an open file
  close();
}

Bool_t HParRootFileIo::open(const Text_t* fname,const Option_t* option,
                        const Text_t* ftitle, Int_t compress) {
  // It opens a ROOT file (default option "READ"). An open file will be closed.
  // The detector I/Os for all detectors defined in the setup are activated.
  close();
  file=new HParRootFile(fname,option,ftitle,compress);
  if (file && file->IsOpen()) {
    inputName=fname;
    gHades->getSetup()->activateParIo(this);
    return kTRUE;
  }
  return kFALSE;
}

void HParRootFileIo::close() {
  // closes an open ROOT file and deletes the detector I/Os
  if (file) {
    file->Close();
    delete file;
    file=0;
    inputName="";
  }
  if (detParIoList) detParIoList->Delete();
}

void HParRootFileIo::print() {
  // prints the content of a open ROOT file and the list of detector I/Os
  if (file) {
    file->ls();
    TIter next(detParIoList);
    HDetParIo* io;
    cout<<"detector I/Os: ";
    while ((io=(HDetParIo*)next())) {
      cout<<" "<<io->GetName();
    }
    cout<<'\n';
  }
  else cout<<"No pointer to ROOT file"<<'\n';
}

HParRootFile* HParRootFileIo::getParRootFile() {
  // returns a pointer to the current ROOT file
  return file;
}

void HParRootFileIo::readVersions(HRun* currentRun) {
  // reads the parameter container versions for the current run from
  // the ROOT file
  if (file) file->readVersions(currentRun);
}

TList* HParRootFileIo::getList() {
  // returns the list of keys found in the ROOT file
  if (file) return file->GetListOfKeys();
  return 0;
}
