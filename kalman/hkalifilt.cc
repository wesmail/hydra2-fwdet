
// from ROOT
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"

// from hydra
#include "hades.h"
#include "hcategory.h"
#include "hcategorymanager.h"
#include "hmdccal2par.h"
#include "hmdccal2parsim.h"
#include "hmdctrackgdef.h"
#include "hmdctrackgfield.h"
#include "hphysicsconstants.h"
#include "hruntimedb.h"
#include "hmdcsizescells.h"

#include "hkalgeomtools.h"
#include "hkalmatrixtools.h"
#include "hkalplane.h"
#include "hkalifilt.h"

// C/C++ libraries
#include <iostream>
using namespace std;

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// This class is responsible for the Kalman filter.
//
// The Kalman filter is started via the 
// Bool_t fitTrack(const TObjArray &hits,  const TVectorD &iniStateVec,
//                 const TMatrixD &iniCovMat)
// function. It needs a series of measurement points (an array of class
// HKalMdcHit) and initial estimations for the track state and covariance
// matrix as input.
//
// It will then make an estimation for the initial track state based on the
// measurement hits and fill the covariance matrix with large values.
// The function will return a boolean to indicate whether problems (high chi^2
// or unrealistic track parameters) were encountered during track fitting.
//
// Track propagation will be done with the Runge-Kutta method.
//
// After the Kalman filter is done, a measurement site may be eliminated
// from the calculation with the function
// excludeSite(Int_t iSite)
//
//
// Several options for the Kalman filter may be set:
// setNumIterations(Int_t kalRuns)
// Number of iterations. After each iteration the Kalman filter will use the
// result of the previous iteration as starting value.
// Default is 1.
//
// setDirection(Bool_t dir)
// Propagation direction (kIterForward, kIterBackward). Direction will switch
// after each iteration.
// Default is forward direction.
//
// setSmoothing(Bool_t smooth)
// Smooth or don't smooth track states after filter. Smoothing attempts to
// improve the filtered track states by using the results of all subsequent
// as well.
// Default is on.
//
// setRotation(Kalman::kalRotateOptions rotate)
// kNoRot: No coordinate transformation. Not recommended for high track angles
//         (> 30°).
// kVarRot (default): Tilt the coordinate system along the initial track
//                    direction to avoid large values for the track state
//                    parameters tx or ty.
//
// setDoEnerLoss(Bool_t dedx)
// setDoMultScat(Bool_t ms)
// Include energy or multiple scattering in the Kalman filter.
// Default is on.
//
// setConstField(Bool_t constField)
// setFieldVector(const TVector3 &B)
// Tells the Kalman filter to use a constant magnetic field instead of the
// field map.
// Field vector must be in Tesla.
// Default is off.
//
// setFilterMethod(Kalman::filtMethod type)
// Changes which formulation of the Kalman equations to use. The formulations
// differ in speed and numerical stability. See the description of the
// functions named filter... for details.
//
// Notation:
// a: track state vector
// m: measurement vector
// r: residual of the measurement vector and the predicted measurement
// f(a): propagation function: Propagate track state a to the next
//       detector.
// h(a): projection function that calculates a measurement vector from
//       track state v
// F: Jacobian of f(a)
// H: Jacobian of h(a)
// C: covariance matrix of the track state
// Q: covariance matrix of the process noise
// V: covariance matrix of the measurement errors
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

ClassImp (HKalIFilt)

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalIFilt::HKalIFilt(Int_t n, Int_t measDim, Int_t stateDim,
                       HMdcTrackGField *fMap, Double_t fpol)
    : TObject(), iniStateVec(stateDim), svVertex(stateDim), svMeta(stateDim) {
    // Constructor for a Kalman system.
    //
    // Input:
    // n:        the maximum number of measurement sites in a track.
    // measDim:  the dimension of a measurement vector (must be the same for
    //           all tracks)
    // stateDim: the dimension of the track state vector (must be the same for all
    //           tracks)
    // fMap:     pointer to field map.
    // fpol:     field scaling factor * polarity.
    // chiMax:   the maximum chi^2 where a track is still accepted.

    betaInput    = -1.;
    nMaxSites    = n;
    nHitsInTrack = n;
    nSites       = n;

    sites = new HKalTrackSite* [nSites];
    for(Int_t i = 0; i < nSites; i++) {
        sites[i] = new HKalTrackSite(measDim, stateDim);
    }

    setFieldMap(fMap, fpol);
    trackNum         = 0;
    pid              = -1;

    setPrintWarnings(kFALSE);
    setPrintErrors(kFALSE);
    bFillPointArrays = kFALSE;

    nCondErrs        = 0;
    nCovSymErrs      = 0;
    nCovPosDefErrs   = 0;

    setNumIterations(1);
    setDirection(kIterForward);
    setSmoothing(kTRUE);
    setFilterMethod(Kalman::kKalConv);
    setDoEnerLoss(kTRUE);
    setDoMultScat(kTRUE);

    setRotation(Kalman::kVarRot);
    pRotMat = new TRotation();
    trackPropagator.setRotationMatrix(pRotMat);

    pointsTrack.SetOwner();
    fieldTrack.SetOwner();
}

HKalIFilt::~HKalIFilt() {

    //?
#if kalDebug > 0
    cout<<"Covariance symmetry errors: "<<nCovSymErrs
        <<", pos. def. errors: "<<nCovPosDefErrs
        <<", covariance ill-conditioned: "<<nCondErrs<<endl;
#endif
    delete sites;
    delete pRotMat;
}

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------

Bool_t HKalIFilt::checkSitePreFilter(Int_t iSite) const {
    // Checks for potential issues when site with index iSite is filtered.

    HKalTrackSite *site = getSite(iSite);

#if kalDebug > 1
    cout<<"#### Filtering site "<<iSite<<" ####"<<endl;
#endif

    const TMatrixD &predCov = site->getStateCovMat(Kalman::kPredicted);

#if kalDebug > 0
    Int_t nRows, nCols;

    Int_t mdim = getMeasDim();
    Int_t sdim = getStateDim();

    // Check dimensions.
    const TVectorD &predState = site->getStateVec(Kalman::kPredicted);
    nRows = predState.GetNrows();
    if(nRows != sdim) {
	Error(Form("Filter site %i.", iSite),
	      Form("State vector from prediction must have dimension %i, but has dimension %i",
		   nRows, sdim));
        exit(1);
    }

    const TVectorD &measVec = getHitVec(site);
    nRows = measVec.GetNrows();
    if(nRows != mdim) {
	Error(Form("Filter site %i.", iSite),
	      Form("Measurement vector must have dimension %i, but has dimension %i",
		   nRows, mdim));
        exit(1);
    }

    const TMatrixD &errMat = getHitErrMat(site);
    nRows = errMat.GetNrows();
    nCols = errMat.GetNcols();
    if(nRows != mdim || nCols != mdim) {
	Error(Form("Filter site %i.", iSite),
	      Form("Measurement error matrix is %ix%i, but must be %ix%i matrix.",
		   nRows, nCols, mdim, mdim ));
        exit(1);
    }

    // Check covariance matrix.
    if(!checkCovMat(predCov)) {
	Warning("filter()",
		Form("Errors found in predicted covariance matrix for site %i.",
		     iSite));
    }

#endif

    if(!HKalMatrixTools::checkValidElems(predCov)) {
        if(bPrintErr) {
	    Error("filter()",
		  Form("Predicted covariance matrix for site %i contains INFs or NaNs.",
		       iSite));
        }
        return kFALSE;
    }

    if(!site->getIsActive()) {
        if(bPrintErr) {
            Error("filter()", Form("Site number %i is inactive.", iSite));
        }
        return kFALSE;
    }
    return kTRUE;
}

