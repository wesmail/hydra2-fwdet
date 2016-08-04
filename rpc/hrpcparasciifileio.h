#ifndef HRPCPARASCIIFILEIO_H
#define HRPCPARASCIIFILEIO_H
using namespace std;
#include <fstream>
                                                                                              
#include "TObject.h"
#include "TArrayI.h"
#include "hdetparasciifileio.h"
                                                                                              
class HParSet;
                                                                                              
class HRpcParAsciiFileIo : public HDetParAsciiFileIo {
	Int_t setSize;
public:
  HRpcParAsciiFileIo(fstream*);
  ~HRpcParAsciiFileIo() {}
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  template<class T> Bool_t read(T*);
  template<class T> Bool_t read(T*, Int_t*, Bool_t needsClear=kFALSE);
  template<class T> Int_t write(T*);
  template<class T> Int_t writeFile3(T*);
  ClassDef(HRpcParAsciiFileIo,0) // Class for RPC parameter I/O from Ascii files
};
                                                                                              
#endif  /* !HRPCPARASCIIFILEIO_H */

