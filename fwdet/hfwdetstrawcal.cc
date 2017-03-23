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
#include "fwdetdef.h"

ClassImp(HFwDetStrawCal);

HFwDetStrawCal::HFwDetStrawCal()
{
    fModule = -1;
    fLayer  = -1;
    fPlane  = -1;
    fStraw  = -1;
    fUpDown = 0;

    fTime   = -100000.;
    fADC    = -100000.;
    fU      = 0;
    fZ      = 0;
}

HFwDetStrawCal::~HFwDetStrawCal()
{
}

Int_t HFwDetStrawCal::getVPlane(Int_t m, Int_t l, Int_t p)
{
        return (m * FWDET_STRAW_MAX_LAYERS + l) * FWDET_STRAW_MAX_PLANES + p;
}
