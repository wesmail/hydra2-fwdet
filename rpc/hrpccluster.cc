//*-- Author  : Diego Gonzalez-Diaz
//*-- Created : 20/12/2007
//*-- Modified: 24/11/2010 P.Cabanelas

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HRpcCluster
//
//  Class for the RPC Cluster data
//
////////////////////////////////////////////////////


#include "hrpccluster.h"

ClassImp(HRpcCluster)

HRpcCluster::HRpcCluster(void) {
  tof     = charge  = -999;
  xmod    = ymod    = zmod    = -999;
	xsec    = ysec    = zsec    = -999;
	xlab    = ylab   = zlab = -999;
	theta   = phi    = -999;
  sigma_x = sigma_y = sigma_z = sigma_tof = -999;
  sector  = index = type = -1;
  detID1  = detID2  = -1;
	isInCell= 0; 
}

HRpcCluster::~HRpcCluster(void) {
}
