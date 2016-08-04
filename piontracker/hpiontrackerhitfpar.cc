//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
// HPionTrackerHitFPar
//
// Container class for PionTracker hit finder parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hpiontrackerhitfpar.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hpiontrackerdetector.h"
#include "hparamlist.h"
#include <iostream>

ClassImp (HPionTrackerHitFPar)

HPionTrackerHitFPar::HPionTrackerHitFPar (const Char_t * name, const Char_t * title, const Char_t * context)
	: HParCond (name, title, context)
{
	clear();
}

void HPionTrackerHitFPar::clear()
{
	// clears the container
	numModules = 0;
	fTimeWindowOffset.Reset();
	fTimeWindowWidth.Reset();
	fChargeThresh.Reset();
	fClusterDistX.Reset();
	fClusterDistT.Reset();
	fClusterThresh.Reset();
	numPlanes = 0;
	fPlanePair.Reset();
	fHitMatchTimeC.Reset();
	fHitMatchTimeW.Reset();
	fHitMatchChargeC.Reset();
	fHitMatchChargeW.Reset();
}

void HPionTrackerHitFPar::putParams (HParamList * l)
{
	// add the parameters to the list for writing
	if (!l) return;

	l->add ("numModules",      numModules);
	l->add ("fTimeWindowOffset", fTimeWindowOffset);
	l->add ("fTimeWindowWidth", fTimeWindowWidth);
	l->add ("fChargeThresh", fChargeThresh);
	l->add ("fClusterDistX", fClusterDistX);
	l->add ("fClusterDistT", fClusterDistT);
	l->add ("fClusterThresh", fClusterThresh);
	l->add ("numPlanes", numPlanes);
	l->add ("fPlanePair", fPlanePair);
	l->add ("fHitMatchTimeC", fHitMatchTimeC);
	l->add ("fHitMatchTimeW", fHitMatchTimeW);
	l->add ("fHitMatchChargeC", fHitMatchChargeC);
	l->add ("fHitMatchChargeW", fHitMatchChargeW);
}

Bool_t HPionTrackerHitFPar::getParams (HParamList * l)
{
	// gets the parameters from the list (read from input)
	if (!l) return kFALSE;

	Int_t n = 0;

	if (!l->fill ("numModules", &n)) return kFALSE;
	setNumModules (n);

	if (!getParam(l, "fTimeWindowOffset", &fTimeWindowOffset, numModules)) return kFALSE;
	if (!getParam(l, "fTimeWindowWidth", &fTimeWindowWidth, numModules)) return kFALSE;
	if (!getParam(l, "fChargeThresh", &fChargeThresh, numModules)) return kFALSE;
	if (!getParam(l, "fClusterDistX", &fClusterDistX, numModules)) return kFALSE;
	if (!getParam(l, "fClusterDistT", &fClusterDistT, numModules)) return kFALSE;
	if (!getParam(l, "fClusterThresh", &fClusterThresh, numModules)) return kFALSE;

	if (!l->fill ("numPlanes", &n)) return kFALSE;
	setNumPlanes (n);

	if (!getParam(l, "fPlanePair", &fPlanePair, numPlanes*2)) return kFALSE;
	if (!getParam(l, "fHitMatchTimeC", &fHitMatchTimeC, numPlanes)) return kFALSE;
	if (!getParam(l, "fHitMatchTimeW", &fHitMatchTimeW, numPlanes)) return kFALSE;
	if (!getParam(l, "fHitMatchChargeC", &fHitMatchChargeC, numPlanes)) return kFALSE;
	if (!getParam(l, "fHitMatchChargeW", &fHitMatchChargeW, numPlanes)) return kFALSE;

	return kTRUE;
}

Bool_t HPionTrackerHitFPar::getParam(HParamList * l, const char* name, TArrayI* arr, Int_t size)
{
	if (!l->fill (name, arr)) return kFALSE;

	if (size != arr->GetSize())
	{
		Error ("HPionTrackerHitFPar::getParam(HParamList *, const char *, TArray *)",
			   "Number of modules does not match array size of %s", name);
		return kFALSE;
	}
	return kTRUE;
}

Bool_t HPionTrackerHitFPar::getParam(HParamList * l, const char* name, TArrayF* arr, Int_t size)
{
	if (!l->fill (name, arr)) return kFALSE;

	if (size != arr->GetSize())
	{
		Error ("HPionTrackerHitFPar::getParam(HParamList *, const char *, TArray *)",
			   "Number of modules does not match array size of %s", name);
		return kFALSE;
	}
	return kTRUE;
}

