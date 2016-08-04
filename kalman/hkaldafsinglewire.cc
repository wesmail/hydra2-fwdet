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

Bool_t HKalDafSingleWire::calcEffMeasVec(Int_t iSite) const {
    // Effective measurement vector needed only in case of
    // competing hits.
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
    // hits:        the array with the measurement hits. Elements must be of class
    //              HKalMdcHit.
    // iniStateVec: initial track state that the filter will use.
    // iniCovMat:   initial covariance matrix that the filter will use.
    // pId:         Geant particle id.
    // momGeant:    Geant momentum that will be written in the output tree. Ignore if this is not simulation data.

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
        Warning("fitWithDaf()", "Cannot use UD-filter together with DAF. Switching to Joseph-stabilization.");
    }

    // Array that stores weights (assignment probabilities) of competing hits.
    // Maximum possible competitors at each layer is 4: two measurements plus
    // left/right ambiguity for each measurement.
    const Int_t maxHitsPerSite = 4;
    //Float_t weights[getNsites()][maxHitsPerSite][nDafs];  //unused

    for(Int_t iDaf = 0; iDaf < nDafs; iDaf++) {

#if dafDebug > 1
        cout<<"********************************************"<<endl;
        cout<<"**** DAF iteration "<< iDaf<<" with temperature "<<dafT[iDaf]<<" ****"<<endl;
        cout<<"********************************************"<<endl;
#endif

        // In each iteration do Kalman prediction/filter steps and then smooth back.
        setTrackChi2(0.);
        setTrackLength(0.);

        HKalTrackSite *fromSite = getSite(iFromSite);
        if(!fromSite) {
            return kFALSE;
        }

	if(!calcEffMeasVec(iFromSite)) {
	    if(getPrintErr()) {
		Error("fitTrack()", "Could not calculate effective measurement for first site");
	    }
            bTrackAccepted = kFALSE;
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
                const TVectorD &measVec = site->getHitVec(iHit);
                TVectorD smooMeasVec(getMeasDim());
                calcMeasVecFromState(smooMeasVec, site, Kalman::kSmoothed, Kalman::kSecCoord, iHit);

#if dafDebug > 2
                cout<<"Updating measurement "<<iHit<<" of "<<site->getNcompetitors()<<endl;
                cout<<"Measurement vector: "<<endl;
                measVec.Print();
                cout<<"Expected measurement from smoothing: "<<endl;
                smooMeasVec.Print();
#endif

                TVectorD  residual     = measVec - smooMeasVec;                      // Residual r
                TMatrixD  residualTrans(1, getMeasDim(), residual.GetMatrixArray()); // r^T
                const TMatrixD &errMat = site->getErrMat(iHit);                      // Measurement error covariance V
#if kalDebug > 0
                if(errMat.NonZeros() == 0) {
                    Error("fitWithDaf()", Form("Measurement error matrix of hit %i is not set.", iHit));
                }
#endif
                TMatrixD  invErrMat    = TMatrixD(TMatrixD::kInverted, errMat);      // G = V^-1

                // Normalization of hit's chi2 distribution: 1 / ((2*pi)^n/2 * sqrt(T * det(V)))
                Double_t  normChi2     = 1. / (TMath::Power(2.*TMath::Pi(), (Double_t)getMeasDim()/2.) *
                                               TMath::Sqrt(dafT[iDaf] * errMat.Determinant()));

                // Standardized distance between smoothed track state and measurement vector:
                //        chi2         =  r^T           * (V_k)^-1  * r
                Double_t  chi2         = (residualTrans * invErrMat * residual)(0);

#if dafDebug > 2
                cout<<"Error matrix: "<<endl;
                errMat.Print();
                cout<<"Hit's chi2: "<<chi2<<endl;
#endif

                site->setHitChi2(chi2, iHit);

                // Normalization for hit's weight recalculation:
                // sum( normchi2 * exp(-chi2/(2T)) + normchi2 * exp(-chi2cut/(2T))
                weightNorm            += normChi2 * TMath::Exp(- chi2       / (2. * dafT[iDaf]));
                weightNorm            += normChi2 * TMath::Exp(- dafChi2Cut / (2. * dafT[iDaf]));
            }

            // Recalculate weight of each hit.
            for(Int_t iHit = 0; iHit < site->getNcompetitors(); iHit++) {
                const TMatrixD &errMat = site->getErrMat(iHit);
                // New weight of hit.
                // 1 / ((2*pi)^n/2 * sqrt(T * det(V))) * exp(-chi2/(2T)) / weightNorm
                Double_t normChi2      = 1. / (TMath::Power(2.*TMath::Pi(), (Double_t)getMeasDim()/2.) * TMath::Sqrt(dafT[iDaf] * errMat.Determinant()));
                Double_t hitWeight     = normChi2 * TMath::Exp(-site->getHitChi2(iHit) / (2. * dafT[iDaf])) / weightNorm;
                if(iHit > maxHitsPerSite - 1) {
                    if(getPrintErr()) {
                        Error("fitTrack()", Form("Site %i has more than %i competing hits.", iSite, maxHitsPerSite));
                    }
                    break;
                }
               // weights[iSite][iHit][iDaf] = hitWeight;    //unused

#if dafDebug > 2
                cout<<"Updating weight of hit "<<iHit<<" out of "<<site->getNcompetitors()
                    <<": old weight "<<site->getHitWeight(iHit)<<", new weight "<<hitWeight
                    <<endl;
#endif

                site->setHitWeight(hitWeight, iHit);
                site->setHitWeightHist(hitWeight, iDaf, iHit);
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
