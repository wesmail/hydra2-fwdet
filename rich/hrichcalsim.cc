//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichCalSim
//
//  cal class for simulated data.
//  contains nTrack1, nTrack2, indexes of the first and last track numbers
//  of the particles that hit one pad in the linear category catRichTrack and
//  fEnergy, average energy of the photon hits on each pad.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichcalsim.h"

ClassImp(HRichCalSim)

HRichCalSim::HRichCalSim()
   : HRichCal()
{
   fTrack1 = 0;
   fTrack2 = 0;
   fEnergy = 0.0;
}

HRichCalSim::HRichCalSim(Float_t ch)
   : HRichCal(ch)
{
   fTrack1 = 0;
   fTrack2 = 0;
   fEnergy = 0.0;
}

HRichCalSim::HRichCalSim(Int_t s, Int_t r, Int_t c)
   : HRichCal(s, c, r)
{
   fTrack1 = 0;
   fTrack2 = 0;
   fEnergy = 0.0;
}