Float_t HPionTrackerHitFPar::getTimeWindowOffset (const Int_t m) const
{
	if (m >= 0 && m < numModules) return fTimeWindowOffset.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getTimeWindowWidth (const Int_t m) const
{
	if (m >= 0 && m < numModules) return fTimeWindowWidth.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getChargeThresh(const Int_t m) const
{
	if (m >= 0 && m < numModules) return fChargeThresh.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getClusterDistX(const Int_t m) const
{
	if (m >= 0 && m < numModules) return fClusterDistX.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getClusterDistT(const Int_t m) const
{
	if (m >= 0 && m < numModules) return fClusterDistT.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getClusterThresh(const Int_t m) const
{
	if (m >= 0 && m < numModules) return fClusterThresh.At (m);
	else return 0;
}
void HPionTrackerHitFPar::getPlanePair (const Int_t m, Int_t & x, Int_t & y) const
{
	if (m >= 0 && m < numPlanes)
	{
		x = fPlanePair.At (m*2);
		y = fPlanePair.At (m*2+1);
	}
	else
	{
		x = 0;
		y = 0;
	}
}

Int_t HPionTrackerHitFPar::getPlaneX(const Int_t m) const
{
	if (m >= 0 && m < numPlanes) return fPlanePair.At (m*2);
	else return 0;
}

Int_t HPionTrackerHitFPar::getPlaneY(const Int_t m) const
{
	if (m >= 0 && m < numPlanes) return fPlanePair.At (m*2+1);
	else return 0;
}

Float_t HPionTrackerHitFPar::getHitMatchTimeC(const Int_t m) const
{
	if (m >= 0 && m < numPlanes) return fHitMatchTimeC.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getHitMatchTimeW(const Int_t m) const
{
	if (m >= 0 && m < numPlanes) return fHitMatchTimeW.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getHitMatchChargeC(const Int_t m) const
{
	if (m >= 0 && m < numPlanes) return fHitMatchChargeC.At (m);
	else return 0;
}

Float_t HPionTrackerHitFPar::getHitMatchChargeW(const Int_t m) const
{
	if (m >= 0 && m < numPlanes) return fHitMatchChargeW.At (m);
	else return 0;
}

Bool_t HPionTrackerHitFPar::setNumModules (const Int_t nMod)
{
	if (nMod > 0)
	{
		Int_t maxModules = 0;
		HPionTrackerDetector * pDet = (HPionTrackerDetector *) gHades->getSetup()->getDetector ("PionTracker");

		if (pDet) maxModules = pDet->getMaxModInSetup();

		if (nMod >= maxModules)
		{
			numModules = nMod;
			return kTRUE;
		}
		else
		{
			Error ("HPionTrackerHitFPar::setNumModules(const Int_t nMod)",
				   "Number of modules smaller than number of active modules in the setup");
			return kFALSE;
		}
	}

	Error ("HPionTrackerHitFPar::setNumModules(const Int_t nMod)", "nMod must be larger than 0");
	return kFALSE;
}

void HPionTrackerHitFPar::setNumPlanes (const Int_t nPlanes)
{
	numPlanes = nPlanes;
}

void HPionTrackerHitFPar::setTimeWindowOffset (const Int_t m, const Float_t v)
{
	if (numModules > 0 && fTimeWindowOffset.GetSize() == 0)
	{
		fTimeWindowOffset.Set (numModules);
	}

	fTimeWindowOffset.SetAt (v, m);
}

void HPionTrackerHitFPar::setTimeWindowWidth (const Int_t m, const Float_t v)
{
	if (numModules > 0 && fTimeWindowWidth.GetSize() == 0)
	{
		fTimeWindowWidth.Set (numModules);
	}

	fTimeWindowWidth.SetAt (v, m);
}

void HPionTrackerHitFPar::setChargeThresh (const Int_t m, const Float_t v)
{
	if (numModules > 0 && fChargeThresh.GetSize() == 0)
	{
		fChargeThresh.Set (numModules);
	}

	fChargeThresh.SetAt (v, m);
}

void HPionTrackerHitFPar::setClusterDistX(const Int_t m, const Float_t v)
{
	if (numModules > 0 && fClusterDistX.GetSize() == 0)
	{
		fClusterDistX.Set (numModules);
	}

	fClusterDistX.SetAt (v, m);
}

void HPionTrackerHitFPar::setClusterDistT(const Int_t m, const Float_t v)
{
	if (numModules > 0 && fClusterDistT.GetSize() == 0)
	{
		fClusterDistT.Set (numModules);
	}

	fClusterDistT.SetAt (v, m);
}

void HPionTrackerHitFPar::setClusterThresh(const Int_t m, const Float_t v)
{
	if (numModules > 0 && fClusterThresh.GetSize() == 0)
	{
		fClusterThresh.Set (numModules);
	}

	fClusterThresh.SetAt (v, m);
}

void HPionTrackerHitFPar::setPlanePair (const Int_t m, const Int_t x, Int_t y)
{
	if (numPlanes > 0 && fPlanePair.GetSize() == 0)
	{
		fPlanePair.Set (numPlanes);
	}

	fPlanePair.SetAt (x, m*2);
	fPlanePair.SetAt (y, m*2+1);
}

void HPionTrackerHitFPar::setHitMatchTime(const Int_t m , const Float_t c, const Float_t w)
{
	if (numModules > 0 && fHitMatchTimeC.GetSize() == 0)
	{
		fHitMatchTimeC.Set (numPlanes);
		fHitMatchTimeW.Set (numPlanes);
	}

	fHitMatchTimeC.SetAt (c, m);
	fHitMatchTimeW.SetAt (w, m);
}

void HPionTrackerHitFPar::setHitMatchCharge(const Int_t m, const Float_t c, const Float_t w)
{
	if (numModules > 0 && fHitMatchChargeC.GetSize() == 0)
	{
		fHitMatchChargeC.Set (numPlanes);
		fHitMatchChargeW.Set (numPlanes);
	}

	fHitMatchChargeC.SetAt (c, m);
	fHitMatchChargeW.SetAt (w, m);
}
