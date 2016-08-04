///////////////////////////////////////////////////////////
// File: $RCSfile: htrbnetunpacker.h,v $
//
// Author : J.Wuestenfeld
// Version: $Revision: 1.1.1.1 $
///////////////////////////////////////////////////////////

#ifndef HTRBNETUNPACKER__H
#define HTRBNETUNPACKER__H

#include "TObject.h"

class HRuntimeDb;
class HLinearCategory;

class HTrbNetUnpacker : public TObject
{
protected:
    Bool_t initialized;
    HRuntimeDb *rtdb;
    HLinearCategory *debugInfo;

public:
    HTrbNetUnpacker(void);
    HTrbNetUnpacker(HTrbNetUnpacker &unp);
    ~HTrbNetUnpacker(void);

    Int_t  execute (void);
    Bool_t init    (void);
    Bool_t reinit  (void);
    Bool_t finalize(void);


    Int_t unpackData(UInt_t *data, Int_t subEventId = 0);

    ClassDef(HTrbNetUnpacker,0)
};

#endif
