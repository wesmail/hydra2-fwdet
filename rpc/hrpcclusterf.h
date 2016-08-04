#ifndef HRPCCLUSTERF_H
#define HRPCCLUSTERF_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HRpcGeomCellPar;
class HRpcClusFPar;
class HRpcClusterSim;

class HRpcClusterF : public HReconstructor {

protected:
  HCategory *pHitCat;              // pointer to the hit data
  HCategory *pClusterCat;          // pointer to the cluster data
  HCategory *pGeantRpcCat;         // pointer to the GeantRpc data
  HLocation loc;                   // location for hit
  HLocation loc_cluster;           // location for cluster
  HRpcGeomCellPar* pGeomCellPar;   // rpc cell geometry parameters
  HRpcClusFPar* pClusFPar;         // cluster finder parameters

  Int_t  maxcells;                 // Maximum number of cells

  Bool_t simulation;               // Flag to decide whether simulation(1) or analisis(0)
  Bool_t doSingleCluster;          // kTRUE : fill clusters as copy from hits (default kFALSE)
  Int_t howManyTracks       (HRpcClusterSim* clus);
  Int_t howManyTracksAtCells(HRpcClusterSim* clus);

public:
  HRpcClusterF();
  HRpcClusterF(const Text_t* name,const Text_t* title);
  ~HRpcClusterF();
  void   setDoSingleClusters(Bool_t single=kFALSE)   { doSingleCluster = single ;}
  void   initParContainer();
  Bool_t init();
  Bool_t finalize() { return kTRUE; }
  Int_t  execute();

  ClassDef(HRpcClusterF,0)      // Cluster Finder hit->cluster for RPC data
};

#endif /* !HRPCCLUSTERF_H */

