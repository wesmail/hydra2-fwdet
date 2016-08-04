#ifndef HTOFCLUSTER_H
#define HTOFCLUSTER_H

#include "htofhit.h"    	

class HTofCluster : public HTofHit {
private:
Int_t clustSize;  // number of hits in the cluster.
Float_t clustProbAll;  // cluster probability (all found clusters).
Float_t clustProbLep;  // cluster probability (lepton found clusters).
Int_t   indexhit2;     // linear index of second hit
public:
  HTofCluster() { ;}
  HTofCluster(HTofHit *hit);
  HTofCluster(HTofHit *hit, Int_t cls, Float_t clpa, Float_t clpl);
  ~HTofCluster() { ;}
  Int_t getClusterSize(void) {return clustSize;}
  Float_t getClusterProbAll(void) {return clustProbAll;}
  Float_t getClusterProbLep(void) {return clustProbLep;}
  Int_t getIndexHit2(void)        {return indexhit2;}
  void setClusterSize(Int_t clustsize) {clustSize = clustsize;}
  void setClusterProbAll(Float_t clustproball) {clustProbAll = clustproball;}
  void setClusterProbLep(Float_t clustproblep) {clustProbLep = clustproblep;}
  void setIndexHit2(Int_t ind) {indexhit2 = ind;}
  ClassDef(HTofCluster,4) //Class with data for a TOF cluster
};

#endif /* !HTOFCLUSTER_H */

