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
    fModule      = 0;
    fDoubleLayer = 0;
    fLayer       = 0;
    fCell        = 0;

    fTime = fEloss = -100000.;
    fDriftRad = 0;
    fX = 0;
    fZ = 0;
    fNStraw = -1000;

    fTrack = -1;
}

HFwDetStrawCal::~HFwDetStrawCal()
{
}
