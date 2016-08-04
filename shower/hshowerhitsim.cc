//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           
// HShowerHitSim
// class joins data in hit level with track number
//                              
///////////////////////////////////////////////////////////////////////////////
#pragma implementation

#include "hshowerhitsim.h"

ClassImp(HShowerHitSim)

void HShowerHitSim::clear() {
  // clears the object
  HShowerHit::clear();
  nTrack = 0;
  for(Int_t i = 0; i < SH0WER_MAX_TRACK; i ++){ nTracks[i] = -1; }
}

HShowerHitSim& HShowerHitSim::operator=(HShowerHitSim& ht) {
  HShowerHit::operator=(ht);
  nTrack = ht.nTrack;
  for(Int_t i = 0; i < SH0WER_MAX_TRACK; i ++){ nTracks[i] = ht.nTracks[i]; }
  return *this;
}

HShowerHitSim& HShowerHitSim::operator=(HShowerHit& ht) {
  HShowerHit::operator=(ht);
  nTrack = 0;
  for(Int_t i = 0; i < SH0WER_MAX_TRACK; i ++){ nTracks[i] = -1;}
  return *this;
}
