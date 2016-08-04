#ifndef HSUHLDFILEINFO_H
#define HSUHLDFILEINFO_H
#pragma interface

// -----------------------------------------------------------------------------

#include "TROOT.h"
#include "hldfilesource.h"

// -----------------------------------------------------------------------------

class HSUHldFileInfo : public HldFileSource
{
public:

                HSUHldFileInfo(const Text_t *psName, Bool_t bAutoProceed = kTRUE);
    virtual    ~HSUHldFileInfo() {}

    // -------------------------------------------------------------------------

    Int_t       getEventsNumber(void)   { return fEventNr; }
    void        proceed(void);
    void        print(void);

    // -------------------------------------------------------------------------

private:

    HEvent     *m_ppEvent;

    // -------------------------------------------------------------------------

    ClassDef(HSUHldFileInfo, 0) // info about the HLD file
};

// -----------------------------------------------------------------------------

#endif
