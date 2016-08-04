#include "hades.h"
#include "hmdccal2par.h"
#include "hmdcsizescells.h"
#include "hruntimedb.h"

#include "hkalfiltwire.h"


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

Bool_t HKalFiltWire::calcDriftDist(Double_t &dist, Double_t &distErr, Double_t &alpha,
				   Int_t iSite, Int_t iHit) const {
    // Converts measured drift time to drift distance.
    // The calculated drift distance depends on the predicted
    // state vectorr.
    //
    // output:
    // dist:    drift distance [nm].
    // distErr: error of drift distance [nm].
    // alpha:   impact angle [0-90 degrees].
    //
    // input:
    // iSite:   index of site.
    // iHit:    index of competing hit of the site.

    HKalTrackSite *site = getSite(iSite);

    if(iHit < 0 || iHit > site->getNcompetitors() - 1) {
        if(getPrintErr()) {
            Error("calcDriftDist()",
                  Form("Hit %i in site number %i not found. Site has %i competing hits.",
                       iHit, iSite, site->getNcompetitors()));
        }
        return kFALSE;
    }

    Double_t driftTime = TMath::Abs(site->getHitDriftTime(iHit));

    // Calculate the intersection of the track with the drift cell.
    // The track is approximated as a straight line definfed by the
    // predicted track position and direction.

    TVector3 posAt, dir;
    site->getPosAndDirFromState(posAt, dir, Kalman::kPredicted);

    Int_t s = site->getSector();
    Int_t m = site->getModule();
    Int_t l = site->getLayer();
    Int_t c = site->getCell(iHit);

    Double_t mindist = 0.;
    // Derive impact angle alpha from predicted state vector.
    getImpact(alpha, mindist, posAt, dir, s, m, l, c);

    if(!TMath::Finite(alpha)) {
	if(getPrintErr()) {
	    Error("calcDriftDist()", Form("Calculated non-finite value for impact angle %f.", alpha));
	}
	return kFALSE;
    }

    // The HMdcCal2Par container can convert the measured drift time
    // and the impact angle into a drift radius.
    HMdcCal2Par *cal2   = (HMdcCal2Par*)gHades->getRuntimeDb()->getContainer("MdcCal2Par");
    if(!cal2) {
        if(getPrintErr()) {
            Error("calcDriftDist()", "Container MdcCal2Par not found.");
        }
        return kFALSE;
    }

    dist   = cal2->calcDistance(s, m, alpha, driftTime);
    /*
    Double_t sigTof[4] = { 1.23, 1.25, 1.5, 1.46 }; //? ns
    Double_t vd[4]     = { 1./24., 1./24., 1./25., 1./25.5 }; //? mm/ns
    Double_t errTof    = vd[m] * sigTof[m];

    distErr = cal2->calcDistanceErr(s, m, alpha, driftTime)  + errTof;
    */
    //?
    distErr = TMath::Sqrt((site->getHit(iHit).getErrMat())(0,0));

#if kalDebug > 2
    cout<<"Converted drift time "<<driftTime<<" to radius "<<dist<<" with error: "<<distErr<<endl;
#endif

    if(dist < 0.) {
	if(dist > -0.5) {
	    dist = 0.;
	} else {
	    if(getPrintErr()) {
		Error("filter()", Form("Drift time %f with impact angle %f yielded negative distance %f to sense wire.",
				       driftTime, alpha, dist));
	    }
	    return kFALSE;
	}
    }

    return kTRUE;
}


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


    HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();  // check if is already there
    if(!fSizesCells) {
	fSizesCells = HMdcSizesCells::getObject();
	fSizesCells->initContainer();
    }

    Int_t s = site->getSector();
    Int_t m = site->getModule();
    Int_t l = site->getLayer();
    Int_t c = site->getCell();
    HMdcSizesCellsLayer  &fSizesCellsLayer  = (*fSizesCells)[s][m][l];

    // Rotation matrix from sector coordinate system to  rotated layer system: array indices 0-8.
    // Translation vector from layer coordinate system to rotated layer system: array indices 9-11.
    const Double_t *transRotLayArr = fSizesCellsLayer.getRLSysRSec(c);

    // Track state has to be transformed from track to sector and then from sector to
    // rotated layer coordinates.
    // Calculate the composition R of both rotations.
    // For the projector matrix, only the matrix elements for the calculation
    // of the v component matter.
    Double_t R1 = transRotLayArr[1] * transSec(0,0) + transRotLayArr[4] * transSec(1,0) + transRotLayArr[7] * transSec(2,0);
    Double_t R4 = transRotLayArr[1] * transSec(0,1) + transRotLayArr[4] * transSec(1,1) + transRotLayArr[7] * transSec(2,1);
    Double_t R7 = transRotLayArr[1] * transSec(0,2) + transRotLayArr[4] * transSec(1,2) + transRotLayArr[7] * transSec(2,2);

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

    // In order to calculate the drift radius from a track state with position vector (x,y,z)
    // on the measurement layer and track direction tangents tx and ty, transform the
    // position and direction vectors to the wire coordinate system.

    const TVectorD &sv = site->getStateVec(Kalman::kPredicted);

    // Transform track position to wire system. v is component on the layer that is
    // perpendicular to the wire.
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
    // Calculate position in wire coordinates.
    Double_t v, w;
    //v = R1 * posAt.X() + R4 * posAt.Y() + R7 * posAt.Z() - (fSizesCellsLayer[c]).getWirePos()
    //    - (transRotLayArr[1]*transRotLayArr[9] + transRotLayArr[4]*transRotLayArr[10] + transRotLayArr[7]*transRotLayArr[11]);
    // posAt.Z() is a function of posAt.X() and posAt.Y().
    fSizesCellsLayer.getYZinWireSys(posAt.X(), posAt.Y(), posAt.Z(), c, v, w);
    Double_t signV = (v < 0.) ? -1. : 1.;
    // No distinction between hits left and right of the wire.
    v = TMath::Abs(v);

    // Transform track direction vector (tx,ty,1).
    // tv = Tan(ThetaV) = Tan(pv / pw).
    Double_t tv = R1 * sv(kIdxTanPhi) + R4 * sv(kIdxTanTheta) + R7;

    // cosine of angle ThetaV.
    // cos(alpha)  = 1. / sqrt(1 + tan(alpha)^2)
    Double_t cv  = 1. / TMath::Sqrt (1. + TMath::Power(tv, 2.));
    Double_t cv3 = 1. / TMath::Power(1. + TMath::Power(tv, 2.), 1.5);

    // Projector matrix @h(a)/@a
    fProj(0, kIdxX0)       = signV * (R1 - R7 * n.X()/n.z()) * cv;
    fProj(0, kIdxY0)       = signV * (R4 - R7 * n.Y()/n.Z()) * cv;
    fProj(0, kIdxTanPhi)   = - v * R1 * tv * cv3;
    fProj(0, kIdxTanTheta) = - v * R4 * tv * cv3;
    fProj(0, kIdxQP)       = 0.;

    site->setStateProjMat(Kalman::kFiltered, fProj);

    return kTRUE;
}

