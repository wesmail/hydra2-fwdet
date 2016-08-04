//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
//
//  HPionTrackerHit
//
//  Hits of a PionTracker detector
//
/////////////////////////////////////////////////////////////////////////////

#include "hpiontrackerhit.h"

ClassImp(HPionTrackerHit)

void HPionTrackerHit::getTimeAndCharge (Float_t & time, Float_t & charge)
{
	time   = fTime;
	charge = fCharge;
}

void HPionTrackerHit::getTimeAndChargeX (Float_t & time, Float_t & charge)
{
	time   = fTimeX;
	charge = fChargeX;
}

void HPionTrackerHit::getTimeAndChargeY (Float_t & time, Float_t & charge)
{
	time   = fTimeY;
	charge = fChargeY;
}

void HPionTrackerHit::setTimeAndCharge (const Float_t tx, const Float_t qx, const Float_t ty, const Float_t qy)
{
	fTimeX   = tx;
	fChargeX = qx;
	fTimeY   = ty;
	fChargeY = qy;
	calcAvgs();
}

void HPionTrackerHit::getLabPos(Float_t& lx, Float_t & ly, Float_t& lz) const
{
	lx = fLabX;
	ly = fLabY;
	lz = fLabZ;
}

void HPionTrackerHit::setLabPos(const Float_t x, const Float_t y, const Float_t z)
{
	fLabX = x;
	fLabY = y;
	fLabZ = z;
}

void HPionTrackerHit::getLocalPos(Float_t& lx, Float_t& ly) const
{
	lx = fLocalX;
	ly = fLocalY;
}

void HPionTrackerHit::setLocalPos(const Float_t x, const Float_t y)
{
	fLocalX = x;
	fLocalY = y; 
}

void HPionTrackerHit::calcAvgs()
{
	Float_t qtot = fChargeX + fChargeY;
	fTime = (fTimeX * fChargeX + fTimeY*fChargeY)/qtot;
	fCharge = (fChargeX + fChargeY) / 2.0;
}
