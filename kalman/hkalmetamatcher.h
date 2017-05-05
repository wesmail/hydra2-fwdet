#ifndef HKalMetaMatcher_h
#define HKalMetaMatcher_h

// from ROOT
class     TVector3;
#include "TMath.h"

// from hydra
class HCategory;
class HGeomVector;
class HMetaMatchPar;
class HRpcGeomPar;
class HShowerGeometry;
class HEmcGeomPar;
class HTofGeomPar;
#include "hgeomtransform.h"
#include "hmetamatch2.h"

#include "hkalifilt.h"
#include "hkalmetamatch.h"

class HKalMetaMatcher : public TObject {

private:
    static const Int_t nMetaTabs   = META_TAB_SIZE; // Number of meta matches that are stored for each system.
    static const Int_t tofClusterSize = 3;          // For tof detector store hit 1, hit 2 and cluster separately.

    Bool_t           bIsMatched;

    HCategory*       fCatShower;      // pointer to the Shower category
    HCategory*       fCatEmcCluster;  // pointer to the Emc cluster category
    HCategory*       fCatTof;         // pointer to the Tof hit category
    HCategory*       fCatTofCluster;  // pointer to the Tof cluster category
    HCategory*       fCatRpcCluster;  // pointer to the Rpc cluster category.

    HRpcGeomPar*     fRpcGeometry;    // Rpc geometry
    HShowerGeometry* fShowerGeometry; // Shower geometry
    HEmcGeomPar*     fEmcGeometry;    // Emc geometry
    HTofGeomPar*     fTofGeometry;    // TOF geometry

    HMetaMatchPar*   fMatchPar;

    HKalMetaMatch    matchesRpc   [nMetaTabs];
    HKalMetaMatch    matchesShower[nMetaTabs];
    HKalMetaMatch    matchesEmc   [nMetaTabs];
    HKalMetaMatch    matchesTof   [nMetaTabs][tofClusterSize];

    HGeomTransform   labSecTrans[6];       //
    HGeomTransform   modSecTransRpc[6];    //
    HGeomTransform   modSecTransShower[6]; //
    HGeomTransform   modSecTransEmc[6]; //
    HGeomTransform   modSecTransTof[6][8]; //

    TVector3         normVecRpc[6];    // normal vector on the RPC module in the sector coordinate system
    TVector3         normVecShower[6]; // normal vector on the Shower module in the sector coordinate system
    TVector3         normVecEmc[6];    // normal vector on the Emc module in the sector coordinate system
    TVector3         normVecTof[6][8]; // normal vector on each Tof module in the sector coordinate system

    HKalIFilt*       pKalsys;         // Pointer to Kalman filter.

protected:

    virtual void    getMetaRecoPos (Float_t &xReco, Float_t &yReco, Float_t &zReco) const;

    virtual Float_t getP           () const;

    virtual Float_t getTrackLength () const;

    virtual Bool_t  traceToMeta    (const TVector3 &metaHit, const TVector3 &metaNorm);

public:

    HKalMetaMatcher();

    virtual ~HKalMetaMatcher() { ; }

    virtual Float_t calcQuality    (Float_t dx, Float_t dy, Float_t s2x, Float_t s2y) const { return TMath::Sqrt(dx*dx/s2x + dy*dy/s2y); }

    virtual void    clearMatches   ();

    virtual void    init           ();

    virtual Bool_t  matchWithMeta  (HMetaMatch2 const* const pMetaMatch, HKalIFilt *const pKalsys);

    virtual void    matchWithRpc   (HMetaMatch2 const* const pMetaMatch);

    virtual void    matchWithShower(HMetaMatch2 const* const pMetaMatch);

    virtual void    matchWithEmc   (HMetaMatch2 const* const pMetaMatch);

    virtual void    matchWithTof   (HMetaMatch2 const* const pMetaMatch);

    virtual void    reinit         ();

    virtual Int_t   getClstrSize   (Int_t sys) const;

    virtual Bool_t  getIsMatched   () const                                { return bIsMatched; }

    virtual const HKalMetaMatch& getMatch(Int_t sys, Int_t tab, Int_t clstr) const;

    virtual Int_t   getNmetaTabs   () const { return nMetaTabs; }

    ClassDef(HKalMetaMatcher, 0)
};

#endif // HKalMetaMatcher_h
