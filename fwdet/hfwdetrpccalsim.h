#ifndef HFWDETRPCCALSIM_H
#define HFWDETRPCCALSIM_H

#include "hfwdetrpccal.h"

#include "TObject.h"

class HFwDetRpcCalSim : public HFwDetRpcCal
{
public:
    Int_t   nTrack[FWDET_RPC_MAX_HITS]; // geant track contributing to the hit
    Float_t fX[FWDET_RPC_MAX_HITS];
    Float_t fY[FWDET_RPC_MAX_HITS];
    Float_t fTof[FWDET_RPC_MAX_HITS];

public:
    HFwDetRpcCalSim();
    virtual ~HFwDetRpcCalSim();

    inline Bool_t setHit(Int_t n, Float_t x, Float_t y, Float_t t);
    inline Bool_t getHit(Int_t n, Float_t & x, Float_t & y, Float_t & t) const;

    inline Bool_t setTrack(Int_t n, Int_t track);
    inline Bool_t getTrack(Int_t n, Int_t & track) const;

    void print() const;

    ClassDef(HFwDetRpcCalSim, 1);
};

Bool_t HFwDetRpcCalSim::setHit(Int_t n, Float_t x, Float_t y, Float_t t)
{
    if (n < nHitsNum)
    {
        fX[n] = x;
        fY[n] = y;
        fTof[n] = t;
        return kTRUE;
    }
    return kFALSE;
}

Bool_t HFwDetRpcCalSim::getHit(Int_t n, Float_t& x, Float_t& y, Float_t& t) const
{
    if (n < nHitsNum)
    {
        x = fX[n];
        y = fY[n];
        t = fTof[n];
        return kTRUE;
    }
    return kFALSE;
}

Bool_t HFwDetRpcCalSim::setTrack(Int_t n, Int_t track)
{
    if (n < nHitsNum)
    {
        nTrack[n] = track;
        return kTRUE;
    }
    return kFALSE;
}

Bool_t HFwDetRpcCalSim::getTrack(Int_t n, Int_t & track) const
{
    if (n < nHitsNum)
    {
        track = nTrack[n];
        return kTRUE;
    }
    return kFALSE;
}

#endif /* !HFWDETRPCCALSIM_H */