void HKalIFilt::filterConventional(Int_t iSite) {
    // Filter a site using the conventional formulation of the Kalman filter
    // equations.
    //
    // C_k = (I - K_k * H_k) * C^{k-1}_k
    // K_k = C^{k-1}_k * H^T_k * (V_k + H_k * C^{k-1}_k * H^T_k)^-1
    //
    // These are simpler equations compared to the Joseph or Swerling
    // forumations. Only one inversion of a matrix with the dimension of the
    // measurement vector is neccessary. However, the updated covariance
    // matrix may not be positive definite due to numerical computing problems.

    // sector or virtual layer coordinates
    Kalman::coordSys sys = getFilterInCoordSys();
    HKalTrackSite *site = getSite(iSite);

    // Predicted state vector for this site.
    const TVectorD &predState = site->getStateVec(Kalman::kPredicted, sys);    // a^k-1_k
    TVectorD projMeasVec(getMeasDim());
    // Expected measurement vector.
    // h_k(a^k-1_k)
    calcMeasVecFromState(projMeasVec, site, Kalman::kPredicted, sys);

    // Residual of measurement vector and expected measurement vector.
    TVectorD residual = getHitVec(site) - projMeasVec;

    const TMatrixD &predCov    = site->getStateCovMat(Kalman::kPredicted, sys); // C^{k-1}_k
    const TMatrixD &errMat     = getHitErrMat(site);                               // V_k
    const TMatrixD &fProj      = site->getStateProjMat(Kalman::kFiltered, sys); // H_k
    const TMatrixD  fProjTrans = TMatrixD(TMatrixD::kTransposed, fProj);

    // Component of the Kalman gain that needs to be inverted.
    TMatrixD R = errMat + fProj * predCov * fProjTrans;

#if kalDebug > 0
    if(!HKalMatrixTools::checkCond(R)) {
        nCondErrs++;
        if(bPrintWarn) {
	    Warning("filterConventional()",
		    "Matrix is ill-conditioned for inversion.");
        }
    }
#endif

    // Calc Kalman gain matrix.
    //       K_k        = C^{k-1}_k * H^T_k      * (V_k + H_k * C^{k-1}_k * H^T_k)^-1
    TMatrixD kalmanGain = predCov   * fProjTrans * R.Invert();

    //                 (I - K_k * H_k)
    TMatrixD covUpdate(TMatrixD(TMatrixD::kUnit,
				TMatrixD(getStateDim(sys),getStateDim(sys))) -
		       kalmanGain * fProj);
    //       C_k     = (I - K_k * H_k) * C^{k-1}_k
    TMatrixD filtCov = covUpdate       * predCov;

#if kalDebug > 0
    Int_t asymCells = HKalMatrixTools::checkSymmetry(filtCov, 1.e-9);
    if(asymCells > 0) {
        nCovSymErrs += asymCells;
        if(bPrintWarn) {
	    Warning("filterConventional()",
		    "Covariance matrix for filter step is not symmetric.");
        }
    }
    if(!HKalMatrixTools::isPosDef(filtCov)) {
        nCovPosDefErrs++;
        if(bPrintWarn) {
	    Warning("filterConventional()",
		    "Covariance for the filtered state is not positive definite.");
        }
    }
#endif

    if(!filtCov.IsSymmetric()) {
        if(!HKalMatrixTools::makeSymmetric(filtCov)) {
            if(bPrintWarn) {
		Warning("filterConventional()",
			"Covariance matrix for filter step is not close to being symmetric.");
            }
        }
    }

    site->setStateCovMat(Kalman::kFiltered, filtCov, sys);

    // Calculate filtered state vector.
    //       a_k       = a^k-1_k   + K_k        * (m_k     - h_k(a^k-1_k) )
    TVectorD filtState = predState + kalmanGain * residual;
    site->setStateVec(Kalman::kFiltered, filtState, sys);

#if kalDebug > 2
    if(sys == Kalman::kSecCoord) {
        cout<<"Filtering in sector coordinates."<<endl;
    }
    if(sys == Kalman::kLayCoord) {
        cout<<"Filtering in virtual layer coordinates."<<endl;
    }
    cout<<"Filter with conventional Kalman equations."<<endl;
    cout<<"Expected state vector:"<<endl;
    predState.Print();
    cout<<"Expected measurement:"<<endl;
    projMeasVec.Print();
    cout<<"Measurement vector:"<<endl;
    getHitVec(site).Print();
    cout<<"Filtered state vector:"<<endl;
    filtState.Print();
    cout<<"Filtered covariance:"<<endl;
    filtCov.Print();
#endif
#if kalDebug > 3
    cout<<"Covariance from prediction:"<<endl;
    predCov.Print();
    cout<<"Measurement error matrix:"<<endl;
    errMat.Print();
    cout<<"Projector matrix:"<<endl;
    fProj.Print();
    cout<<"Kalman gain:"<<endl;
    kalmanGain.Print();
#endif
}

void HKalIFilt::filterJoseph(Int_t iSite) {
    // This filter method uses the Joseph stabilized update of the covariance
    // matrix.
    //
    // K_k = C^k-1_k * H^T * (V_k + H_k * C^k-1_k * H_k^T)^-1
    // C^k = (I - K_k * H) * C^k-1_k * (I - K_k * H)^T + K_k * V_k * K_k^T
    //
    // This method is numerically more stable than the conventional or
    // Swerling formulations of the Kalman equations.
    // It guarantees that the updated covariance matrix is positive definite
    // if the covariance from the prediction step and the measurement noise
    // covariance are also positive definite.

    Kalman::coordSys sys = getFilterInCoordSys();
    HKalTrackSite *site  = getSite(iSite);

    // Predicted state vector for this site.
    // a^k-1_k
    const TVectorD &predState  = site->getStateVec(kPredicted, sys);
    TVectorD projMeasVec(getMeasDim());
    // Expected measurement vector.
    // h_k(a^k-1_k)
    calcMeasVecFromState(projMeasVec, site, Kalman::kPredicted, sys);

    // Residual of measurement vector and expected measurement vector.
    TVectorD residual = getHitVec(site) - projMeasVec;

    const TMatrixD &errMat     = getHitErrMat(site);                               // V_k
    const TMatrixD &predCov    = site->getStateCovMat(Kalman::kPredicted, sys); // C^k-1_k
    const TMatrixD &fProj      = site->getStateProjMat(Kalman::kFiltered, sys); // H_k
    const TMatrixD  fProjTrans = TMatrixD(TMatrixD::kTransposed, fProj);

    TMatrixD R(errMat + fProj * predCov * fProjTrans);

#if kalDebug > 0
    if(!HKalMatrixTools::checkCond(R)) {
        nCondErrs++;
        if(bPrintWarn) {
	    Warning("filterJospeh()",
		    "Matrix is ill-conditioned for inversion.");
        }
    }
#endif

    //       K_k        = C^k-1_k * H^T        * (V_k + H_k * C^k-1_k * H_k^T)^-1
    TMatrixD kalmanGain = predCov * fProjTrans * R.Invert();

    //                (I - K_k * H_k)
    TMatrixD covUpdate(TMatrixD(TMatrixD::kUnit,
				TMatrixD(getStateDim(sys),getStateDim(sys))) -
		       kalmanGain * fProj);
    TMatrixD covUpdateT(TMatrixD::kTransposed, covUpdate);
    //       C^k     = (I - K_k * H) * C^k-1_k * (I - K_k * H)^T + 
    TMatrixD filtCov = covUpdate     * predCov * covUpdateT      +
        // K_k     * V_k    * K_k^T
	kalmanGain * errMat * TMatrixD(TMatrixD::kTransposed, kalmanGain);

#if kalDebug > 0
    Int_t asymCells = HKalMatrixTools::checkSymmetry(filtCov, 1.e-9);
    if(asymCells > 0) {
        nCovSymErrs += asymCells;
        if(bPrintWarn) {
	    Warning("filterJoseph()",
		    "Covariance matrix for filter step is not symmetric.");
        }
    }
    if(!HKalMatrixTools::isPosDef(filtCov)) {
        nCovPosDefErrs++;
        if(bPrintWarn) {
	    Warning("filterJoseph()",
		    "Covariance for the filtered state is not positive definite.");
        }
    }
#endif

    if(!filtCov.IsSymmetric()) {
        if(!HKalMatrixTools::makeSymmetric(filtCov)) {
            if(bPrintWarn) {
		Warning("filterJoseph()",
			"Covariance matrix for filter step is not close to being symmetric.");
            }
        }
    }

    site->setStateCovMat(kFiltered, filtCov, sys);

    // Calculate filtered state vector.
    //       a_k       = a^k-1_k   + K_k        * (m_k - h_k(a^k-1_k) )
    TVectorD filtState = predState + kalmanGain * residual;
    site->setStateVec(kFiltered, filtState, sys);

#if kalDebug > 2
    if(sys == Kalman::kSecCoord) {
        cout<<"Filtering in sector coordinates."<<endl;
    }
    if(sys == Kalman::kLayCoord) {
        cout<<"Filtering in virtual layer coordinates."<<endl;
    }
    cout<<"Filter with Joseph-stabilized Kalman equations."<<endl;
    cout<<"Expected state vector:"<<endl;
    predState.Print();
    cout<<"Expected measurement:"<<endl;
    projMeasVec.Print();
    cout<<"Measurement vector:"<<endl;
    getHitVec(site).Print();
    cout<<"Filtered state vector:"<<endl;
    filtState.Print();
    cout<<"Filtered covariance:"<<endl;
    filtCov.Print();
#endif
#if kalDebug > 3
    cout<<"Covariance from prediction:"<<endl;
    predCov.Print();
    cout<<"Measurement error matrix:"<<endl;
    errMat.Print();
    cout<<"Projector matrix:"<<endl;
    fProj.Print();
    cout<<"Kalman gain:"<<endl;
    kalmanGain.Print();
#endif
}

void HKalIFilt::filterSequential(Int_t iSite) {
    // Implements the sequential Kalman filter..
    // May only be used if:
    // the measurement noise matrix is diagonal or it is constant
    // and the projection function is h(a) = H * a.
    //
    // The advantage of this method is that is avoids matrix inversions.

    Kalman::coordSys sys = getFilterInCoordSys();
    HKalTrackSite *site = getSite(iSite);

    Int_t sdim = getStateDim();
    Int_t mdim = getMeasDim();

    const TMatrixD &errMat    = getHitErrMat(site);
    const TVectorD &measVec   = getHitVec(site);

    TMatrixD filtCov   = site->getStateCovMat(Kalman::kPredicted, sys);
    TVectorD filtState = site->getStateVec(Kalman::kPredicted, sys);

    const TMatrixD &fProj   = site->getStateProjMat(Kalman::kFiltered, sys);

#if kalDebug > 2
    if(sys == Kalman::kSecCoord) {
        cout<<"Filtering in sector coordinates."<<endl;
    }
    if(sys == Kalman::kLayCoord) {
        cout<<"Filtering in virtual layer coordinates."<<endl;
    }
    cout<<"Filter with sequential Kalman filter."<<endl;
    cout<<"Expected state vector:"<<endl;
    filtState.Print();
    cout<<"Expected measurement:"<<endl;
    (fProj * filtState).Print();
    cout<<"Measurement vector:"<<endl;
    getHitVec(site).Print();
#endif
#if kalDebug > 3
    cout<<"Measurement error matrix:"<<endl;
    errMat.Print();
    cout<<"Projector matrix:"<<endl;
    fProj.Print();
#endif

    TMatrixD kalmanGain(sdim, 1);
    TMatrixD I(sdim, sdim);
    I.UnitMatrix();

    Double_t hrow[sdim];
    TMatrixD H(1, sdim);
    TMatrixD Ht(sdim, 1);

    // The main idea of the sequential filter is to process the components of
    // the measurement vector one at a time.
    //
    // Literature:
    // Optimal State Estimation
    // by Dan Simon
    // Chapter 6.1.: Sequential Kalman filtering

    for(Int_t iMeas = 0; iMeas < mdim; iMeas++) {
        fProj.ExtractRow(iMeas, 0, hrow);
        H .Use(0, 0, 0, H.GetNcols()-1, hrow);
        Ht.Use(0, Ht.GetNrows()-1, 0, 0, hrow);

        Double_t alpha = (H * filtCov * Ht)(0,0) + errMat(iMeas, iMeas);
        kalmanGain = (1./alpha) * filtCov * Ht;
        Double_t residual = (measVec(iMeas) - (H * filtState)(0));
        for(Int_t i = 0; i < filtState.GetNrows(); i++) {
            filtState(i) += kalmanGain(i, 0) * residual;
        }
        filtCov = (I - kalmanGain * H) * filtCov;
    }

    if(!filtCov.IsSymmetric()) {
        if(!HKalMatrixTools::makeSymmetric(filtCov)) {
            if(bPrintWarn) {
		Warning("filterJoseph()",
			"Covariance matrix for filter step is not close to being symmetric.");
            }
        }
    }

    // Store results.
    site->setStateCovMat(Kalman::kFiltered, filtCov, sys);
    site->setStateVec(Kalman::kFiltered, filtState, sys);

#if kalDebug > 2
    cout<<"Filtered state vector:"<<endl;
    filtState.Print();
    cout<<"Filtered covariance:"<<endl;
    filtCov.Print();
#endif
}

