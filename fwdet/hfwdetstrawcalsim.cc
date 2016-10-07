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

HFwDetStrawCalSim::HFwDetStrawCalSim()
{
    fTrack = -1;
    fModule = fGeantCell = fStraw = 0;
    fTime = fEloss = -100000.;
    fDriftRad = 0;
    fX = 0;
    fZ = 0;
    fNStraw = -1000;
}

HFwDetStrawCalSim::~HFwDetStrawCalSim()
{
}
