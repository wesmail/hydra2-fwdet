#ifndef HKALTRACK_H
#define HKALTRACK_H

// from ROOT
#include "TObject.h"
#include "TMath.h"

// from hydra
#include "hbasetrack.h"

class HKalTrack : public HBaseTrack {

private:

    Float_t betaInput;      // Beta value used for particle hyposthesis.
    Float_t chi2;           // Chi2.
    Float_t momInput;       // Starting estimation of momentum.
    Float_t ndf;            // Number degrees of freedom.
    Int_t   nIter;          // Number of Kalman filtering iterations.
    Float_t trackLength;    // Track length through all MDCs
    Int_t   pid;            // Geant particle id.
    Float_t txInput;        // Starting estimate of direction (tan(px/pz))
    Float_t tyInput;        // Starting estimate of direction (tan(px/pz))
    Float_t xInput;         // Starting estimate of position in sector coords. (mm)
    Float_t yInput;         // Starting estimate of position in sector coords. (mm)

    Float_t dxMeta;         // KF propagation point on meta - x MetaHit
    Float_t dyMeta;         // KF propagation point on meta - y MetaHit
    Float_t dzMeta;         // KF propagation point on meta - z MetaHit
    Float_t qualityRpc;
    Float_t qualityShower;
    Float_t qualityTof;

    Bool_t isWireHit;    // Is a drift chamber wire hit.

    Int_t idxFirst;      // Index of first hit in the track.
    Int_t idxLast;       // Indes of last hit in the track.

public:

    HKalTrack();

    virtual ~HKalTrack() { ; }

    virtual Float_t getBetaInput     () const { return betaInput; }

    virtual Float_t getChi2          () const { return chi2; }

    virtual Int_t   getIdxFirst      () const { return idxFirst; }

    virtual Int_t   getIdxLast       () const { return idxLast; }

    virtual Bool_t  getIsWireHit     () const { return isWireHit; }

    virtual Float_t getMETAdx        () const { return dxMeta;}

    virtual Float_t getMETAdy        () const { return dyMeta;}

    virtual Float_t getMETAdz        () const { return dzMeta;}

    virtual Float_t getMomInput      () const { return momInput; }

    virtual Float_t getNdf           () const { return ndf; }

    virtual Float_t getTrackLength   () const { return trackLength; }

    virtual Int_t   getPid           () const { return pid; }

    virtual Float_t getQualityRpc    () const {return qualityRpc; }

    virtual Float_t getQualityShower () const {return qualityShower;}

    virtual Float_t getQualityEmc    () const {return qualityShower;}
    
    virtual Float_t getQualityTof    () const {return qualityTof;}

    virtual Float_t getTxInput       () const { return txInput; }

    virtual Float_t getTyInput       () const { return tyInput; }

    virtual Float_t getXinput        () const { return xInput; }

    virtual Float_t getYinput        () const { return yInput; }

    virtual void    setBetaInput     (Float_t b)   { betaInput = b; }

    virtual void    setChi2          (Float_t c)   { chi2 = c;}

    virtual void    setIdxFirst      (Int_t i)     { idxFirst = i; }

    virtual void    setIdxLast       (Int_t i)     { idxLast = i; }

    virtual void    setIsWireHit     (Bool_t wire) { isWireHit = wire; }

    virtual void    setMETAdx        (Float_t x)   { dxMeta = x;}

    virtual void    setMETAdy        (Float_t y)   { dyMeta = y;}

    virtual void    setMETAdz        (Float_t z)   { dzMeta = z;}

    virtual void    setMomInput      (Float_t mom) { momInput = mom; }

    virtual void    setNdf           (Float_t n)   { ndf = n; }

    virtual void    setTrackLength   (Float_t l)   { trackLength = l; }

    virtual void    setPid           (Int_t id)    { pid = id; }

    virtual void    setQualityRpc    (Float_t qRpc) {qualityRpc = qRpc; }

    virtual void    setQualityShower (Float_t qShower) {qualityShower = qShower;}

    virtual void    setQualityEmc    (Float_t qEmc)    {qualityShower = qEmc;}
    
    virtual void    setQualityTof    (Float_t qTof) {qualityTof = qTof;}

    virtual void    setTxInput       (Float_t tx)   { txInput = tx; }

    virtual void    setTyInput       (Float_t ty)   { tyInput = ty; }

    virtual void    setXinput        (Float_t x)    { xInput = x; }

    virtual void    setYinput        (Float_t y)    { yInput = y; }

    Float_t getMetaRadius            ()             { return  (dxMeta == -10000)? -1: TMath::Sqrt(dxMeta*dxMeta+dyMeta*dyMeta);}

    ClassDef (HKalTrack,1)
};

#endif