void HKalIFilt::filterSwerling(Int_t iSite) {
    // Swerling calculation of covariance matrix for filtered state and the
    // Kalman gain matrix.
    //
    // C_k = [ (C^k-1_k)^-1 + H^T_k * (V_k)^-1 * H_k ]^-1
    // K_k = C_k * H^T_k * (V_k)^-1
    //
    // This form requires two inversions of matrices with the dimension of the
    // state vector and one inversion of a matrix with the dimension of the
    // measurement vector. It can be more efficient if the dimension of the
    // state vector is small.

    Kalman::coordSys sys = getFilterInCoordSys();
    HKalTrackSite *site  = getSite(iSite);

    // Predicted state vector for this site.
    // a^k-1_k
    const TVectorD &predState  = site->getStateVec(kPredicted, sys);

    TVectorD projMeasVec(getMeasDim());
    // Expected measurement vector.
    // h_k(a^k-1_k)
    calcMeasVecFromState(projMeasVec, site, Kalman::kPredicted, sys);

    // Residual of measurement vector and expected measurement vector.
    TVectorD residual = getHitVec(site) - projMeasVec;

    // C^k-1_k
    const TMatrixD &predCov = site->getStateCovMat(Kalman::kPredicted, sys);
#if kalDebug > 0
    if(!HKalMatrixTools::checkCond(predCov)) {
        nCondErrs++;
        if(bPrintWarn) {
	    Warning("filterSwerling()",
		    "Matrix is ill-conditioned for inversion.");
        }
    }
#endif
    const TMatrixD  predCovInv(TMatrixD::kInverted, predCov);

     // (V_k)^-1
    const TMatrixD errInv(TMatrixD::kInverted, getHitErrMat(site));

    // H_k
    const TMatrixD &fProj   = site->getStateProjMat(Kalman::kFiltered, sys);
    const TMatrixD fProjTrans = TMatrixD(TMatrixD::kTransposed, fProj);

    //       C_k        = [                          
    TMatrixD filtCov    = TMatrixD(TMatrixD::kInverted,
                              // (C^k-1_k)^-1 + H^T_k      * (V_k)^-1 * H_k]^-1
				   predCovInv + fProjTrans * errInv   * fProj);

#if kalDebug > 0
    Int_t asymCells = HKalMatrixTools::checkSymmetry(filtCov, 1.e-9);
    if(asymCells > 0) {
        nCovSymErrs += asymCells;
        if(bPrintWarn) {
	    Warning("filterSwerling()",
		    "Covariance matrix for filter step is not symmetric.");
        }
    }
    if(!HKalMatrixTools::isPosDef(filtCov)) {
        nCovPosDefErrs++;
        if(bPrintWarn) {
	    Warning("filterSwerling()",
		    "Covariance for the filtered state is not positive definite.");
        }
    }
#endif

    if(!filtCov.IsSymmetric()) {
        if(!HKalMatrixTools::makeSymmetric(filtCov)) {
            if(bPrintWarn) {
		Warning("filterSwerling()",
			"Covariance matrix for filter step is not close to being symmetric.");
            }
        }
    }

    site->setStateCovMat(kFiltered, filtCov, sys);

    // Kalman gain matrix.
    //       K_k        = C_k     * H^T_k      * G_k
    TMatrixD kalmanGain = filtCov * fProjTrans * errInv;

    // Calculate filtered state vector.
    //       a_k       = a^k-1_k   + K_k        * (m_k - h_k(a^k-1_k) )
    TVectorD filtState = predState + kalmanGain * residual;
    site->setStateVec(kFiltered, filtState, sys);

#if kalDebug > 2
    if(sys == Kalman::kSecCoord) {
        cout<<"Filtering in sector coordinates."<<endl;
    }
    if(sys == Kalman::kLayCoord) {
        cout<<"Filtering in virtual layer coordinates."<<endl;
    }
    cout<<"Filter with Swerling Kalman equations."<<endl;
    cout<<"Expected state vector:"<<endl;
    predState.Print();
    cout<<"Expected measurement:"<<endl;
    projMeasVec.Print();
    cout<<"Measurement vector:"<<endl;
    getHitVec(site).Print();
    cout<<"Filtered state vector:"<<endl;
    filtState.Print();
    cout<<"Filtered covariance:"<<endl;
    filtCov.Print();
#endif
#if kalDebug > 3
    cout<<"Covariance from prediction:"<<endl;
    predCov.Print();
    cout<<"Measurement error matrix:"<<endl;
    getHitErrMat(site).Print();
    cout<<"Projector matrix:"<<endl;
    fProj.Print();
    cout<<"Kalman gain:"<<endl;
    kalmanGain.Print();
#endif

}

void HKalIFilt::filterUD(Int_t iSite) {
    // Update of the covariance matrix and state vector for the filter step
    // using the U-D filter. A UD decomposition of the covariance matrix
    // is used and only the U and D matrices are propagated.
    //
    // This method increases the numerical precision of the Kalman filter.
    // It cannot be used together with the annealing filter.
    //
    // Literature:
    // Optimal State Estimation: Kalman, H infinity and Nonlinear Approaches
    // Chapter 6.4: U-D filtering
    // Dan Simon
    //
    // Kalman Filtering: Theory and Practice using MATLAB, 3rd edition
    // Chapter 6.5: Square Root and UD Filters
    // Mohinder S. Grewal, Angus P. Andrews
    //
    // G. J. Bierman, Factorization Methods for Discrete Sequential Estimation,
    // Academic Press, New York 1977

    Kalman::coordSys sys = getFilterInCoordSys();
    HKalTrackSite *site = getSite(iSite);
    Int_t sdim = getStateDim(sys);
    Int_t mdim = getMeasDim();

    const TVectorD &measVec   = getHitVec(site); // m_k
    const TMatrixD &errMat    = getHitErrMat(site); // V_k

    TVectorD projMeasVec(getMeasDim());
    const TVectorD &predState = site->getStateVec(Kalman::kPredicted, sys);
    // Expected measurement vector.
    // h_k(a^k-1_k)
    calcMeasVecFromState(projMeasVec, site, Kalman::kPredicted, sys);

    // Residual.
    TVectorD resVec = measVec - projMeasVec;

    // Extract the U and D matrices.
    const TMatrixD &predCov   = site->getStateCovMat(Kalman::kPredicted, sys);
    TMatrixD U(sdim, sdim);
    TMatrixD D(sdim, sdim);
    HKalMatrixTools::resolveUD(U, D, predCov);

    // H_k
    const TMatrixD &fProj   = site->getStateProjMat(Kalman::kFiltered, sys);

    TVectorD filtState = predState;

    // Algorithm from the CD supplied with Grewal's and Andrews' book
    // Kalman Filtering: Theory and Practice Using Matlab.
    // Chapter 6, File: bierman.m

    // Process one component of the measurement vector at at time.
    for(Int_t iMeas = 0; iMeas < mdim; iMeas++) {
        // Get row number iMeas from the projector matrix.
        Double_t hrow[sdim];
        fProj.ExtractRow(iMeas, 0, hrow);
        TMatrixD H(1, sdim, hrow);

        Double_t residual = measVec(iMeas) - (H * filtState)(0);

        // U^T * H^T
        TMatrixD UtHt(TMatrixD(TMatrixD::kTransposed, U) * H.Transpose(H));

        TVectorD kalmanGain(sdim, (D * UtHt).GetMatrixArray());

        Double_t alpha = errMat(iMeas, iMeas);
        Double_t gamma = 1. / alpha;

        // Calculate U,D factors of covariance matrix and the Kalman gain.
        for(Int_t j = 0; j < sdim; j++) {
            Double_t beta = alpha;
            alpha += UtHt(j,0) * kalmanGain(j);
            Double_t lambda = - UtHt(j,0) * gamma;
            gamma = 1. / alpha;
            D(j,j) = beta * gamma * D(j,j);

            for(Int_t i = 0; i < j; i++) {
                beta = U(i,j);
                U(i,j) = beta + kalmanGain(i) * lambda;
                kalmanGain(i) += kalmanGain(j) * beta;
            }
        }
        kalmanGain *= gamma * residual;
        filtState = filtState + kalmanGain;
    }

#if kalDebug > 0
    if(!HKalMatrixTools::isPosDef(D)) {
        nCovPosDefErrs++;
        if(bPrintWarn) {
	    Warning("filterUD()",
		    "D-matrix for the filtered state is not positive definite.");
        }
    }
#endif

    // Store the U and D factors of the covariance matrix in a single matrix.
    // The diagonal elements of U are always 1. Replace them with the
    // elements of D.
    for(Int_t i = 0; i < D.GetNrows(); i++) {
        U(i,i) = D(i,i);
    }

    site->setStateCovMat(Kalman::kFiltered, U, sys);
    site->setStateVec(Kalman::kFiltered, filtState, sys);

#if kalDebug > 2
    if(sys == Kalman::kSecCoord) {
        cout<<"Filtering in sector coordinates."<<endl;
    }
    if(sys == Kalman::kLayCoord) {
        cout<<"Filtering in virtual layer coordinates."<<endl;
    }
    cout<<"Filter with UD Kalman equations."<<endl;
    cout<<"Expected state vector:"<<endl;
    predState.Print();
    cout<<"Expected measurement:"<<endl;
    projMeasVec.Print();
    cout<<"Measurement vector:"<<endl;
    getHitVec(site).Print();
    cout<<"Filtered state vector:"<<endl;
    filtState.Print();
    cout<<"U and D factors of filtered covariance:"<<endl;
    U.Print();
#endif
#if kalDebug > 3
    cout<<"U and D factors of covariance from prediction:"<<endl;
    predCov.Print();
    cout<<"Measurement error matrix:"<<endl;
    errMat.Print();
    cout<<"Projector matrix:"<<endl;
    fProj.Print();
#endif

}

