//*-- AUTHOR : Jaroslav Bielcik
//*-- Modified : 17/12/99 by Jaroslav Bielcik

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HTofHitSim                                                                //
// hit class simulated data                                                  //
// contains up to 2 track numbers ofcurrent hit                              //
///////////////////////////////////////////////////////////////////////////////

#include "htofhitsim.h"

ClassImp(HTofHitSim)
ClassImp(HTofHitSimTmp)

 void HTofHitSim::clear() {
  // clears the object
  HTofHitSim::clear();
  nTrack1 = nTrack2 = -1;
}
