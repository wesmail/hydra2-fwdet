#include "hmdcsizescells.h"

#include "hkaldafsinglewire.h"


//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// DAF for wire hits. Does not allow competition between hits.
// See doc for HKalIFilter.
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

ClassImp (HKalDafSingleWire)

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------

Bool_t HKalDafSingleWire::calcEffErrMat(Int_t iSite, Int_t iWire) const {
    // Calculates an effective measurement error matrix from the measurement
    // errors of all hits in this site.
    // Used for the annealing filter.
    // Effective measurement error covariance:
    // V = [ sum(p_i * G_i) ]^-1
    // p_i: weight of hit i
    // G_i: inverse measurement error matrix of hit i

    HKalTrackSite *site = getSite(iSite);
    if(!site) {
	return kFALSE;
    }

    TMatrixD effErrMat(getMeasDim(), getMeasDim());
    Double_t lmt = 100. * numeric_limits<Double_t>::epsilon();
    // Calculate the weighted mean of the inverse measurement error
    // covariances.
    for(Int_t i = iWire*2; i <= iWire*2+1; i++) {
        Double_t w = site->getHitWeight(i);
        if(w < lmt) w = 1.e-10; // avoid numerical problems due to inversion.
        effErrMat += w * TMatrixD(TMatrixD::kInverted, site->getErrMat(i));
    }
    Double_t det = 0.;
    effErrMat.Invert(&det);
    site->setEffErrMat(effErrMat);

    return kTRUE;
}

Bool_t HKalDafSingleWire::calcEffMeasVec(Int_t iSite, Int_t iWire) const {
    // Calculates an effective measurement vector from the measurement
    // of all hits in this site and the effective measurement error matrix.
    // Used for the annealing filter.
    //
    // Effective measurement vector:
    // m_eff = V * sum(p_i * G_i * m_i)
    // V:   effective measurement covariance matrix
    // p_i: weight of hit i
    // G_i: inverse measurement error matrix of hit i
    // m_i: measurement vector of hit i
    //
    // Effective measurement error covariance:
    // V = [ sum(p_i * G_i) ]^-1

    HKalTrackSite *site = getSite(iSite);
    if(!site) {
	return kFALSE;
    }

    if(!calcEffErrMat(iSite, 0)) {
        return kFALSE;
    }

    TVectorD effMeasVec(getMeasDim());
    const TMatrixD &effErrMat = site->getEffErrMat();

#if dafDebug > 2
    cout<<"Calculating effective measurement"<<endl;
#endif

    for(Int_t i = iWire*2; i <= iWire*2+1; i++) {
	effMeasVec += site->getHitWeight(i) *
	    TMatrixD(TMatrixD::kInverted, site->getErrMat(i)) *
	    site->getHitVec(i);

#if dafDebug > 2
	cout<<"Weight of hit "<<i<<": "<<site->getHitWeight(i)
	    <<", drift radius "<<(site->getHitVec(i))(0)<<endl;
#endif
    }
    effMeasVec = effErrMat * effMeasVec;

#if dafDebug > 2
    cout<<"Effective measurement vector"<<endl;
    effMeasVec.Print();
    cout<<"Effective measurement error matrix"<<endl;
    effErrMat.Print();
#endif

    site->setEffMeasVec(effMeasVec);

    return kTRUE;
}


//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalDafSingleWire::HKalDafSingleWire(Int_t n, Int_t measDim, Int_t stateDim,
			 HMdcTrackGField *fMap, Double_t fpol)
    : HKalFiltWire(n, measDim, stateDim, fMap, fpol) {

    const Int_t nDafs = 5;
    Double_t T[nDafs] = { 81., 9., 4., 1., 1. };
    setDafPars(9., &T[0], nDafs);
    setWireNr(0); // This class only handles one wire measurement.
}

//  --------------------------------
//  Implementation of public methods
//  --------------------------------

