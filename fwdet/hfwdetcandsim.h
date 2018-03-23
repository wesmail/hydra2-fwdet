#ifndef HFWDETCANDSIM_H
#define HFWDETCANDSIM_H

#include "Rtypes.h"

#include "hfwdetcand.h"
#include "hvirtualcandsim.h"
#include "haddef.h"
#include "fwdetdef.h"

#include "TBits.h"
#include "TLorentzVector.h"
#include "TMatrixDSym.h"
#include "TObject.h"

class HFwDetCandSim: public HFwDetCand, public HVirtualCandSim
{
private:
    // sim info
    Int_t   fGeantTrackRpc;             // GEANT track number in FwDet RPC

    Double_t fGeantPx1;                 // momentum of the hit in the first straw
    Double_t fGeantPy1;
    Double_t fGeantPz1;

    Double_t fGeantPx2;                 // momentum of the hit in the last straw
    Double_t fGeantPy2;
    Double_t fGeantPz2;

    Double_t fGeantX1;                  // position of the hit in the first straw
    Double_t fGeantY1;
    Double_t fGeantZ1;

    Double_t fGeantX2;                  // position of the hit in the last straw
    Double_t fGeantY2;
    Double_t fGeantZ2;

    Int_t fGeantTrackInds[FWDET_STRAW_MAX_VPLANES];  // hit indices in planes

public:
    HFwDetCandSim();
    virtual ~HFwDetCandSim();

    Int_t    getGeantTrackRpc() const { return fGeantTrackRpc; }
    Double_t getGeantPx1() const { return fGeantPx1; }
    Double_t getGeantPy1() const { return fGeantPy1; }
    Double_t getGeantPz1() const { return fGeantPz1; }
    Double_t getGeantPx2() const { return fGeantPx2; }
    Double_t getGeantPy2() const { return fGeantPy2; }
    Double_t getGeantPz2() const { return fGeantPz2; }
    Double_t getGeantX1() const { return fGeantX1; }
    Double_t getGeantY1() const { return fGeantY1; }
    Double_t getGeantZ1() const { return fGeantZ1; }
    Double_t getGeantX2() const { return fGeantX2; }
    Double_t getGeantY2() const { return fGeantY2; }
    Double_t getGeantZ2() const { return fGeantZ2; }

    void setGeantTrackRpc(Int_t a) { fGeantTrackRpc = a; }
    void setGeantPx1(Double_t px) { fGeantPx1 = px; }
    void setGeantPy1(Double_t py) { fGeantPy1 = py; }
    void setGeantPz1(Double_t pz) { fGeantPz1 = pz; }
    void setGeantPx2(Double_t px) { fGeantPx2 = px; }
    void setGeantPy2(Double_t py) { fGeantPy2 = py; }
    void setGeantPz2(Double_t pz) { fGeantPz2 = pz; }
    void setGeantX1(Double_t x) { fGeantX1 = x; }
    void setGeantY1(Double_t y) { fGeantY1 = y; }
    void setGeantZ1(Double_t z) { fGeantZ1 = z; }
    void setGeantX2(Double_t x) { fGeantX2 = x; }
    void setGeantY2(Double_t y) { fGeantY2 = y; }
    void setGeantZ2(Double_t z) { fGeantZ2 = z; }

    inline Float_t getGeantTx() const;
    inline Float_t getGeantTy() const;

    inline Int_t addGeantHitTrack(Int_t track);

    void print(UChar_t mask = 0xff) const;

    Int_t calcGeantCorrTrackIds();

private:

    ClassDef(HFwDetCandSim, 2);
};

Float_t HFwDetCandSim::getGeantTx() const
{
    // Calculates simulated Tx slope
    Float_t dz = fGeantZ2 - fGeantZ1;
    Float_t dx = fGeantX2 - fGeantX1;
    return dx/dz;
}

Float_t HFwDetCandSim::getGeantTy() const
{
    // Calculates simulated Ty slope
    Float_t dz = fGeantZ2 - fGeantZ1;
    Float_t dy = fGeantY2 - fGeantY1;
    return dy/dz;
}

Int_t HFwDetCandSim::addGeantHitTrack(Int_t track)
{
    Int_t fNhits = getNofHits();
    if (!fNhits)
        return 0;

    fGeantTrackInds[fNhits-1] = track;
    return fNhits;
}

#endif
