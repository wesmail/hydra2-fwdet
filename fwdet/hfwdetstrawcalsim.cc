//*-- Author   : Georgy Kornakov
//*-- Created  : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetStrawCalSim
//
//  This class contains Forward Straw detector Cal data
//
//  Containing calibrated Time and energy loss
//
/////////////////////////////////////////////////////////////

#include "hfwdetstrawcalsim.h"

ClassImp(HFwDetStrawCalSim);

HFwDetStrawCalSim::HFwDetStrawCalSim() :
    fTrack(-1), fToF(0.0), fDriftRad(0.0),  px(-10000.), py(-10000.), pz(-10000.)
{
}

HFwDetStrawCalSim::~HFwDetStrawCalSim()
{
}

void HFwDetStrawCalSim::getP(Float_t & x, Float_t & y, Float_t & z) const
{
    x = px;
    y = py;
    y = pz;
}

void HFwDetStrawCalSim::setP(Float_t x, Float_t y, Float_t z)
{
    px = x;
    py = y;
    pz = z;
}
