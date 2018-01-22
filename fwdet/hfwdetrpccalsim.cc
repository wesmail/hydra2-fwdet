//*-- Author   : Luis Silva
//*-- Created  : 27.01.2016 

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcCalSim
//
//  This class contains Forward Rpc detector Cal data
//
//  Containing calibrated Time and energy loss
//
/////////////////////////////////////////////////////////////

#include "hfwdetrpccalsim.h"

ClassImp(HFwDetRpcCalSim);

HFwDetRpcCalSim::HFwDetRpcCalSim() : HFwDetRpcCal()
{
    for (Int_t i = 0; i < FWDET_RPC_MAX_HITS; ++i)
    {
        nTrack[i] = -1;
        fXGea[i] = 0.0;
        fYGea[i] = 0.0;
        fTofGea[i] = 0.0;
    }
}

HFwDetRpcCalSim::~HFwDetRpcCalSim()
{
}

void HFwDetRpcCalSim::print() const
{
    printf(" RPC hit: m=%d l=%d s=%d\n", fModule, fLayer, fStrip);
    for (Int_t i = 0; i < nHitsNum; ++i)
    {
        printf("  x,y,t=%f,%f,%f  track=%d\n", fXGea[i], fYGea[i], fTofGea[i], nTrack[i]);
        printf("  hit %d : t_l=%f t_r=%f q_l=%f q_r=%f  tof=%f  u,v=%f,%f  x,y=%f,%f\n",
               i, fTimeL[i], fTimeR[i], fChargeL[i], fChargeR[i], fTof[i], fU[i], fV[i], fX[i], fY[i]);
    }
}
