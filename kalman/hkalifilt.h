#ifndef HKalIFilt_h
#define HKalIFilt_h

// from ROOT
#include "TRotation.h"
#include "TMatrixD.h"
class     TVector3;
#include "TVectorD.h"

// from hydra
class HCategory;
class HMdcTrackGField;

#include "hkaldef.h"
#include "hkalrungekutta.h"
#include "hkaltracksite.h"


class HKalIFilt : public TObject {

private:

    Bool_t   bDoDEDX;        // Energy loss.
    Bool_t   bDoMS;          // Multiple scattering.
    Bool_t   bDoSmooth;      //! Do smoothing.
    Bool_t   bPrintWarn;     //! Print warning messages.
    Bool_t   bPrintErr;      //! Print error messages.
    Bool_t   bTrackAccepted; // Track filtering done without errors.
    Double_t betaInput;      // Beta value used for initial particle hypothesis.
    Double_t chi2;           // chi2 of track. Will be -1 if fit failed.
    Bool_t   direction;      // Propagation direction: kIterForward/kIterBackward
    Int_t    filtType;       // Switch between alternate formulations of the Kalman equations.
    Int_t    hitType;        // Wire or segment hits.
    TVectorD iniStateVec;    // Estimation of the track state vector at the first measurement site.
    Int_t    nCondErrs;      // Number of times when a matrix was ill-conditioned for inversion. Only filled if kalDebug > 0.
    Int_t    nCovSymErrs;    // Number of symmetry breaking in the covariance matrix. Only filled if kalDebug > 0.
    Int_t    nCovPosDefErrs; // Number of times the covariance matrix was not positive definite. Only filled if kalDebug > 0.
    Int_t    nKalRuns;       // Number of Kalman filter iterations.
    Int_t    nHitsInTrack;   // Number of sites in current track.
    Int_t    nMaxSites;      // Maximum number of sites that can be stored.
    Int_t    pid;            // Geant particle id.
    Int_t    rotCoords;      // Option to tilt coordinate system for track fitting.
    Int_t    nSites;         // Number of measurement sites in track.
    Double_t trackLength;    // Track length from target.
    Double_t trackLengthAtLastMdc; // Track length from target to last MDC.
    Int_t    trackNum;       // Number of tracks fitted.

    HKalTrackSite  **sites;           //! Array of MDC track sites.
    TVector3         posVertex;       //! Position information at vertex.
    TVectorD         svVertex;        //! Track state at vertex.
    TVector3         posMeta;         //! Position information at meta detector.
    TVectorD         svMeta;          //! Track state at META detector.
    HKalRungeKutta   trackPropagator; //! The object that realizes the track propagation.
    TRotation       *pRotMat;         //! Rotation matrix for possible coordinate tilting.
    TVector3        *pTransVec;       //! Translation vector for possible coordinate transformations.

    Bool_t     bFillPointArrays; // == kTRUE if point arrays should be filled
    TObjArray  pointsTrack;      // Points from Runge-Kutta stepping along trajectory stored
    TObjArray  fieldTrack;       // Field from Runge-Kutta stepping at Points along trajectory stored

protected:

    virtual Bool_t calcProjector      (Int_t iSite) const = 0;

    virtual Bool_t checkSitePreFilter (Int_t iSite) const;

    virtual void   filterConventional (Int_t iSite);

    virtual void   filterJoseph       (Int_t iSite);

    virtual void   filterSequential   (Int_t iSite);

    virtual void   filterSwerling     (Int_t iSite);

    virtual void   filterUD           (Int_t iSite);

    virtual void   newTrack           (const TObjArray &hits, const TVectorD &iniSv,
				       const TMatrixD &iniCovMat, Int_t pid);

    virtual void   propagateCovUD     (Int_t iFromSite, Int_t iToSite);

    virtual void   propagateCovConv   (Int_t iFromSite, Int_t iToSite);

    virtual Bool_t propagateTrack     (Int_t iFromSite, Int_t iToSite);

    virtual void   updateChi2Filter   (Int_t iSite);

