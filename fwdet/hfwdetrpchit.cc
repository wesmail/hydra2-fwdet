//*-- Author  : Rafal Lalik
//*-- Created : 10.01.2017

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcHit
//
//  This class contains Forward Rpc detector Hit data
//
//  Containing hit (x, y, z, tof) data
//
/////////////////////////////////////////////////////////////

#include "hfwdetrpchit.h"

#include "TMath.h"

const Float_t c2 = 2.99 * 2.99 * 1.0e16;

ClassImp(HFwDetRpcHit)

HFwDetRpcHit::HFwDetRpcHit() : fX(0.0), fY(0.0), fZ(0.0), fTof(-1.0), fP(-100.0)
{
}

HFwDetRpcHit::~HFwDetRpcHit()
{
}

void HFwDetRpcHit::print() const
{
    printf(" RPC hit: x=%f y=%f z=%f  tof=%f   p=%f\n", fX, fY, fZ, fTof, fP);
}

Float_t HFwDetRpcHit::calcMomentum(Float_t mass)
{
    if (fTof < 0.0)
        return -100.0;

    return calcMomentum(fZ, fTof, mass);
}

Float_t HFwDetRpcHit::calcMomentum(Float_t l, Float_t t, Float_t mass)
{
    Float_t tt = t * 1.0e-09;
    Float_t zz = l * 1.0e-03;

    return mass/TMath::Sqrt((tt*tt*c2)/(zz*zz) - 1.0);
}
