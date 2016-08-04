#pragma implementation

#include <Rtypes.h>
#include "TString.h"
#include <stdio.h>
#include "hsuprogressrec.h"

// -----------------------------------------------------------------------------
//*-- Author : Marcin Jaskula
// -----------------------------------------------------------------------------

//______________________________________________________________________________
// A wrapper around HSUProgress class. It works as a HReconstructor
// so it may be added to the list of tasks:
//
// gHades->getTaskSet("real")
//              ->connect(new HSUProgressRec("Progress", "Progress Bar", 10000);
//

ClassImp(HSUProgressRec)

// -----------------------------------------------------------------------------

HSUProgressRec::HSUProgressRec(const Text_t *pName,const Text_t *pTitle, Int_t iMax) :
        HReconstructor(pName, pTitle), HSUProgress(iMax)
{
// The first two arguments like for each HReconstructor.
// iMax sets the maximum value of counter in the HSUProgress
}

// -----------------------------------------------------------------------------

HSUProgressRec::~HSUProgressRec()
{
}

// -----------------------------------------------------------------------------

Bool_t HSUProgressRec::init(void)
{
    return kTRUE;
}

// -----------------------------------------------------------------------------

Bool_t HSUProgressRec::reinit(void)
{
// Reset the counter
    Restart();

    return kTRUE;
}

// -----------------------------------------------------------------------------

Bool_t HSUProgressRec::finalize(void)
{
// Print the last line of the progress output - timing information
    Final();
    return kTRUE;
}

// -----------------------------------------------------------------------------

Int_t HSUProgressRec::execute(void)
{
// Call HSUProgress::Next() to do a normal HSUProgress behavior
    Next();
    return 0;
}
