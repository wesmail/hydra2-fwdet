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

protected:
    virtual Bool_t   calcEffMeasVec       (Int_t iSite) const;

public:

    HKalDafSingleWire(Int_t nHits, Int_t measDim, Int_t stateDim, HMdcTrackGField *fMap, Double_t fpol);

    virtual ~HKalDafSingleWire() {}

    virtual Bool_t              fitTrack        (const TObjArray &hits, const TVectorD &iniSv,
                                                 const TMatrixD &iniCovMat, Int_t pId);

    virtual Double_t            getDafChi2Cut   () const           { return dafChi2Cut; }

    virtual const Double_t*     getDafT         () const           { return dafT.GetArray(); }

    virtual Bool_t              getDoDaf        () const           { return kTRUE; }

    virtual Int_t               getNdafs        () const           { return dafT.GetSize(); }

    virtual void                setDafPars      (Double_t chi2cut, const Double_t *T, Int_t n);

    ClassDef(HKalDafSingleWire,0)
};

#endif // HKalDafSingleWire_h

