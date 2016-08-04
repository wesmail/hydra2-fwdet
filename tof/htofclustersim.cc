#include "htofclustersim.h"

// Author: Dusan Zovinec (23.10.2002)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////
// HTofClusterSim
//
// cluster level of the ToF data reconstruction for simulation
///////////////////////////////////////////////////////////////

ClassImp(HTofClusterSim)

HTofClusterSim::HTofClusterSim(void) {
  clear();
}

HTofClusterSim::~HTofClusterSim(void) {;}

void HTofClusterSim::clear(void) {
  nParticipants = 0;
  for(Int_t i=0;i<MAXPARTICIPANTS;i++) {
    nTrack1[i] = -1;
    nTrack2[i] = -1;
  }
}

HTofClusterSim::HTofClusterSim(HTofHitSim *hit){
// HTofCluster Constructor
//
// Creates HTofCluster object and makes the copy of HTofHit object
// data members. The rest of the data members of HTofCluster object
// is set to:
//   clustSize=1
//   clustProbAll=1.0
//   clustProbLep=1.0
//

  setTof(hit->getTof());
  setXpos(hit->getXpos());
  setXposAdc(hit->getXposAdc());
  setEdep(hit->getEdep());
  setLeftAmp(hit->getLeftAmp());
  setRightAmp(hit->getRightAmp());
  Float_t xl,yl,zl,d,ph,th;
  hit->getXYZLab(xl,yl,zl);
  setXYZLab(xl,yl,zl);
  hit->getDistance(d);
  setDistance(d);
  hit->getPhi(ph);
  setPhi(ph);
  hit->getTheta(th);
  setTheta(th);
  setSector(hit->getSector());
  setModule(hit->getModule());
  setCell(hit->getCell());
  setAdcFlag(hit->getAdcFlag());
  setNParticipants(1);
  setNTrack1(hit->getNTrack1());
  setNTrack2(hit->getNTrack2());

  setClusterSize(1);
  setClusterProbAll(1.0);
  setClusterProbLep(1.0);
}

HTofClusterSim::HTofClusterSim(HTofHitSim *hit, Int_t cls, Float_t clpa, Float_t clpl){
// HTofCluster Constructor
//
// Creates HTofCluster object and makes the copy of HTofHit object
// data members. The rest of the data members of HTofCluster object
// is set to:
//   clustSize=cls
//   clustProbAll=clpa
//   clustProbLep=clpl
//

  setTof(hit->getTof());
  setXpos(hit->getXpos());
  setXposAdc(hit->getXposAdc());
  setEdep(hit->getEdep());
  setLeftAmp(hit->getLeftAmp());
  setRightAmp(hit->getRightAmp());
  Float_t xl,yl,zl,d,ph,th;
  hit->getXYZLab(xl,yl,zl);
  setXYZLab(xl,yl,zl);
  hit->getDistance(d);
  setDistance(d);
  hit->getPhi(ph);
  setPhi(ph);
  hit->getTheta(th);
  setTheta(th);
  setSector(hit->getSector());
  setModule(hit->getModule());
  setCell(hit->getCell());
  setAdcFlag(hit->getAdcFlag());
  setNParticipants(1);
  setNTrack1(hit->getNTrack1());
  setNTrack2(hit->getNTrack2());

  setClusterSize(cls);
  setClusterProbAll(clpa);
  setClusterProbLep(clpl);
}


