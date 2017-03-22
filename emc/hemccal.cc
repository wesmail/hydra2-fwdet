//*-- Author  : K. Lapidus
//*-- Created :
//*-- Modified: V.Pechenov

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HEmcCal
//
//  Class for the calibrated EMC data
//
////////////////////////////////////////////////////

#include "hemccal.h"

ClassImp(HEmcCal)

void HEmcCal::clear(void) {
  nHits        = 0;
  cell         = 0;
  sector       = -1;
  row          = -1;
  column       = -1;
  time1        = -999.F;
  time2        = -999.F;
  energy       = -999.F;
  sigmaEnergy  = 0.F;
  sigmaTime1   = 0.F;
  sigmaTime2   = 0.F;
  isRpcMatch   = kFALSE;
  statusTime1  = 0;
  statusTime2  = 0;
  clusterIndex = -1;
}
