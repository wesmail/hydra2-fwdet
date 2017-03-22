//*-- Author  : V. Pechenov
//*-- Created : 
//*-- Modified: 

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HEmcClusterSim
//
//  Class for the cluster in EMC data.
//  Cluster is set of neighboring cells correlated in time
//
////////////////////////////////////////////////////


#include "hemcclustersim.h"

ClassImp(HEmcClusterSim)

void HEmcClusterSim::setTrack(Int_t trackNumber,Float_t energy) {
  totMult++;
  if (nTracks<5) {
    listTracks[nTracks]  = trackNumber;
    trackEnergy[nTracks] = energy;
    nTracks++;
  }
}
