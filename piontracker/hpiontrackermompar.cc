//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HPionTrackerMomPar
//
// Container class for PionTracker hit finder parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hpiontrackermompar.h"
#include "hparamlist.h"
#include <iostream>

ClassImp (HPionTrackerMomPar)

HPionTrackerMomPar::HPionTrackerMomPar(const Char_t * name, const Char_t * title, const Char_t * context)
	: HParCond (name, title, context)
{
	clear();
}

void HPionTrackerMomPar::clear()
{
	// clears the container
	fYYWeight = 0;           
	fYYSlope = 0.F;	   
	fYYOffset = 0.F;	   
	fMomWeight = 0;          
	fElossCorr = 0.F;	   


}

void HPionTrackerMomPar::putParams(HParamList * l)
{
	// add the parameters to the list for writing
	if (!l) return;

	l->add ("fYYWeight",     fYYWeight);
	l->add ("fYYSlope",      fYYSlope);
	l->add ("fYYOffset",     fYYOffset);
	l->add ("fMomWeight",    fMomWeight);
	l->add ("fElossCorr",    fElossCorr);


}

Bool_t HPionTrackerMomPar::getParams(HParamList * l)
{
	// gets the parameters from the list (read from input)
	if (!l) return kFALSE;

	if (!l->fill ("fYYWeight",     &fYYWeight))      return kFALSE;
	if (!l->fill ("fYYSlope",      &fYYSlope))       return kFALSE;
	if (!l->fill ("fYYOffset",     &fYYOffset))      return kFALSE;
	if (!l->fill ("fMomWeight",    &fMomWeight))     return kFALSE;
	if (!l->fill ("fElossCorr",    &fElossCorr))     return kFALSE;


	return kTRUE;
}
