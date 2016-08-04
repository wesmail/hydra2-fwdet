using namespace std;
#include "htofrawsimfilter.h"
#include "htofrawsim.h"
#include <fstream> 

ClassImp(HTofRawSimFilter)

// Author : Dusan Zovinec
// Date: 02/07/2001

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////
//
// HTofRawSimFilter is an HFilter to reduce the number of the
// HTofRawSim data in the catTofRaw category.
// (If HTofRawSim object has LeftCharge==0 && RightCharge==0 &&
// LeftTime==0 && RightTime==0 it is deleted from the category.)
//
//////////////////////////////////////////////////////////////////


Bool_t HTofRawSimFilter :: check(TObject* raw){
  if (((((HTofRawSim*)raw)->getLeftCharge())==0)&&
      ((((HTofRawSim*)raw)->getRightCharge())==0)&&
      ((((HTofRawSim*)raw)->getLeftTime())==0)&&
      ((((HTofRawSim*)raw)->getRightTime())==0)) return kFALSE;
  else return kTRUE;
}
