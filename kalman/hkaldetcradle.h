#ifndef HKalDetCradle_h
#define HKalDetCradle_h

// from ROOT
#include "TObjArray.h"

// from hydra
class HMdcSizesCells;

class HKalMdcMeasLayer;
class HKalMixture;


class HKalDetCradle : public TObjArray {

private:

    static const Int_t kNumMdcMats = 5; // Number of materials in an mdc layer.
    static const Int_t kNumMdcs    = 4; // Number of mdc modules.
    static const Int_t kNumSecs    = 6; // Number of mdc sectors in a module.

    Int_t nLayersInMdc;       // Number of layers in each Mdc.

    HKalMixture  *matAir;     // Air. Material between Mdc's.
    HKalMixture **matsMdc;    // Properties of  material mixture for an mdc layer.

public:
    enum mdcComponent { // Index for the different components of an mdc layer.
        kCatWi   = 0,
        kFieldWi = 1,
        kSenseWi = 2,
        kWindow  = 3,
        kGas     = 4
    };

public:

    HKalDetCradle();

    HKalDetCradle(Int_t nLayInMdc, const TObjArray *customMats=NULL);

    virtual ~HKalDetCradle();

    virtual void         addMdcLayersToCradle     (const HMdcSizesCells *fSizesCells, Int_t sector, Int_t module, Int_t nLayers);

    virtual Int_t        getLayersInMdc           () const        { return nLayersInMdc; }

    virtual const HKalMdcMeasLayer* getMdcLayerAt (Int_t sector, Int_t module, Int_t layer=0) const;

    virtual HKalMixture* getMdcMaterial           (mdcComponent comp, Int_t module) const;

    virtual void         getMdcMaterialVolumeFracs(Float_t volFracs[kNumMdcMats], Int_t sector, Int_t module, Int_t layer) const;

    static  void         getProperties            (Float_t mat[kNumMdcMats], const Char_t *id);

    virtual void         initMdcMaterials         ();

    virtual void         install                  (HKalMdcMeasLayer *layer);

    virtual Bool_t       layOk                    (Int_t l) const { return (l>=0 && l<nLayersInMdc); }

    virtual Bool_t       modOk                    (Int_t m) const { return m>=0 && m<kNumMdcs; }

    virtual Bool_t       secOk                    (Int_t s) const { return s>=0 && s<kNumSecs; }

    ClassDef(HKalDetCradle, 0)
};

#endif // HKalDetCradle_h
