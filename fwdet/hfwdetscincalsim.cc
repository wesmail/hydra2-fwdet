//*-- Author   : Ilse Koenig
//*-- Created  : 19.02.2016

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////
//  HFwDetScinCalSim
//
//  This class for the Cal data of the Forward Scinillator detector
//
//  Contains calibrated Time and energy loss
//
//////////////////////////////////////////////////////////////////

#include "hfwdetscincalsim.h"

ClassImp(HFwDetScinCalSim);

HFwDetScinCalSim::HFwDetScinCalSim()
{
    fTrack = -1;
    fModule = fGeantCell = fScin = -1;
    fTime = fElos = -100000.F;
}

HFwDetScinCalSim::~HFwDetScinCalSim()
{
}
