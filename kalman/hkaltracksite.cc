
// from ROOT
#include "TMath.h"
#include "TRotation.h"
#include "TVector3.h"

// from hydra
#include "hkalgeomtools.h"
#include "hkalmatrixtools.h"
#include "hkaltracksite.h"
#include "hkaltrackstate.h"

#include <iostream>
using namespace std;
#include <cstdlib>

ClassImp(HKalTrackSite)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// A track site stores information about a measurement hit and parameters of
// the Kalman filter track states (prediction, filtering, smoothing, inverse filtering)
// for this hit.
// The hit object for a site may be replaced with the setHit(const HKalMdcHit &newhit)
// function.
// The site provides read/write access to the state vectors and the matrices for all
// track states via the setState* functions.
// The dimension of the measurement vector and track state vector must be defined
// when building the object and may not be changed later on.
// Track states can be stored in two coordinate systems: a system common for
// all measurement sites (usually sector coordinate) and coordinates defined by a
// virtual layer. The virtual layer has to be defined with the setHitVirtPlane()
// function.
// The functions transVirtLayToSec() and transSecToVirtLay() calculate and store
// the track state vectors and covariance matrices in the other coordinate system.
// The virtual layer coordinate system is used for drift chamber wire hits where
// the measurement point does not lie on the measurement layer.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalTrackSite::HKalTrackSite(Int_t measDim, Int_t stateDimSec, Int_t stateDimVirtLay)
    : TObject(), bActive(kTRUE), effErrMat(measDim, measDim), effMeasVec(measDim) {
    // Creates a dummy hit and track states.
    // measDim:  dimension of measurement vector.
    // stateDimSec: dimension of the track state vectors in the sector coordinate system.
    // stateDimVirtLay: dimension  of the track state vectors in the virtual layer coordinate system.

    Int_t nCoordSys = 2;
    trackStates = new HKalTrackState**[nCoordSys];
    for(Int_t i = 0; i < nCoordSys; i++) {
        trackStates[i] = new HKalTrackState*[kNumKalStates];
    }

    for(Int_t stateType = 0; stateType < kNumKalStates; stateType++) {
        trackStates[kSecCoord][stateType] = new HKalTrackState((Kalman::kalFilterTypes)stateType, measDim, stateDimSec);
    }

    for(Int_t stateType = 0; stateType < kNumKalStates; stateType++) {
        trackStates[kLayCoord][stateType] = new HKalTrackState((Kalman::kalFilterTypes)stateType, measDim, stateDimVirtLay);
    }

    hits = new TObjArray();
    hits->Add(new HKalMdcHit(measDim)); // add a dummy hit to site
}

HKalTrackSite::~HKalTrackSite() {
    delete hits;
    delete [] trackStates;
}

//  -----------------------------------
//  Implementation of private methods
//  -----------------------------------

HKalMdcHit* HKalTrackSite::getHitPtr(Int_t idx) {
    // Get hit at index idx that this site contains.

    if(idx >= hits->GetEntries() || idx < 0 || hits->GetEntries() == 0) {
        Error("getHit()", Form("No hit at index %i. Only %i hits stored. Returning first hit object.", idx, hits->GetEntries()));
        return (HKalMdcHit*)hits->At(0);
    } else {
        return (HKalMdcHit*)hits->At(idx);
    }
}

void HKalTrackSite::transformHit(const TRotation &transMat) {
    // Transform the measurement hit coordinates and layer vectors
    // using the rotation matrix transMat.

    for(Int_t i = 0; i < hits->GetLast() + 1; i++) {
        getHitPtr(i)->transformHit(transMat);
    }
    getHitPtr()->transformLayer(transMat);
}