    virtual Kalman::coordSys    getFilterInCoordSys() const { return Kalman::kSecCoord; }

    virtual TRotation*          getRotMat()           const { return pRotMat; }

    virtual void   setChi2            (Double_t c)    { chi2 = c; }

    virtual void   setNSites          (Int_t n)       { nSites = n; }

    virtual void   setNHitsInTrack    (Int_t n)       { nHitsInTrack = n; }

    virtual void   setTrackChi2       (Double_t c)    { chi2 = c; }

    virtual void   setTrackLength     (Double_t l)    { trackLength = l; }

    virtual void   setTrackLengthAtLastMdc (Double_t l)    { trackLengthAtLastMdc = l; }

public:

    HKalIFilt(Int_t nHits, Int_t measDim, Int_t stateDim, HMdcTrackGField *fMap, Double_t fpol);

    virtual ~HKalIFilt();

    virtual Bool_t calcMeasVecFromState      (TVectorD &projMeasVec, HKalTrackSite const* const site,
                                              Kalman::kalFilterTypes stateType, Kalman::coordSys sys) const;

    virtual Bool_t checkCovMat               (const TMatrixD &fCov) const;

    virtual Bool_t excludeSite               (Int_t iSite);

    virtual Bool_t filter                    (Int_t iSite);

    virtual Bool_t fitTrack                  (const TObjArray &hits, const TVectorD &iniStateVec,
					      const TMatrixD &iniCovMat, Int_t pId);

    virtual Bool_t propagate                 (Int_t iFromSite, Int_t iToSite);

    virtual Bool_t propagateToPlane          (TVectorD& svTo, const TVectorD &svFrom,
					      const HKalMdcMeasLayer &measLayFrom, const HKalMdcMeasLayer &measLayTo,
					      Int_t pid, Bool_t dir);

    virtual Bool_t runFilter                 (Int_t fromSite, Int_t toSite);

    virtual Bool_t smooth                    ();

    virtual void   sortHits                  ();

    virtual Bool_t traceToMeta               (const TVector3& metaHit, const TVector3& metaNormVec);

    virtual Bool_t traceToVertex             ();

    virtual void   transformFromSectorToTrack();

    virtual void   transformFromTrackToSector();

    virtual void   updateSites               (const TObjArray &hits);

    virtual Double_t            getBetaInput    () const           { return betaInput; }

    virtual Double_t            getChi2         () const           { return chi2; }

    virtual void                getCovErrs      (Int_t &nSymErrs, Int_t &nPosDefErrs) const { nSymErrs = nCovSymErrs; nPosDefErrs = nCovPosDefErrs; }

    virtual Double_t            getDafChi2Cut   () const           { return -1.; }

    virtual const Double_t*     getDafT         () const           { return NULL; }

    virtual Bool_t              getDirection    () const           { return direction; }

    virtual Bool_t              getDoDaf        () const           { return kFALSE; }

    virtual Bool_t              getDoEnerLoss   () const           { return bDoDEDX; }

    virtual Bool_t              getDoMultScat   () const           { return bDoMS; }

    virtual Bool_t              getDoSmooth     () const           { return bDoSmooth; }

    virtual Double_t            getFieldIntegral() const           { return trackPropagator.calcFieldIntegral().Mag(); }

    virtual Int_t               getFilterMethod () const           { return filtType; }

    virtual TVectorD  const&    getIniStateVec  () const           { return iniStateVec; }

    virtual TObjArray const&    getFieldTrack   () const           { return fieldTrack; }

    virtual TMatrixD  const&    getHitErrMat    (HKalTrackSite* const site) const;

    virtual Int_t               getHitType      () const           { return hitType; }

    virtual TVectorD  const&    getHitVec       (HKalTrackSite* const site) const;

    virtual Bool_t              getLastTrackAccepted() const       { return bTrackAccepted; }

    virtual Int_t               getMeasDim      () const           { return getSite(0)->getMeasDim(); }

    virtual void                getMetaPos      (Double_t &x, Double_t &y, Double_t &z) const;