Bool_t HKalDafSingleWire::fitTrack(const TObjArray &hits, const TVectorD &iniSv,
				   const TMatrixD &iniCovMat, Int_t pId) {
    // Runs the Kalman filter over a set of measurement hits.
    // Returns false if the Kalman filter encountered problems during
    // track fitting or the chi^2 is too high.
    //
    // Input:
    // hits:        the array with the measurement hits. Elements must be of
    //              class HKalMdcHit.
    // iniStateVec: initial track state that the filter will use.
    // iniCovMat:   initial covariance matrix that the filter will use.
    // pId:         Geant particle id.

#if dafDebug > 1
    cout<<"******************************"<<endl;
    cout<<"**** Fit track number "<<getTrackNum()<<" ****"<<endl;
    cout<<"******************************"<<endl;
#endif

    newTrack(hits, iniSv, iniCovMat, pId);

    Int_t iFromSite, iToSite;
    if(getDirection() == kIterForward) {
        iFromSite = 0;
        iToSite   = getNsites() - 1;
    } else {
        iFromSite = getNsites() - 1;
        iToSite   = 0;
    }

    Bool_t bTrackAccepted = kTRUE;

    const Int_t nDafs = dafT.GetSize(); // Number of DAF iterations.

    //? Check why UD-filter is not working with DAF.
    if(getFilterMethod() == Kalman::kKalUD) {
        setFilterMethod(Kalman::kKalJoseph);
	Warning("fitWithDaf()",
		"Cannot use UD-filter together with DAF. Switching to Joseph-stabilization.");
    }

    // Array that stores weights (assignment probabilities) of competing hits.
    // Maximum possible competitors at each layer is 4: two measurements plus
    // left/right ambiguity for each measurement.
    const Int_t maxHitsPerSite = 4;

    for(Int_t iSite = 0; iSite < getNsites(); iSite++) {
	if(!calcEffMeasVec(iSite, 0)) {
	    if(getPrintErr()) {
		Error("fitTrack()",
		      Form("Could not calculate effective measurement for site %i",
			   iSite));
	    }
            bTrackAccepted = kFALSE;
	    return kFALSE;
	}
    }


    for(Int_t iDaf = 0; iDaf < nDafs; iDaf++) {

#if dafDebug > 1
        cout<<"********************************************"<<endl;
	cout<<"**** DAF iteration "<< iDaf<<" with temperature "
	    <<dafT[iDaf]<<" ****"<<endl;
        cout<<"********************************************"<<endl;
#endif

	// In each iteration do Kalman prediction/filter steps and then
	// smooth back.
        setTrackChi2(0.);
        setTrackLength(0.);

        HKalTrackSite *fromSite = getSite(iFromSite);
        if(!fromSite) {
            return kFALSE;
        }

	bTrackAccepted &= runFilter(iFromSite, iToSite);

        smooth();

        for(Int_t iSite = 0; iSite < getNsites(); iSite++) {
            HKalTrackSite *site = getSite(iSite);
            Double_t weightNorm = 0.;

#if dafDebug > 1
            cout<<"Running DAF for site "<<iSite<<endl;
#endif

	    for(Int_t iHit = 0; iHit < site->getNcompetitors(); iHit++) {
                setWireNr(iHit/2);
                const TVectorD &measVec = site->getHitVec(iHit);
                TVectorD smooMeasVec(getMeasDim());
		calcMeasVecFromState(smooMeasVec, site, Kalman::kSmoothed,
				     Kalman::kSecCoord);

#if dafDebug > 2
		cout<<"Updating measurement "<<iHit<<" of "
		    <<site->getNcompetitors()<<endl;
                cout<<"Measurement vector: "<<endl;
                measVec.Print();
                cout<<"Expected measurement from smoothing: "<<endl;
                smooMeasVec.Print();
#endif

                // Residual r
		TVectorD  residual     = measVec - smooMeasVec;
                // r^T
		TMatrixD  residualTrans(1, getMeasDim(),
					residual.GetMatrixArray());
                const TMatrixD &errMat = site->getErrMat(iHit);                      // Measurement error covariance V
#if kalDebug > 0
                if(errMat.NonZeros() == 0) {
		    Error("fitWithDaf()",
			  Form("Measurement error matrix of hit %i is not set.",
			       iHit));
                }
#endif
                // G = V^-1
		TMatrixD  invErrMat    = TMatrixD(TMatrixD::kInverted, errMat);

		// Normalization of hit's chi2 distribution:
		// 1 / ((2*pi)^n/2 * sqrt(T * det(V)))
		Double_t  normChi2     =
		    1./(TMath::Power(2.*TMath::Pi(),(Double_t)getMeasDim()/2.)*
			TMath::Sqrt(dafT[iDaf] * errMat.Determinant()));

		// Standardized distance between smoothed track state and
		// measurement vector:
                //        chi2         =  r^T          *(V_k)^-1 *r
                Double_t  chi2         = (residualTrans*invErrMat*residual)(0);

#if dafDebug > 2
                cout<<"Error matrix: "<<endl;
                errMat.Print();
                cout<<"Hit's chi2: "<<chi2<<endl;
#endif

                site->setHitChi2(chi2, iHit);

                // Normalization for hit's weight recalculation:
		// sum( normchi2 * exp(-chi2/(2T)) +
		// normchi2 * exp(-chi2cut/(2T))
		weightNorm            +=
		    normChi2 * TMath::Exp(- chi2       / (2. * dafT[iDaf]));
		weightNorm            +=
		    normChi2 * TMath::Exp(- dafChi2Cut / (2. * dafT[iDaf]));
            }

            // Recalculate weight of each hit.
            for(Int_t iHit = 0; iHit < site->getNcompetitors(); iHit++) {
                const TMatrixD &errMat = site->getErrMat(iHit);
                // New weight of hit.
                // 1 / ((2*pi)^n/2 * sqrt(T * det(V))) * exp(-chi2/(2T)) / weightNorm
		Double_t normChi2      =
		    1./(TMath::Power(2.*TMath::Pi(), (Double_t)getMeasDim()/2.)*
			TMath::Sqrt(dafT[iDaf] * errMat.Determinant()));
		Double_t hitWeight     =
		    normChi2 * TMath::Exp(-site->getHitChi2(iHit) /
					  (2. * dafT[iDaf])) / weightNorm;
                if(iHit > maxHitsPerSite - 1) {
                    if(getPrintErr()) {
			Error("fitTrack()",
			      Form("Site %i has more than %i competing hits.",
				   iSite, maxHitsPerSite));
                    }
                    break;
                }

#if dafDebug > 2
		cout<<"Updating weight of hit "<<iHit<<" out of "
		    <<site->getNcompetitors()<<" from site "<<iSite<<
		    "old weight "<<site->getHitWeight(iHit)<<", new weight "
		    <<hitWeight<<endl;
#endif

		site->setHitWeight(hitWeight, iHit);
                site->setHitWeightHist(hitWeight, iDaf, iHit);
	    }

	    if(!calcEffMeasVec(iSite, 0)) {
		if(getPrintErr()) {
		    Error("fitTrack()",
			  Form("Could not calculate effective measurement for site %i", iSite));
		}
		bTrackAccepted = kFALSE;
		return kFALSE;
	    }
	}
    }

#if dafDebug > 1
        cout<<"**** Finished with Deterministic Annealing Filter ****"<<endl;
#endif


    // Transform hit and state vectors from track back to sector coordinates.
    if(getRotation() == Kalman::kVarRot) {
        transformFromTrackToSector();
    }

    setTrackLengthAtLastMdc(getTrackLength());

#if dafDebug > 1
    if(bTrackAccepted) {
        cout<<"**** Fitted track ****"<<endl;
    } else {
        cout<<"**** Errors during track fitting ****"<<endl;
    }
#endif

    if(!bTrackAccepted) setChi2(-1.F);

    return bTrackAccepted;
}

