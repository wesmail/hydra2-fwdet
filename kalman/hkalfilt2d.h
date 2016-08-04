#ifndef HKalFilt2d_h
#define HKalFilt2d_h

// from hydra
class HCategory;
class HMdcTrackGField;

#include "hkalifilt.h"


class HKalFilt2d : public HKalIFilt {

protected:
    virtual Bool_t   calcProjector             (Int_t iSite) const;

public:

    HKalFilt2d(Int_t nHits, Int_t measDim, Int_t stateDim, HMdcTrackGField *fMap, Double_t fpol);

    virtual ~HKalFilt2d() { ; }

    virtual Bool_t   calcMeasVecFromState      (TVectorD &projMeasVec, HKalTrackSite const* const site,
						Kalman::kalFilterTypes stateType, Kalman::coordSys sys,
						Int_t iHit=0) const;

    ClassDef(HKalFilt2d,0)
};

#endif // HKalFilt2d_h

