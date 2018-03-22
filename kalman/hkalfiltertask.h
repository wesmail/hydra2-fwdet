#ifndef HKALFILTER_H
#define HKALFILTER_H

// from ROOT
class TArrayI;
class TF1;
#include "TString.h"
class TObjArray;

// from hydra
#include "hcategory.h"
class    HMetaMatch2;
class    HMdcTrkCand;
#include "hreconstructor.h"

#include "hkaldef.h"
#include "hkalifilt.h"
#include "hkalinput.h"
#include "hkalmetamatcher.h"
#include "hkaltrack.h"

class  HKalDetCradle;


class HKalFilterTask : public HReconstructor {

private:

    HKalIFilt*  kalsys;            //! Kalman filter object.
    HKalInput*  input;             //! Retrieves measurements of the next track.
    HKalMetaMatcher metaMatcher;   // Matches track with Meta detector.
    const TString noKalman;

    HCategory*  fCatKalTrack;      //! Output category.
    HCategory*  fCatKalSite;       //! Output category.
    HCategory*  fCatKalHit2d;      //! Output category. Filled when fitting segments.
    HCategory*  fCatKalHitWire;    //! Output category. Filled when fitting wire hits.
    HCategory*  fCatMetaMatch;     //! Meta match category.
    HCategory*  fCatTofHit;        //! TofHit categeory.
    HCategory*  fCatRpcClst;       //! RpcCluster category.
    HCategory*  fCatSplineTrack;   //1 Retrieve momentum estimate from spline.

    TF1*        fMomErr;           //! Function to calculate the initial momentum error. Depends on theta in degrees.
    TF1*        fTxErr;            //! Function to calculate the initial parameter tan(p_x/p_z). Depends on theta in degrees.
    TF1*        fTyErr;            //! Function to calculate the initial parameter tan(p_x/p_z). Depends on theta in degrees.

    TObjArray*  materials;         //! Optional pointer to array with materials for the MDCs.

    Bool_t      bCompHits;         // Consider competing hits for the DAF.
    Bool_t      bDaf;              // Apply the Deterministic Annealing Filter (DAF).
    Bool_t      bGeantPid;         // Run with Geant PID.
    Bool_t      bSim;              // Simulation/real data.
    Bool_t      bWire;             // Fit wire or segment hits.

    Bool_t      bFillSites;        // Fill Kalman categories for measurement sites and hits. Turned off by default.

    Int_t       iniSvMethod;       // Input for initial state vector. default = Spline, 1 = Geant, 2 = smeared Geant, 2 = Runge-Kutta
    Int_t       numTracks;         // Number of reconstructed tracks
    Int_t       numTracksErr;      // Number of tracks where reconstruction failed
    Int_t       measDim;           // Dimension of measurement vector.
    Int_t       stateDim;          // Dimension of track state vector.

    TArrayI     dopid;             // Fit only tracks with certain pids.
    Double_t    errMom;            // Error of initial momentum estimate as a fraction: 0.05 <=> 5% uncertainty.  Will only be used if fMomErr is not defined or if the initial state estimate is from Geant.
    Double_t    errX;              // Error of track state x-position in sector coordinates and mm.
    Double_t    errY;              // Error of track state y-position in sector coordinates and mm.
    Double_t    errTx;             // Error of track state Tan(p_x/p_z) in sector coordinates and radians. Will only be used if fTxErr is not defined or if the initial state estimate is from Geant.
    Double_t    errTy;             // Error of track state Tan(p_y/p_z) in sector coordinates and radians. Will only be used if fTyErr is not defined or if the initial state estimate is from Geant.

    Int_t       counterstep;       // Print out after fitting this amount of events.
    Bool_t      bPrintWarn;        // Print/suppress warnings during tracking.
    Bool_t      bPrintErr;         // Print/suppress error messages during tracking.

    Int_t       refId;             // Runtime reference ID.

    Int_t       (*fGetPid)(HMdcTrkCand*); //! Function for initial PID hypothesis.

protected:

    virtual Bool_t       createKalSystem();

    virtual void         fillData       (HMetaMatch2* const pMetaMatch, const TObjArray &allhitsGeantMdc,
					 HMdcSeg* const inSeg, HMdcSeg* const outSeg);

    virtual void         fillDataSites  (Int_t &iFirstSite, Int_t &iLastSite,
                                         const TObjArray &allhitsGeantMdc);

    virtual HKalTrack*   fillDataTrack(Int_t &iKalTrack, HMetaMatch2* const pMetaMatch,
                                       Int_t iCatSiteFist, Int_t iCatSiteLast,
                                       HMdcSeg* const inSeg, HMdcSeg* const outSeg,
				       const HKalMetaMatch &kalMatch) const;

public:

    HKalFilterTask(Int_t refid, Bool_t sim=kFALSE, Bool_t wire=kFALSE,
		   Bool_t daf=kFALSE, Bool_t comp=kFALSE);

    ~HKalFilterTask();

    virtual Int_t        getIniPid   (HMdcTrkCand const* const cand) const;

    virtual Bool_t       getIniSv    (TVectorD &inisv, TMatrixD &iniC,
				      Int_t pid, const TObjArray &allhits,
				      HMetaMatch2 const* const pMetaMatch,
				      HMdcSeg const* const inSeg,
				      HMdcSeg const* const outSeg) const;

    virtual Bool_t       getIniSvGeant(TVectorD &inisv, TMatrixD &iniC,
				       const TObjArray &allhitsGeantMdc,
				       Int_t pid, Bool_t bSmear) const;

