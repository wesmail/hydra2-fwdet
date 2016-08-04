//*-- AUTHOR : K. Lapidus

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HEmcCalSim                                                                //
// this is the cal data level for simulated data                             //
// it stores 5 references to hgeantemc objects and its corresponding tracks  //
///////////////////////////////////////////////////////////////////////////////


#include "hemccalsim.h"

ClassImp(HEmcCalSim)

HEmcCalSim::HEmcCalSim() {
  nTracks = totMult = 0;
}

HEmcCalSim::~HEmcCalSim() {
}

void HEmcCalSim::setTrack(Int_t trackNumber) {
  totMult++;
  if (nTracks<5) {
    listTracks[nTracks] = trackNumber;
    nTracks++;
  }
}
