//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//                                                                           
// HShowerPIDTrack
// class joins data in PID level with track number
//                              
///////////////////////////////////////////////////////////////////////////////
#pragma implementation

#include "hshowerpidtrack.h"

ClassImp(HShowerPIDTrack)

void HShowerPIDTrack::clear() {
  // clears the object
  HShowerPID::clear();
  nTrack = 0;
}

HShowerPIDTrack& HShowerPIDTrack::operator=(HShowerPIDTrack& pt) {
  HShowerPID::operator=(pt);
  nTrack = pt.nTrack;

  return *this;
}

HShowerPIDTrack& HShowerPIDTrack::operator=(HShowerPID& pt) {
  HShowerPID::operator=(pt);
  nTrack = 0;

  return *this;
}

