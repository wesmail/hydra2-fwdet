//*-- Author  : R. Lalik <Rafal.Lalik@ph.tum.de>
//*-- Created : 2017

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetCandSim
//
//  Sim information for track vector
//
/////////////////////////////////////////////////////////////

#include "hfwdetcandsim.h"
#include "hfwdetrpchit.h"
#include "hmdcsizescells.h"

HFwDetCandSim::HFwDetCandSim() : HFwDetCand(),
    fTrack(-1), fRpcTrack(-1), 
    fPx1(0.0), fPy1(0.0), fPz1(0.0), fPx2(0.0), fPy2(0.0), fPz2(0.0),
    fX1(0.0), fY1(0.0), fZ1(0.0), fX2(0.0), fY2(0.0), fZ2(0.0)
{
}

HFwDetCandSim::~HFwDetCandSim() { }

void HFwDetCandSim::print() const
{
    // Print vector simulation parameters
    HFwDetCand::print();
    printf("  SIM VECTOR -----\n");
    printf("   track=%d   rpc track=%d\n", fTrack, fRpcTrack);
    printf("   Entry vector=%f,%f,%f at %f,%f,%f\n", fPx1, fPy1, fPz1, fX1, fY1, fZ1);
    printf("   Exit vector=%f,%f,%f at %f,%f,%f\n", fPx2, fPy2, fPz2, fX2, fY2, fZ2);
    printf("   Direction vector=%f,%f at %f,%f,%f\n", getSimTx(), getSimTy(), fX1, fY1, fZ1);
    printf("                       tracks= ");
    for (Int_t i = 0; i < getNofHits(); ++i)
        printf("%2d,", fTrackInds[i]);
    printf("\n");
}

ClassImp(HFwDetCandSim);
