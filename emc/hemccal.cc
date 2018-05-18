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
  time         = -999.F;
  energy       = -999.F;
  isRpcMatch   = kFALSE;
  clusterIndex = -1;
  statusTime   = 0;
}
