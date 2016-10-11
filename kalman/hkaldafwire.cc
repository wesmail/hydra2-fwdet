#include "hmdcsizescells.h"

#include "hkaldafwire.h"
#include "hkalgeomtools.h"


//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// This class implements the deterministic annealing filter for
// wire hits. Competition between multiple wire hits on an MDC layer
// is allowed and the ambiguity on which side of the sense wire the
// particle passed by is taken into account.
// Also see doc for HKalIFilter.
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

ClassImp (HKalDafWire)

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalDafWire::HKalDafWire(Int_t n, Int_t measDim, Int_t stateDim,
			 HMdcTrackGField *fMap, Double_t fpol)
    : HKalDafSingleWire(n, measDim, stateDim, fMap, fpol) {
}

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------
Bool_t HKalDafWire::calcProjector(Int_t iSite) const {

    HKalTrackSite *site = getSite(iSite);
    if(!site) {
        return kFALSE;
    }

    TMatrixD fProj(getMeasDim(), getStateDim(Kalman::kLayCoord));
    fProj.Zero();
    fProj(0, kIdxY0) = 1.;

    site->setStateProjMat(Kalman::kFiltered, fProj, Kalman::kLayCoord);

    return kTRUE;
}

Bool_t HKalDafWire::calcVirtPlane(Int_t iSite) const {
    // Calculate virtual plane that will be aligned as follows:
    // One of the plane's axis points along the wire, the other one
    // from the point of closest approach (PCA) on the wire to the PCA
    // on the track.
    // The predicted track position will be the centre of the plane.
    // Using such a virtual plane will make the projection function and
    // projector matrix easy to calculate.

    HKalTrackSite *site = getSite(iSite);
    if(!site) {
	return kFALSE;
    }

    // Assume the track is a straight line in the vicinity of the current
    // track position.
    const TVectorD &predState = site->getStateVec(Kalman::kPredicted);
    TVector3 posAt;
    HKalTrackState::calcPosAtPlane(posAt, site->getHitMeasLayer(), predState);
    TVector3 dir;
    HKalTrackState::calcDir(dir, predState);

    TVector3 pos1 = posAt - dir;
    TVector3 pos2 = posAt + dir;

    // Number of competing hits.
    const Int_t nComp = site->getNcompetitors();

    // Left/right ambiguity.
    const Int_t nWires = nComp/2;

    for(Int_t iWire = 0; iWire < nWires; iWire++) {

	Int_t iHit = iWire*2;

	// Find the PCA to a segment defined by the wire endpoints.
	TVector3 wire1, wire2;
	site->getHitWirePts(wire1, wire2, iHit);

	Int_t iflag;
	Double_t length, mindist;
	TVector3 pcaTrack, pcaWire;

	// Find the PCA's on the wire and the track.
	HKalGeomTools::Track2ToLine(pcaTrack, pcaWire, iflag, mindist, length,
				    pos1, pos2, wire1, wire2);

	if(iflag == 1) {
	    if(getPrintWarn()) {
		Warning("calcVirtPlane()", "PCA is outside of wire");
	    }
	}
	if(iflag == 2) {
	    if(getPrintErr()) {
		Error("calcVirtPlane()", "Track parallel to wire.");
	    }
	    return kFALSE;
	}

	// Make virtual plane.
	// origin: current track position
	// u axis: along wire
	// v axis: from PCA on wire to PCA on track
	TVector3 u = (wire2 - wire1).Unit();
	TVector3 v = (pcaTrack - pcaWire).Unit();


	// Ensure v axis of virtual layer points in direction of positive y-axis
	// in the layer coordinate system.
	//const TVector3 &n = site->getHitMeasLayer().getNormal();   // gcc warning unused var

	HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();
	HMdcSizesCellsLayer &fSizesCellsLayer =
	    (*fSizesCells)[site->getSector()][site->getModule()][site->getLayer()];
	const HGeomTransform &transRotLaySysRSec =
	    fSizesCellsLayer.getRotLaySysRSec(site->getCell());

	HGeomVector ygeom(0., 1., 0.);
	transRotLaySysRSec.transFrom(ygeom);

	TVector3 y(ygeom.getX(), ygeom.getY(), ygeom.getZ());

	if(y.Dot(v) < 0.) {
	    v = -1. * v;
	}

	// Repeating the above for all competitors of a site would result in
	// different virtual layers. Since the wires are parallel and the track
	// is approximately a straight line these layers would be parallel, i.e.
	// its axis u and v would be (anit-)parallel.
	// One common virtual layer is used for all of the measurement site's
	// competing hits. The centre point of this virtual plane will be the
	// predicted track position. The predicted position will usually be
	// located between the competing wires.
	//
	// When calculating the residual in the filter step the drift radii of
	// both wires will be projected onto this virtual layer.
	// Placing the virtual plane between the wires keeps the projection error
	// small, allows that a common, simple projection function and projector
	// matrix can be used for both competitors and avoids a bias towards one
	// of the two competing wires.
	//
	//
	//                              Axis v of common virtual plane is parallel to v1 and v2
	//                                   /
	//         |------------*----|------/----------|
	//      PCA for hit 1 -> *   |     /    cell 2 |
	//         |            / *  |    /            |
	//         |           /90 * |   /             |
	//Axis v for hit 1 -> /     *|  /              |
	//         |         /       * /    Wire 2     |
	//    -----|--------X--------|*-------X--------------> Measurement layer
	//         |      Wire 1     / *     /         |
	//         |                /|  *90 / <- Axis v for hit 2
	//         |               / |   * /           |
	//         |cell 1        /  |    * <- PCA for hit 2
	//         |-------------/---|-----*-----------|
	//                      /           * track

        // Left/right ambiguity of wire hits..
	site->setHitVirtPlane(pcaWire, u, v, iHit);
	site->setHitVirtPlane(pcaWire, u, v, iHit+1);
#if dafDebug > 2
    cout<<"Virtual plane for wire hit "<<iWire<<" of site "<<iSite<<endl;
    cout<<"Centre: "<<endl;
    posAt.Print();
    cout<<"Axis U: "<<endl;
    u.Print();
    cout<<"Axis V: "<<endl;
    v.Print();
#endif
    }
    return kTRUE;
}

