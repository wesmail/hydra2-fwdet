#ifndef HKalDafSingleWire_h
#define HKalDafSingleWire_h

// from ROOT
#include "TArrayD.h"

// from hydra
class HCategory;
class HMdcTrackGField;

#include "hkalfiltwire.h"


class HKalDafSingleWire : public HKalFiltWire {

private:
    Double_t dafChi2Cut;     // Cut-off parameter for the annealing filter.
    TArrayD  dafT;           // Annealing factors ("temperatures") used in the DAF iterations.
    Int_t    wireNr;         // Index of wire measurement currently filtered (0 or 1)

protected:

    virtual Bool_t   calcEffErrMat        (Int_t iSite, Int_t iWire) const;

    virtual Bool_t   calcEffMeasVec       (Int_t iSite, Int_t iWire) const;

    virtual Int_t    getWireNr            ()            const { return wireNr; }

    virtual void     setWireNr            (Int_t w)     { wireNr = w; }

public:

    HKalDafSingleWire(Int_t nHits, Int_t measDim, Int_t stateDim, HMdcTrackGField *fMap, Double_t fpol);

    virtual ~HKalDafSingleWire() {}

    virtual Bool_t              fitTrack        (const TObjArray &hits, const TVectorD &iniSv,
                                                 const TMatrixD &iniCovMat, Int_t pId);

    virtual Double_t            getDafChi2Cut   () const           { return dafChi2Cut; }

    virtual const Double_t*     getDafT         () const           { return dafT.GetArray(); }

    virtual Bool_t              getDoDaf        () const           { return kTRUE; }

    virtual TMatrixD const&     getHitErrMat    (HKalTrackSite* const site) const;

    virtual TVectorD const&     getHitVec       (HKalTrackSite* const site) const;

    virtual Int_t               getNdafs        () const           { return dafT.GetSize(); }

    virtual Double_t            getNdf          () const;

    virtual void                setDafPars      (Double_t chi2cut, const Double_t *T, Int_t n);

    virtual void                updateSites(const TObjArray &hits);

    ClassDef(HKalDafSingleWire,0)
};

#endif // HKalDafSingleWire_h

