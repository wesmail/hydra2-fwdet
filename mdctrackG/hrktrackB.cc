//*-- Author : A.Sadovsky
//*-- Last modified : 10/08/2005 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
//  HRKTrackB 
//
//  data container holding all information from Runge-Kutta tracking
//  See HBaseTrack for functions and data members which are not listed here...
// 
//  void setChiq(Double_t Chiq)      - Sets #chi^{2}_{RK}
//  void setMETAdx(Float_t x)        - Sets x-component of vector of difference between Runge-Kutta track intersection on the surface of the META subdetector and the coordinate of hit/cluster on META detector
//  void setMETAdy(Float_t y)        - Sets y-component of vector of difference between Runge-Kutta track intersection on the surface of the META subdetector and the coordinate of hit/cluster on META detector
//  void setMETAdz(Float_t z)        - Sets z-component of vector of difference between Runge-Kutta track intersection on the surface of the META subdetector and the coordinate of hit/cluster on META detector
//
//  Float_t getChiq(void)    - #chi^{2} of Runge Kutta tracking from x,y points (fitted minus measured) on each of the MDCs
//  Float_t getMETAdx(void)  - x-deviation of track trajectory of Runge Kutta propagation from the hit/cluster on the META detector
//  Float_t getMETAdy(void)  - y-deviation of track trajectory of Runge Kutta propagation from the hit/cluster on the META detector
//  Float_t getMETAdz(void)  - z-deviation of track trajectory of Runge Kutta propagation from the hit/cluster on the META detector
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "hrktrackB.h"
using namespace std;
#include <iostream>

ClassImp(HRKTrackB)

HRKTrackB::HRKTrackB(){
    chiq        = -1.0;
    dxRkMeta    = -10000.0;
    dyRkMeta    = -10000.0;
    dzRkMeta    = -10000.0;
    zSeg1RK     = -1000.;
    rSeg1RK     = -1000;
    thetaSeg1RK = 0.;
    phiSeg1RK   = 0.;
    zSeg2RK     = -1000.;
    rSeg2RK     = -1000;
    thetaSeg2RK = 0.;
    phiSeg2RK   = 0.;
    qualityRpc = -1.;
    qualityShower = -1.;
    qualityTof = -1.;
}