    virtual Int_t               getNdafs        () const           { return 0; }

    virtual Double_t            getNdf          () const;

    virtual Int_t               getNhits        () const           { return nHitsInTrack; }

    virtual Int_t               getNiter        () const           { return nKalRuns; }

    virtual Int_t               getNmaxSites    () const           { return nMaxSites; }

    virtual Int_t               getNsites       () const           { return nSites; }

    virtual Int_t               getPid          () const           { return pid; }

    virtual TObjArray const&    getPointsTrack  () const           { return pointsTrack; }

    virtual const HKalRungeKutta* getTrackPropagator()  const      { return &trackPropagator; }

    virtual inline HKalTrackSite* getSite       (UInt_t site) const {

#if kalDebug > 0
        if(site < (UInt_t)nMaxSites) {
            return sites[site];
        } else {
            Warning("getSite()", Form("Index %i out of range.", site));
            return NULL;
        }
#else
        return sites[site];
#endif
    }

    virtual Bool_t              getPrintErr     () const           { return bPrintErr; }

    virtual Bool_t              getPrintWarn    () const           { return bPrintWarn; }

    virtual Int_t               getRotation     () const           { return rotCoords; }

    virtual Int_t               getStateDim     (Kalman::coordSys coord=kSecCoord) const { return getSite(0)->getStateDim(coord); }

    virtual Double_t            getTrackLength  () const           { return trackLength; }

    virtual Int_t               getTrackNum     () const           { return trackNum; }

    virtual void                getVertexPos    (Double_t &x, Double_t &y, Double_t &z) const;

    virtual void setBetaInput       (Double_t b)                              { betaInput = b; }

    virtual void setConstField      (Bool_t constField)                       { trackPropagator.setUseConstField(constField); }

    virtual void setDafPars         (Double_t chi2cut, const Double_t *T, Int_t n) { ; }

    virtual void setDirection       (Bool_t dir)                              { direction = dir; trackPropagator.setDirection(dir); }

    virtual void setDoEnerLoss      (Bool_t dedx)                             { bDoDEDX = dedx; trackPropagator.setDoEnerLoss(dedx); }

    virtual void setDoMultScat      (Bool_t ms)                               { bDoMS = ms; trackPropagator.setDoMultScat(ms); }

    virtual void setFillPointsArrays(Bool_t fill)                             { bFillPointArrays = fill; trackPropagator.setFillPointsArrays(fill); }

    virtual void setFieldMap        (HMdcTrackGField *fMap, Double_t fpol)    { trackPropagator.setFieldMap(fMap, fpol); }

    virtual void setFieldVector     (const TVector3 &B)                       { trackPropagator.setFieldVector(B); }

    virtual void setFilterMethod    (Kalman::filtMethod type)                 { filtType = type; }

    virtual void setHitType         (Kalman::kalHitTypes hType)               { hitType = hType; }

    virtual void setNumIterations   (Int_t kalRuns)                           { nKalRuns = kalRuns; }

    virtual void setPrintWarnings   (Bool_t print);

    virtual void setPrintErrors     (Bool_t print);

    virtual void setRotationAngles  (Double_t rotXAngle, Double_t rotYAngle);

    virtual void setRotation        (Kalman::kalRotateOptions rotate);

    virtual void setRungeKuttaParams(Float_t initialStpSize, Float_t stpSizeDec,
				     Float_t stpSizeInc, Float_t maxStpSize,
				     Float_t minStpSize, Float_t minPrec,
				     Float_t maxPrec, Int_t maxNumStps,
				     Int_t maxNumStpsPCA, Float_t maxDst,
				     Double_t minLngthCalcQ) { trackPropagator.setRungeKuttaParams(initialStpSize, stpSizeDec, stpSizeInc, maxStpSize, minStpSize, minPrec, maxPrec, maxNumStps, maxNumStpsPCA, maxDst, minLngthCalcQ); }

    virtual void setSmoothing       (Bool_t smooth)                           { bDoSmooth = smooth; }

    ClassDef(HKalIFilt,0)
};

#endif // HKalIFilt_h
