//*-- AUTHOR : Leszek Kidon
//*-- Modified : 19/04/05 by Jacek Otwinowski
//
//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           
// HShowerHitTofTrack
// 
// Data object for correlated Shower/Tofino hits and
// corresponding geant track numbers.  
// This data object is filled by HShowerHitTofTrackMatcher 
// which takes information from HShowerHitTof 
// and HShowerTrack data containers.
//                              
///////////////////////////////////////////////////////////////////////////////

#pragma implementation

#include "hshowerhittoftrack.h"

ClassImp(HShowerHitTofTrack)

void HShowerHitTofTrack::clear() {
  // clears the object
  HShowerHitTof::clear();
  nTrack = 0;
  for(Int_t i = 0; i < SH0WER_MAX_TRACK; i ++){ nTracks[i] = -1;}
}

HShowerHitTofTrack& HShowerHitTofTrack::operator=(HShowerHitTofTrack& ht) {
  HShowerHitTof::operator=(ht);
  nTrack = ht.nTrack;
  for(Int_t i = 0; i < SH0WER_MAX_TRACK; i ++){ nTracks[i] = ht.nTracks[i]; }
  return *this;
}

HShowerHitTofTrack& HShowerHitTofTrack::operator=(HShowerHitTof& ht) {
  HShowerHitTof::operator=(ht);
  nTrack = 0;
  for(Int_t i = 0; i < SH0WER_MAX_TRACK; i ++){ nTracks[i] = -1;}

  return *this;
}

