//*-- Author  : R. Lalik <Rafal.Lalik@ph.tum.de>
//*-- Created : 2017

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HVectorCandSim
//
//  Sim information for track vector
//
/////////////////////////////////////////////////////////////

#include "hvectorcandsim.h"
#include "hfwdetrpchit.h"
#include "hmdcsizescells.h"

HVectorCandSim::HVectorCandSim() : HVectorCand(),
    fTrack(-1), fRpcTrack(-1), 
    fPx1(0.0), fPy1(0.0), fPz1(0.0), fPx2(0.0), fPy2(0.0), fPz2(0.0),
    fX1(0.0), fY1(0.0), fZ1(0.0), fX2(0.0), fY2(0.0), fZ2(0.0)
{
}

HVectorCandSim::~HVectorCandSim() { }

void HVectorCandSim::print() const
{
    // Print vector simulation parameters
    HVectorCand::print();
    printf("----- VECTOR -----\n");
    printf("   track=%d   rpc track=%d\n", fTrack, fRpcTrack);
    printf("   Entry vector=%f,%f,%f at %f,%f,%f\n", fPx1, fPy1, fPz1, fX1, fY1, fZ1);
    printf("   Exit vector=%f,%f,%f at %f,%f,%f\n", fPx2, fPy2, fPz2, fX2, fY2, fZ2);
    printf("   Direction vector=%f,%f at %f,%f,%f\n", getSimTx(), getSimTy(), fX1, fY1, fZ1);
}

ClassImp(HVectorCandSim);
