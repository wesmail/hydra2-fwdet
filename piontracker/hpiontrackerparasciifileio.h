#ifndef HPIONTRACKERPARASCIIFILEIO_H
#define HPIONTRACKERPARASCIIFILEIO_H

#include "hdetparasciifileio.h"
#include <fstream>

class HParSet;

class HPionTrackerParAsciiFileIo : public HDetParAsciiFileIo {
public:
   HPionTrackerParAsciiFileIo(fstream*);
   ~HPionTrackerParAsciiFileIo() {}
   Bool_t init(HParSet*, Int_t*);
   Int_t write(HParSet*);
   template<class T> Int_t write(T*);
   template<class T> Bool_t read(T*, Int_t*);
   ClassDef(HPionTrackerParAsciiFileIo, 0) // Class for PionTracker parameter I/O from Ascii files
};

#endif  /* !HPIONTRACKERPARASCIIFILEIO_H */
