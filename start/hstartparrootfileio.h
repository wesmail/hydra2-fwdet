#ifndef HSTARTPARROOTFILEIO_H
#define HSTARTPARROOTFILEIO_H

#include "hdetparrootfileio.h"

class HParRootFile;
class HParSet;
class HStart2Calpar;

class HStartParRootFileIo : public HDetParRootFileIo {
public:
   HStartParRootFileIo(HParRootFile* f);
   ~HStartParRootFileIo();
   Bool_t init(HParSet*, Int_t*);
   Bool_t read(HStart2Calpar*, Int_t*);
   ClassDef(HStartParRootFileIo, 0) // Class for START parameter I/O from ROOT file
};

#endif  /* !HSTARTPARROOTFILEIO_H */

