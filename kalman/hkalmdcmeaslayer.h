#ifndef HKalMdcMeasLayer_h
#define HKalMdcMeasLayer_h

// from ROOT
class TVector3;

// from kalman
#include "hkalmixture.h"
#include "hkalplane.h"


class HKalMdcMeasLayer : public HKalPlane {

private:
    HKalMixture *materialInside;   //! Material inside the layer.
    HKalMixture *materialOutside;  //! Material outside the layer.
    Int_t module;                  // Module number of the Hades detector where the layer is located.
    Int_t sector;                  // Sector number of the Hades detector where the layer is located.
    Int_t layer;                   // Layer number in the sector and module.
    Int_t nLayersInModule;         // Number of layers in this module.
    Double_t thickness;            // Thickness of the mdc layer, i.e. the distance between the two cathodes.

public:

    HKalMdcMeasLayer() {}

    HKalMdcMeasLayer(const TVector3 &center, const TVector3 &normal, const HKalMixture *matIn, const HKalMixture *matOut,
            Int_t mod, Int_t sec, Int_t lay, Int_t nLayMod, Double_t catDist);

    HKalMdcMeasLayer(const TVector3 &origin, const TVector3 &u, const TVector3 &v, const HKalMixture *matIn, const HKalMixture *matOut,
            Int_t mod, Int_t sec, Int_t lay, Int_t nLayMod, Double_t catDist);

    virtual ~HKalMdcMeasLayer();

    virtual Bool_t   isInside                (const TVector3 &pos) const;

    virtual Bool_t   isInsideMod             (const TVector3 &point) const;

    virtual void     print                   (Option_t* opt="") const;

    virtual Float_t  getA                    (Bool_t inside=kTRUE) const;

    virtual Float_t  getZ                    (Bool_t inside=kTRUE) const;

    virtual Float_t  getDensity              (Bool_t inside=kTRUE) const;

    virtual Double_t getDistToModBound       (Bool_t dir) const;

    virtual Float_t  getRadLength            (Bool_t inside=kTRUE) const;

    virtual Float_t  getExcitationEnergy     (Bool_t inside=kTRUE) const;

    virtual const HKalMixture* getMaterialPtr(Bool_t inside=kTRUE) const;

    virtual Int_t    getModule               () const                      { return module; }

    virtual Int_t    getSector               () const                      { return sector; }

    virtual Int_t    getLayer                () const                      { return layer; }

    virtual Double_t getThickness            () const                      { return thickness; }

    ClassDef(HKalMdcMeasLayer, 0)
};


#endif // HKalMdcMeasLayer_h
