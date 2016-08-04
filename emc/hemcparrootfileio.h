#ifndef HEMCPARROOTFILEIO_H
#define HEMCPARROOTFILEIO_H
                                                                                              
#include "hdetparrootfileio.h"
                                                                                              
class HParRootFile;
class HParSet;
class HEmcCalPar;
                                                                                              
class HEmcParRootFileIo : public HDetParRootFileIo {
public:
  HEmcParRootFileIo(HParRootFile* f);
  ~HEmcParRootFileIo();
  Bool_t init(HParSet*,Int_t*);
  Bool_t read(HEmcCalPar*, Int_t*);
  ClassDef(HEmcParRootFileIo,1) // Class for EMC parameter I/O from ROOT file
};
                                                                                              
#endif  /*!HEMCPARROOTFILEIO_H*/

