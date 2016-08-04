#ifndef HWALLPARROOTFILEIO_H
#define HWALLPARROOTFILEIO_H

#include "hdetparrootfileio.h"
#include "TFile.h"
#include "TArrayI.h"

class HParRootFile;
class HParSet;
class HWallLookup;
class HWallCalPar;

class HWallParRootFileIo : public HDetParRootFileIo {
public:
  HWallParRootFileIo(HParRootFile* f);
  ~HWallParRootFileIo(void) {}
  Bool_t init(HParSet*,Int_t*);
  Bool_t read(HWallLookup*,Int_t*);
  Bool_t read(HWallCalPar*,Int_t*);
  ClassDef(HWallParRootFileIo,0) // Class for Forward WALL parameter I/O from ROOT file
};

#endif  /*!HWALLPARROOTFILEIO_H*/
