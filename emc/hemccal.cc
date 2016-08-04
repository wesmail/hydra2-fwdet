//*-- Author  : K. Lapidus
//*-- Created : 
//*-- Modified: 

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
  nHits=cell=0;
  sector=row=column=-1;
  time1=time2=energy=-999.F;
}
