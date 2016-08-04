#include "hkalhit.h"

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// 
// Data container holding information about hits from the Kalman filter.
//
///////////////////////////////////////////////////////////////////////////////


ClassImp(HKalSite)
HKalSite::HKalSite() {
    sec = -1;
    mod = -1;
    lay = -1;
    enerLoss = -1.F;
    idxFirst = -1;
    idxLast  = -1;
    momFilt  = -1.F;
    momSmoo  = -1.F;
    momReal  = -1.F;
    nComp    = -1;
    trackNum = -1;
}

ClassImp(HKalHit2d)
HKalHit2d::HKalHit2d() : TObject() {
    xMeas = 0.;
    yMeas = 0.;
    xReco = 0.;
    yReco = 0.;
}

ClassImp(HKalHitWire)
HKalHitWire::HKalHitWire() : TObject() {
    cell        = -1;
    chi2Daf     = -1.F;
    t1          = -1.F;
    t2          = -1.F;
    t1Err       = -1.F;
    mindist     = -1.F;
    alpha       = -1.F;
    tReco       = -1.F;
    tMeas       = -1.F;
    tTof        = -1.F;
    tTofCal1    = -1.F;
    tWireOffset = -1.F;
    for(Int_t iDaf = 0; iDaf < nDafs; iDaf++) {
	weight[iDaf] = -1.F;
    }
}
