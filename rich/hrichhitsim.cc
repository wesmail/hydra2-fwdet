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
//  HRichHitSim
//
//  End of track list via 0, -1 is generated in HRichRingFindSim.
//  No need to initialize track array in Constructor.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichhitsim.h"

ClassImp(HRichHitSim)

HRichHitSim::HRichHitSim()
{
   track1 = track2 = track3 = 0;
   weigTrack1 = weigTrack2 = weigTrack3 = 0;
   flag1 =  flag2 = flag3 = -1;
}







