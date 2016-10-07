#include "hades.h"
#include "hmdccal2par.h"
#include "hmdcsizescells.h"
#include "hruntimedb.h"

#include "hkalfiltwire.h"
#include "hkalgeomtools.h"


//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// Kalman filter for wire hits. See doc for HKalIFilter
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

ClassImp (HKalFiltWire)

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalFiltWire::HKalFiltWire(Int_t n, Int_t measDim, Int_t stateDim,
                       HMdcTrackGField *fMap, Double_t fpol)
    : HKalIFilt(n, measDim, stateDim, fMap, fpol) {

    setHitType(Kalman::kWireHit);
}

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------

Bool_t HKalFiltWire::calcProjector(Int_t iSite) const {
    // The projection function h(a) extracts a measurement vector from a track state a.
    // The projector matrix is the Jacobian of this function, i.e. @h(a)/@a.
    // The projector matrix is stored in the site's filtered track state.

    HKalTrackSite *site = getSite(iSite);
    if(!site) {
        return kFALSE;
    }

    Int_t mdim = getMeasDim();
    Int_t sdim = getStateDim();

    TMatrixD fProj(mdim, sdim);

    fProj.Zero();

    // Matrix that transforms from track coordinate system (sector coordinates
    // rotated in initial track direction) to sector coordinate system.
    TRotation transSec = getRotMat()->Inverse();


    HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();
    if(!fSizesCells) {
	fSizesCells = HMdcSizesCells::getObject();
	fSizesCells->initContainer();
    }

    Int_t s = site->getSector();
    Int_t m = site->getModule();
    Int_t l = site->getLayer();
    Int_t c = site->getCell();
    HMdcSizesCellsLayer  &fSizesCellsLayer  = (*fSizesCells)[s][m][l];
    HMdcSizesCellsCell   &fSizesCellsCell   = (*fSizesCells)[s][m][l][c];

    // Track state has to be transformed from track to sector and then from
    // sector to rotated layer coordinates.
    // Calculate the composition R of both rotations.
    // For the projector matrix, only the matrix elements for the calculation
    // of the v component matter.

    // Rotation matrix from rotated layer system to sector coordinates.
    // Needs to be inverted.
    const HGeomRotation& transRotLaySysRSec =
	fSizesCellsLayer.getRotLaySysRSec(c).getRotMatrix();
    HGeomRotation transRotSecToRotLay = transRotLaySysRSec.inverse();
    HGeomVector transVecRotLayToSec =
	fSizesCellsLayer.getRotLaySysRSec(c).getTransVector();

    // Compose track to sector rotation and sector to rotated layer rotation.
    Double_t R1 =
	transRotSecToRotLay.getElement(1,0) * transSec(0,0) +
	transRotSecToRotLay.getElement(1,1) * transSec(1,0) +
	transRotSecToRotLay.getElement(1,2) * transSec(2,0);
    Double_t R4 =
	transRotSecToRotLay.getElement(1,0) * transSec(0,1) +
	transRotSecToRotLay.getElement(1,1) * transSec(1,1) +
	transRotSecToRotLay.getElement(1,2) * transSec(2,1);
    Double_t R7 =
	transRotSecToRotLay.getElement(1,0) * transSec(0,2) +
	transRotSecToRotLay.getElement(1,1) * transSec(1,2) +
	transRotSecToRotLay.getElement(1,2) * transSec(2,2);

    // Wire coordinate system:
    // Axis U points along wire.
    // Axis V is on layer and perpendicular to wire.
    // Axis W is perpendicular to U and V.

    //     W
    //
    //     ^
    //     |  |------------*----|
    //     |  | cell        *  -|---ThetaV
    //     |  |            / *  |
    //     |  |           /90 * |
    //     |  | driftDist/     *|
    //     |  |         /       *
    //   --|--|--------*^-|-----|*---------> V
    //     |  |      Wire |     | *
    //     |  |           |     |  *
    //        |         ThetaV  |   *
    //        |                 |    *
    //        |-----------------|     * track

    // Projection function:
    // h(a) = driftDist = v * cos(ThetaV) = v / sqrt(1 + tan(ThetaV)^2)

    // In order to calculate the drift radius from a track state with position
    // vector (x,y,z) on the measurement layer and track direction tangents tx
    // and ty, transform the position and direction vectors to the rotated
    // layer coordinate system.

    const TVectorD &sv = site->getStateVec(Kalman::kPredicted);

    // Transform track position to wire system. v is component on the layer
    // that is perpendicular to the wire.
    TVector3 posAt;

    if(!HKalTrackState::calcPosAtPlane(posAt, site->getHitMeasLayer(), sv)) {
	if(getPrintErr()) {
	    Error("calcProjector()", "Could not extract a position from track state");
	}
	return kFALSE;
    }

    const TVector3 &n = site->getHitMeasLayer().getNormal();

    // Rotate from track to sector coordinates.
    if(getRotation() != Kalman::kNoRot) {
	posAt.Transform(transSec);
    }

    // Transform to rotated layer coordinates.
    Double_t vWire = fSizesCellsCell.getWirePos();
    // Transform position from state vector to rotated layer coordinates.
    Double_t v =
	transRotSecToRotLay.getElement(1,0) *
	(posAt.X()-transVecRotLayToSec.X()) +
	transRotSecToRotLay.getElement(1,1) *
	(posAt.Y()-transVecRotLayToSec.Y()) +
	transRotSecToRotLay.getElement(1,2) *
	(posAt.Z()-transVecRotLayToSec.Z());
    Double_t dv = v - vWire;

    // Transform track direction vector (tx,ty,1).
    // tv = Tan(ThetaV) = Tan(pv / pw).
    Double_t tv = R1 * sv(kIdxTanPhi) + R4 * sv(kIdxTanTheta) + R7;

    // cosine of angle ThetaV.
    // cos(alpha)  = 1. / sqrt(1 + tan(alpha)^2)
    Double_t cv  = 1. / TMath::Sqrt (1. + TMath::Power(tv, 2.));
    Double_t cv3 = 1. / TMath::Power(1. + TMath::Power(tv, 2.), 1.5);

    fProj(0, kIdxX0)       = (R1 - R7 * n.X()/n.z()) * cv;
    fProj(0, kIdxY0)       = (R4 - R7 * n.Y()/n.Z()) * cv;
    fProj(0, kIdxTanPhi)   = - dv * R1 * tv * cv3;
    fProj(0, kIdxTanTheta) = - dv * R4 * tv * cv3;
    fProj(0, kIdxQP)       = 0.;

    site->setStateProjMat(Kalman::kFiltered, fProj);

    return kTRUE;
}

