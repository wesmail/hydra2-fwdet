#ifndef HKalDafWire_h
#define HKalDafWire_h

#include "hkaldafsinglewire.h"

class HKalDafWire : public HKalDafSingleWire {

protected:

    virtual Bool_t   calcProjector        (Int_t iSite) const;

    virtual Bool_t   calcVirtPlane        (Int_t iSite) const;

    virtual Kalman::coordSys    getFilterInCoordSys() const { return Kalman::kLayCoord; }

public:

    HKalDafWire(Int_t nHits, Int_t measDim, Int_t stateDim, HMdcTrackGField *fMap, Double_t fpol);

    virtual ~HKalDafWire() {}

    virtual Bool_t calcMeasVecFromState(TVectorD &projMeasVec, HKalTrackSite const* const site,
					Kalman::kalFilterTypes stateType, Kalman::coordSys sys) const;

    virtual Bool_t filter              (Int_t iSite);

    virtual Bool_t propagate           (Int_t iFromSite, Int_t iToSite);

    virtual void   updateSites         (const TObjArray &hits);

    ClassDef(HKalDafWire,0)
};

#endif // HKalDafWire_h

