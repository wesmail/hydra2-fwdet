//*-- Author  : Pablo Cabanelas
//*-- Created : 30/08/2007

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HRpcHit
//
//  Class for the RPC Hit data
//
////////////////////////////////////////////////////


#include "hrpchit.h"

ClassImp(HRpcHit)

HRpcHit::HRpcHit(void) {
  tof     = charge = -999;
  xmod    = ymod   = zmod = -999;
  xsec    = ysec   = zsec = -999;
  xlab    = ylab   = zlab = -999;
  theta   = phi    = -999;
  address = logBit = -1;
  sigma_x = sigma_y= sigma_z = sigma_tof = -999;
	isInCell= false;
}

void HRpcHit::clear(void) {
  tof     = charge = -999;
  xmod    = ymod   = zmod = -999;
  xsec    = ysec   = zsec = -999;
  xlab    = ylab   = zlab = -999;
  theta   = phi    = -999;
  address = logBit = -1;
  sigma_x = sigma_y= sigma_z = sigma_tof = -999;
	isInCell= false;
}



HRpcHit::~HRpcHit(void) {
}

