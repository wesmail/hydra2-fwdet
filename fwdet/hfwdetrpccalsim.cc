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
        fX[i] = 0.0;
        fY[i] = 0.0;
        fTof[i] = 0.0;
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
        printf("  x,y,t=%f,%f,%f  track=%d\n", fX[i], fY[i], fTof[i], nTrack[i]);
        printf("  hit %d : t_l=%f t_r=%f q_l=%f q_r=%f\n",
               i, fTimeL[i], fTimeR[i], fChargeL[i], fChargeR[i]);
    }
}
