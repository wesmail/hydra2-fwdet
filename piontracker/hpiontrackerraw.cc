//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
//  HPionTrackerRaw
//
//  Unpacked TRB raw data of the PionTracker detector using the TRB for readout
//
//  The class accepts up to 4 hits per channel, each with time and charge.
//  If the number of hits exceeds 4, the hit counter will be incremented, but
//  no data are stored for these hits.
//
///////////////////////////////////////////////////////////////////////////////

#include "hpiontrackerraw.h"

ClassImp (HPionTrackerRaw)

// Int_t HPionTrackerRaw::cnt = 0;

HPionTrackerRaw::HPionTrackerRaw() : fMultiplicity (0), fModule (-1), fStrip(-1)
{
	for (Int_t h = 0; h < fMaxMultiplicity; ++h)
	{
		fTime[h] = -1000;
		fCharge[h] = -1000;
		fPileUp[h] = -10;
		fOverflow[h] = -10;
	}
}

HPionTrackerRaw::~HPionTrackerRaw() { }

Float_t HPionTrackerRaw::getTime (const Int_t n) const
{
	if (n > (fMultiplicity+1))
	{
		Error ("getTime()", "hit number: %i out of range [0,%d]", n, fMultiplicity-1);
		return -1.F;
	}
	else
		return fTime[n];
}

Float_t HPionTrackerRaw::getCharge (const Int_t n) const
{
	if (n > (fMultiplicity+1))
	{
		Error ("getCharge()", "hit number: %i out of range [0,%d]", n, fMultiplicity-1);
		return -1.F;
	}
	else
		return fCharge[n];

}

void HPionTrackerRaw::getTimeAndCharge (const Int_t n, Float_t & time, Float_t & charge)
{
	if (n > (fMultiplicity+1))
	{
		Error ("getTimeAndCharge()", "hit number: %i out of range [0,%d]", n, fMultiplicity-1);
		time = -1.F;
		charge = -1.F;
	}
	else
	{
		time = fTime[n];
		charge = fCharge[n];
	}
}

Char_t HPionTrackerRaw::getPileup(const Int_t n) const
{
	if (n > (fMultiplicity+1))
	{
		Error ("getPileup()", "hit number: %i out of range [0,%d]", n, fMultiplicity-1);
		return -1;
	}
	else
		return fPileUp[n];
}

Char_t HPionTrackerRaw::getOverflow(const Int_t n) const
{
	if (n > (fMultiplicity+1))
	{
		Error ("getOverflow()", "hit number: %i out of range [0,%d]", n, fMultiplicity-1);
		return -1;
	}
	else
		return fOverflow[n];
}


void HPionTrackerRaw::getErrors (const Int_t n, Char_t & pileup, Char_t & overflow)
{
	if (n > (fMultiplicity+1))
	{
		Error ("getErrors()", "hit number: %i out of range [0,%d]", n, fMultiplicity-1);
		pileup = -1;
		overflow = -1;
	}
	else
	{
		pileup = fPileUp[n];
		overflow = fOverflow[n];
	}
}

Bool_t HPionTrackerRaw::setData (const Float_t time, const Float_t charge, const Char_t pileup, const Char_t overflow)
{
	if (fMultiplicity == fMaxMultiplicity)
	{
		Error ("setData()", "hit number: %i out of range [0,%d] for m=%d, s=%d", fMultiplicity+1, fMaxMultiplicity-1, fModule, fStrip);
		return kFALSE;
	}

	fTime[fMultiplicity] = time;
	fCharge[fMultiplicity] = charge;
	fPileUp[fMultiplicity] = pileup;
	fOverflow[fMultiplicity] = overflow;

	++fMultiplicity;

	return kTRUE;
}
