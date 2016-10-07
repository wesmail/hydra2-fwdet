#ifndef HFWDETPARROOTFILEIO_H
#define HFWDETPARROOTFILEIO_H

#include "hdetparrootfileio.h"

class HParRootFile;
class HParSet;
class HFwDetCalPar;

class HFwDetParRootFileIo : public HDetParRootFileIo
{
public:
    HFwDetParRootFileIo(HParRootFile* f);
    virtual ~HFwDetParRootFileIo();
    Bool_t init(HParSet*, Int_t*);
    ClassDef(HFwDetParRootFileIo, 1); // Class for parameter I/O from ROOT file for the Forward Detector
};

#endif  /* !HFWDETPARROOTFILEIO_H */
