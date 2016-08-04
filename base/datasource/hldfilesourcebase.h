#ifndef HLDFILESOURCEBASE_H
#define HLDFILESOURCEBASE_H

#include "hldsource.h"
#include "TIterator.h"
#include "TString.h"

class HldFileDesc : public TObject {
  TString fName; // File name
  Int_t fRunId;  // RunId
  Int_t fRefId;  // Reference run Id for initialization
  HldFileDesc(void) : fRunId(-1) {}
 public:
  HldFileDesc(const Text_t *name,const Int_t runId,const Int_t refId=-1) : fName(name),fRunId(runId) {
    fRefId=refId;
  }
  ~HldFileDesc(void) {}
  Text_t const *GetName(void) const { return fName.Data(); }
  Int_t getRunId(void) { return fRunId; }
  Int_t getRefId(void) { return fRefId; }
  void setRefId(Int_t r) { fRefId=r; }
  ClassDef(HldFileDesc,0) // File descriptor for Hld source
};

class HldFileSourceBase : public HldSource {
protected:
  TList fFileList;  // List with all files to be analyzed
  HldFileDesc *fCurrentFile; // Pointer to current file.
  TString fCurrentDir; //Current directory for addFile
  TIterator* iter;  // Iterator over list of files
  Int_t fEventNr, fEventLimit; //Event counter and maximun event nr per file
public:
  HldFileSourceBase(void);
  virtual ~HldFileSourceBase(void);
  HldFileSourceBase(HldFileSourceBase &so);
  Bool_t init(void);
  virtual Bool_t rewind()=0;
  void setMaxEventPerFile(Int_t n) {fEventLimit = n; } 
  EDsState getNextEvent(Bool_t doUnpack=kTRUE);
  virtual Bool_t getNextFile()=0;
  Int_t getCurrentRunId(void);
  Int_t getCurrentRefId(void);
  TList *getListOfFiles(void){return &fFileList;};
  virtual Int_t getRunId(const Text_t *fileName)=0;
  Text_t const *getCurrentFileName(void) {return fCurrentFile->GetName();}
  void addFile(const Text_t *fileName,const Text_t *refFile);
  virtual void addFile(const Text_t *fileName, Int_t refId=-1)=0;
  void setDirectory(const Text_t *direc) {fCurrentDir = direc;}
  ClassDef(HldFileSourceBase,0) //! Data source to read HLD files;
};
#endif /* !HLDFILESOURCEBASE_H */

