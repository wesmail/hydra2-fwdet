#ifndef HPIONTRACKERPARROOTFILEIO_H
#define HPIONTRACKERPARROOTFILEIO_H

#include "hdetparrootfileio.h"

class HParRootFile;
class HParSet;
class HPionTrackerCalPar;

class HPionTrackerParRootFileIo : public HDetParRootFileIo {
public:
   HPionTrackerParRootFileIo(HParRootFile* f);
   ~HPionTrackerParRootFileIo();
   Bool_t init(HParSet*, Int_t*);
   Bool_t read(HPionTrackerCalPar*, Int_t*);
   ClassDef(HPionTrackerParRootFileIo, 0) // Class for parameter I/O from ROOT file for PionTracker
};

#endif  /* !HPIONTRACKERPARROOTFILEIO_H */

