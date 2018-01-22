#ifndef HFWDETRPCCAL_H
#define HFWDETRPCCAL_H

#include "hades.h"
#include "fwdetdef.h"

#include "TObject.h"

class HFwDetRpcCal : public TObject
{
protected:
    Char_t  fModule;    // module number (0..1 for Rpc modules)
    Char_t  fLayer; // geant cell number
    Int_t   fStrip;
    Float_t fTimeL[FWDET_RPC_MAX_HITS];     // time left
    Float_t fTimeR[FWDET_RPC_MAX_HITS];     // time right
    Float_t fChargeL[FWDET_RPC_MAX_HITS];   // charge left
    Float_t fChargeR[FWDET_RPC_MAX_HITS];   // charge right
    Float_t fTof[FWDET_RPC_MAX_HITS];
    Float_t fU[FWDET_RPC_MAX_HITS];
    Float_t fV[FWDET_RPC_MAX_HITS];
    Float_t fX[FWDET_RPC_MAX_HITS];
    Float_t fY[FWDET_RPC_MAX_HITS];
    Int_t   nHitsNum;

public:
    HFwDetRpcCal();
    ~HFwDetRpcCal();

    void   getAddress(Char_t& m, Char_t& l, Char_t& s) const;
    Int_t  getHitsNum() const { return nHitsNum; }
    Bool_t getHit(Int_t n, Float_t& tl, Float_t& tr, Float_t& ql, Float_t& qr) const;

    void   setAddress(Char_t m, Char_t l, Char_t s);
    Bool_t setHit(Int_t n, Float_t tl, Float_t tr, Float_t ql, Float_t qr);
    Bool_t addHit(Float_t tl, Float_t tr, Float_t ql, Float_t qr);

    Float_t getTof(Int_t n) const;
    Float_t getU(Int_t n) const;
    Float_t getV(Int_t n) const;
    Float_t getX(Int_t n) const;
    Float_t getY(Int_t n) const;

    //                   v_prop     length     width      gap size   #strips  st. offset  rot,
    void reconstructHits(Float_t v, Float_t L, Float_t W, Float_t g, Int_t s, Float_t so, Float_t r);
    void print() const;

private:
    void calcHit(Int_t n, Float_t v, Float_t L, Float_t W, Float_t g, Int_t s, Float_t so, Float_t r);

    ClassDef(HFwDetRpcCal, 1)
};

#endif /* !HFWDETRPCCAL_H */