void HKalTrackSite::transformStates(const TRotation &transMat) {
    // Transform the track state vectors using the rotation matrix transMat.
    // Since information about the measurement layer is needed, make sure
    // the hit has not been transformed yet.

    for(Int_t stateType = 0; stateType < kNumKalStates; stateType++) {
	if(getHitType() == Kalman::kSegHit) {
	    trackStates[kSecCoord][stateType]->transform(transMat, getHitPtr(0)->getMeasLayer());
	}
	if(getHitType() == Kalman::kWireHit) {
	    trackStates[kSecCoord][stateType]->transform(transMat, getHitPtr(0)->getVirtPlane());
	}
    }
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

void HKalTrackSite::addHit(HKalMdcHit *newhit) {
    // Add a new hit to this site.
    hits->Add(newhit);
}

void HKalTrackSite::calcJacLayToSec(TMatrixD &jac, const TVectorD &svLay, const HKalPlane &plane) const {
    // Track states can be stored in two coordinate systems:
    // the sector coordinate system common for all sites and a local
    // coordinate system for this site defined by a virtual plane.
    // This calculated the Jacobian matrix for the transformation
    // function from virtual layer to sector coordinates.
    //
    // Output:
    // jac:   Jacobian matrix
    //
    // Input:
    // svSec: State vector in virtual layer coordinates.
    // plane: Virtual layer.

    Int_t secDim = getStateDim(kSecCoord);
    Int_t layDim = getStateDim(kLayCoord);
    Int_t nRows = jac.GetNrows();
    Int_t nCols = jac.GetNcols();
    if(nRows != secDim || nCols != layDim) {
        jac.ResizeTo(secDim, layDim);
        ::Warning("calcJacobianFromLayerToSector()", Form("Output parameter for Jacobian matrix had to be resized from %ix%i to %ix%i.", nRows, nCols, secDim, layDim));
    }

#if kalDebug > 0
    if(svLay.GetNrows() != layDim) {
        ::Error("calcJacobianFromLayerToSector()", Form("Input state vector has dimension %i, but needs to be %i.", svLay.GetNrows(), layDim));
        return;
    }
#endif

    const TVector3 &u = plane.getAxisU();
    const TVector3 &v = plane.getAxisV();
    const TVector3 &w = plane.getNormal();

    // State vector in sector coordinate system: (x,y,tx,ty,qp,z).
    // State vector in layer  coordinate system: (u,v,tu,tv,qp).
    // Layer coordinate system is defined by an origin vector O,
    // two orthogonal Axis U and V on the layer and a normal vector W.
    // All axis are given in sector coordinate system.

    // Transformation from layer to sector:
    // Calculate position in sector coordinates (x,y,z) from layer coordinates:
    // P = O + u*U + v*V
    // direction in sector coordinates:
    // A = 1/norm(W + tu*U + tv*V) * (W + tu*U + tv*V)
    // tx = A.x() / A.z()
    // ty = A.y() / A.z()

    // jac = d(x,y,tx,ty,qp,z) / d(u,v,tu,tv,qp)
    // row index:    state vector parameter in sector coordinates
    // column index: state vector parameter in layer coordinates

    Double_t tu      = svLay(kIdxTanPhi);
    Double_t tv      = svLay(kIdxTanTheta);
    TVector3 dir     = tu*u + tv*v + w;      // non normalized direction from layer state vector.
    Double_t dirz2   = dir.Z() * dir.Z();

    jac.Zero();

    jac(kIdxX0, kIdxX0) = u.X(); // dx/du
    jac(kIdxY0, kIdxX0) = u.Y(); // dy/du

    jac(kIdxX0, kIdxY0) = v.X(); // dx/dv
    jac(kIdxY0, kIdxY0) = v.Y(); // dy/dv

    if(secDim > 5) {
        jac(kIdxZ0, kIdxX0) = u.Z(); // dz/du
        jac(kIdxZ0, kIdxY0) = v.Z(); // dz/dv
    }

    jac(kIdxTanPhi,   kIdxTanPhi)   = (tv*u.X()*v.Z() + u.X()*w.Z() - tv*u.Z()*v.X() - u.Z()*w.X()) / dirz2; // dtx/dtu
    jac(kIdxTanPhi,   kIdxTanTheta) = (tu*u.Z()*v.X() + v.X()*w.Z() - tu*u.X()*v.Z() - v.Z()*w.X()) / dirz2; // dtx/dtv
    jac(kIdxTanTheta, kIdxTanPhi)   = (tv*u.y()*v.Z() + u.Y()*w.Z() - tv*u.Z()*v.Y() - u.Z()*w.Y()) / dirz2; // dty/dtu
    jac(kIdxTanTheta, kIdxTanTheta) = (tu*u.Z()*v.Y() + v.Y()*w.Z() - tu*u.Y()*v.Z() - v.Z()*w.Y()) / dirz2; // dty/dtv

    jac(kIdxQP,       kIdxQP)       = 1.;
}

void HKalTrackSite::calcJacSecToLay(TMatrixD &jac, const TVectorD &svSec, const HKalPlane &plane) const {
    // Track states can be stored in two coordinate systems:
    // the sector coordinate system common for all sites and a local
    // coordinate system for this site defined by a virtual plane.
    // This calculated the Jacobian matrix for the transformation
    // function from sector to virtual layer coordinates.
    //
    // Output:
    // jac:   Jacobian matrix
    //
    // Input:
    // svSec: State vector in sector coordinates.
    // plane: Virtual layer.

    Int_t secDim = getStateDim(kSecCoord);
    Int_t layDim = getStateDim(kLayCoord);
    Int_t nRows = jac.GetNrows();
    Int_t nCols = jac.GetNcols();
    if(nRows != layDim || nCols != secDim) {
        jac.ResizeTo(layDim, secDim);
        ::Warning("calcJacobianFromLayerToSector()", Form("Output parameter for Jacobian matrix had to be resized from %ix%i to %ix%i.", nRows, nCols, layDim, secDim));
    }

#if kalDebug > 0
    if(svSec.GetNrows() != secDim) {
        ::Error("calcJacobianFromSectorToLayer()", Form("Input state vector has dimension %i, but needs to be %i", svSec.GetNrows(), secDim));
        return;
    }
#endif

    const TVector3 &u = plane.getAxisU();
    const TVector3 &v = plane.getAxisV();
    const TVector3 &w = plane.getNormal();

    // State vector in sector coordinate system: (x,y,tx,ty,qp).
    // State vector in layer  coordinate system: (u,v,tu,tv,qp).
    // Layer coordinate system is defined by an origin vector O,
    // two orthogonal Axis U and V on the layer and a normal vector W.
    // All axis are given in sector coordinate system.

    // Transformation from sector to layer:
    // position on layer:
    // u = (P - O) * U
    // v = (P - O) * V
    // direction on layer:
    // tu = (A * U) / (A * W)
    // tv = (A * V) / (A * W)
    // P = (x,y,z) - the position vector in sector coordinate system
    // with A = 1/sqrt(1 + tx^2 + ty^2) * (tx, ty, 1)^T the normalized direction vector in the sector coordinate system

    // jac = d(u,v,tu,tv,qp) / d(x,y,tx,ty,qp,z)
    // row index:    state vector parameter in layer  coordinates
    // column index: state vector parameter in sector coordinates

    TVector3 dir;
    Double_t tx = svSec(kIdxTanPhi);
    Double_t ty = svSec(kIdxTanTheta);
    dir.SetXYZ(tx, ty, 1.);

    Double_t aw  = dir * w;
    Double_t aw2 = aw * aw;

    jac.Zero();

    jac(kIdxX0, kIdxX0) = u.X(); // du/dx
    jac(kIdxX0, kIdxY0) = u.Y(); // du/dy
    jac(kIdxY0, kIdxX0) = v.X(); // dv/dx
    jac(kIdxY0, kIdxY0) = v.Y(); // dv/dy

    if(secDim > 5) {
        jac(kIdxX0, kIdxZ0) = u.Z(); // du/dz
        jac(kIdxY0, kIdxZ0) = v.Z(); // dv/dz
    }

    jac(kIdxTanPhi,   kIdxTanPhi)   = (ty*u.X()*w.Y() + u.X()*w.Z() - ty*u.Y()*w.X() - u.Z()*w.X()) / aw2; // dtu/dtx
    jac(kIdxTanPhi,   kIdxTanTheta) = (tx*u.Y()*w.X() + u.Y()*w.Z() - tx*u.X()*w.Y() - u.Z()*w.Y()) / aw2; // dtu/dty
    jac(kIdxTanTheta, kIdxTanPhi)   = (ty*v.X()*w.Y() + v.X()*w.Z() - ty*v.Y()*w.X() - v.Z()*w.X()) / aw2; // dtv/dtx
    jac(kIdxTanTheta, kIdxTanTheta) = (tx*v.Y()*w.X() + v.Y()*w.Z() - tx*v.X()*w.Y() - v.Z()*w.Y()) / aw2; // dtv/dty

    jac(kIdxQP,       kIdxQP)       = 1.;
}


void HKalTrackSite::clearHits() {
    hits->Delete();
    effMeasVec.Zero();
    effErrMat.Zero();
    bActive = kTRUE;
}

void HKalTrackSite::sortHits() {
    hits->Sort();
}

void HKalTrackSite::print(const Option_t *opt) const {
    // Prints information about the hit and track states for this site.
    // opt: Determines what information about the object will be printed.
    // If opt contains:
    // "Hit": print information about hit's measurement coordinates and layer
    // "Pred": print state vector and used matrices of the predicted state
    // "Filt": print state vector and used matrices of the filtered state
    // "Smoo": print state vector and used matrices of the smoothed state
    // "Inv":  print state vector and used matrices of the inverse filtered state
    // If opt is empty all of the above will be printed.
    // Only the state vector and covariance matrix are printed for all states.
    // The content of unused matrices will not be printed:
    // The propagator and process noise matrices are always stored in the filtered state.
    // The projector matrix is stored in class HKalSystem.
    // idx: Index of hit object in site.

    cout<<"**** Print content of site ****"<<endl;
    TString stropt(opt);
    if(stropt.Contains("Hit", TString::kIgnoreCase)  || stropt.IsNull()) {
        for(Int_t i = 0; i < getNcompetitors(); i++) {
            cout<<"Printing hit number "<<i<<" in site."<<endl;
            getHit(i).print("Hit");
        }
    }
    if(stropt.Contains("Pred", TString::kIgnoreCase) || stropt.IsNull()) {
        trackStates[kSecCoord][kPredicted]  ->print("SFCQ");
    }
    if(stropt.Contains("Filt", TString::kIgnoreCase) || stropt.IsNull()) {
        trackStates[kSecCoord][kFiltered]   ->print("SC");
    }
    if(stropt.Contains("Smoo", TString::kIgnoreCase) || stropt.IsNull()) {
        trackStates[kSecCoord][kSmoothed]   ->print("SC");
    }
    if(stropt.Contains("Inv", TString::kIgnoreCase)  || stropt.IsNull()) {
        trackStates[kSecCoord][kInvFiltered]->print("SC");
    }
    cout<<"**** End print of site ****"<<endl;
}

void HKalTrackSite::transform(const TRotation &transMat) {
    // Transform the track state vectors and measurement hit coordinates
    // and the hit's measurement layer using the rotation matrix transMat.

#if kalDebug > 1
    if(transMat.IsIdentity()) {
        Warning("transform()", "Rotation matrix for coordinate transformation not set.");
    }
#endif
    // Track states must be transformed before the hit!
    transformStates(transMat);
    transformHit(transMat);
}

void HKalTrackSite::transVirtLayToSec(Kalman::kalFilterTypes stateType,
                                      Int_t iHit,
				      Bool_t bCovUD) {
    // Track states can be stored in two coordinate systems:
    // the sector coordinate system common for all sites and a local
    // coordinate system for this site defined by a virtual plane.
    // This functions calculates and stores the track state and
    // covariance matrix in virtual layer coordinates from sector.
    //
    // Input:
    // stateType: state to transform (kPredicted, kFiltered, kSmoothed, kInvFiltered)
    // iHit:      index of hit stored in site
    // bCovUD:    set to true if using the UD filter.

    Int_t secDim = getStateDim();
    Int_t layDim = getStateDim(kLayCoord);
    const HKalPlane &virtPlane = getHitVirtPlane(iHit);
    const TVectorD  &svLay     = getStateVec(stateType, kLayCoord);

    TVectorD svSec(secDim);
    HKalTrackState::transformFromLayerToSector(svSec, svLay, virtPlane);
    setStateVec(stateType, svSec, kSecCoord);

    TMatrixD jacLaySec(secDim, layDim);
    calcJacLayToSec(jacLaySec, svLay, virtPlane);
    TMatrixD jacLaySecTrans = TMatrixD(TMatrixD::kTransposed, jacLaySec);

    const TMatrixD &covLay = getStateCovMat(stateType, kLayCoord);

    if(bCovUD) {
        TMatrixD U(layDim, layDim);
        TMatrixD D(layDim, layDim);
        HKalMatrixTools::resolveUD(U, D, covLay);

        TMatrixD covLayUD = U * D * TMatrixD(TMatrixD::kTransposed, U);
        TMatrixD covSec = jacLaySec * covLayUD * jacLaySecTrans;
        if(!covSec.IsSymmetric()) {
            HKalMatrixTools::makeSymmetric(covSec);
        }
        HKalMatrixTools::decomposeUD(covSec);
        setStateCovMat(stateType, covSec, kSecCoord);
    } else {

        TMatrixD covSec = jacLaySec * covLay * jacLaySecTrans;
        setStateCovMat(stateType, covSec, kSecCoord);
    }
}

void HKalTrackSite::transSecToVirtLay(Kalman::kalFilterTypes stateType,
                                      Int_t iHit,
				      Bool_t bCovUD) {
    // Track states can be stored in two coordinate systems:
    // the sector coordinate system common for all sites and a local
    // coordinate system for this site defined by a virtual plane.
    // This functions calculates and stores the track state and
    // covariance matrix in sector coordinates from virtual layer.
    //
    // Input:
    // stateType: state to transform (predicted, filtered, etc.)
    // iHit:      index of hit stored in site
    // bCovUD:    set to true if using the UD filter.

    Int_t secDim = getStateDim();
    Int_t layDim = getStateDim(kLayCoord);
    const HKalPlane &virtPlane = getHitVirtPlane(iHit);
    const TVectorD  &svSec     = getStateVec(stateType, kSecCoord);

    TVectorD svLay(layDim);
    HKalTrackState::transformFromSectorToLayer(svLay, svSec, virtPlane);
    setStateVec(stateType, svLay, kLayCoord);

    TMatrixD jacSecLay(layDim, secDim);
    calcJacSecToLay(jacSecLay, svSec, virtPlane);
    TMatrixD jacSecLayTrans = TMatrixD(TMatrixD::kTransposed, jacSecLay);

    const TMatrixD &covSec = getStateCovMat(stateType, kSecCoord);

    if(bCovUD) {
        TMatrixD U(secDim, secDim);
        TMatrixD D(secDim, secDim);
        HKalMatrixTools::resolveUD(U, D, covSec);

        TMatrixD covSecUD = U * D * TMatrixD(TMatrixD::kTransposed, U);
        TMatrixD covLay = jacSecLay * covSecUD * jacSecLayTrans;
        if(!covLay.IsSymmetric()) {
            HKalMatrixTools::makeSymmetric(covLay);
        }
        HKalMatrixTools::decomposeUD(covLay);
        setStateCovMat(stateType, covLay, kLayCoord);
    } else {
        TMatrixD covLay = jacSecLay * covSec * jacSecLayTrans;
        if(!covLay.IsSymmetric()) {
            HKalMatrixTools::makeSymmetric(covLay);
        }
        setStateCovMat(stateType, covLay, kLayCoord);
    }

}

TMatrixD const& HKalTrackSite::getErrMat(Int_t idx) const {
    // Return the measurement covariance matrix.
    //
    // idx: The index of the hit stored in this measurement site.
    //      -1 will return the effective covariance which is
    //      used for the annealing filter. This matrix is a weighted
    //      mean of the measurement errors of all competing
    //      hits and must be calculated by calling the calcEffErrMat()
    //      function beforehand.

    if(idx < 0) {

#if kalDebug > 0
        if(getHitsTotalWeight() > 0. && effErrMat.NonZeros() == 0) {
            Warning("getErrMat()", "Effective measurement covariance is zero.");
            effErrMat.Print();
        }
#endif

        return effErrMat;
    }
    return getHit(idx).getErrMat();
}

HKalMdcHit const& HKalTrackSite::getHit(Int_t idx) const {
    // Get hit at index idx that this site contains.

    if(idx >= hits->GetEntries() || idx < 0 || hits->GetEntries() == 0) {
        Error("getHit()", Form("No hit at index %i. %i hits are stored.", idx, hits->GetEntries()));
        return (const HKalMdcHit&) (* (HKalMdcHit*)hits->At(0));
    } else {
        return (const HKalMdcHit&) (* (HKalMdcHit*)hits->At(idx));
    }
}

Double_t HKalTrackSite::getHitsTotalWeight() const {
    // Returns the sum of the weights from all hits in this site.

    Double_t totalWeight = 0.;
    for(Int_t i = 0; i < getNcompetitors(); i++) {
        totalWeight += getHit(i).getWeight();
    }
    return totalWeight;
}

TVectorD const& HKalTrackSite::getHitVec(Int_t idx) const {
    // Return the hit vector (measurement vector).
    //
    // idx: The index of the hit stored in this measurement site.
    //      -1 will return the effective measurement which is
    //      used for the annealing filter. The effective measurement
    //      is a weighted mean of the measurements of all competing
    //      hits and must be calculated by calling the calcEffMeasVec()
    //      function beforehand.

    if(idx < 0) {

#if kalDebug > 0
        if(getHitsTotalWeight() > 0. && effMeasVec.NonZeros() == 0) {
            Warning("getHitVec()", "Effective measurement vector is zero.");
        }
#endif

        return effMeasVec;
    }
    return getHit(idx).getHitVec();
}

void HKalTrackSite::setNdafs(Int_t n) {
    // Change number of DAF iterations.

    for(Int_t i = 0; i < getNcompetitors(); i++ ) {
        getHitPtr(i)->setNdafs(n);
    }
}

void HKalTrackSite::getPosAndDirFromState(TVector3 &pos, TVector3 &dir, kalFilterTypes stateType) const {
    // Extracts a position and direction vector from a track state's parameters.
    //
    // Output:
    // pos:       returns the hit coordinate vector
    // dir:       returns the track direction vector
    // Input:
    // stateType: track state type (kPredicted, kFiltered, kSmoothed, kInvFiltered)

    if(getHitType() == Kalman::kWireHit) {
	const HKalPlane &plane = getHit().getVirtPlane();
	trackStates[kSecCoord][stateType]->calcPosAndDirAtPlane(pos, dir, plane);
    } else {
	const HKalPlane &plane = getHit().getMeasLayer();
	trackStates[kSecCoord][stateType]->calcPosAndDirAtPlane(pos, dir, plane);
    }
}

void HKalTrackSite::setEffErrMat(const TMatrixD &errMat) {
#if kalDebug > 0
    if(errMat.GetNrows() != getMeasDim() || errMat.GetNcols() != getMeasDim()) {
        Warning("setEffErrMat()", Form("Measurement vector is %i-dimensional. New effective error matrix is %ix%i.",
                                       errMat.GetNrows(), errMat.GetNcols(), getMeasDim()));
    } else {
        effErrMat = errMat;
    }
#else
    effErrMat = errMat;
#endif
}

void HKalTrackSite::setEffMeasVec(const TVectorD &measVec) {
#if kalDebug > 0
    if(measVec.GetNrows() != getMeasDim()) {
        Warning("setEffMeasVec()", Form("New effective measurement vector is %i-dimensional, but must be %i-dimensional.",
                                        measVec.GetNrows(), getMeasDim()));
    } else {
        effMeasVec = measVec;
    }
#else
    effMeasVec = measVec;
#endif
}