void HKalIFilt::propagateCovConv(Int_t iFromSite, Int_t iToSite) {
    // Propagate the covariance matrix between measurement sites.
    // Must not be called before propagateTrack().
    // When applying the UD filter use propagateCovUD() function instead.
    //
    // iFromSite:  index of measurement site to start propagation from.
    // iToSite:    index of measurement site to propagate to (will be modified).

    HKalTrackSite *fromSite = getSite(iFromSite);
    HKalTrackSite *toSite   = getSite(iToSite);

    const TMatrixD &covMatFromSite = fromSite->getStateCovMat(kFiltered);              // C_k-1
#if kalDebug > 0
    Int_t nRows = covMatFromSite.GetNrows();
    Int_t nCols = covMatFromSite.GetNcols();
    if(nRows != getStateDim() || nCols != getStateDim()) {
	Error(Form("Prediction to site %i.", iToSite),
	      Form("Covariance matrix is %ix%i, but must be %ix%i matrix.",
		   nRows, nCols, getStateDim(), getStateDim()));
        exit(1);
    }
#endif
    const TMatrixD &propMatFromSite = fromSite->getStatePropMat(Kalman::kFiltered);
    const TMatrixD &procMatFromSite = fromSite->getStateProcMat(Kalman::kFiltered);

    TMatrixD propMatFromSiteTrans = TMatrixD(TMatrixD::kTransposed, propMatFromSite); // F_k-1

    //       C^k-1_k      = F_k-1           * C_k-1          *
    TMatrixD covMatToSite = propMatFromSite * covMatFromSite *
        // F^T_k-1           + Q_k-1
	propMatFromSiteTrans + procMatFromSite;
    if(!covMatToSite.IsSymmetric()) {
        if(!HKalMatrixTools::makeSymmetric(covMatToSite)) {
            if(bPrintWarn) {
		Warning("predictAndFilter2DHits()",
			"Covariance matrix for prediction is not close to being symmetric.");
            }
        }
    }
    toSite->setStateCovMat(Kalman::kPredicted, covMatToSite);
}

void HKalIFilt::propagateCovUD(Int_t iFromSite, Int_t iToSite) {
    // Update of the covariance matrix' U and D factors after the prediction
    // step.
    //
    // iFromSite:  index of measurement site to start propagation from.
    // iToSite:    index of measurement site to propagate to (will be modified).
    //
    // Only the matrix upper triangular U with the elements of D on its
    // diagonal are stored in the site.
    //
    // Literature:
    // Kalman Filtering: Theory and Practice using MATLAB, 3rd edition
    // Chapter 6.5: Square Root and UD Filters
    // Mohinder S. Grewal, Angus P. Andrews
    // and
    // Catherine L. Thornton, Triangular covariance factorizations for Kalman filtering,
    // Ph.D. thesis, University of California at Los Angeles, 1976

    HKalTrackSite *fromSite = getSite(iFromSite);
    HKalTrackSite *toSite   = getSite(iToSite);

    Int_t sdim = getStateDim();

    TMatrixD Uin(sdim, sdim);
    TMatrixD Din(sdim, sdim);
    const TMatrixD &covPrev = fromSite->getStateCovMat(Kalman::kFiltered);
    HKalMatrixTools::resolveUD(Uin, Din, covPrev);
    TMatrixD predCov(sdim, sdim);

    const TMatrixD &fProp = fromSite->getStatePropMat(Kalman::kFiltered);
    TMatrixD propU = fProp * Uin;

    TMatrixD  fProc       = fromSite->getStateProcMat(Kalman::kFiltered);

    TMatrixD UQ(fProc.GetNrows(), fProc.GetNcols());
    TMatrixD DQ(fProc.GetNrows(), fProc.GetNcols());
    HKalMatrixTools::resolveUD(UQ, DQ, fProc);

#if kalDebug > 0
    if(fProp.GetNrows() != covPrev.GetNrows() ||
       fProp.GetNcols() != covPrev.GetNcols()) {
        Error("covUpdateUD()",
              Form("Dimensions of covariance and propagator matrix do not match. Covariance is a %ix%i matrix while propagator is %ix%i.",
		   covPrev.GetNrows(), covPrev.GetNcols(),
		   fProp.GetNrows(), fProp.GetNcols()));
    }
    if(fProc.GetNrows() != covPrev.GetNrows() || fProc.GetNcols() != covPrev.GetNcols()) {
        Error("covUpdateUD()",
              Form("Dimensions of covariance and process noise matrix do not match. Covariance is a %ix%i matrix while process noise is %ix%i.",
		   covPrev.GetNrows(), covPrev.GetNcols(),
		   fProc.GetNrows(), fProc.GetNcols()));
    }
#endif

    for(Int_t i = sdim - 1; i >= 0; i--) {
        Double_t sigma = 0.;
        for(Int_t j = 0; j < sdim; j++) {
            sigma += propU(i,j) * Din(j,j) * propU(i,j);
            sigma += UQ(i,j) * DQ(j,j) * UQ(i,j);
        }
        predCov(i,i) = sigma;
        for(Int_t j = 0; j < i; j++) {
            sigma = 0.;
            for(Int_t k = 0; k < sdim; k++) {
                sigma += propU(i,k) * Din(k,k) * propU(j,k);
                sigma += UQ(i,k) * fProc(k,k) * UQ(j,k);
            }
            predCov(j,i) = sigma / predCov(i,i);
            for(Int_t k = 0; k < sdim; k++) {
                propU(j,k) -= predCov(j,i) * propU(i,k);
            }
            for(Int_t k = 0; k < sdim; k++) {
                UQ(j,k) -= predCov(j,i) * UQ(i,k);
            }
        }
    }

    toSite->setStateCovMat(Kalman::kPredicted, predCov);
}

Bool_t HKalIFilt::propagateTrack(Int_t iFromSite, Int_t iToSite) {
    // Propagates the track between measurement sites.
    //
    // iFromSite:  index of measurement site to start propagation from.
    // iToSite:    index of measurement site to propagate to (will be modified).

    HKalTrackSite *fromSite = getSite(iFromSite);
    HKalTrackSite *toSite  = getSite(iToSite);

    Bool_t propagationOk = kTRUE;

    // a_(k-1): filtered state vector of current site
    const TVectorD   &filtStateFromSite   = fromSite->getStateVec(kFiltered);
    // Plane to propagate from.
    const HKalMdcMeasLayer &planeFromSite = fromSite->getHitMeasLayer();
    // Plane to propagate to.
    const HKalMdcMeasLayer &planeToSite   = toSite  ->getHitMeasLayer();

    Int_t sdim = getStateDim();

#if kalDebug > 0
    Int_t nRows;
    nRows = filtStateFromSite.GetNrows();
    if(nRows != sdim) {
        if(bPrintErr) {
	    Error("propagateTrack()",
		  Form("Filtered state vector of site %i must have dimension %i, but has dimension %i",
		       iFromSite, nRows, sdim));
        }
        return kFALSE;
    }
#endif

    // a^(k-1)_k: predicted state for next site, to be calculated
    TVectorD predStateToSite(sdim);
    // F_k-1: propagation matrix, to be calculated
    TMatrixD propMatFromSite(sdim, sdim);
    // Q_k-1: process noise matrix, to be calculated
    TMatrixD procMatFromSite(sdim, sdim);

    // Propagate track to next measurement site. Calculate the expected state
    // vector a^(k-1)_k = f(a_k-1), the Jacobian F_k-1 and process noise
    // covariance Q_k-1.
    if(!trackPropagator.propagateToPlane(propMatFromSite, procMatFromSite,
                                         predStateToSite, filtStateFromSite,
                                         planeFromSite, planeToSite,
                                         planeFromSite, planeToSite,
                                         pid, getDirection())) {
        if(bPrintErr) {
	    Error("propagateTrack()",
		  Form("Propagation from measurement site %i in sec %i/mod %i/lay %i to site %i in sec %i/mod %i/lay %i failed. Skipping this site.",
		       iFromSite, planeFromSite.getSector(),
		       planeFromSite.getModule(), planeFromSite.getLayer(),
		       iToSite,   planeToSite.getSector(),
		       planeToSite.getModule(),   planeToSite.getLayer()));
        }
        toSite->setActive(kFALSE);
        return kFALSE;
    }

    // Fill arrays with RK stepping points.
    if(bFillPointArrays) {
        const TObjArray &pointsTrk = trackPropagator.getPointsTrack();
        for(Int_t i = 0; i < pointsTrk.GetEntries(); i++) {
            pointsTrack.Add(pointsTrk.At(i));
        }
        const TObjArray &fieldTrk = trackPropagator.getFieldTrack();
        for(Int_t i = 0; i < fieldTrk.GetEntries(); i++) {
            fieldTrack.Add(fieldTrk.At(i));
        }
    }

    trackLength += trackPropagator.getTrackLength();

    // Store propagation results.
    // -----------
    // a^(k-1)_k = f(a_k-1), store prediction for next site
    toSite  ->setStateVec(kPredicted, predStateToSite);
    // F_k-1, store propagator matrix
    fromSite->setStatePropMat(kFiltered, propMatFromSite);

    if(filtType == Kalman::kKalUD) {
        if(!HKalMatrixTools::decomposeUD(procMatFromSite)) {
	    Warning("predictAndFilter2DHits()",
		    "Could not decompose process noise into UD factors.");
        }
    }
    // Q_k-1, store process noise covariance
    fromSite->setStateProcMat(kFiltered, procMatFromSite);

    toSite->setEnergyLoss(fromSite->getEnergyLoss() +
			  trackPropagator.getEnergyLoss());
    // -----------

    return propagationOk;
}

