#ifndef HCONDPARROOTFILEIO_H
#define HCONDPARROOTFILEIO_H

#include "hdetparrootfileio.h"

class HParRootFile;
class HParSet;

class HCondParRootFileIo : public HDetParRootFileIo {
public:
  HCondParRootFileIo(HParRootFile* f=0);
  ~HCondParRootFileIo() {}
  Bool_t init(HParSet*,Int_t*);
  ClassDef(HCondParRootFileIo,0) // I/O from ROOT file for parameter containers derived from HParCond
};

#endif  /* !HCONDPARROOTFILEIO_H */
