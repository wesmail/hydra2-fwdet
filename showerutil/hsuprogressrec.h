#ifndef HSUPROGRESSREC_H
#define HSUPROGRESSREC_H
#pragma interface

// -----------------------------------------------------------------------------

#include "hsuprogress.h"
#include "hreconstructor.h"

// -----------------------------------------------------------------------------

class HSUProgressRec : public HReconstructor, public HSUProgress
{
public:

                HSUProgressRec(const Text_t *pName,const Text_t *pTitle, Int_t iMax = 100);
    virtual    ~HSUProgressRec();

    // -------------------------------------------------------------------------

    Bool_t      init(void);
    Bool_t      reinit(void);
    Bool_t      finalize(void);
    Int_t       execute(void);

    // -------------------------------------------------------------------------

    ClassDef(HSUProgressRec, 0) //! Progress bar working as a reconstructor
};

// -----------------------------------------------------------------------------

#endif //!HSUPROGRESSREC_H
