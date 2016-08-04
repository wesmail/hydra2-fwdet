//*-- Author  : D. Gonzalez-Diaz
//*-- Created : 08/06/2006
//*-- Modified: 24/08/2007 by P.Cabanelas/D.Gonzalez-Diaz

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HRpcCal
//
//  Class for the Calibrated RPC data
//
////////////////////////////////////////////////////


#include "hrpccal.h"

ClassImp(HRpcCal)

HRpcCal::HRpcCal(void) {
     rightTime    = leftTime     = -999;
     rightTime2   = leftTime2    = -999;
     rightCharge  = leftCharge   = 0;
     rightCharge2 = leftCharge2  = 0;
     address      = -1;
     logBit       = -1;
}

HRpcCal::~HRpcCal(void) {
}