void HKalDafSingleWire::setDafPars(Double_t chi2cut, const Double_t *T, Int_t n) {
    // Set parameters for the deterministic annealing filter.
    //
    // input:
    // chi2cut: Cut-off parameter.
    // T:       Array with annealing factors (temperatures).
    // n:       Number of annealing factors.

    dafChi2Cut = chi2cut;
    dafT.Set(n, &T[0]);

    for(Int_t i = 0; i < getNsites(); i++) {
        getSite(i)->setNdafs(n);
    }
}

TMatrixD const& HKalDafSingleWire::getHitErrMat(HKalTrackSite* const site) const {
    // Returns the effective measurement error of site.
    //
    // Input:
    // site: measurement site.

    return site->getEffErrMat();
}

TVectorD const& HKalDafSingleWire::getHitVec(HKalTrackSite* const site) const {
    // Returns the effective measurement vector of measurement site.
    //
    // Input:
    // site: measurement site.

    return site->getEffMeasVec();
}

Double_t HKalDafSingleWire::getNdf() const {
    // Returns number degrees of freedom, i.e. number of experimental points
    // minus number of parameters that are estimated.

    Double_t w = 0.;
    for(Int_t iSite = 0; iSite < getNsites(); iSite++) {
	HKalTrackSite *site = getSite(iSite);
	if(!site) {
	    Error("getNdf()", Form("Site %i not found.", iSite));
	    continue;
	}
	for(Int_t iHit = 0; iHit < site->getNcompetitors(); iHit++) {
	    w += site->getHitWeight(iHit);
	}
    }
    return (w * getMeasDim() - getStateDim());
}

