//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700RingFitterCOP
//
//
//////////////////////////////////////////////////////////////////////////////
#include "hrich700ringfittercop.h"

#include "TError.h"

#include <iostream>
#include <cmath>

using namespace std;



void HRich700RingFitterCOP::FitRing(HRich700Ring* ring)
{
    Int_t nofHits = ring->fHits.size();
    if (nofHits < 3) {
	ring->fCircleRadius = 0.;
	ring->fCircleXCenter = 0.;
	ring->fCircleYCenter = 0.;
	return;
    }

    if (nofHits >= 500) {
	Error("FitRing()","Too many hits in the ring: %i", nofHits);
	ring->fCircleRadius = 0.;
	ring->fCircleXCenter = 0.;
	ring->fCircleYCenter = 0.;
	return;
    }
    Int_t iterMax = 4;
    Float_t Xi, Yi, Zi;
    Float_t M0, Mx, My, Mz, Mxy, Mxx, Myy, Mxz, Myz, Mzz, Mxz2, Myz2, Cov_xy;
    Float_t A0, A1, A2, A22;
    Float_t epsilon = 0.00001;
    Float_t Dy, xnew, xold, ynew, yold = 10000000.;

    M0 = nofHits;
    Mx = My = 0.;

    // calculate center of gravity
    for (Int_t i = 0; i < nofHits; i++) {
	Mx += ring->fHits[i].fX;
	My += ring->fHits[i].fY;
    }
    Mx /= M0;
    My /= M0;

    // computing moments (note: all moments are normed, i.e. divided by N)
    Mxx = Myy = Mxy = Mxz = Myz = Mzz = 0.;

    for (Int_t i = 0; i < nofHits; i++) {
	// transform to center of gravity coordinate system
	Xi = ring->fHits[i].fX - Mx;
	Yi = ring->fHits[i].fY - My;
	Zi = Xi * Xi + Yi * Yi;

	Mxy += Xi * Yi;
	Mxx += Xi * Xi;
	Myy += Yi * Yi;
	Mxz += Xi * Zi;
	Myz += Yi * Zi;
	Mzz += Zi * Zi;
    }
    Mxx /= M0;
    Myy /= M0;
    Mxy /= M0;
    Mxz /= M0;
    Myz /= M0;
    Mzz /= M0;

    //computing the coefficients of the characteristic polynomial
    Mz = Mxx + Myy;
    Cov_xy = Mxx * Myy - Mxy * Mxy;
    Mxz2 = Mxz * Mxz;
    Myz2 = Myz * Myz;

    A2 = 4. * Cov_xy - 3. * Mz * Mz - Mzz;
    A1 = Mzz * Mz + 4. * Cov_xy * Mz - Mxz2 - Myz2 - Mz * Mz * Mz;
    A0 = Mxz2 * Myy + Myz2 * Mxx - Mzz * Cov_xy - 2. * Mxz * Myz * Mxy + Mz
	* Mz * Cov_xy;

    A22 = A2 + A2;
    xnew = 0.;

    //Newton's method starting at x=0
    Int_t iter;
    for (iter = 0; iter < iterMax; iter++) {
	ynew = A0 + xnew * (A1 + xnew * (A2 + 4. * xnew * xnew));

	if (fabs(ynew) > fabs(yold)) {
	    xnew = 0.;
	    break;
	}

	Dy = A1 + xnew * (A22 + 16. * xnew * xnew);
	xold = xnew;
	xnew = xold - ynew / Dy;
	if (xnew == 0 || fabs((xnew - xold) / xnew) < epsilon){
	    break;
	}
    }

    Float_t radius = 0.;
    Float_t centerX = 0.;
    Float_t centerY = 0.;

    //computing the circle parameters
    Float_t GAM = -Mz - xnew - xnew;
    Float_t DET = xnew * xnew - xnew * Mz + Cov_xy;
    if (DET != 0.) {
	centerX = (Mxz * (Myy - xnew) - Myz * Mxy) / DET / 2.;
	centerY = (Myz * (Mxx - xnew) - Mxz * Mxy) / DET / 2.;
	radius = sqrt(centerX * centerX + centerY * centerY - GAM);
	centerX += Mx;
	centerY += My;
    }

    ring->fCircleRadius = radius;
    ring->fCircleXCenter = centerX;
    ring->fCircleYCenter = centerY;

    CalcChi2(ring);
}

void HRich700RingFitterCOP::CalcChi2(
				     HRich700Ring* ring)
{
    Int_t nofHits = ring->fHits.size();
    if ( nofHits < 4 ) {
	ring->fCircleChi2 = 0.;
	return;
    }

    Float_t chi2 = 0.;
    Float_t r = ring->fCircleRadius;
    Float_t xc = ring->fCircleXCenter;
    Float_t yc = ring->fCircleYCenter;

    for (Int_t i = 0; i < nofHits; i++) {
	Float_t xh = ring->fHits[i].fX;
	Float_t yh = ring->fHits[i].fY;
	Float_t d = r - sqrt((xc - xh)*(xc - xh) + (yc - yh)*(yc - yh));

	chi2 += d*d;
    }
    ring->fCircleChi2 = chi2;
}

