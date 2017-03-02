//*-- Author   : Georgy Kornakov
//*-- Created  : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetStrawCal
//
//  This class contains Forward Straw detector Cal data
//
//  Containing calibrated Time and energy loss
//
/////////////////////////////////////////////////////////////

#include "hfwdetstrawcal.h"

ClassImp(HFwDetStrawCal);

HFwDetStrawCal::HFwDetStrawCal()
{
    fModule      = -1;
    fLayer       = -1;
    fPlane       = -1;
    fCell        = -1;

    fTime = fEloss = -100000.;
    fDriftRad = 0;
    fX = 0;
    fZ = 0;
    fStraw = -1000;
}

HFwDetStrawCal::~HFwDetStrawCal()
{
}