void HKalDafSingleWire::updateSites(const TObjArray &hits) {
    // Replace sites of the Kalman system with new hits.
    // hits: array with new measurement hits. Must be of class HKalMdcHit.

    for(Int_t i = 0; i < getNmaxSites(); i++) {
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

	// Skip hits from the same layer.
	if(iHit > 0) {
	    HKalMdcHit *prevhit = (HKalMdcHit*)hits.At(iHit - 1);
	    if(hit->areCompetitors(*hit, *prevhit)) {
		iHit++;
                continue;
	    }
	}

	// Add second hit with "negative" drift time. The sign of the
	// drift time represents that.
        HKalMdcHit *hit2    = new HKalMdcHit(*hit);
	hit2->setDriftTime(hit->getDriftTime() * (-1.),
			   hit->getDriftTimeErr());
        TVectorD hitVec2 = hit->getHitVec();
        hitVec2 *= -1.;
        hit2->setHitAndErr(hitVec2, hit->getErrVec());
        Double_t w = hit->getWeight();
        hit ->setWeight(w / 2.);
        hit2->setWeight(w / 2.);

	if(iSite >= 0 && iSite < getNmaxSites()) {
	    HKalTrackSite *site = getSite(iSite);
	    site->addHit(hit);
	    site->addHit(hit2);
	} else {
	    Error("updateSites()",
		  Form("Attempt to access site %i out of %i",
		       iSite, getNmaxSites()-1));
            return;
	}

        iSite++;
        iHit++;
    }

    // Add last hit in array to a site.
    HKalMdcHit *lastHit = (HKalMdcHit*)hits.At(hits.GetEntries()-1);

    HKalMdcHit *hit2    = new HKalMdcHit(*lastHit);
    hit2->setDriftTime(lastHit->getDriftTime() * (-1.),
		       lastHit->getDriftTimeErr());
    TVectorD hitVec2 = lastHit->getHitVec();
    hitVec2 *= -1.;
    hit2->setHitAndErr(hitVec2, lastHit->getErrVec());
    Double_t w = lastHit->getWeight();
    lastHit->setWeight(w / 2.);
    hit2   ->setWeight(w / 2.);

    // Don't store competing hits.
    if(hits.GetEntries() > 1) {
        HKalMdcHit *secondLastHit = (HKalMdcHit*)hits.At(hits.GetEntries()-2);
	if(!lastHit->areCompetitors(*lastHit, *secondLastHit)) {
	    if(iSite >= 0 && iSite < getNmaxSites()) {
		HKalTrackSite *site = getSite(iSite);
		site->addHit(lastHit);
		site->addHit(hit2);
		iSite++;
	    } else {
		Error("updateSites()",
		      Form("Attempt to access site %i out of %i",
			   iSite, getNmaxSites()-1));
	    }
        }
    } else {
	if(iSite >= 0 && iSite < getNmaxSites()) {
	    HKalTrackSite *site = getSite(iSite);
	    site->addHit(lastHit);
	    site->addHit(hit2);
	    iSite++;
	} else {
	    Error("updateSites()",
		  Form("Attempt to access site %i out of %i",
		       iSite, getNsites()-1));
	}
    }

    setNSites(iSite);
    setNHitsInTrack(iSite * 2);
#if kalDebug > 1
    cout<<"New track has "<<getNsites()<<" measurement sites."<<endl;
#endif
}
