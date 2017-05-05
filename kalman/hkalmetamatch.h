#ifndef HKalMetaMatch_h
#define HKalMetaMatch_h

// from ROOT
#include "TVector3.h"

class HKalMetaMatch : public TObject {

private:
    Float_t  beta;          // Relativistic velocity.
    Int_t    ind;           // Index of RPC hit, shower hit, emc hit or tof cluster.
    Float_t  mass2;         // Squared mass.
    Float_t  metaEloss;     // Energy loss from META detector.
    TVector3 metaHit;       // Hit in META detector in sector coordinates.
    TVector3 metaHitLocal;  // Hit in META detector in detector specific coordinates.
    TVector3 metaReco;      // Reconstructed META hit in sector coordinates.
    TVector3 metaRecoLocal; // Reconstructed META hit in detector specific coordinates.
    Int_t    metaSys;       // 0 = Rpc, 1 = Shower, 2 = Tof, 3 = Emc
    Float_t  quality;       // Quality of META match.
    Float_t  tof;           // Time-of-flight measurement for RPC/TOF hits in ns.
    Float_t  trackLength;   // Length of reconstructed track in mm.

public:

    HKalMetaMatch();

    virtual ~HKalMetaMatch() { ; }

    virtual void      clear            ();

    virtual Bool_t    isFilled         () const { return (ind >= 0); }

    virtual Float_t   getBeta          () const { return beta; }

    virtual Int_t     getIndex         () const { return ind; }

    virtual Float_t   getMass2         () const { return mass2; }

    virtual Float_t   getMetaEloss     () const { return metaEloss; }

    virtual const TVector3& getMetaHit () const { return metaHit; }

    virtual void      getMetaHit       (Float_t &x, Float_t &y, Float_t &z) const { x = metaHit.X(), y = metaHit.Y(), z = metaHit.Z(); }

    virtual void      getMetaHitLocal  (Float_t &x, Float_t &y, Float_t &z) const { x = metaHitLocal.X(), y = metaHitLocal.Y(), z = metaHitLocal.Z(); }

    virtual const TVector3& getMetaReco() const { return metaReco; }

    virtual void      getMetaReco      (Float_t &x, Float_t &y, Float_t &z) const { x = metaReco.X(), y = metaReco.Y(), z = metaReco.Z(); }

    virtual void      getMetaRecoLocal (Float_t &x, Float_t &y, Float_t &z) const { x = metaRecoLocal.X(), y = metaRecoLocal.Y(), z = metaRecoLocal.Z(); }

    virtual Int_t     getMetaSys       () const { return metaSys; }

    virtual Float_t   getQuality       () const { return quality; }

    virtual Int_t     getSystem        () const { return metaSys; }

    virtual Float_t   getTof           () const { return tof; }

    virtual Float_t   getTrackLength   () const { return trackLength; }

    virtual void      setBeta          (Float_t b)  { beta = b; }

    virtual void      setIndex         (Short_t i)  { ind = i; }

    virtual void      setMass2         (Float_t m2) { mass2 = m2; }

    virtual void      setMetaEloss     (Float_t de) { metaEloss = de; }

    virtual void      setMetaHit       (Float_t x, Float_t y, Float_t z) { metaHit.SetXYZ(x, y, z); }

    virtual void      setMetaHitLocal  (Float_t x, Float_t y, Float_t z) { metaHitLocal.SetXYZ(x, y, z); }

    virtual void      setMetaReco      (Float_t x, Float_t y, Float_t z) { metaReco.SetXYZ(x, y, z); }

    virtual void      setMetaRecoLocal (Float_t x, Float_t y, Float_t z) { metaRecoLocal.SetXYZ(x, y, z); }

    virtual void      setSystem        (Int_t sys)  { metaSys = sys; }

    virtual void      setQuality       (Float_t q)  { quality = q; }

    virtual void      setTof           (Float_t t)  { tof = t; }

    virtual void      setTrackLength   (Float_t l)  { trackLength = l; }

    ClassDef(HKalMetaMatch, 0)
};

#endif // HKalMetaMatch_h