//  --------------------------------
//  Implementation of public methods
//  --------------------------------

Bool_t HKalFiltWire::calcMeasVecFromState(TVectorD &projMeasVec, HKalTrackSite const* const site,
                                          Kalman::kalFilterTypes stateType, Kalman::coordSys sys,
                                          Int_t iHit) const {
    // Extracts measurement vector from state vector for segment hits.
    //
    // Output:
    // projMeasVec: The calculated measurement vector.
    // Input:
    // site:      Pointer to current site of the track.
    // stateType: Which state (predicted, filtered, smoothed) to project on a measurement vector.
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
    getImpact(alpha, mindist, posAt, dir, s, m, l, c);

    projMeasVec(0) = mindist;
    return kTRUE;
}

Bool_t HKalFiltWire::filter(Int_t iSite) {
    // Run the filter step in Kalman filter for a site.
    // Must be called after the prediction step.
    // Different Kalman filter equations are used as set by setFilterMethod().
    // iSite: index of site to filter

    Double_t dist    = 0.;
    Double_t distErr = 0.;
    Double_t alpha   = 0.;
    if(calcDriftDist(dist, distErr, alpha, iSite)) {
	// Store drift radius as the "measurement" vector that is used for filtering.
	HKalTrackSite *site = getSite(iSite);
	TVectorD driftDist(1);
	driftDist[0] = dist;
	TVectorD driftDistErr(1);
        driftDistErr[0] = distErr;
	site->setHitAndErr(driftDist, driftDistErr);
	site->setHitImpactAngle(alpha);
    } else {
        return kFALSE;
    }

    switch(getFilterMethod()) {
    case Kalman::kKalConv:
        filterConventional(iSite);
        break;
    case Kalman::kKalJoseph:
        filterJoseph(iSite);
        break;
    case Kalman::kKalUD:
        filterUD(iSite);
        break;
    case Kalman::kKalSeq:
        filterSequential(iSite);
        break;
    case Kalman::kKalSwer:
        filterSwerling(iSite);
        break;
    default:
        filterConventional(iSite);
        break;
    }

    return kTRUE;
}

Bool_t HKalFiltWire::getImpact(Double_t& alpha, Double_t& mindist, const TVector3 &pos, const TVector3 dir,
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
    // pos: track position.
    // dir: track direction.
    // sec/mod/lay/cell: Sector, module, layer, drift cell number.

    TVector3 pos1 = pos;
    TVector3 pos2 = pos + 0.5 * dir;

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
    return (fSizesCellsLayer.getImpact(pos1.x(), pos1.y(), pos1.z(),
                                       pos2.x(), pos2.y(), pos2.z(),
                                       cell, alpha, mindist));
}
