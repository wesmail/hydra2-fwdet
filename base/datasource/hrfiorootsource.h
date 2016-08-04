#ifndef HRFIOROOTSOURCE_H
#define HRFIOROOTSOURCE_H

#include "hrootsource.h"
#include "TFile.h"
#include <iostream>

class HRFIORootSource : public HRootSource {
protected:
  TString fArchive; //! Name of archive (path in server)
  TString fServer;  //! Name of GSI RFIO server (adsmexp / lxgstore)
  Bool_t access(const TString &name, Int_t mode); 
  TFile* getFile(TString name);
  TString getFileName(const Text_t file[]);
public:
  HRFIORootSource(Bool_t fPersistent=kTRUE,Bool_t fMerge=kFALSE);
  virtual ~HRFIORootSource(void);
  void setArchive(const Text_t archiveName[]);
  const Char_t* getArchive(void){return fArchive.Data();}
  Bool_t setInput(const Text_t *fileName,const Text_t *archiveName,const Text_t *treeName);
  ClassDef(HRFIORootSource,1) //Data source to read Root files from the robot.
};

#endif /* !HRFIOROOTSOURCE_H */


 