void HKalIFilt::updateChi2Filter(Int_t iSite) {
    // Update chi^2 after having filtered site number iSite.

    Kalman::coordSys sys = getFilterInCoordSys();
    HKalTrackSite *site = getSite(iSite);

    Int_t mdim = getMeasDim();

    // Residual between predicted and filtered state vectors.
    const TVectorD &predState = site->getStateVec(Kalman::kPredicted, sys);
    const TVectorD &filtState = site->getStateVec(Kalman::kFiltered, sys);
    TVectorD diffPredFilt = filtState - predState;
    // Need a TMatrixD object later.
    // (a_k - a^k-1_k )^T
    TMatrixD diffPredFiltTrans(1, getStateDim(sys),
			       diffPredFilt.GetMatrixArray());

    // C^k-1_k
    TMatrixD predCov   = site->getStateCovMat(Kalman::kPredicted, sys);
    if(filtType == Kalman::kKalUD) {
        Int_t sdim = getStateDim(sys);
        TMatrixD U(sdim, sdim);
        TMatrixD D(sdim, sdim);
        HKalMatrixTools::resolveUD(U, D, predCov);
        predCov = U * D * TMatrixD(TMatrixD::kTransposed, U);
    }
    TMatrixD predCovInv(TMatrixD::kInverted, predCov);

    const TMatrixD &errMat    = getHitErrMat(site); // V_k

    const TMatrixD  errInv(TMatrixD::kInverted, errMat);

    TVectorD projFiltMeasVec(mdim);
    // Expected measurement from filter step.
    calcMeasVecFromState(projFiltMeasVec, site, Kalman::kFiltered, sys);

    TVectorD diffMeasFilt = getHitVec(site) - projFiltMeasVec;

    // Transform TVectorD objects into matrices for later matrix multiplications.
    // (m_k - a_k )^T.
    TMatrixD diffMeasFiltTrans(1, mdim , diffMeasFilt.GetMatrixArray());

    // Update chi2.
    //                 (a_k - a^k-1_k  )^T * (C^k-1_k)^-1 * (a_k - a^k-1_k)
    Double_t chi2Inc = (diffPredFiltTrans  * predCovInv   * diffPredFilt  )(0)+
                   //  (m_k - h_k(a_k))^T  * (V_k)^-1     * (m_k - h_k(a_k)   )
                       (diffMeasFiltTrans  * errInv       * diffMeasFilt  )(0);

    site->setChi2(chi2Inc);

    chi2 += chi2Inc;

#if kalDebug > 2
    cout<<"Site "<<iSite<<" added "<<chi2Inc<<" to chi2."<<endl;
    cout<<"Residual predicted - filtered state vectors: "<<endl;
    diffPredFilt.Print();
    cout<<"Residual measurement - measurement from filtered state: "<<endl;
    diffMeasFilt.Print();
#endif
}

