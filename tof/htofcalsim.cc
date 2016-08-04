//*-- AUTHOR : Jaroslav Bielcik
//*-- Modified : 17/12/99 by Jaroslav Bielcik

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HTofCalSim                                                                //
// cal class simulated data                                                  //
// contains up to 2 track numbers ofcurrent hit                              //
///////////////////////////////////////////////////////////////////////////////

#include "htofcalsim.h"

ClassImp(HTofCalSim)

 void HTofCalSim::clear() {
  // clears the object
  HTofCalSim::clear();
  nTrack1 = nTrack2 = 0;
}
