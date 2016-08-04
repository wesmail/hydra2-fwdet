//*-- AUTHOR : Jaroslav Bielcik
//*-- Modified : 18/12/99
//*-- Modified : 19/4/2005  Filip Krizek

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HWallRawSim                                                               //
// this is raw data level for simulated data                                 //
// it stores 4 track numbers                                                 //
///////////////////////////////////////////////////////////////////////////////

#include "hwallrawsim.h"

ClassImp(HWallRawSim)

void HWallRawSim::clear(void) {
  // clears the object
  HWallRawSim::clear();
  nTrack1 = nTrack2 = -1;
  //nTrack1 = nTrack2 = nTrack3 = nTrack4 = -1;
}

void HWallRawSim::setNTrack( Int_t nTrack) {
  // set the track number
  // up to 4 hits per cell can be stored now
   if(nHit==1) setNTrack1(nTrack);
   if(nHit==2) setNTrack2(nTrack);
   //if(nHit==3) setNTrack3(nTrack);
   //if(nHit==4) setNTrack4(nTrack);
 }