void HKalIFilt::newTrack(const TObjArray &hits, const TVectorD &iniSv,
			 const TMatrixD &iniCovMat, Int_t pId) {
    // Reset data members to fit a new track.
    //
    // Input:
    // hits:        the array with the measurement hits. Elements must be of
    //              class HKalMdcHit.
    // iniStateVec: initial track state that the filter will use.
    // iniCovMat:   initial covariance matrix that the filter will use.
    // pId:         Geant particle id.
    // momGeant:    Geant momentum that will be written in the output tree.
    //              Ignore if this is not simulation data.
    bTrackAccepted = kTRUE;
    chi2 = 0.;
    fieldTrack.Clear();
    pointsTrack.Clear();
    trackLength = 0.;
    trackLengthAtLastMdc = 0.;
    trackPropagator.Clear();

    if(iniStateVec.GetNrows() != iniSv.GetNrows()) {
        iniStateVec.ResizeTo(iniSv.GetNrows());
        if(bPrintWarn) {
            Warning("fitTrack()", Form("Passed initial state vector has dimension %i. Class member of HKalIFilt has dimension %i.",
                                       iniSv.GetNrows(), iniStateVec.GetNrows()));
        }
    }
    iniStateVec = iniSv;

    // Add new hits to measurement sites.
    updateSites(hits);
    trackNum++;
    pid = pId;

    // Depending on the direction for propagation, set the index of the first
    // site (istart) and last site (igoal).
    // idir: increment (forward propagation) or decrement (backward
    // propagation) sites indices.
    Int_t fromSite; //, toSite; //unused
    if(getDirection() == kIterForward) {
        fromSite = 0;
       // toSite   = getNsites() - 1;  //unused
    } else {
        fromSite = getNsites() - 1;
        //toSite   = 0;                //unused
    }

    // Store the state vector and covariance matrix for the first site.
    HKalTrackSite *first = getSite(fromSite);

    first->setStateVec   (kPredicted, iniStateVec);
    first->setStateVec   (kFiltered,  iniStateVec);
    if(filtType == Kalman::kKalUD) {
        TMatrixD iniCovUD = iniCovMat;
        if(!HKalMatrixTools::decomposeUD(iniCovUD)) {
            Warning("fitTrack()","Could not decompose initial covariance matrix into UD factors.");
        }
        first->setStateCovMat(kPredicted, iniCovUD);
        first->setStateCovMat(kFiltered , iniCovUD);
    } else {
        first->setStateCovMat(kPredicted, iniCovMat);
        first->setStateCovMat(kFiltered , iniCovMat);
    }

    first->transSecToVirtLay(Kalman::kPredicted, (filtType == kKalUD));
    first->transSecToVirtLay(Kalman::kFiltered, (filtType == kKalUD));

    // Tilt coordinate system.
    // There are three options:
    // 1. kNoRot:
    //    Don't tilt the coordinate system.
    // 2. kVarRot:
    //    Tilt the coordinate system so that the z-axis will always
    //    point in the initial track direction. Assuming track curvature
    //    is not large this ensures that the track state parameters
    //    tx = tan(p_x / p_z) and ty = tan(p_y / p_z) remain small.
    //    Angles close to 90° would result in large values for tx and ty
    //    due to the tangens and poor results for the Kalman filter.
    if(rotCoords == Kalman::kVarRot) {
        // Calculate rotation matrix depending on track direction.
        TVector3 dir;
        HKalTrackState::calcDir(dir, iniStateVec);
        Double_t rotXangle = dir.Theta();
        Double_t rotYangle = - TMath::Pi()/2 + dir.Phi();
        setRotationAngles(rotXangle, rotYangle);
    }
    if(rotCoords == Kalman::kVarRot) {
        // Rotate coordinates of all sites.
        transformFromSectorToTrack();
    }
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

Bool_t HKalIFilt::calcMeasVecFromState(TVectorD &projMeasVec,
				       HKalTrackSite const * const site,
				       Kalman::kalFilterTypes stateType,
				       Kalman::coordSys sys) const {
    // Implements the projection function h(a) that calculates a measurement
    // vector from a track state.
    //
    // Output:
    // projMeasVec: The calculated measurement vector.
    // Input:
    // site:      Pointer to current site of the track.
    // stateType: Which state (predicted, filtered, smoothed) to project on a
    //            measurement vector.
    // sys:       Coordinate system to use (sector or virtual layer coordinates)

#if kalDebug > 0
    Int_t mdim = getMeasDim();
    if(projMeasVec.GetNrows() != mdim) {
	Warning("calcMeasVec", Form("Needed to resize TVectorD. Dimension of measurement vector (%i) does not match that of function parameter (%i).", mdim, projMeasVec.GetNrows()));
        projMeasVec.ResizeTo(mdim);
    }
#endif

    const TVectorD &sv = site->getStateVec(stateType, sys);

    projMeasVec = site->getStateProjMat() * sv;

    return kTRUE;
}

Bool_t HKalIFilt::checkCovMat(const TMatrixD &fCov) const {
    // Covariance matrices must be positive definite, i.e. symmetric and
    // all its eigenvalues are real and positive. Returns true if this
    // is the case.

    Bool_t   bCovOk  = kTRUE;

    // Diagonal elements should not be zero.
    for(Int_t i = 0; i < fCov.GetNrows(); i++) {
        if(fCov(i, i) == 0.) {
            if(bPrintWarn) {
		Warning("checkCovMat()",
			"A diagonal elment of the covariance matrix is 0.");
            }
            bCovOk = kFALSE;
        }
    }

    // Covariance matrix should be symmetric.
    if(filtType != Kalman::kKalUD) {
        if(!fCov.IsSymmetric()) {
            if(bPrintWarn) {
                Warning("checkCovMat()", "Covariance matrix is not symmetric.");
            }
            bCovOk = kFALSE;
        }
    }

    if(filtType != Kalman::kKalUD) {
        if(!HKalMatrixTools::isPosDef(fCov)) {
            if(bPrintWarn) {
		Warning("checkCovMat()",
			"Covariance matrix is not positive definite.");
            }
            bCovOk = kFALSE;
        }
    } else {
        TMatrixD U(fCov.GetNrows(), fCov.GetNcols());
        TMatrixD D(fCov.GetNrows(), fCov.GetNcols());
        HKalMatrixTools::resolveUD(U, D, fCov);
        TMatrixD UD = U * D * TMatrixD(TMatrixD::kTransposed, U);
        if(!HKalMatrixTools::isPosDef(UD)) {
            if(bPrintWarn) {
		Warning("checkCovMat()",
			"Covariance matrix is not positive definite.");
            }
            bCovOk = kFALSE;
        }
    }

    return bCovOk;
}

Bool_t HKalIFilt::excludeSite(Int_t iSite) {
    // Exclude a site from the measurement. Requires that prediction, filtering and smoothing
    // has been done before. If smoothing has been disabled it will be done in this function.
    // iSite: index in sites array of the site to exclude

    Kalman::coordSys sys = getFilterInCoordSys();

    if(iSite < 0 || iSite > getNsites() - 1) {
        Warning("excludeSite()", "Invalid site index.");
        return kFALSE;
    }

    // Transform state vector from sector to track coordinates.
    if(rotCoords == Kalman::kVarRot) {
        transformFromSectorToTrack();
    }

    // Do smoothing including site k if it hasn't been done before.
    if(!bDoSmooth) {
        if(!smooth()) {
            if(bPrintErr) {
		Error("excludeSite()",
		      Form("Cannot exclude site %i. Smoothing of track failed.", iSite));
            }
            return kFALSE;
        }
    }

    HKalTrackSite* exSite = getSite(iSite);                           // Site k to exclude.

    if(!exSite->getIsActive()) {
        if(bPrintErr) {
	    Error("excludeSite()",
		  Form("Attempted to exclude inactive site number %i.", iSite));
        }
        return kFALSE;
    }

    if(getFilterInCoordSys() == Kalman::kLayCoord &&
       exSite->getNcompetitors() > 2) {
	Error("excludeSite()",
	      "This function does not work with competing wire measurements.");
	return kFALSE;
    }


    // Smoothed state vector of site to exclude.
    TVectorD exSmooState  = exSite->getStateVec(kSmoothed, sys);       // a^n_k

    // Covariance and inverse covariance from smoothing step.
    TMatrixD exSmooCov    = exSite->getStateCovMat(kSmoothed, sys);    // C^n_k
    TMatrixD exSmooCovInv = TMatrixD(TMatrixD::kInverted, exSmooCov);  // (C^n_k)^-1

    // Measurement and inverse measurement matrices.
    TMatrixD exErr        = getHitErrMat(exSite);                      // V_k
    TMatrixD exErrInv     = TMatrixD(TMatrixD::kInverted, exErr);      // G_k

    // Measurement vector.
    TVectorD exMeasVec    = getHitVec(exSite);                         // m_k

    const TMatrixD &fProj = exSite->getStateProjMat(Kalman::kFiltered, sys); // H_k47
    const TMatrixD fProjTrans = TMatrixD(TMatrixD::kTransposed, fProj);

    // Calculate a measurement vector from smoothed state vector.
    TVectorD exSmooProjMeasVec(getMeasDim());
    calcMeasVecFromState(exSmooProjMeasVec, exSite, Kalman::kSmoothed,
			 Kalman::kSecCoord); //? daf?

    //                             - V_k   + H_k   * C^n_k     * H_k
    TMatrixD kalmanGainHelper = -1.* exErr + fProj * exSmooCov * fProj;
    //       K^n*_k     = C^n_k     * H^T_k      *
    TMatrixD kalmanGain = exSmooCov * fProjTrans *
        //      (- V_k   + H_k   * C^n_k     * H_k    )^-1
	TMatrixD(TMatrixD::kInverted, kalmanGainHelper);

    // Inverse filter: calculate new state vector at site k excluding
    // measurement at this site.
    //       a^n*_k         = a^n_k       + K^n*_k     * (m_k       - h_k(a^n_k)   )
    TVectorD exInvFiltState = exSmooState + kalmanGain * (exMeasVec - exSmooProjMeasVec);
    exSite->setStateVec(kInvFiltered, exInvFiltState, sys);

    // Calculate new covariance at site k.
    //       C^n*_k       =         (
    TMatrixD exInvFiltCov = TMatrixD(TMatrixD::kInverted,
    //                               (C^n_k)^-1   - H^T_k      * G_k      * H_k  )^-1
				     exSmooCovInv - fProjTrans * exErrInv * fProj);
    exSite->setStateCovMat(kInvFiltered, exInvFiltCov, sys);

    // Update chi^2.
    TVectorD projMeasVecInvFilt(getMeasDim());
    calcMeasVecFromState(projMeasVecInvFilt, exSite,
			 Kalman::kInvFiltered, Kalman::kSecCoord); //? daf?

    // Transform TVectorD objects into column vectors for matrix multiplication.
    // (a^n*_k - a^n_k)^T
    TMatrixD diffStateTrans(1, getStateDim(),
			    (exInvFiltState - exSmooState).GetMatrixArray());
    // (m_k - h_k(a^n*_k). Transposed residual.
    TMatrixD resTrans(1, getMeasDim(),
		      (exMeasVec - projMeasVecInvFilt).GetMatrixArray());

    //        (a^n*_k - a^n_k)^T    * (C^n_k)^-1 * (a^n*_k         - a^n_k      )
    chi2 -=   (diffStateTrans       * exSmooCov  * (exInvFiltState - exSmooState)  )(0)
    //        (m_k - h_k(a^n*_k))^T * G          * (m_k       - h_k(a^n*_k)       )
            - (resTrans             * exErrInv   * (exMeasVec - projMeasVecInvFilt))(0);

    // Transform state vectors back from track to sector coordinates.
    if(rotCoords == Kalman::kVarRot) {
        transformFromTrackToSector();
    }

    exSite->transVirtLayToSec(Kalman::kInvFiltered, 0);

    return kTRUE;
}

Bool_t HKalIFilt::filter(Int_t iSite) {
    // Run the filter step in Kalman filter for a site.
    // Must be called after the prediction step.
    // Different Kalman filter equations are used as set by setFilterMethod().
    // iSite: index of site to filter

    switch(filtType) {
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

Bool_t HKalIFilt::fitTrack(const TObjArray &hits, const TVectorD &iniSv,
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
    // momGeant:    Geant momentum that will be written in the output tree.
    //              Ignore if this is not simulation data.

#if kalDebug > 1
    cout<<"******************************"<<endl;
    cout<<"**** Fit track number "<<trackNum<<" ****"<<endl;
    cout<<"******************************"<<endl;
#endif
#if kalDebug > 2
    cout<<"Initial state vector:"<<endl;
    iniSv.Print();
    cout<<"Initial covariance:"<<endl;
    iniCovMat.Print();
    cout<<"Initial particle hypothesis: "<<pId<<endl;
#endif

    newTrack(hits, iniSv, iniCovMat, pId);
    if(getNsites() == 0) {
	if(bPrintWarn) {
            Warning("fitTrack()", "Track contained no measurements.");
	}
        return kFALSE;
    }

    // Propagation dir may change when doing several Kalman Filter iterations.
    Bool_t orgDirection = getDirection();

    Int_t fromSite, toSite;
    if(getDirection() == kIterForward) {
        fromSite = 0;
        toSite   = getNsites() - 1;
    } else {
        fromSite = getNsites() - 1;
        toSite   = 0;
    }

    // Fit with standard Kalman Filter.
    for(Int_t i = 0; i < nKalRuns; i++) {
        chi2 = 0.;

        bTrackAccepted &= runFilter(fromSite, toSite);

        if(i < nKalRuns-1) { // At least one more iteration to do.
	    // Set the filtered state of the last site from the previous
	    // iteration as the prediction for the next iteration.
            getSite(toSite)->setStateVec(kPredicted, getSite(toSite)->getStateVec(kFiltered));
            //getSite(toSite)->setStateCovMat(kPredicted , getSite(toSite)->getStateCovMat(kFiltered));

            // Change propagation direction in each iteration.
            setDirection(!direction);
            Int_t oldTo = toSite;
            toSite      = fromSite;
            fromSite    = oldTo;
        }
    }

    // Do smoothing.
    if(bDoSmooth) {
	if(!smooth()) {
	    if(getPrintErr()) {
                Error("fitTrack()", "Smoothing track failed.");
	    }
            bTrackAccepted = kFALSE;
	}
    }

    // Direction changes when running Kalman Filter with several iterations.
    // Restore original direction passed by user.
    setDirection(orgDirection);

    // Transform hit and state vectors from track back to sector coordinates.
    if(rotCoords == Kalman::kVarRot) {
        transformFromTrackToSector();
    }

    setTrackLengthAtLastMdc(trackLength);

#if kalDebug > 1
    if(bTrackAccepted) {
        cout<<"**** Fitted track ****"<<endl;
    } else {
        cout<<"**** Errors during track fitting ****"<<endl;
    }
#endif

    if(!bTrackAccepted) chi2 = -1.F;

    return bTrackAccepted;
}

Bool_t HKalIFilt::propagate(Int_t iFromSite, Int_t iToSite) {
    // Propagates the track to the next measurement site and update
    // covariance matrices.
    //
    // iFromSite:  index of measurement site to start propagation from.
    // iToSite:    index of measurement site to propagate to.

    if(!propagateTrack(iFromSite, iToSite)) {
        return kFALSE;
    }

    if(filtType == Kalman::kKalUD) {
        propagateCovUD(iFromSite, iToSite);
    } else {
        propagateCovConv(iFromSite, iToSite);
    }
    return kTRUE;
}

Bool_t HKalIFilt::propagateToPlane(TVectorD& svTo, const TVectorD &svFrom,
				   const HKalMdcMeasLayer &measLayFrom,
				   const HKalMdcMeasLayer &measLayTo,
                                   Int_t pid, Bool_t dir) {
    Int_t sdim = svTo.GetNrows();
    TMatrixD F(sdim, sdim);
    TMatrixD Q(sdim, sdim);

    return trackPropagator.propagateToPlane(F, Q,
                                            svTo, svFrom,
                                            measLayFrom, measLayTo,
                                            measLayFrom, measLayTo,
                                            pid, dir);

}

Bool_t HKalIFilt::runFilter(Int_t iFromSite, Int_t toSite) {
    // Do the prediction and filter steps of the Kalman filter by propagating
    // the track between the measurement sites iFromSite and toSite.
    //
    // iFromSite: index of track site to start propagation from
    // toSite:   index of track site to propagate to.

    HKalTrackSite *fromSite = getSite(iFromSite);
    if(!fromSite) {
	return kFALSE;
    }

    if(!calcProjector(iFromSite)) {
	if(getPrintErr()) {
	    Error("runFilter()",
		  Form("Bad projector matrix for site %i.", iFromSite));
	}
        return kFALSE;
    }

    Bool_t trackOk = kTRUE;

    Int_t iDir;
    if(iFromSite < toSite) { // propagate in forward direction
        iDir = +1;
    } else {                // propagate in backward direction
        iDir = -1;
    }

    // Search for first active measurement site.
    Int_t iCurSite  = iFromSite;        // Index of current site k-1.
    while(!getSite(iCurSite)->getIsActive()) {
        iCurSite += iDir;
        if((iDir < 0 && iCurSite < toSite) || (iDir > 0 && iCurSite > toSite)) {
            if(bPrintErr) {
		Error("predictAndFilter2DHits()",
		      Form("No two valid sites between %i and %i.", iFromSite, toSite));
            }
            return kFALSE;
        }
    }

    Int_t iNextSite = iCurSite + iDir; // Index of next site k.

    while((iDir > 0 && iNextSite <= toSite) ||
	  (iDir < 0 && iNextSite >= toSite)) {

        HKalTrackSite *nextSite  = getSite(iNextSite);

#if kalDebug > 1
        HKalTrackSite *curSite  = getSite(iCurSite);
	cout<<"##### Prediction from site "<<iCurSite<<" at "
	    <<curSite->getSector()<<" / "<<curSite->getModule()<<" / "
	    <<curSite->getLayer()
	    <<" to site "<<iNextSite<<" at "
	    <<nextSite->getSector()<<" / "<<nextSite->getModule()<<" / "
	    <<nextSite->getLayer()
            <<" #####"<<endl;
#endif

        // Skip inactive sites.
        if(!nextSite->getIsActive()) {
            iNextSite += iDir;
            continue;
        }

        if(!propagate(iCurSite, iNextSite)) {
            // Propagation failed. Skip one site and try again.
            iNextSite += iDir;
            continue;
        }

        if(!calcProjector(iNextSite)) {
            if(getPrintErr()) {
		Error("runFilter()",
		      Form("Errors calculating projector matrix for site %i.",
			   iNextSite));
            }
            nextSite->setActive(kFALSE);
            return kFALSE;
        }

        if(!checkSitePreFilter(iNextSite)) {
            if(getPrintErr()) {
		Error("runFilter()",
		      Form("Unable to filter site %i.", iNextSite));
            }
            nextSite->setActive(kFALSE);
            return kFALSE;
        }

        // Filter next site and calculate new chi2.
        if(!filter(iNextSite)) {
            if(bPrintErr) {
		Error("runFilter()",
		      Form("Failed to filter site %i. Skipping this site.", iNextSite));
            }
            trackOk = kFALSE;
            nextSite->setActive(kFALSE);
        } else {
            updateChi2Filter(iNextSite);
            iCurSite = iNextSite;
        }
        iNextSite += iDir;

    } // end loop through sites

    return trackOk;
}

Bool_t HKalIFilt::smooth() {
    // Implements the smoother by Rauch, Tung and Striebel for the Kalman filter.
    // Requires that the prediction and filter steps for all measurement
    // sites have been executed before.

    if(filtType == Kalman::kKalUD) {
        // Transform covariances from UD format.
        for(Int_t iSite = 0; iSite < getNsites(); iSite++) {
            HKalTrackSite *site = getSite(iSite);
            TMatrixD cov = site->getStateCovMat(Kalman::kPredicted);
            TMatrixD U(cov.GetNrows(), cov.GetNcols());
            TMatrixD D(cov.GetNrows(), cov.GetNcols());
            HKalMatrixTools::resolveUD(U, D, cov);
            cov = U * D * TMatrixD(TMatrixD::kTransposed, U);
            site->setStateCovMat(Kalman::kPredicted, cov);

            cov = site->getStateCovMat(kFiltered);
            HKalMatrixTools::resolveUD(U, D, cov);
            cov = U * D * TMatrixD(TMatrixD::kTransposed, U);
            site->setStateCovMat(Kalman::kFiltered, cov);
        }
    }

    Int_t iStartSite, iGoalSite, iDir;
    // For smoothing, iterate through the sites in opposite direction as in
    // the prediction/filter steps.
    Bool_t dir = getDirection();
    if(dir == kIterForward) {
        iStartSite = getNsites() - 1;
        iGoalSite  = 0;
        iDir       = -1;
    } else {
        iStartSite = 0;
        iGoalSite  = getNsites() - 1;
        iDir       = +1;
    }

    // The result of filtering for the last site coincides with that of
    // smoothing.
    HKalTrackSite *cur = getSite(iStartSite);
    cur->setStateVec   (kSmoothed,    cur->getStateVec(kFiltered));
    cur->setStateCovMat(kSmoothed,    cur->getStateCovMat(kFiltered));
    cur->setStateCovMat(kInvFiltered, cur->getStateCovMat(kFiltered));
    iStartSite += iDir;

    Int_t iSite = iStartSite;
    while((dir == kIterBackward && iSite <= iGoalSite) ||
	  (dir == kIterForward && iSite >= iGoalSite)) {
        // Site k to be smoothed in this step.
	HKalTrackSite* cur     = getSite(iSite);
        if(!cur->getIsActive()) { // Skip inactive sites.
            iSite += iDir;
            continue;
        }

#if kalDebug > 1
        cout<<"**** Smoothing site "<<iSite<<" **** "<<endl;
#endif
        // Site k+1 that has been smoothed in the previous step.
	Int_t iPreSite = iSite - iDir;
        HKalTrackSite* pre = getSite(iPreSite);
        while(!pre->getIsActive()) { // Skip inactive sites.
            iPreSite -= iDir;
            if(iPreSite < 0 || iPreSite >= getNsites()) {
                if(bPrintWarn) {
                    Warning("smoothAll()", Form("No active site found to smooth site number %i.", iSite));
                }
                return kFALSE;
            }
            pre = getSite(iPreSite);
        }

        // Predicted state vector of site k+1.
        const TVectorD &prePredState  = pre->getStateVec    (kPredicted);          // a^k_k+1

        // Smoothed state vector of site k+1.
        const TVectorD &preSmooState  = pre->getStateVec    (kSmoothed);           // a^n_k+1

        // Filtered state vector of site k.
        const TVectorD &curFiltState  = cur->getStateVec    (kFiltered);           // a_k

        // Covariance from filter step for site k.
        const TMatrixD &curFiltCov    = cur->getStateCovMat (kFiltered);           // C_k

        // Covariance and inverted covariance from prediction for site k+1.
        const TMatrixD &prePredCov    = pre->getStateCovMat (kPredicted);          // C^k_k+1
        const TMatrixD  prePredCovInv = TMatrixD(TMatrixD::kInverted, prePredCov); // (C^k_k-1)^-1

        // Propagator and transposed propagator from predicion for site k.
        const TMatrixD &curProp       = cur->getStatePropMat(kFiltered);           // F_k
        const TMatrixD  curPropTrans  = TMatrixD(TMatrixD::kTransposed, curProp);  // F^T_k

        // Covariance from smoothing step for site k+1.
        const TMatrixD &preSmooCov    = pre->getStateCovMat (kSmoothed);           // C^n_k+1

        // Gain matrix for smoothing.
        //       A_k          = C_k        * F^T_k        * (C^k_k+1)^-1
        TMatrixD smootherGain = curFiltCov * curPropTrans * prePredCovInv;

        // Calculate residual of smoothed and prediction.
        TVectorD residual = preSmooState - prePredState;

	// z-coordinate is not a degree of freedom since track must always be
	// on a layer.
        if(residual.GetNrows() > 5) {
            residual(kIdxZ0) = 0.;
        }

        //       a^n_k        = a_k          + A_k          * (a^n_k+1 - a^k_k+1)
        TVectorD curSmooState = curFiltState + smootherGain * residual;

#if kalDebug > 2
        cout<<"Residual"<<endl;
        residual.Print();
        cout<<"Smoother gain matrix "<<endl;
        smootherGain.Print();
	cout<<"Filtered State:"<<endl;
        curFiltState.Print();
	cout<<"Smoothed State:"<<endl;
        curSmooState.Print();
#endif

	cur->setStateVec(kSmoothed,    curSmooState);
        // Inverse filtering works with smoothed states.
	cur->setStateVec(kInvFiltered, curSmooState);

	// Calculate covariance for smoothed state. Needed for possible
	// inverse filtering.
        //       C^n_k      = C_k        + 
	TMatrixD curSmooCov = curFiltCov +
            // A_k       * (C^n_k-1    - C^k_k+1   ) *
	    smootherGain * (preSmooCov - prePredCov) *
            //  A^T_k
	    TMatrixD(TMatrixD::kTransposed, smootherGain);
        cur->setStateCovMat(kSmoothed,    curSmooCov);
        cur->setStateCovMat(kInvFiltered, curSmooCov);

	// Need the smoothing results in virt. layer coordinates for
	// inverse filtering.
        if(getFilterInCoordSys() == Kalman::kLayCoord) {
            cur->transSecToVirtLay(kSmoothed, 0);
        }

        iSite += iDir;
    }

    return kTRUE;
}

void HKalIFilt::sortHits() {
    // Sort the competitors in each site by weight.

    for(Int_t iSite = 0; iSite < getNsites(); iSite++) {
        getSite(iSite)->sortHits();
    }
}

Bool_t HKalIFilt::traceToVertex() {

    HKalTrackSite *fromSite = getSite(0);

    Int_t sec = fromSite->getSector();
    Int_t mod = fromSite->getModule();
    // Vertex in module coordinate system.
    Double_t xVertex = 0.;
    Double_t yVertex = 0.;
    Double_t zVertex = -500.;
    if(mod == 1) zVertex = -650;
    if(mod > 2) {
        if(getPrintErr()) {
            Error("traceToVertex", "No inner segment.");
        }
        return kFALSE;
    }
    HGeomVector vertex(xVertex, yVertex, zVertex);

    HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();  // check if is already there
    if(!fSizesCells) {
	fSizesCells = HMdcSizesCells::getObject();
	fSizesCells->initContainer();
    }

    // Transform vertex coordinates to sector system.
    HMdcSizesCellsMod &fSizesCellsMod = (*fSizesCells)[sec][mod];
    vertex = fSizesCellsMod.getSecTrans()->transFrom(vertex);
    TVector3 pointForVertexRec(vertex.getX(), vertex.getY(), vertex.getZ());

    const TVectorD         &svFrom    = fromSite->getStateVec(Kalman::kSmoothed);
    const HKalMdcMeasLayer &planeFrom = fromSite->getHitMeasLayer();

    HKalMdcMeasLayer planeVertex(pointForVertexRec, planeFrom.getNormal(),
                                 planeFrom.getMaterialPtr(kFALSE),
                                 planeFrom.getMaterialPtr(kFALSE),
                                 -1, sec, 0, 1, 0.);

    Int_t sdim = getStateDim();

#if metaDebug > 0
    Int_t nRows = svFrom.GetNrows();
    if(nRows != sdim) {
        if(bPrintErr) {
	    Error("traceToVertex()",
		  Form("Smoothed state vector of site %i must have dimension %i, but has dimension %i", 0, nRows, sdim));
        }
        return kFALSE;
    }

    if(!bDoSmooth) {
        if(getPrintErr()) {
	    Error("traceToVertex()",
		  "Cannot trace to vertex without doing smoothing.");
        }
        return kFALSE;
    }
#endif

    // Dummy matrices.
    TMatrixD F(sdim, sdim);
    TMatrixD Q(sdim, sdim);

    // Propagate track to vertex.
    if(!trackPropagator.propagateToPlane(F, Q,
                                         svVertex, svFrom,
                                         planeFrom, planeVertex,
                                         planeFrom, planeVertex,
                                         pid, kIterBackward)) {
        if(getPrintErr()) {
	    Error("traceToVertex()",
		  "Failed propagation of smoothed track to vertex.");
        }
        return kFALSE;
    }

    TVector3 posVertex;
    HKalTrackState::calcPosAtPlane(posVertex, planeVertex, svVertex);

    HMdcSizesCellsSec& fSCSec = (*fSizesCells)[sec];
    const HGeomVector& target = fSCSec.getTargetMiddlePoint();

    //?
    Double_t dist = TMath::Sqrt(TMath::Power(target.getX() - posVertex.X(), 2.) +
                                TMath::Power(target.getY() - posVertex.Y(), 2.) +
                                TMath::Power(target.getZ() - posVertex.Z(), 2.));
#if metaDebug > 2
    cout<<"Tracklength from target to first MDC: "
	<<trackPropagator.getTrackLength()<<", dist correction :"<<dist<<endl;
#endif
#if metaDebug > 3
    cout<<"Position at target: "<<endl;
    posVertex.Print();
#endif
    trackLength += dist + trackPropagator.getTrackLength();
    setTrackLengthAtLastMdc(trackLength);

    return kTRUE;
}

Bool_t HKalIFilt::traceToMeta(const TVector3& metaHit, const TVector3& metaNormVec) {
    // Propagates the track from the last fitted MDC position till track intersection
    // with META plane (is defined by META-hit and normal vector to rod's plane.
    // Should only be called after having filtered all sites.
    //
    // Input:
    // metaHit:
    // metaNormVec:

    HKalTrackSite *fromSite = getSite(getNsites() - 1);

    trackLength = trackLengthAtLastMdc;

    Int_t sdim = getStateDim();
    TMatrixD F(sdim, sdim);
    TMatrixD Q(sdim, sdim);

    const TVectorD         &svFrom    = fromSite->getStateVec(Kalman::kSmoothed);
    const HKalMdcMeasLayer &planeFrom = fromSite->getHitMeasLayer();

    HKalMdcMeasLayer planeMeta(metaHit, metaNormVec,
			       planeFrom.getMaterialPtr(kFALSE),
			       planeFrom.getMaterialPtr(kFALSE),
			       fromSite->getModule(), fromSite->getSector(),
			       0, 1, 0);

    if(!trackPropagator.propagateToPlane(F, Q,
                                         svMeta, svFrom,
                                         planeFrom, planeMeta,
                                         planeFrom, planeMeta,
                                         pid, kIterForward)) {
        if(getPrintErr()) {
	    Error("traceToMeta()",
		  "Failed propagation of smoothed track to Meta.");
        }
        return kFALSE;
    }

    HKalTrackState::calcPosAtPlane(posMeta, planeMeta, svMeta);

    trackLength += trackPropagator.getTrackLength();

#if metaDebug > 1
    cout<<"Kalman filter: trace to Meta hit ("<<metaHit.X()<<"/"<<metaHit.Y()
	<<"/"<<metaHit.Z()<<")"
	<<" with normal vector ("<<metaNormVec.X()<<"/"<<metaNormVec.Y()
	<<"/"<<metaNormVec.Z()<<")."<<endl;
#endif
#if metaDebug > 2
    cout<<"Distance last MDC to Meta: "<<trackPropagator.getTrackLength()<<endl;
#endif
#if metaDebug > 3
    cout<<"State at Meta: "<<endl;
    svMeta.Print();
    cout<<"Reconstructed pos at Meta: ("<<posMeta.X()<<"/"<<posMeta.Y()
	<<"/"<<posMeta.Z()<<")"<<endl;
#endif

    return kTRUE;
}


void HKalIFilt::transformFromSectorToTrack() {
    // Transform all sites using the Kalman system's rotation matrix.

#if kalDebug > 0
    if(pRotMat->IsIdentity()) {
	Warning("transformFromSectorToTrack()",
		"Transformation matrix has not been set.");
    }
#endif
    for(Int_t i = 0; i < getNsites(); i++) {
        getSite(i)->transform(*pRotMat);
    }
}

void HKalIFilt::transformFromTrackToSector() {
    // Transform all sites using the inverse rotation matrix stored in the
    // Kalman system. This will transform the sites back to sector coordinates
    // after a call of transformFromSectorToTrack() and if the rotation matrix
    // has not been modified in the meantime.

    TRotation rotInv = pRotMat->Inverse();
    // Rotate back sites.
    for(Int_t i = 0; i < getNsites(); i++) {
        getSite(i)->transform(rotInv);
    }

    // Rotate back points from propagation.

    for(Int_t i = 0; i < pointsTrack.GetEntries(); i++) {

#if kalDebug > 0
        if(!pointsTrack.At(i)->InheritsFrom("TVector3")) {
	    Error("transformFromTrackToSector()",
		  Form("Object at index %i in trackPoints array is of class %s. Expected class is TVector3.",
		       i, pointsTrack.At(i)->ClassName()));
            return;
        }
#endif

        ((TVector3*)pointsTrack.At(i))->Transform(rotInv);
    }
}

void HKalIFilt::updateSites(const TObjArray &hits) {
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

        // Don't store competing hits.
	if(!hit->areCompetitors(*hit, *nexthit)) {
            getSite(iSite)->addHit(hit);
            iSite++;
        }
        iHit++;
    }

    // Add last hit in array to a site.
    HKalMdcHit *lastHit = (HKalMdcHit*)hits.At(hits.GetEntries()-1);
    // Don't store competing hits.
    if(hits.GetEntries() > 1) {
        HKalMdcHit *secondLastHit = (HKalMdcHit*)hits.At(hits.GetEntries()-2);
        if(!lastHit->areCompetitors(*lastHit, *secondLastHit)) {
	    getSite(iSite)->addHit(lastHit);
            iSite++;
        }
    } else {
	getSite(iSite)->addHit(lastHit);
        iSite++;
    }

    nSites = iSite;
    nHitsInTrack = hits.GetEntries();
#if kalDebug > 1
    cout<<"New track has "<<nSites<<" measurement sites."<<endl;
#endif
}

TMatrixD const& HKalIFilt::getHitErrMat(HKalTrackSite* const site) const {
    // Returns the measurement error of site.
    //
    // Input:
    // site: measurement site.

    return site->getErrMat(0);
}

TVectorD const& HKalIFilt::getHitVec(HKalTrackSite* const site) const {
    // Returns the measurement vector of measurement site.
    //
    // Input:
    // site: measurement site.

    return site->getHitVec(0);
}

void HKalIFilt::getMetaPos(Double_t &x, Double_t &y, Double_t &z) const {
    x = posMeta.X();
    y = posMeta.Y();
    z = posMeta.Z();
}

void HKalIFilt::getVertexPos(Double_t &x, Double_t &y, Double_t &z) const {
    x = posVertex.X();
    y = posVertex.Y();
    z = posVertex.Z();
}

Double_t HKalIFilt::getNdf() const {
    // Returns number degrees of freedom, i.e. number of experimental points
    // minus number of parameters that are estimated.

    return getNsites() * getMeasDim() - getStateDim();
}

void HKalIFilt::setPrintErrors(Bool_t print) {
    // Print or suppress error messages.
    bPrintErr = print;
    trackPropagator.setPrintErrors(print);
}

void HKalIFilt::setPrintWarnings(Bool_t print) {
    // Print or suppress warning messages.
    bPrintWarn = print;
    trackPropagator.setPrintWarnings(print);
}

void HKalIFilt::setRotation(Kalman::kalRotateOptions rotate) {
    // Set options for the Kalman filter.
    // kalRotateOptions: Tilt coordinate system.
    // There are (approximately) three options:
    // 1. kNoRot:
    //    Don't tilt the coordinate system.
    // 2. kVarRot:
    //    Tilt the coordinate system so that the z-axis will always
    //    point in the initial track direction. Assuming track curvature
    //    is not large this ensures that the track state parameters
    //    tx = tan(p_x / p_z) and ty = tan(p_y / p_z) remain small.
    //    Angles close to 90° would result in large values for tx and ty
    //    due to the tangens and poor results for the Kalman filter.

    rotCoords = rotate;
    if( rotCoords == kVarRot) {
        trackPropagator.setRotateBfieldVecs(kTRUE);
    } else {
        trackPropagator.setRotateBfieldVecs(kFALSE);
    }
}

void HKalIFilt::setRotationAngles(Double_t rotXangle, Double_t rotYangle) {
    // Set the rotation matrix.
    // rotXangle: rotation angle around x axis in radians.
    // rotYangle: rotation angle around y axis in radians.
    pRotMat->SetToIdentity();
    pRotMat->RotateX(rotXangle);
    pRotMat->RotateY(rotYangle);
}
