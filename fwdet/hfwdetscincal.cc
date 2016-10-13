//*-- Author   : Ilse Koenig
//*-- Created  : 19.02.2016

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////
//  HFwDetScinCal
//
//  This class for the Cal data of the Forward Scinillator detector
//
//  Contains calibrated Time and energy loss
//
//////////////////////////////////////////////////////////////////

#include "hfwdetscincal.h"

ClassImp(HFwDetScinCal);

HFwDetScinCal::HFwDetScinCal()
{
    fTrack = -1;
    fModule = fGeantCell = fScin = -1;
    fTime = fElos = -100000.F;
}

HFwDetScinCal::~HFwDetScinCal()
{
}
