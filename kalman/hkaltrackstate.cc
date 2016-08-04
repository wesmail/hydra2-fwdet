
#include "TMatrixD.h"
#include "TMath.h"
#include "TVector3.h"

#include "hkalplane.h"
#include "hkaltrackstate.h"

#include <iostream>
using namespace std;

ClassImp(HKalTrackState)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// The state of the particle track at a given site k can be described by a vector
// a_k containing a set of track parameters:
// 1. x0: x position
// 2. y0: y position
// 3. tx: p_x / p_z
// 4. ty: p_y / p_z
// 5. qp: particle charage in elementary charges divided by
//        magnitude of momentum in MeV/c
//
// The class also stores the matrices needed for the Kalman filter:
// 1. propagator: F_k = @f_k(a_k) / @a_k
// 2. projector : H_k = @h_k(a_k) / @a^(k-1)_k
// 3. covariance
// 4. process noise
// where f_k(a_k) is the propagator function that propagates the track to
// the next site and h_k(a_k) projects the track state onto a measurement vector.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalTrackState::HKalTrackState(Kalman::kalFilterTypes stateType, Int_t measDim, Int_t stateDim)
: TObject(),
  fPropagator(stateDim, stateDim),
  fProjector(measDim, stateDim),
  fCovariance(stateDim, stateDim),
  fProcessNoise(stateDim, stateDim),
  stateVec(stateDim) {
    // Create state with dummy state vector and matrices.
    // stateType: state is either for the prediction, filtering, smoothing or inverse filtering step in the Kalman filter
    // measDim:  dimension of measurement vector
    // stateDim: dimension of state vector.

    type = stateType;
    fPropagator.UnitMatrix();
    fProjector.UnitMatrix();
    fCovariance.UnitMatrix();
}

HKalTrackState::HKalTrackState(Kalman::kalFilterTypes stateType, const TVectorD &sv, Int_t measDim)
: TObject(),
  fPropagator(sv.GetNrows(), sv.GetNrows()),
  fProjector(measDim, sv.GetNrows()),
  fCovariance(sv.GetNrows(), sv.GetNrows()),
  fProcessNoise(sv.GetNrows(), sv.GetNrows()),
  stateVec(sv) {
    // Create a state with dummy matrices and sets the state vector equal to function
    // parameter sv.
    // stateType: state is either for the prediction, filtering, smoothing or inverse filtering step in the Kalman filter
    // sv:        state vector.
    // measDim:   dimension of measurement vector.

    type = stateType;
    fPropagator.UnitMatrix();
    fProjector.UnitMatrix();
    fCovariance.UnitMatrix();
}

