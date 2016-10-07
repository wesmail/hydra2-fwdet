#ifndef HKalTrackSite_h
#define HKalTrackSite_h

// from ROOT
#include "TMatrixD.h"
#include "TObjArray.h"
class     TVector3;
#include "TVectorD.h"

// from kalman
#include "hkaldef.h"
#include "hkalmdchit.h"
class     HKalMdcMeasLayer;
#include "hkaltrackstate.h"

#include <iostream>
using namespace std;
#include <cstdlib>

class HKalTrackSite : public TObject {

private:

    Bool_t            bActive;      // Site is active/not active.
    Double_t          chi2Inc;      // This site's contribution to chi^2 of the fit.
    Double_t          energyLoss;   // Energy loss from the first site to this site in MeV/particle charge in elementary charges. Sign depends on propagation direction. This number is positive for forward and negative for backward iteraton.
    TObjArray        *hits;         //! Stores all competing measurement hits.
    TMatrixD          effErrMat;    //! Weighted error matrix of all competing hits in this site.
    TVectorD          effMeasVec;   //! Weighted measurement vector  of all competing hits in this site.
    HKalTrackState ***trackStates;  //! array with the track parameters for each track state in the sector coordinate system.

private:

    virtual HKalMdcHit* getHitPtr(Int_t idx=0);

    virtual void transformHit   (const TRotation &transMat);

    virtual void transformStates(const TRotation &transMat);

public:

    HKalTrackSite(Int_t measDim, Int_t stateDim, Int_t stateDimVirtLay=5);

    virtual ~HKalTrackSite();

    virtual void addHit               (HKalMdcHit *newhit);

    virtual void calcJacLayToSec      (TMatrixD &jac, const TVectorD &svLay, const HKalPlane &plane) const;

    virtual void calcJacSecToLay      (TMatrixD &jac, const TVectorD &svSec, const HKalPlane &plane) const;

    virtual void clearHits            ();

    virtual void print                (const Option_t *opt="") const;

    virtual void sortHits             ();

    virtual void transform            (const TRotation &transMat);

    virtual void transVirtLayToSec    (Kalman::kalFilterTypes stateType, Int_t iHit, Bool_t bCovUD=kFALSE);

    virtual void transSecToVirtLay    (Kalman::kalFilterTypes stateType, Int_t iHit, Bool_t bCovUD=kFALSE);

    virtual Int_t             getCell               (Int_t idx=0) const                 { return getHit(idx).getCell(); }

    virtual Double_t          getChi2               () const                            { return chi2Inc; }

    virtual Double_t          getEnergyLoss         () const                            { return energyLoss; }

    virtual TMatrixD   const& getEffErrMat          () const                            { return effErrMat; }

    virtual TVectorD   const& getEffMeasVec         () const                            { return effMeasVec; }

    virtual TMatrixD   const& getErrMat             (Int_t idx=0) const;

    virtual TVectorD   const& getErrVec             (Int_t idx=0) const                 { return getHit(idx).getErrVec(); }

    virtual HKalMdcHit const& getHit                (Int_t idx=0) const;

    virtual Double_t          getHitChi2            (Int_t idx=0) const                 { return getHit(idx).getChi2(); }

    virtual Double_t          getHitDriftTime       (Int_t idx=0) const                 { return getHit(idx).getDriftTime(); }

    virtual Double_t          getHitDriftTimeErr    (Int_t idx=0) const                 { return getHit(idx).getDriftTimeErr(); }

    virtual Float_t           getHitImpactAngle     (Int_t idx=0) const                 { return getHit(idx).getImpactAngle(); }

    virtual HKalMdcMeasLayer const& getHitMeasLayer () const                            { return getHit().getMeasLayer(); }

    virtual TVectorD   const& getHitVec             (Int_t idx=0) const;

    virtual void              getHitVec3            (TVector3 &hit3, Int_t idx=0) const { getHit(idx).getHitVec3(hit3); }

    virtual Double_t          getHitTimeTof         (Int_t idx=0) const                 { return getHit(idx).getTimeTof(); }

    virtual Double_t          getHitsTotalWeight    () const;

    virtual Int_t             getHitType            () const                            { return getHit().getHitType(); }

    virtual Double_t          getHitWeight          (Int_t idx=0) const                 { return getHit(idx).getWeight(); }

    virtual Double_t          getHitWeightHist      (Int_t iDaf, Int_t idx=0) const     { return getHit(idx).getWeightHist(iDaf); }

    virtual void              getHitWirePts         (TVector3 &wire1, TVector3 &wire2, Int_t idx=0) const { getHit(idx).getWirePts(wire1, wire2); }

    virtual HKalPlane  const& getHitVirtPlane       (Int_t idx=0) const                 { return getHit(idx).getVirtPlane(); }

    virtual Bool_t            getIsActive           () const                            { return bActive; }

    virtual Int_t             getLayer              (Int_t idx=0) const                 { return getHit(idx).getLayer(); }

    virtual HKalTrackState const& getState          (Kalman::kalFilterTypes stateType, Kalman::coordSys coord=kSecCoord) const { return *trackStates[coord][stateType]; }

