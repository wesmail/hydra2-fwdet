using namespace std;
#include "hwallrawsimfilter.h"
#include "hwallrawsim.h"
#include <fstream> 

ClassImp(HWallRawSimFilter)

// Author : Dusan Zovinec
// Date: 02/07/2001
// Modified 19.4. 2005 Filip Krizek

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////
//
// HWallRawSimFilter is an HFilter to reduce the number of the
// HWallRawSim data in the catWallRaw category.
// (If HWallRawSim object has Charge==0  && Time==0 it is deleted from the category.)
//
//////////////////////////////////////////////////////////////////


Bool_t HWallRawSimFilter :: check(TObject* raw){
  if (((((HWallRawSim*)raw)->getCharge())==0)&&         
      ((((HWallRawSim*)raw)->getTime())==0)) return kFALSE;
  else return kTRUE;
}
