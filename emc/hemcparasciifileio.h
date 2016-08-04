#ifndef HEMCPARASCIIFILEIO_H
#define HEMCPARASCIIFILEIO_H

using namespace std;
#include <fstream>
#include "hdetparasciifileio.h"
                                                                                              
class HParSet;
                                                                                              
class HEmcParAsciiFileIo : public HDetParAsciiFileIo {
public:
  HEmcParAsciiFileIo(fstream*);
  ~HEmcParAsciiFileIo() {}
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  template<class T> Bool_t read(T*, Int_t*);
  ClassDef(HEmcParAsciiFileIo,0) // Class for EMC parameter I/O from Ascii files
};
                                                                                              
#endif  /* !HEMCPARASCIIFILEIO_H */

