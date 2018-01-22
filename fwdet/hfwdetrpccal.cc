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
        fTof[i] = -1.0;
        fU[i] = fV[i] = -10000.0;
        fX[i] = fY[i] = -10000.0;
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
    if (nHitsNum < FWDET_RPC_MAX_HITS)
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

Float_t HFwDetRpcCal::getTof(Int_t n) const
{
    if (n < nHitsNum)
        return fTof[n];
    else
        return -10000.;
}

Float_t HFwDetRpcCal::getU(Int_t n) const
{
    if (n < nHitsNum)
        return fU[n];
    else
        return -10000.;
}

Float_t HFwDetRpcCal::getV(Int_t n) const
{
    if (n < nHitsNum)
        return fV[n];
    else
        return -10000.;
}

Float_t HFwDetRpcCal::getX(Int_t n) const
{
    if (n < nHitsNum)
        return fX[n];
    else
        return -10000.;
}

Float_t HFwDetRpcCal::getY(Int_t n) const
{
    if (n < nHitsNum)
        return fY[n];
    else
        return -10000.;
}

void HFwDetRpcCal::calcHit(Int_t n, Float_t v, Float_t L, Float_t W, Float_t g, Int_t s, Float_t so, Float_t r)
{
    fTof[n]= (fTimeR[n]+fTimeL[n] - L/v)/2.0;   // tof
    fV[n] = (fTimeR[n]-fTimeL[n])*v/2.0;        // pos along time axis

    if ((s & 0x1) == 1) // odd strips calc
    {
        fU[n] = (g+W)*(fStrip - (s-1)/2);   // fStrip counts from lew edge
    }                                       // so convert from 0..s, to -s/2..s/2
    else // even strips calc
    {
        Int_t ss = 0;
        if (fStrip < s/2)
        {
            ss = fStrip - s/2;
            fU[n] = (W+g)/2.0;
        }
        else
        {
            ss = fStrip - s/2 + 1;
            fU[n] = -(W+g)/2.0;
        }

        fU[n] += (g+W)*ss;
    }
    fU[n] += so;

    fX[n] = fV[n] * cos(r) - fU[n] * sin(r);
    fY[n] = fV[n] * sin(r) + fU[n] * cos(r);
}

void HFwDetRpcCal::reconstructHits(Float_t v, Float_t L, Float_t W, Float_t g, Int_t s, Float_t so, Float_t r)
{
    for (Int_t i = 0; i < nHitsNum; ++i)
        calcHit(i, v, L, W, g, s, so, r);
}

void HFwDetRpcCal::print() const
{
    printf(" RPC hit: m=%d l=%d s=%d:\n", fModule, fLayer, fStrip);
    for (Int_t i = 0; i < nHitsNum; ++i)
    {
        printf("  hit %d : t_l=%f t_r=%f q_l=%f q_r=%f  tof=%f  u,v=%f,%f  x,y=%f,%f\n",
               i, fTimeL[i], fTimeR[i], fChargeL[i], fChargeR[i], fTof[i], fU[i], fV[i], fX[i], fY[i]);
    }
}
