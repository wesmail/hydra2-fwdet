#ifndef HKalFiltWire_h
#define HKalFiltWire_h

// from hydra
class HCategory;
class HMdcTrackGField;

#include "hkalifilt.h"


class HKalFiltWire : public HKalIFilt {

protected:

    virtual Bool_t calcDriftDist        (Double_t &dist, Double_t &distErr,
                                         Double_t &alpha, Int_t iSite, Int_t iHit=0) const;

    virtual Bool_t calcProjector        (Int_t iSite) const;

public:

    HKalFiltWire(Int_t nHits, Int_t measDim, Int_t stateDim, HMdcTrackGField *fMap, Double_t fpol);

    virtual ~HKalFiltWire() {}

    virtual Bool_t calcMeasVecFromState(TVectorD &projMeasVec, HKalTrackSite const* const site,
                                        Kalman::kalFilterTypes stateType, Kalman::coordSys sys,
                                        Int_t iHit=0) const;

    virtual Bool_t filter              (Int_t iSite);

    virtual Bool_t getImpact           (Double_t& alpha, Double_t& mindist,
                                        const TVector3 &pos, const TVector3 dir,
                                        Int_t sec, Int_t mod, Int_t lay, Int_t cell) const;

    ClassDef(HKalFiltWire,0)
};

#endif // HKalFiltWire_h

