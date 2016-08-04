//*-- Author   : Georgy Kornakov
//*-- Created  : 27.01.2016 

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcCalSim
//
//  This class contains Forward Rpc detector Cal data
//
//  Containing calibrated Time and energy loss
//
/////////////////////////////////////////////////////////////

#include "hfwdetrpccalsim.h"

ClassImp(HFwDetRpcCalSim)

HFwDetRpcCalSim::HFwDetRpcCalSim(void) {
  fTrack = -1;
  fModule = fGeantCell = fRpc = -1;
  fTime = fElos = -100000.F;
}
