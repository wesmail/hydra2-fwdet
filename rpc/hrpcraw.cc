//*-- AUTHORS :  Pablo Cabanelas / Hector Alvarez Pol
//*-- Created : 18/10/2005
//*-- Modified: 28/09/2006
//*-- Modified: 22/08/2007 Pablo Cabanelas /Diego Gonzalez-Diaz

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HRpcRaw
//
//  Class for the raw data of the RPC detector
//
/////////////////////////////////////////////////////////////

#include "hrpcraw.h"

ClassImp(HRpcRaw)

HRpcRaw::HRpcRaw(void) {
     rightTime     = leftTime  = -999;
     rightTime2    = leftTime2 = -999;
     rightTot      = leftTot   = -999;
     rightTot2     = leftTot2  = -999;
     rightTotLast  = leftTotLast  = -999;

     address = -1; 
     rightFeeAddress = -1;
     leftFeeAddress = -1;
     rightLogBit  = -1;  
     leftLogBit  = -1;  

}

HRpcRaw::~HRpcRaw(void) {
}
