//*-- AUTHOR : Diego Gonzalez-Diaz 17/12/2007
//*-- Modified: 12/12/2009
//(Included flags 'isAtBox'). Only relevant for hgeantrpc hits
//created by interaction of secondary charged particles stemming
//from *neutral particles* impinging at the RPC virtual box.
//*-- Modified: 29/12/2009
//Included arrays of tracks sorted (in the digitizer) by geant time
//
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HRpcHitSim                                                                //
// this is the hit data level for simulated data			     //
///////////////////////////////////////////////////////////////////////////////


#include "hrpchitsim.h"

ClassImp(HRpcHitSim)

HRpcHitSim::HRpcHitSim(void) {
  clear();
}
void HRpcHitSim::clear(void) {

  RefL       = RefR       = -1;
  RefLDgtr   = RefRDgtr   = -1;
  nTracksL   = nTracksR   =  0;

  for(Int_t i=0;i<10;i++)     {
    TrackL[i]   = TrackLDgtr[i] = TrackR[i] = TrackRDgtr[i] = -999;
    LisAtBox[i] = RisAtBox[i]   = kFALSE;
  }
}