    virtual Bool_t       getIniSvRungeKutta(TVectorD &inisv, TMatrixD &iniC, Int_t pid,
				      const TObjArray& allhits,
				      HMetaMatch2 const* const pMetaMatch) const;

    virtual Bool_t       doPid       (Int_t pid);

    virtual Int_t        execute     (void);

    virtual Bool_t       init        (void);

    virtual Bool_t       reinit      (void);

    virtual Bool_t       finalize    (void);

    virtual HMdcTrkCand* nextTrack   (TObjArray& allhits);

    virtual Double_t             getChi2         () const            { return kalsys->getChi2(); }

    virtual void                 getCovErrs      (Int_t &nSymErrs, Int_t &nPosDefErrs) const { kalsys->getCovErrs(nSymErrs, nPosDefErrs); }

    virtual Double_t             getFieldIntegral() const            { return kalsys->getFieldIntegral(); }

    virtual Int_t                getHitType      () const            { return kalsys->getHitType(); }

    virtual Int_t                getNhits        () const            { return kalsys->getNhits(); }

    virtual Double_t             getNdf          () const            { return kalsys->getNdf(); }

    virtual Int_t                getNsites       () const            { return kalsys->getNsites(); }

    virtual Int_t                getMeasDim      () const            { return kalsys->getMeasDim(); }

    virtual TObjArray const&     getPointsTrack  () const            { return kalsys->getPointsTrack(); }

    virtual TObjArray const&     getFieldTrack   () const            { return kalsys->getFieldTrack(); }

    virtual const HKalIFilt*       getKalSys         () const            { return kalsys; }
    virtual const HKalInput*       getKalInput       () const            { return input; }
    virtual const HKalMetaMatcher* getKalMetaMatcher () const            { return &metaMatcher; }

    virtual Int_t                getStateDim     (Kalman::coordSys coord=kSecCoord) const { return kalsys->getStateDim(coord); }

    virtual Double_t             getTrackLength  () const            { return kalsys->getTrackLength(); }

    virtual Int_t                getTrackNum     () const            { return kalsys->getTrackNum(); }

    virtual void   setConstField      (Bool_t constField)                       { (kalsys) ?   kalsys->setConstField(constField) : Warning("setConstField()","%s", noKalman.Data()); }

    virtual void   setCounterStep     (Int_t  c)                                { counterstep = c; }

    virtual void   setDafPars         (Double_t chi2cut, const Double_t *T, Int_t n) { kalsys->setDafPars(chi2cut, &T[0], n); }

    virtual void   setDirection       (Bool_t dir)                              { (kalsys) ? kalsys->setDirection(dir)          : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setDoEnerLoss      (Bool_t dedx)                             { (kalsys) ? kalsys->setDoEnerLoss(dedx)        : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setDoMultScat      (Bool_t ms)                               { (kalsys) ? kalsys->setDoMultScat(ms)          : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setDoPid           (Int_t pid[], Int_t n);

    virtual void   setErrors          (Double_t dx, Double_t dy, Double_t dtx, Double_t dty, Double_t dmom);

    virtual void   setFillPointsArrays(Bool_t fill)                             { (kalsys) ? kalsys->setFillPointsArrays(fill)  : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setFillSites       (Bool_t fill)                             { bFillSites = fill; }

    virtual void   setFieldMap        (HMdcTrackGField *fMap, Double_t scale)   { (kalsys) ? kalsys->setFieldMap(fMap, scale)   : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setFieldVector     (const TVector3 &B)                       { (kalsys) ? kalsys->setFieldVector(B)          : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setFilterMethod    (Kalman::filtMethod type)                 { (kalsys) ? kalsys->setFilterMethod(type)      : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual Bool_t setFilterPars      (Bool_t wire, Bool_t daf, Bool_t comp);

    virtual void   setFuncMomErr      (TF1* f)                                  { fMomErr = f; }

    virtual void   setFuncTxErr       (TF1* f)                                  { fTxErr = f; }

    virtual void   setFunKyErr        (TF1* f)                                  { fTyErr = f; }

    virtual void   setIniStateMethod  (Int_t value) { iniSvMethod = value; }

    virtual void   setMdcMaterials    (TObjArray *mats) { materials = mats; }

    virtual void   setNumIterations   (Int_t kalRuns)                           { (kalsys) ? kalsys->setNumIterations(kalRuns)  : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setPid             (Int_t (*fPid)(HMdcTrkCand*)) { fGetPid = fPid; }

    virtual void   setRotation        (Kalman::kalRotateOptions rotate)         { (kalsys) ? kalsys->setRotation(rotate)        : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setRungeKuttaParams(Float_t initialStpSize,
				       Float_t stpSizeDec, Float_t stpSizeInc,
				       Float_t maxStpSize, Float_t minStpSize,
				       Float_t minPrec, Float_t maxPrec,
				       Int_t maxNumStps, Int_t maxNumStpsPCA,
				       Float_t maxDst, Double_t minLngthCalcQ) {
	(kalsys) ? kalsys->setRungeKuttaParams(initialStpSize, stpSizeDec, stpSizeInc, maxStpSize, minStpSize, minPrec, maxPrec, maxNumStps, maxNumStpsPCA, maxDst, minLngthCalcQ)       : Warning("setRungeKuttaParams()","%s",  noKalman.Data()); }

    virtual void   setSmoothing       (Bool_t smooth)                           { (kalsys) ? kalsys->setSmoothing(smooth)       : Warning("setConstField()","%s",  noKalman.Data()); }

    virtual void   setUseGeantPid     (Bool_t geant)                            { bGeantPid = geant; }

    virtual void   setVerbose         (Int_t v);

    ClassDef(HKalFilterTask, 0);
};
#endif
