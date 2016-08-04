#ifndef HCONDPARASCIIFILEIO_H
#define HCONDPARASCIIFILEIO_H

using namespace std;
#include <fstream>
#include "hdetparasciifileio.h"

class HParSet;
class HParCond;

class HCondParAsciiFileIo : public HDetParAsciiFileIo {
private:
   template <class type> UChar_t* readData(type, const Char_t*, TString&, Int_t&);
   template <class type> void writeData(type*, Int_t);
   void writeHexData(UInt_t*, Int_t);
public:
  HCondParAsciiFileIo(fstream* f=0);
  ~HCondParAsciiFileIo() {}
  Bool_t init(HParSet*,Int_t*);
  Int_t write(HParSet*);
  Bool_t readCond(HParCond* pPar);
  Int_t writeCond(HParCond* pPar);
  ClassDef(HCondParAsciiFileIo,0) // I/O from Ascii file for parameter containers derived from HParCond
};

#endif  /* !HCONDPARASCIIFILEIO_H */
