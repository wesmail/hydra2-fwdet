//*-- Author : M. Sanchez (2.10.2000)
//*-- Modified : M. Sanchez (2.10.2000)
#include "hmdctrk.h"
#include "hmdcseg.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////
//HMdcTrk
//
//      FIXME: < Write documentation here >
//////////////////////////////////////////////////////////

void HMdcTrk::readFromSegment(HMdcSeg *seg) {
  setZ(seg->getZ(),seg->getErrZ());
  setR(seg->getR(),seg->getErrR());
  setTheta(seg->getTheta(),seg->getErrTheta());
  setPhi(seg->getPhi(),seg->getErrPhi());
} 

ClassImp(HMdcTrk)
