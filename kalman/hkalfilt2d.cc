
#include "hkalfilt2d.h"

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// Kalman filter for segment hits. See doc for HKalIFilter
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

ClassImp (HKalFilt2d)

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalFilt2d::HKalFilt2d(Int_t n, Int_t measDim, Int_t stateDim,
                       HMdcTrackGField *fMap, Double_t fpol)
    : HKalIFilt(n, measDim, stateDim, fMap, fpol) {

    setHitType(Kalman::kSegHit);
}

Bool_t HKalFilt2d::calcMeasVecFromState(TVectorD &projMeasVec, const HKalTrackSite *site,
					Kalman::kalFilterTypes stateType, Kalman::coordSys sys,
					Int_t iHit) const {
    // Extracts measurement vector from state vector for segment hits.

#if kalDebug > 0
    Int_t mdim = getMeasDim();
    if(projMeasVec.GetNrows() != mdim) {
        Warning("calcMeasVecFromState()", Form("Dimension of measurement vector (%i) does not match that of function parameter (%i).", mdim, projMeasVec.GetNrows()));
        projMeasVec.ResizeTo(mdim);
    }

    if(mdim != 2) {
        Error("calcMeasVecFromState()", Form("Dimension of measurement vector is %i. Expected 2 for segment hits.", mdim));
        return kFALSE;
    }
#endif
    const TVectorD &sv = site->getStateVec(stateType);

    projMeasVec(0) = sv(kIdxX0);
    projMeasVec(1) = sv(kIdxY0);

    return kTRUE;
}

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------

Bool_t HKalFilt2d::calcProjector(Int_t iSite) const {
    // The projection function h(a) extracts a measurement vector from a track state a.
    // The projector matrix is the Jacobian of this function, i.e. @h(a)/@a.
    // The projector matrix is stored in the site's filtered track state.

    HKalTrackSite *site = getSite(iSite);
    if(!site) {
	return kFALSE;
    }

    Int_t mdim = getMeasDim();
    Int_t sdim = getStateDim();

#if kalDebug > 0
    if(getHitType() != Kalman::kSegHit) {
    }
    if(mdim != 2) {
        Error("calcProjector()",
              Form("Dimension of measurement vector is %i. Expected 2 for segment hits.", mdim));
        return kFALSE;
    }
    if(sdim < Kalman::kIdxX0 || sdim < Kalman::kIdxY0) {
        Error("calcProjector()",
              Form("Dimension of state vector is %i. Expected at least %i.", sdim, TMath::Max(kIdxX0, kIdxY0)));
        return kFALSE;
    }
#endif

    TMatrixD fProj(mdim, sdim);
    fProj.Zero();

    // For segment hits, x0 and y0 are coordinates of the measurement
    // in the sector system.
    fProj(0, kIdxX0) = 1.;
    fProj(1, kIdxY0) = 1.;

    // Store result.
    site->setStateProjMat(Kalman::kFiltered, fProj);

    return kTRUE;
}