//  --------------------------------
//  Implementation of public methods
//  --------------------------------

Bool_t HKalFiltWire::calcMeasVecFromState(TVectorD &projMeasVec, HKalTrackSite const* const site,
					  Kalman::kalFilterTypes stateType,
					  Kalman::coordSys sys) const {
    // Extracts measurement vector from state vector for segment hits.
    //
    // Output:
    // projMeasVec: The calculated measurement vector.
    // Input:
    // site:      Pointer to current site of the track.
    // stateType: Which state (predicted, filtered, smoothed) to project on a
    //            measurement vector.
    // sys:       Coordinate system to use (sector or virtual layer coordinates)
    // iHit:      Index of the competitor in the measurement hit.

    const TVectorD &sv = site->getStateVec(stateType);

    TVector3 posAt;
    if(!HKalTrackState::calcPosAtPlane(posAt, site->getHitVirtPlane(), sv)) {
	if(getPrintErr()) {
	    Error("calcMeasVecFromState()", "Could not extract position vector from track state.");
	}
	return kFALSE;
    }
    TVector3 dir;
    HKalTrackState::calcDir(dir, sv);

    // State vector is in sector coordinates.
    // The projection of the track state onto a measurement vector is
    // the insection of a straight line defined by the track position
    // and direction with the drift chamber cell.

    Double_t mindist, alpha;
    Int_t s = site->getSector();
    Int_t m = site->getModule();
    Int_t l = site->getLayer();
    Int_t c = site->getCell(0);
    Double_t driftTime = TMath::Abs(site->getHitDriftTime(0));
    Bool_t bOk = getImpact(alpha, mindist, driftTime, posAt, dir, s, m, l, c);

    projMeasVec(0) = mindist;
    return bOk;
}

Bool_t HKalFiltWire::getImpact(Double_t& alpha, Double_t& mindist,
                               Double_t driftTime,
			       const TVector3 &pos, const TVector3 dir,
			       Int_t sec, Int_t mod, Int_t lay, Int_t cell) const {
    // Calculates impact angle and drift distance of a straight line defined
    // by position and direction with an MDC cell. Returns true if the line
    // intersects the drift cell, otherwise false.
    //
    // Output:
    // alpha:   Impact angle, 0° <= alpha <= 90°.
    // mindist: Drift distance.
    //
    // Input:
    // driftTime: drift time
    // pos: track position.
    // dir: track direction.
    // sec/mod/lay/cell: Sector, module, layer, drift cell number.

    TVector3 pos1 = pos - dir;
    TVector3 pos2 = pos + dir;

    // getImpact() from fSizesCellsLayer expects sector coordinates.
    if(getRotation() != Kalman::kNoRot) {
        TRotation rotInv = getRotMat()->Inverse();
        pos1.Transform(rotInv);
        pos2.Transform(rotInv);
    }

    HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();  // check if is already there
    if(!fSizesCells) {
        fSizesCells = HMdcSizesCells::getObject();
        fSizesCells->initContainer();
    }

    HMdcSizesCellsLayer  &fSizesCellsLayer  = (*fSizesCells)[sec][mod][lay];

    // Calculate the impact angle alpha of the track with the drift cell.

    HMdcSizesCellsCell &fSizesCellsCell = (*fSizesCells)[sec][mod][lay][cell];
    HGeomVector &wire1 = fSizesCellsCell.getWirePnt1();
    TVector3 wirePoint1(wire1.X(), wire1.Y(), wire1.Z());
    HGeomVector &wire2 = fSizesCellsCell.getWirePnt2();
    TVector3 wirePoint2(wire2.X(), wire2.Y(), wire2.Z());

    Int_t iflag;
    Double_t length;
    TVector3 pcaFinal, pcaWire;

    HKalGeomTools::Track2ToLine(pcaFinal, pcaWire, iflag,
				mindist, length,
				TVector3(pos1.x(),pos1.y(),pos1.z()),
				TVector3(pos2.x(),pos2.y(),pos2.z()),
				wirePoint1, wirePoint2);

    Double_t yw, zw = 0.;
    fSizesCellsLayer.getYZinWireSys(pcaFinal.x(), pcaFinal.y(), pcaFinal.z(),
				    cell, yw, zw);
    if(yw < 0) mindist *= -1.;


    if(iflag != 0) {
	if(getPrintErr()) {
	    Error("getImpact()", "Error calculating point of closest approach.");
	}
	return kFALSE;
    }
    return kTRUE;
}
