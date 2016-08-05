#ifndef HKalMdcHit_h
#define HKalMdcHit_h

// from ROOT
#include "TArrayD.h"
#include "TMatrixD.h"
class TRotation;
class TVector3;
#include "TVectorD.h"

// from hydra
#include "hkaldef.h"
#include "hkalmdcmeaslayer.h"

#include <iostream>
using namespace std;
#include <cstdlib>

class HKalMdcHit : public TObject {

private:

    TVectorD hitVec;             //  Measurement vector.
    TVector3 hitVec3;            //! 3 dimensional hit vector in sector coordinates.
    TVector3 wirePoint1;         //! Wire geometry.
    TVector3 wirePoint2;         //! Wire geometry.
    TVectorD errVec;             //! Error vector.
    TMatrixD fErr;               //  Error matrix.

    Double_t driftTime;          // Drift time. Used if this is a wire hit.
    Double_t driftTimeErr;       // Error of drift time. Used if this is a wire hit.

    // For writing to data container HKalHitWire only.
    // ------
    Float_t angle;       // Impact angle of reconstructed track with drift cell.
    Float_t t1;          // Drift time of first hit in ns.
    Float_t t2;          // Drift time of second hit in ns.
    Float_t tTof;        // Time of flight correction of time 1.
    Float_t tTofCal1;    // Time of flight correction of time 1 from HMdcCal1Sim.
    Float_t tWireOffset; // Travel time on wire correction of time 1.
    // ------

    HKalMdcMeasLayer *measLayer; //! Measurement layer where hit was detected.
    HKalPlane virtLayer;

    Double_t chi2;            // Distance of the observation to the smoothed track state. (Used for competing hits.)
    Double_t weight;          // Weight of the measurement compared to its competitors. (Used for competing hits.)
    TArrayD  weightsHist;     // Weight of the measurement compared to its competitors during each iteration of the DAF. (Used for competing hits.)

    Int_t    driftCell;       // Drift cell number on measurement layer.
    Int_t    hitType;         // Segment/wire hit.

protected:

    virtual void              setHitVec(const TVector3 &newhit);

    virtual void              setHitVec(const TVectorD &newhit);

public:

    HKalMdcHit(Int_t dim);

    HKalMdcHit(Double_t *hit, Double_t *err, const HKalMdcMeasLayer &layer, Int_t dim,
	       const TVector3 &impact, Kalman::kalHitTypes type, Double_t w=1., Int_t cell=-1);

    HKalMdcHit(const HKalMdcHit &hit);

    virtual ~HKalMdcHit();

    static  Bool_t areCompetitors(const HKalMdcHit &hit1, const HKalMdcHit &hit2);

    virtual Int_t  Compare       (const TObject *obj) const;

    virtual void   print         (const Option_t *opt="") const;

    virtual void   setHitAndErr  (const TVector3 &newhit, const TVector3 &newerr);

    virtual void   setHitAndErr  (const TVectorD &newhit, const TVectorD &newerr);

    virtual void   transformHit  (const TRotation &transMat);

    virtual void   transformLayer(const TRotation &transMat);

    virtual Int_t             getCell           () const { return driftCell; }

    virtual Double_t          getChi2           () const { return chi2;   }

    virtual Int_t             getDimension      () const { return hitVec.GetNrows(); }

    virtual Double_t          getDriftTime      () const { return driftTime; }

    virtual Double_t          getDriftTimeErr   () const { return driftTimeErr; }

    virtual TMatrixD const&   getErrMat         () const { return fErr; }

    virtual TVectorD const&   getErrVec         () const { return errVec; }

    virtual TVectorD const&   getHitVec         () const {  return hitVec; }

    virtual void              getHitVec3        (TVector3 &hit3) const;

    virtual Int_t             getHitType        () const { return hitType; }

    virtual Float_t           getImpactAngle    () const { return angle; }

    virtual Int_t             getLayer          () const { return measLayer->getLayer(); }

    virtual Int_t             getModule         () const { return measLayer->getModule(); }

    virtual Int_t             getSector         () const { return measLayer->getSector(); }

    virtual Float_t           getTime1          () const { return t1; }

    virtual Float_t           getTime2          () const { return t2; }

    virtual Float_t           getTimeTof        () const { return tTof; }

    virtual Float_t           getTimeTofCal1    () const { return tTofCal1; }

    virtual Float_t           getTimeWireOffset () const { return tWireOffset; }

    virtual Double_t          getWeight         () const { return weight; }

    virtual Double_t          getWeightHist     (Int_t iDaf) const { if(iDaf >= 0 && iDaf < weightsHist.GetSize()) weightsHist[iDaf]; return 0.; }

    virtual void              getWirePts        (TVector3 &wire1, TVector3 &wire2) const;

    virtual HKalMdcMeasLayer const& getMeasLayer() const { if(!measLayer) { Warning("getMeasLayer()", "Measurement layer of hit has not been set"); } return *measLayer; }

    virtual HKalPlane const&  getVirtPlane      () const { return virtLayer; }

    virtual Bool_t            IsSortable        () const { return kTRUE; }

    virtual void              setChi2           (Double_t chi)              { chi2 = chi; }

    virtual void              setDriftTime      (Double_t t, Double_t tErr) { driftTime = t; driftTimeErr = tErr;}

    virtual void              setImpactAngle    (Float_t a)                 { angle = a; }

    virtual void              setNdafs          (Int_t n)                   { weightsHist.Set(n); }

    virtual void              setTime1          (Float_t t)                 { t1 = t; }

    virtual void              setTime2          (Float_t t)                 { t2 = t; }

    virtual void              setTimeTof        (Float_t t)                 { tTof = t; }

    virtual void              setTimeTofCal1    (Float_t t)    { tTofCal1 = t; }

    virtual void              setTimeWireOffset (Float_t t)                 { tWireOffset = t; }

    virtual Bool_t            setVirtPlane      (const TVector3 &origin, const TVector3 &n) { return virtLayer.setPlane(origin, n); }

    virtual Bool_t            setVirtPlane      (const TVector3 &origin, const TVector3 &u, const TVector3 &v) { return virtLayer.setPlane(origin, u, v); }

    virtual void              setWeight         (Double_t w)                { weight = w; }

    virtual void              setWeightHist     (Double_t w, Int_t i)       { if(i >= 0 && i<weightsHist.GetSize()) weightsHist[i] = w; }

    ClassDef(HKalMdcHit, 0)
};

#endif // HKalMdcHit_h
