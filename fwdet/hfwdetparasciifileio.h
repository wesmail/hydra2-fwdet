#ifndef HFWDETPARASCIIFILEIO_H
#define HFWDETPARASCIIFILEIO_H

using namespace std;
#include <fstream>
#include "hdetparasciifileio.h"

class HParSet;

class HFwDetParAsciiFileIo : public HDetParAsciiFileIo
{
public:
    HFwDetParAsciiFileIo(fstream*);
    virtual ~HFwDetParAsciiFileIo();

    Bool_t init(HParSet*, Int_t*);
    Int_t write(HParSet*);

    ClassDef(HFwDetParAsciiFileIo, 0); // Class for parameter I/O from Ascii files for Forward Detector
};

#endif  /* !HFWDETPARASCIIFILEIO_H */
