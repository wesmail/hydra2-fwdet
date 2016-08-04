#ifndef HLDFILESOURCE_H
#define HLDFILESOURCE_H

#include "hldfilesourcebase.h"
#include "TIterator.h"
#include "TString.h"

class HldFileSource : public HldFileSourceBase {
public:
  HldFileSource(void);
  virtual ~HldFileSource(void);
  HldFileSource(HldFileSource &so);
  Bool_t rewind();
  void setMaxEventPerFile(Int_t n) {fEventLimit = n; } 
  EDsState getNextEvent(Bool_t doUnpack=kTRUE);
  Bool_t getNextFile();
  Int_t getRunId(const Text_t *fileName);
  void addFile(const Text_t *fileName, Int_t refId=-1);
  ClassDef(HldFileSource,0) //! Data source to read LMD files;
};
#endif /* !HLDFILESOURCE_H */

