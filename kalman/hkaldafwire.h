#ifndef HKalDafWire_h
#define HKalDafWire_h

#include "hkaldafsinglewire.h"

class HKalDafWire : public HKalDafSingleWire {

protected:

    virtual Bool_t   calcEffErrMat        (Int_t iSite) const;

    virtual Bool_t   calcEffMeasVec       (Int_t iSite) const;

    virtual Bool_t   calcProjector        (Int_t iSite) const;

    virtual Bool_t   calcVirtPlane        (Int_t iSite) const;

    virtual Kalman::coordSys    getFilterInCoordSys() const { return Kalman::kLayCoord; }

    virtual Double_t getEffMeasVecSign    (const TVectorD &driftCentShift) const;

public:

    HKalDafWire(Int_t nHits, Int_t measDim, Int_t stateDim, HMdcTrackGField *fMap, Double_t fpol);

    virtual ~HKalDafWire() {}

    virtual Bool_t calcMeasVecFromState(TVectorD &projMeasVec, HKalTrackSite const* const site,
					Kalman::kalFilterTypes stateType, Kalman::coordSys sys,
					Int_t iHit=0) const;

    virtual Bool_t filter              (Int_t iSite);

    virtual Bool_t propagate           (Int_t iFromSite, Int_t iToSite);

    virtual void   updateSites         (const TObjArray &hits);

    virtual TMatrixD const&     getHitErrMat    (HKalTrackSite* const site) const;

    virtual TVectorD const&     getHitVec       (HKalTrackSite* const site) const;

    ClassDef(HKalDafWire,0)
};

#endif // HKalDafWire_h

