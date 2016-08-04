// File: hparrootfileio.h
// Author: Ilse Koenig
//
/////////////////////////////////////////////////////////////////////////
// Interface class for parameter I/O from ROOT file                    //
/////////////////////////////////////////////////////////////////////////

#ifndef HPARROOTFILEIO_H
#define HPARROOTFILEIO_H

#include "hpario.h"
#include "TFile.h"
#include "TList.h"
#include <string.h>

class HSpectrometer;
class HRun;

class HParRootFile : public TFile {
public:
  HRun* run;     //! pointer to current run in ROOT file
  HParRootFile(const Text_t* fname,const Option_t* option="READ",
               const Text_t* ftitle="",Int_t compress=1);
  ~HParRootFile();
  HRun* getRun() {return run;}
  void readVersions(HRun*);
  ClassDef(HParRootFile,0) // ROOT file for Parameter I/O 
};


class HParRootFileIo : public HParIo {
protected:
  HParRootFile* file;  // pointer to ROOT file
public:
  HParRootFileIo();
  ~HParRootFileIo();
  Bool_t open(const Text_t* fname,const Option_t* option="READ",
              const Text_t* ftitle="",Int_t compress=1);
  void close();
  void print();
  HParRootFile* getParRootFile();
  void readVersions(HRun*);
  TList* getList();
  Bool_t check() {
    // returns kTRUE if file is open
    if (file) return file->IsOpen();
    else return kFALSE;
  }
  void cd() {
    // sets the global ROOT file pointer gFile
    if (file) file->cd();
  }
  ClassDef(HParRootFileIo,0) // Parameter I/O from ROOT files
};

#endif  /* !HPARROOTFILEIO_H */

