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
    fTrack(-1), fToF(0.0), fDriftRad(0.0),
    fPx(-10000.), fPy(-10000.), fPz(-10000.),
    fXhit(0.0), fYhit(0.0), fZhit(0.0)
{
}

HFwDetStrawCalSim::~HFwDetStrawCalSim()
{
}
