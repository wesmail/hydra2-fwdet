//*-- AUTHOR : Diego Gonzalez-Diaz 19/12/2007
//*-- Modified: 12/12/2009
//(Included flag 'isAtBox').
//*-- Modified: 29/12/2009
// Geant references and tracks extended to 10-dimensional arrays
//  
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HRpcClusterSim                                                            //
// this is the ClusterSim data level for simulated data                      //
///////////////////////////////////////////////////////////////////////////////

#include "hrpcclustersim.h"

ClassImp(HRpcClusterSim)

HRpcClusterSim::HRpcClusterSim(void) {
  clear();
}

void HRpcClusterSim::clear(void) {
  // clears the object

  Track   =   nTracksAtBox  = nTracksAtCells = -999;
  isAtBox = kFALSE;

  for(Int_t i=0;i<4;i++)     TrackList[i]   = -999;
  for(Int_t i=0;i<4;i++)     RefList[i]     = -999;
  for(Int_t i=0;i<4;i++)     isAtBoxList[i] = kFALSE;

}

Bool_t HRpcClusterSim::isTrack(Int_t track) {

  for(Int_t i=0;i<4;i++){
    if (TrackList[i]==track) return kTRUE;
    else return kFALSE;
  }
  return -1;
}

Bool_t HRpcClusterSim::isRef(Int_t ref) {

  for(Int_t i=0;i<4;i++){
    if (RefList[i]==ref) return kTRUE;
    else return kFALSE;
  }
  return -1;
}
