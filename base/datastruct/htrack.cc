#include "htrack.h"
//*-- Author : Manuel Sanchez Garcia
//*-- Modified : 30/03/98
//*-- Copyright : FPGENP

//*************************************************************************
//
// HTrack
// 
// This class is dummy.
//
//*************************************************************************

ClassImp(HTrack)

HTrack::HTrack(void) {
  // Default constructor
  fP=0.0;
}

HTrack::HTrack(HTrack &aTrack) : HRecObject(aTrack) {
  // Copy constructor.
  fP=aTrack.fP;
}

HTrack::~HTrack(void) {  
  // Destructor.
}

void HTrack::setMomentum(Float_t aP) {
  // Set guess what.
  fP=aP;
}

