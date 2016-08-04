#ifndef HSHOWERPARROOTFILEIO_H
#define HSHOWERPARROOTFILEIO_H

#include "hdetparrootfileio.h"

class HShowerParRootFileIo : public HDetParRootFileIo {
public:
  HShowerParRootFileIo(HParRootFile* f);
  ~HShowerParRootFileIo();
  Bool_t init(HParSet*,Int_t*);

  ClassDef(HShowerParRootFileIo,0) // Class for Shower parameter I/O from ROOT file
};

#endif  /*!HSHOWERPARROOTFILEIO_H*/

