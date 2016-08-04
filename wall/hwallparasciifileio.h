#ifndef HWALLPARASCIIFILEIO_H
#define HWALLPARASCIIFILEIO_H

using namespace std;
#include "TObject.h"
#include "TArrayI.h"
#include "hdetparasciifileio.h"
#include <fstream> 

class HParSet;

class HWallParAsciiFileIo : public HDetParAsciiFileIo {
public:
  HWallParAsciiFileIo(fstream*);
  ~HWallParAsciiFileIo(void) {}
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  template<class T> Bool_t read(T*);
  template<class T> Bool_t read(T*, Int_t*);
  template<class T> Int_t write(T*);
  template<class T> Int_t writeFile1(T*);
  template<class T> Int_t writeFile3(T*);
  ClassDef(HWallParAsciiFileIo,0) // Class for Forward Wall parameter I/O from Ascii files
};

#endif  /* !HWALLPARASCIIFILEIO_H */
