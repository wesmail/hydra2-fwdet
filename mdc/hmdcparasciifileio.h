#ifndef HMDCPARASCIIFILEIO_H
#define HMDCPARASCIIFILEIO_H
using namespace std;
#include <fstream> 

#include "TObject.h"
#include "TArrayI.h"
#include "hdetparasciifileio.h"

class HParSet;

class HMdcParAsciiFileIo : public HDetParAsciiFileIo {
public:
  HMdcParAsciiFileIo(fstream*);
  ~HMdcParAsciiFileIo() {}
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  template<class T> Bool_t read(T*, Int_t*,Int_t num=24);
  template<class T> Int_t writeFile3(T*);
  template<class T> Int_t writeFile4(T*);
  template<class T> Int_t writeFile2(T*);
  template<class T> Int_t writeFile2_x(T*);
  template<class T> Int_t writeFile3_x(T*);

  ClassDef(HMdcParAsciiFileIo,0) // Class for MDC parameter I/O from Ascii files
};

#endif  /* !HMDCPARASCIIFILEIO_H */



