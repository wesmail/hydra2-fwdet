#ifndef HSHOWERPARASCIIFILEIO_H
#define HSHOWERPARASCIIFILEIO_H

using namespace std;
#include "hdetparasciifileio.h"
#include <fstream> 

class HParSet;

class HShowerParAsciiFileIo : public HDetParAsciiFileIo {
public:
  HShowerParAsciiFileIo(fstream* f);
  ~HShowerParAsciiFileIo() {}

  // calls special read-function for each container type
  Bool_t init(HParSet*,Int_t*);

  // calls special update-function for each container type
  Int_t write(HParSet*);

  ClassDef(HShowerParAsciiFileIo,0) 
};

#endif  /* !HSHOWERPARASCIIFILEIO_H */