//  --------------------------------
//  Implementation of public methods
//  --------------------------------

Bool_t HKalDafWire::calcMeasVecFromState(TVectorD &projMeasVec,
					 HKalTrackSite const* const site,
					 Kalman::kalFilterTypes stateType,
					 Kalman::coordSys sys) const {
#if kalDebug > 0
    Int_t mdim = getMeasDim();
    if(projMeasVec.GetNrows() != mdim) {
	Warning("calcMeasVecFromState()",
		Form("Dimension of measurement vector (%i) does not match that of function parameter (%i).",
		     mdim, projMeasVec.GetNrows()));
        projMeasVec.ResizeTo(mdim);
    }
#endif

    const TVectorD &sv = site->getStateVec(stateType, sys);

    if(sys == Kalman::kSecCoord) {
	// State vector is in sector coordinates.
	// The projection of the track state onto a measurement vector is
	// the insection of a straight line defined by the track position
	// and direction with the drift chamber cell.

	TVector3 posAt;
	//if(!HKalTrackState::calcPosAtPlane(posAt, site->getHitVirtPlane(iHit), sv)) {
        //?
	if(!HKalTrackState::calcPosAtPlane(posAt, site->getHitMeasLayer(), sv)) {
	    if(getPrintErr()) {
		Error("calcMeasVecFromState()",
		      "Could not extract position vector from track state.");
	    }
	    return kFALSE;
	}
	TVector3 dir;
        HKalTrackState::calcDir(dir, sv);

        // Determine sign of minimum distance.
        // The minimum distance is negative if the PCA of the track to the
        // sense wire and the origin of the coordinate system are on the same
        // side of the MDC layer.

        // Assume straight line for PCA calculation.
	TVector3 pos1 = posAt - dir;
        TVector3 pos2 = posAt + dir;
        // Wire end points.
        TVector3 wire1, wire2;
        Int_t iHit = getWireNr()*2;
        site->getHitWirePts(wire1, wire2, iHit);

        // PCAs on track and projected onto wire.
        TVector3 pcaTrack, pcaWire;
        // Error flag.
        Int_t Iflag = 0;
        // Distance between pca and wire.
        Double_t dist = 0.;
        // Arc length.
        Double_t length = 0.;
        HKalGeomTools::Track2ToLine(pcaTrack, pcaWire, Iflag, dist, length,
                                    pos1, pos2, wire1, wire2);

        if(site->getHitVirtPlane(iHit).getAxisV().Dot(pcaTrack - pcaWire) < 0.) {
            dist *= -1.;
        }

        projMeasVec(0) = dist;
    } else {
	// State vector is in virtual layer coordinates.
	// The second component corresponds to a drift radius.
	projMeasVec(0) = sv(kIdxY0);
    }
    return kTRUE;
}