    virtual Int_t             getMeasDim            () const                            { return getHit().getDimension(); }

    virtual Int_t             getModule             (Int_t idx=0) const                 { return getHit(idx).getModule(); }

    virtual Int_t             getNcompetitors       () const                            { return hits->GetEntries(); }

    virtual void              getPosAndDirFromState (TVector3 &pos, TVector3 &dir, kalFilterTypes stateType) const;

    virtual Int_t             getSector             (Int_t idx=0) const                 { return getHit(idx).getSector(); }

    virtual Int_t             getStateDim           (Kalman::coordSys coord=kSecCoord) const       { return trackStates[coord][kPredicted]->getStateDim(); }

    virtual Double_t          getStateParam         (Kalman::kalFilterTypes stateType, Kalman::kalStateIdx par, Kalman::coordSys coord=kSecCoord) const { return trackStates[coord][stateType]->getStateParam(par); }

    virtual TVectorD   const& getStateVec           (Kalman::kalFilterTypes stateType, Kalman::coordSys coord=kSecCoord) const               { return trackStates[coord][stateType]->getStateVec(); }

    virtual TMatrixD   const& getStateCovMat        (Kalman::kalFilterTypes stateType, Kalman::coordSys coord=kSecCoord) const               { return trackStates[coord][stateType]->getCovMat(); }

    virtual TMatrixD   const& getStatePropMat       (Kalman::kalFilterTypes stateType, Kalman::coordSys coord=kSecCoord) const               { return trackStates[coord][stateType]->getPropMat(); }

    virtual TMatrixD   const& getStateProjMat       (Kalman::kalFilterTypes stateType=kFiltered, Kalman::coordSys coord=kSecCoord) const     { return trackStates[coord][stateType]->getProjMat(); }

    virtual TMatrixD   const& getStateProcMat       (Kalman::kalFilterTypes stateType, Kalman::coordSys coord=kSecCoord) const               { return trackStates[coord][stateType]->getProcNoiseMat(); }

    virtual void      setActive           (Bool_t active)                                             { bActive = active; }

    virtual void      setChi2             (Double_t c)                                                { chi2Inc = c; }

    virtual void      setEffErrMat        (const TMatrixD &errMat);

    virtual void      setEffMeasVec       (const TVectorD &measVec);

    virtual void      setEnergyLoss       (Double_t eloss)                                            { energyLoss = eloss; }

    virtual void      setHitAndErr        (const TVectorD &newhit, const TVectorD &newerr, Int_t idx=0) { getHitPtr(idx)->setHitAndErr(newhit, newerr); }

    virtual void      setHitChi2          (Double_t chi2, Int_t idx=0)                                { getHitPtr(idx)->setChi2(chi2); }

    virtual void      setHitDriftTime     (Double_t time, Double_t timeErr, Int_t idx=0)              { getHitPtr(idx)->setDriftTime(time, timeErr); }

    virtual void      setHitImpactAngle   (Float_t alpha, Int_t idx=0)                                { getHitPtr(idx)->setImpactAngle(alpha); }

    virtual Bool_t    setHitVirtPlane     (const TVector3 & origin, const TVector3 &n, Int_t idx=0)   { return getHitPtr(idx)->setVirtPlane(origin, n); }

    virtual Bool_t    setHitVirtPlane     (const TVector3 & origin, const TVector3 &u, const TVector3 &v, Int_t idx=0) { return getHitPtr(idx)->setVirtPlane(origin, u, v); }

    virtual void      setHitWeight        (Double_t weight, Int_t idx=0)                              { getHitPtr(idx)->setWeight(weight); }

    virtual void      setHitWeightHist    (Double_t weight, Int_t iDaf, Int_t idx=0)                  { getHitPtr(idx)->setWeightHist(weight, iDaf); }

    virtual void      setNdafs            (Int_t n);

    virtual void      setStateCovMat      (Kalman::kalFilterTypes stateType, const TMatrixD &fCov,  Kalman::coordSys coord=kSecCoord)   { trackStates[coord][stateType]->setCovMat(fCov); }

    virtual void      setStatePropMat     (Kalman::kalFilterTypes stateType, const TMatrixD &fProp, Kalman::coordSys coord=kSecCoord)   { trackStates[coord][stateType]->setPropMat(fProp); }

    virtual void      setStateProjMat     (Kalman::kalFilterTypes stateType, const TMatrixD &fProj, Kalman::coordSys coord=kSecCoord)   { trackStates[coord][stateType]->setProjMat(fProj); }

    virtual void      setStateProcMat     (Kalman::kalFilterTypes stateType, const TMatrixD &fProc, Kalman::coordSys coord=kSecCoord)   { trackStates[coord][stateType]->setProcNoiseMat(fProc); }

    virtual void      setStateVec         (Kalman::kalFilterTypes stateType, const TVectorD &sv,    Kalman::coordSys coord=kSecCoord)   { trackStates[coord][stateType]->setStateVec(sv); }

    ClassDef(HKalTrackSite, 0)
};

#endif // HKalTrackSite_h
