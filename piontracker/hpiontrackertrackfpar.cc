//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HPionTrackerTrackFPar
//
// Container class for PionTracker hit finder parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hpiontrackertrackfpar.h"
#include "hparamlist.h"
#include <iostream>

ClassImp (HPionTrackerTrackFPar)

HPionTrackerTrackFPar::HPionTrackerTrackFPar(const Char_t * name, const Char_t * title, const Char_t * context)
	: HParCond (name, title, context)
{
	clear();
}

void HPionTrackerTrackFPar::clear()
{
	// clears the container
	fTrackingFlag = 0;
	fRefMom       = 0.F;
	fRefX1        = 0.F;
	fRefY1        = 0.F;
	fRefX2        = 0.F;
	fRefY2        = 0.F;
}

void HPionTrackerTrackFPar::putParams(HParamList * l)
{
	// add the parameters to the list for writing
	if (!l) return;

	l->add ("fTrackingFlag", fTrackingFlag);
	l->add ("fRefMom",       fRefMom);
	l->add ("fRefX1",        fRefX1);
	l->add ("fRefY1",        fRefY1);
	l->add ("fRefX2",        fRefX2);
	l->add ("fRefY2",        fRefY2);
}

Bool_t HPionTrackerTrackFPar::getParams(HParamList * l)
{
	// gets the parameters from the list (read from input)
	if (!l) return kFALSE;

	if (!l->fill ("fTrackingFlag", &fTrackingFlag)) return kFALSE;
	if (!l->fill ("fRefMom",       &fRefMom))       return kFALSE;
	if (!l->fill ("fRefX1",        &fRefX1))        return kFALSE;
	if (!l->fill ("fRefY1",        &fRefY1))        return kFALSE;
	if (!l->fill ("fRefX2",        &fRefX2))        return kFALSE;
	if (!l->fill ("fRefY2",        &fRefY2))        return kFALSE;

	return kTRUE;
}