Bool_t HKalDafWire::filter(Int_t iSite) {
    // Run the filter step in Kalman filter for a site.
    // Must be called after the prediction step.
    // Different Kalman filter equations are used as set by setFilterMethod().
    // iSite: index of site to filter

    HKalTrackSite *site = getSite(iSite);
    if(!site) {
	return kFALSE;
    }

    const Int_t nWires = site->getNcompetitors()/2;

    vector<TVectorD> states(nWires, TVectorD(getStateDim()));
    vector<TMatrixD> invCovs(nWires, TMatrixD(getStateDim(), getStateDim()));

    for(Int_t iWire = 0; iWire < nWires; iWire++) {
	// Each wire measurement has two hits assigned to it
        // (left and right from the wire).
        setWireNr(iWire);
        Int_t iHit = iWire*2;
	getSite(iSite)->transSecToVirtLay(Kalman::kPredicted, iHit,
					  (getFilterMethod() == Kalman::kKalUD));

	if(!calcEffMeasVec(iSite, iWire)) {
	    if(getPrintErr()) {
		Error("filter()",
		      Form("Could not calculate effective measurement for site %i.",
			   iSite));
	    }
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

	// Filtering wire hits is done in the coordinate system of the virtual
	// layer. Convert the results to sector system.
	site->transVirtLayToSec(Kalman::kFiltered, iHit,
				(getFilterMethod() == Kalman::kKalUD));

	states.at(iWire) = TVectorD(site->getStateVec(Kalman::kFiltered,
						      Kalman::kSecCoord));

	invCovs.at(iWire) = TMatrixD(TMatrixD::kInverted,
				     site->getStateCovMat(Kalman::kFiltered,
							  Kalman::kSecCoord));
    }

    // Calculate average state and covariance matrix.
    // C = (C1^{-1} + C2^{-1})^{-1}
    // sv = C * (C1^{-1} * sv1 + C2^{-1} * sv2)
    TMatrixD cov(getStateDim(), getStateDim());
    cov.Zero();
    for(UInt_t i = 0; i < invCovs.size(); i++) {
	cov += invCovs.at(i);
    }
    cov.Invert();
    site->setStateCovMat(Kalman::kFiltered, cov,
			 Kalman::kSecCoord);

    TVectorD state(getStateDim());
    state.Zero();
    for(UInt_t i = 0; i < states.size(); i++) {
	state += invCovs.at(i)*states.at(i);
    }
    state = cov*state;
    site->setStateVec(Kalman::kFiltered, state,
		      Kalman::kSecCoord);

    return kTRUE;
}

Bool_t HKalDafWire::propagate(Int_t iFromSite, Int_t iToSite) {
    // Propagates the track to the next measurement site and update
    // covariance matrices.
    //
    // iFromSite:  index of measurement site to start propagation from.
    // iToSite:    index of measurement site to propagate to.

    if(!propagateTrack(iFromSite, iToSite)) {
        return kFALSE;
    }

    if(!calcVirtPlane(iToSite)) {
	return kFALSE;
    }

    if(getFilterMethod() == Kalman::kKalUD) {
        propagateCovUD(iFromSite, iToSite);
    } else {
        propagateCovConv(iFromSite, iToSite);
    }

    return kTRUE;
}


void HKalDafWire::updateSites(const TObjArray &hits) {
    // Replace sites of the Kalman system with new hits.
    // hits: array with new measurement hits. Must be of class HKalMdcHit.

    for(Int_t i = 0; i < getNsites(); i++) {
        getSite(i)->clearHits();
        getSite(i)->setActive(kTRUE);
    }

    Int_t iHit  = 0;
    Int_t iSite = 0;

    while(iHit < hits.GetEntries() - 1) {
#if kalDebug > 0
        if(!hits.At(iHit)->InheritsFrom("HKalMdcHit")) {
	    Error("updateSites()",
		  Form("Object at index %i in hits array is of class %s. Expected class is HKalMdcHit.",
		       iHit, hits.At(iHit)->ClassName()));
            exit(1);
        }
#endif

        HKalMdcHit *hit     = (HKalMdcHit*)hits.At(iHit);
        HKalMdcHit *nexthit = (HKalMdcHit*)hits.At(iHit + 1);

        getSite(iSite)->addHit(hit);
	// Add second hit with "negative" drift time. The sign of the
	// drift time represents that.
        HKalMdcHit *hit2    = new HKalMdcHit(*hit);
        hit2->setDriftTime(hit->getDriftTime() * (-1.), hit->getDriftTimeErr());
        TVectorD hitVec2 = hit->getHitVec();
        hitVec2 *= -1.;
        hit2->setHitAndErr(hitVec2, hit->getErrVec());
        getSite(iSite)->addHit(hit2);
        Double_t w = hit->getWeight();
        hit ->setWeight(w / 2.);
        hit2->setWeight(w / 2.);

	if(!hit->areCompetitors(*hit, *nexthit)) {
            iSite++;
        }
        iHit++;
    }

    // Add last hit in array to a site.
    HKalMdcHit *lastHit = (HKalMdcHit*)hits.At(hits.GetEntries()-1);
    getSite(iSite)->addHit(lastHit);

    HKalMdcHit *hit2    = new HKalMdcHit(*lastHit);
    hit2->setDriftTime(lastHit->getDriftTime() * (-1.),
		       lastHit->getDriftTimeErr());
    Double_t w = lastHit->getWeight();
    lastHit->setWeight(w / 2.);
    hit2   ->setWeight(w / 2.);
    getSite(iSite)->addHit(hit2);
    iSite++;

    setNSites(iSite);
    setNHitsInTrack(hits.GetEntries() * 2);
#if kalDebug > 1
    cout<<"New track has "<<getNsites()<<" measurement sites."<<endl;
#endif
}
