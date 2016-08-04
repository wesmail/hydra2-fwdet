//*--- AUTHOR: Vladimir Pechenov
//*--- Modified: Vladimir Pechenov 05/04/2005

using namespace std;
#include "hmdcclusfitsim.h"
#include <iostream> 
#include <iomanip>
#include "hphysicsconstants.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HMdcClusFitSim
//
// Container class keep geant data for HMdcClusFit
//
//////////////////////////////////////////////////////////////////////////////




ClassImp(HMdcClusFitSim)

HMdcClusFitSim::HMdcClusFitSim() {
  geantTrack=-1;
  nTracks=nTracksClus=nTrWires=nTrWiresClus=0;
  direction=kTRUE;
  particleID=0;
  momentum=0.;
  clear();
}

void HMdcClusFitSim::printSimVsRec(void) const {
  printf(" GEANT  Track %i ID=%i",geantTrack,particleID);
  if(particleID>0) printf(" (%s)",HPhysicsConstants::pid(particleID));
  printf(" p=%.0f MeV/c:  %i wires passed fit from %i in cluster.\n",
      momentum,nTrWires,nTrWiresClus);
  printf(" Param: %9.3f %9.3f %9.3f %9.3f\n",x1,y1,x2,y2);
  printf(" Geant: %9.3f %9.3f %9.3f %9.3f\n",x1geant,y1geant,x2geant,y2geant);
  printf(" G.-R.: %9.3f %9.3f %9.3f %9.3f\n",x1geant-x1,y1geant-y1,x2geant-x2,
      y2geant-y2);
}

void HMdcClusFitSim::print() const {
  HMdcClusFit::print();
  printf(" GEANT Track line:    (%8.2f,%8.2f,%7.2f) - (%8.2f,%8.2f,%8.2f)\n",
      x1geant,y1geant,z1geant,x2geant,y2geant,z2geant);
  printf(" GEANT: %itr. in cluster, %itr. in fit output.\n",nTracksClus,nTracks);
  printf(" GEANT  Track %i ID=%i",geantTrack,particleID);
  if(particleID>0) printf(" (%s)",HPhysicsConstants::pid(particleID));
  printf(" p=%.0f MeV/c:  %i wires passed fit from %i in cluster.\n",
      momentum,nTrWires,nTrWiresClus);
}

void HMdcClusFitSim::setFakeTrack(Bool_t dir) {
  x1geant=y1geant=z1geant=x2geant=y2geant=z2geant=-10000.;
  direction=dir;
}
