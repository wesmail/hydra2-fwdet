//*-- Author   : Georgy Kornakov
//*-- Created  : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcCalSim
//
//  This class contains Forward Rpc detector Cal data
//
//  Containing calibrated Time and energy loss
//
/////////////////////////////////////////////////////////////

#include "hfwdetrpccal.h"

ClassImp(HFwDetRpcCal)

HFwDetRpcCal::HFwDetRpcCal()
{
//     fTrack = -1;
    nHitsNum = 0;
    fModule = fLayer = -1;
    for (Int_t i = 0; i < FWDET_RPC_MAX_HITS; ++i)
    {
        fTimeL[i] = fTimeR[i] = -100000.F;
        fChargeL[i] = fChargeR[i] = -100000.F;
    }
}

HFwDetRpcCal::~HFwDetRpcCal()
{
}

void HFwDetRpcCal::getAddress(Char_t& m, Char_t& l, Char_t& s) const
{
    m = fModule;
    l = fLayer;
    s = fStrip;
}

void HFwDetRpcCal::setAddress(Char_t m, Char_t l, Char_t s)
{
    fModule = m;
    fLayer = l;
    fStrip = s;
}

Bool_t HFwDetRpcCal::getHit(Int_t n, Float_t& tl, Float_t& tr, Float_t& ql, Float_t& qr) const
{
    if (n < FWDET_RPC_MAX_HITS)
    {
        tl = fTimeL[n];
        tr = fTimeR[n];
        ql = fChargeL[n];
        qr = fChargeR[n];
        return true;
    }
    else
    {
        tl = -100000.F;
        tr = -100000.F;
        ql = -100000.F;
        qr = -100000.F;
        return false;
    }
}

Bool_t HFwDetRpcCal::setHit(Int_t n, Float_t tl, Float_t tr, Float_t ql, Float_t qr)
{
    if (n < nHitsNum)
    {
        fTimeL[n] = tl;
        fTimeR[n] = tr;
        fChargeL[n] = ql;
        fChargeR[n] = qr;

        return true;
    }

    return false;
}

Bool_t HFwDetRpcCal::addHit(Float_t tl, Float_t tr, Float_t ql, Float_t qr)
{
    if (nHitsNum < FWDET_RPC_MAX_HITS-1)
    {
        fTimeL[nHitsNum] = tl;
        fTimeR[nHitsNum] = tr;
        fChargeL[nHitsNum] = ql;
        fChargeR[nHitsNum] = qr;

        ++nHitsNum;
        return true;
    }

    return false;
}

void HFwDetRpcCal::print() const
{
    printf(" RPC hit: m=%d l=%d s=%d:\n", fModule, fLayer, fStrip);
    for (Int_t i = 0; i < nHitsNum; ++i)
    {
        printf("  hit %d : t_l=%f t_r=%f q_l=%f q_r=%f\n",
               i, fTimeL[i], fTimeR[i], fChargeL[i], fChargeR[i]);
    }
}
