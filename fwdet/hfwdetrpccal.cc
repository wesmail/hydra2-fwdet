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
    fTrack = -1;
    fModule = fGeantCell = fRpc = -1;
    fTime = fElos = -100000.F;
}

HFwDetRpcCal::~HFwDetRpcCal()
{
}

void HFwDetRpcCal::getAddress(Char_t& m, Char_t& c, Char_t& s) const
{
    m = fModule;
    c = fGeantCell;
    s = fRpc;
}

void HFwDetRpcCal::setAddress(const Char_t m, const Char_t c, const Char_t s)
{
    fModule    = m;
    fGeantCell = c;
    fRpc       = s;
}

void HFwDetRpcCal::getHit(Float_t& time, Float_t& elos) const
{
    time = fTime;
    elos = fElos;
}

void HFwDetRpcCal::setHit(const Float_t time, const Float_t elos)
{
    fTime = time;
    fElos = elos;
}
