//*--- AUTHOR : Vladimir Pechenov

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HMdcWireFit
//
// Container class keep geant and digitizer data for HMdcWireFit
//
//  void setGeantTrackNum(Int_t n)
//                Set GEANT track number.
//  void setGeantMinDist(Float_t v)
//                Set minimal dispance of GEANT track to the wire.
//  void setGeantAlpha(Float_t v)
//                Set impact angle of GEANT track to the wire system.
//  void setDigiTimeErr(Float_t v)
//                Set digitizer drift time errors 
//  void setGeantTof(Float_t v)
//                Set GEANT tof
//  void setClusFitTrFlag(Bool_t f)
//                Set clusFitTrackFl to kTRUE if 
//                trackNum==HMdcClusFitSim::geantTrack
//
//  Int_t   getGeantTrackNum(void)
//                Get GEANT track number.
//  Float_t getGeantMinDist(void)
//                Get minimal dispance of GEANT track to the wire.
//  Float_t getGeantAlpha(void)
//                Get impact angle of GEANT track to the wire system.
//  Float_t getDigiTimeErr(void)
//                Get digitizer drift time errors 
//  Float_t getGeantTof(void)
//                Get GEANT tof
//  Bool_t  isClusFitTrack(void)
//                Return kTRUE if 
//                HMdcWireFir::getGeantTrackNum()==HMdcClusFitSim::geantTrack()
//
// Bool_t isGeant()
//                Return kTRUE
// void print()
//                Print container
//
//////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hmdcwirefitsim.h"
#include <iostream>
#include <iomanip>

ClassImp(HMdcWireFitSim)

void HMdcWireFitSim::print() {
  // Print this container
  HMdcWireFit::print();
  printf("  GEANT:%4itr. %5.2fmm %5.1fdeg. tof=%5.1f\n",trackNum,
      minDistGeant,alphaGeant,tofGeant);
}
