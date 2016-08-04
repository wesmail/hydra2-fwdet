#ifndef HTOFPARASCIIFILEIO_H
#define HTOFPARASCIIFILEIO_H
using namespace std;
#include <fstream> 

#include "TObject.h"
#include "TArrayI.h"
#include "hdetparasciifileio.h"

class HParSet;

class HTofParAsciiFileIo : public HDetParAsciiFileIo {
  Int_t setSize;
public:
  HTofParAsciiFileIo(fstream*);
  ~HTofParAsciiFileIo() {}
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  template<class T> Bool_t read(T*);
  template<class T> Bool_t read(T*, Int_t*, Bool_t needsClear=kFALSE);
  template<class T> Int_t write(T* pPar);
  template<class T> Int_t writeFile3(T*);
  ClassDef(HTofParAsciiFileIo,0) // Class for Tof parameter I/O from Ascii files
};

#endif  /* !HTOFPARASCIIFILEIO_H */
