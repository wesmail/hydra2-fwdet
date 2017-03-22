//*-- AUTHOR : K. Lapidus
//*-- Modified : 07/12/16 by Vladimir Pechenov

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// HEmcCalSim                                                                  //
// this is the cal data level for simulated data                               //
// it stores list of tracknumbers (GEANT) and its corresponding energy deposit //
// Lists sorted by energy deposit (trackEnergy[5]):                            //
// track listTracks[0] has maximal contribution to the cluster energy          //
/////////////////////////////////////////////////////////////////////////////////


#include "hemccalsim.h"

ClassImp(HEmcCalSim)

void HEmcCalSim::setTrack(Int_t trackNumber,Float_t energy) {
  totMult++;
  if (nTracks<5) {
    listTracks[nTracks]  = trackNumber;
    trackEnergy[nTracks] = energy;
    nTracks++;
  }
}