HKalTrackState::~HKalTrackState() {
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

void HKalTrackState::calcDir(TVector3 &dir) const {
    // Calculates a direction unit vector from this state.
    // dir: track direction (return value).

    calcDir(dir, stateVec);
}

void HKalTrackState::calcDir(TVector3 &dir, const TVectorD &sv) {
    // Calculates a direction unit vector from a state vector given by function
    // parameter sv.
    // dir: track direction (return value)
    // sv:  state vector to calculate direction from.

    Double_t tanx = sv(kIdxTanPhi);
    Double_t tany = sv(kIdxTanTheta);
    Double_t qp   = sv(kIdxQP);

    dir.SetZ( 1./(TMath::Abs(qp) * TMath::Sqrt(tanx*tanx + tany*tany + 1)) );
    dir.SetX(tanx * dir.Z());
    dir.SetY(tany * dir.Z());
    dir = dir.Unit();
}


void HKalTrackState::calcMomVec(TVector3 &dir) const {
    // Calculates the momentum vector from this state.
    // dir: track momentum (return value).

    calcMomVec(dir, stateVec);
}

void HKalTrackState::calcMomVec(TVector3 &dir, const TVectorD &sv) {
    // Calculates momentum vector from a state vector given by function
    // parameter sv.
    // dir: track momentum (return value)
    // sv:  state vector to calculate direction from.

    Double_t tanx = sv(kIdxTanPhi);
    Double_t tany = sv(kIdxTanTheta);
    Double_t qp   = sv(kIdxQP);

    dir.SetZ( 1./(TMath::Abs(qp) * TMath::Sqrt(tanx*tanx + tany*tany + 1)) );
    dir.SetX(tanx * dir.Z());
    dir.SetY(tany * dir.Z());
}

Bool_t HKalTrackState::calcPosAtPlane(TVector3 &pos, const HKalPlane &plane) const {
    // Calculate the track position as a three dimensional vector from this state.
    // Only two coordinates are stored in the state vector. The z-coordinate is
    // calculated by going in z-direction and find the intersection with a plane.
    // pos:   track position (return value).
    // plane: the plane the track is on.

    return calcPosAtPlane(pos, plane, stateVec);
}

Bool_t HKalTrackState::calcPosAtPlane(TVector3 &pos, const HKalPlane &plane, const TVectorD &sv) {
    // Calculate the track position as a three dimensional vector from the state
    // given by function parameter sv.
    // Only two coordinates are stored in the state vector. The z-coordinate is
    // calculated by going in z-direction and find the intersection with a plane.
    // pos:   track position (return value).
    // plane: the plane the track is on.

    TVector3 posFrom;
    posFrom.SetX(sv(kIdxX0));
    posFrom.SetY(sv(kIdxY0));
    posFrom.SetZ(0.);
    TVector3 dir(0., 0., 1.);
    return plane.findIntersection(pos, posFrom, dir);
}

Bool_t HKalTrackState::calcPosAndDirAtPlane(TVector3 &pos, TVector3 &dir, const HKalPlane &plane) const {
    // Calculate the track position and direction as three dimensional vectors
    // from this state.
    // pos:   track position (return value)
    // dir:   track direction (return value)
    // plane: the plane the track is on

    calcDir(dir);
    return calcPosAtPlane(pos, plane);
}

void HKalTrackState::calcStateVec(TVectorD &sv, Double_t qp, const TVector3 &pos, const TVector3 &dir) {
    // Return the state vector from a position, direction and momentum.
    // sv:  the state vector (return value)
    // qp:  particle charage in elementary charges divided by magnitude of momentum in MeV/c
    // pos: track position
    // dir: track direction.

#if kalDebug > 0
    Int_t dim = sv.GetNrows();
    Int_t minDim = 5;
    if(dim < minDim) {
        sv.ResizeTo(minDim);
        ::Warning("calcStateVec", Form("Dimension of output function parameter is %i, but must be at least %i.", dim, minDim));
    }
#endif

    sv(kIdxX0)       = pos.x();
    sv(kIdxY0)       = pos.y();
    sv(kIdxTanPhi)   = dir.x() / dir.z();
    sv(kIdxTanTheta) = dir.y() / dir.z();
    sv(kIdxQP)       = qp;
    if(sv.GetNrows() > 5) {
        sv(kIdxZ0) = pos.z();
    }
}

void HKalTrackState::clear() {
    // Resets state vector to zeroes, matrices to unit matrix.

    stateVec.Zero();
    fPropagator.UnitMatrix();
    fProjector.UnitMatrix();
    fCovariance.UnitMatrix();
    fProcessNoise.Zero();
}

void HKalTrackState::print(const Option_t *opt) const {
    // Prints information about the state.
    // opt: Determines what information about the object will be printed.
    // Capitalization does not matter.
    // If opt contains:
    // "S": print state vector
    // "F": print propagator matrix
    // "C": print covariance matrix
    // "H": print projector matrix
    // "Q": print process noise matrix
    // If opt is empty all of the above will be printed.

    TString stropt(opt);

    switch (type) {
    case kPredicted:
        cout<<"**** Predicted state: ****"<<endl;
        break;
    case kFiltered:
        cout<<"**** Filtered state: ****"<<endl;
        break;
    case kSmoothed:
        cout<<"**** Smoothed state: ****"<<endl;
        break;
    case kInvFiltered:
        cout<<"**** Inverse filtered vector: ****"<<endl;
        break;
    }
    if(stropt.Contains("S", TString::kIgnoreCase) || stropt.IsNull()) {
        cout<<"State vector:"<<endl;
        stateVec.Print();
    }
    if(stropt.Contains("F", TString::kIgnoreCase) || stropt.IsNull()) {
        cout<<"Propagator matrix:"<<endl;
        fPropagator.Print();
    }
    if(stropt.Contains("C", TString::kIgnoreCase) || stropt.IsNull()) {
        cout<<"Covariance matrix:"<<endl;
        fCovariance.Print();
    }
    if(stropt.Contains("H", TString::kIgnoreCase) || stropt.IsNull()) {
        cout<<"Projector matrix:"<<endl;
        fProjector.Print();
    }
    if(stropt.Contains("Q", TString::kIgnoreCase) || stropt.IsNull()) {
        cout<<"Process noise matrix:"<<endl;
        fProcessNoise.Print();
    }
    cout<<"**** End of track state print. ****"<<endl;
}

void HKalTrackState::transform(const TRotation &transMat, const HKalPlane &plane) {
    // Transform the state vector of this object using a rotation matrix.
    // transMat: the rotation matrix
    // plane:    the plane the track is on. Needed to calculate the z-coordinate of the
    //           track position.

    TVector3 pos; // track position
    TVector3 dir; // track direction
    calcPosAndDirAtPlane(pos, dir, plane);
    pos.Transform(transMat);
    dir.Transform(transMat);
    Double_t qp = getStateParam(kIdxQP);
    setStateVec(qp, pos, dir);
}

void HKalTrackState::transformFromLayerToSector(TVectorD &svSec, const TVectorD &svLay, const HKalPlane &plane) {
    // Transform a state vector given in virtual layer coordinates to sector coordinates.
    //
    // Output:
    // svSec: Track state vector in sector coordinates.
    //
    // Input:
    // svLay: Track state vector in virtual layer coordinates.
    // plane: The plane defining the virtual layer system.
    //
    // State vector in sector coordinate system: (x,y,tx,ty,qp).
    // State vector in layer  coordinate system: (u,v,tu,tv,qp).
    // Layer coordinate system is defined by an origin vector O,
    // two orthogonal Axis U and V on the layer and a normal vector W.
    // All axis are given in sector coordinate system.
    //
    // Transformation from layer to sector:
    // position in sector coordinates:
    // P = O + u*U + v*V
    // direction in sector coordinates:
    // A = 1/sqrt(1 + tu^2 + tv^2) * (w + tu*U + tv*V)
    // tx = A.x() / A.z()
    // ty = A.y() / A.z()

#if kalDebug > 0
    if(svLay.GetNrows() != 5) {
        ::Error("transformFromSectorToLayer()", "Wrong dimension in input state vector svLay. No coordinate transformation done.");
        return;
    }
    if(svSec.GetNrows() != 5 && svSec.GetNrows() != 6) {
        ::Error("transformFromSectorToLayer()", Form("Wrong dimension in output state vector svSec."));
        return;
    }
#endif

    const TVector3 &origin = plane.getCenter();
    const TVector3 &u      = plane.getAxisU();
    const TVector3 &v      = plane.getAxisV();
    const TVector3 &w      = plane.getNormal();

    TVector3 pos = origin + svLay(kIdxX0) * u + svLay(kIdxY0) * v;

    svSec(kIdxX0) = pos.X();
    svSec(kIdxY0) = pos.Y();
    if(svSec.GetNrows() > 5) {
        svSec(kIdxZ0) = pos.Z();
    }

    Double_t tu  = svLay(kIdxTanPhi);
    Double_t tv  = svLay(kIdxTanTheta);
    TVector3 dir = w + tu*u + tv*v;

    svSec(kIdxTanPhi)   = dir.X() / dir.Z();
    svSec(kIdxTanTheta) = dir.Y() / dir.Z();

    svSec(kIdxQP)       = svLay(kIdxQP);
}

void HKalTrackState::transformFromSectorToLayer(TVectorD &svLay, const TVectorD &svSec, const HKalPlane &plane) {
    // Transform a state vector given in virtual layer coordinates to sector coordinates.
    //
    // Output:
    // svLay: Track state vector in virtual layer coordinates.
    //
    // Input:
    // svSec: Track state vector in sector coordinates.
    // plane: The plane defining the virtual layer system.
    //
    // State vector in sector coordinate system: (x,y,tx,ty,qp).
    // State vector in layer  coordinate system: (u,v,tu,tv,qp).
    // Layer coordinate system is defined by an origin vector O,
    // two orthogonal Axis U and V on the layer and a normal vector W.
    // All axis are given in sector coordinate system.
    //
    // Transformation from sector to layer:
    // position on layer:
    // u = (pos_sector - O) * U
    // v = (pos_sector - O) * V
    // direction on layer:
    // tu = (A * U) / (A * W)
    // tv = (A * V) / (A * W)
    // with A = 1/sqrt(1 + tx^2 + ty^2) * (tx, ty, 1)^T the normalized direction vector in the sector coordinate system

#if kalDebug > 0
    if(svSec.GetNrows() != 5 && svSec.GetNrows() != 6) {
        ::Error("transformFromSectorToLayer()", "Wrong dimension in input state vector svSec. No coordinate transformation done.");
        return;
    }
#endif

    Int_t sdim = 5;
    if(svLay.GetNrows() != sdim) {
        svLay.ResizeTo(sdim);
        ::Warning("transformFromSectorToLayer()", Form("Wrong dimension in input state vector svLay. Resized to %ix1", sdim));
    }


    const TVector3 &origin = plane.getCenter();
    const TVector3 &u      = plane.getAxisU();
    const TVector3 &v      = plane.getAxisV();
    const TVector3 &w      = plane.getNormal();

    TVector3 pos;
    TVector3 dir;
    calcPosAtPlane(pos, plane, svSec);
    calcDir(dir, svSec);

    TVector3 diffSecOrg = pos - origin;
    Double_t aw         = dir * w;

    svLay(kIdxX0)       = diffSecOrg * u;
    svLay(kIdxY0)       = diffSecOrg * v;
    svLay(kIdxTanPhi)   = dir * u / aw;
    svLay(kIdxTanTheta) = dir * v / aw;
    svLay(kIdxQP)       = svSec(kIdxQP);
}


void HKalTrackState::setCovMat(const TMatrixD &fCov) {
    // Replace the state's covariance matrix.

#if kalDebug > 0
    Int_t nRowsOld = getCovMat().GetNrows();
    Int_t nColsOld = getCovMat().GetNcols();
    Int_t nRowsNew = fCov.GetNrows();
    Int_t nColsNew = fCov.GetNcols();
    if(nRowsOld != nRowsNew || nColsOld != nColsNew) {
        Error("setCovMat()", Form("Matrices not compatible. Cannot replace %ix%i matrix with %ix%i matrix.", nRowsOld, nColsOld, nRowsNew, nColsNew));
        exit(1);
    }
#endif

    fCovariance = fCov;
}

void HKalTrackState::setPropMat(const TMatrixD &fProp) {
    // Replace the state's propagator matrix.

#if kalDebug > 0
    Int_t nRowsOld = getPropMat().GetNrows();
    Int_t nColsOld = getPropMat().GetNcols();
    Int_t nRowsNew = fProp.GetNrows();
    Int_t nColsNew = fProp.GetNcols();
    if(nRowsOld != nRowsNew || nColsOld != nColsNew) {
        Error("setPropMat()", Form("Matrices not compatible. Cannot replace %ix%i matrix with %ix%i matrix.", nRowsOld, nColsOld, nRowsNew, nColsNew));
        exit(1);
    }
#endif

    fPropagator = fProp;
}

void HKalTrackState::setProjMat(const TMatrixD &fProj) {
    // Replace the state's projector matrix.

#if kalDebug > 0
    Int_t nRowsOld = getProjMat().GetNrows();
    Int_t nColsOld = getProjMat().GetNcols();
    Int_t nRowsNew = fProj.GetNrows();
    Int_t nColsNew = fProj.GetNcols();
    if(nRowsOld != nRowsNew || nColsOld != nColsNew) {
        Error("setProjMat()", Form("Matrices not compatible. Cannot replace %ix%i matrix with %ix%i matrix.", nRowsOld, nColsOld, nRowsNew, nColsNew));
        exit(1);
    }
#endif
    fProjector = fProj;
}

void HKalTrackState::setProcNoiseMat(const TMatrixD &fProc) {
    // Replace the state's process noise matrix.

    fProcessNoise.ResizeTo(fProc.GetNrows(), fProc.GetNcols());
    fProcessNoise = fProc;
}

void HKalTrackState::setStateVec(const TVectorD &sv) {
    // Replace the state's track parameter vector.

#if kalDebug > 0
    Int_t oldStateDim = getStateVec().GetNrows();
    Int_t newStateDim = sv.GetNrows();
    if(oldStateDim != newStateDim) {
        Error("setStateVec()", Form("Dimension of new state vector (%i) not the same as dimension of current state vector (%i).", newStateDim, oldStateDim));
        exit(1);
    }
#endif
    stateVec = sv;
}
