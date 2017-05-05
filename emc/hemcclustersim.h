#ifndef HEMCCLUSTERSIM_H
#define HEMCCLUSTERSIM_H

#include "hemccluster.h"

class HEmcClusterSim : public HEmcCluster {
protected:
  Short_t nTracks;        // number of tracks in the list
  Short_t totMult;        // total number of tracks in the cluster
  Int_t   rpcTrack;       // GEANT track number of matched RPC cluster
  Int_t   listTracks[5];  // list of GEANT track numbers sorted by energy
  Float_t trackEnergy[5]; // energy deposit for the each track
  
public:
  HEmcClusterSim() :
    nTracks(0),
    totMult(0),
    rpcTrack(0)
  {
    for(Int_t i=0;i<5;i++) {
      listTracks[i]  = 0;
      trackEnergy[i] = 0.F;
    }
  }

  ~HEmcClusterSim() {}
    
  void    setTrack(Int_t trackNumber, Float_t energy);
  void    setRpcTrack(Int_t tr) {rpcTrack = tr;}
 
  Short_t getNTracks(void)          const  {return nTracks;}
  Int_t   getTrack(Short_t n=0)     const  {return n>=0&&n<nTracks ? listTracks[n]  : 0;}
  Float_t getTrackEnergy(Short_t n) const  {return n>=0&&n<nTracks ? trackEnergy[n] : 0.F;}
  Short_t getTotMult(void)          const  {return totMult;}
  Int_t   getRpcTrack(void)         const  {return rpcTrack;}
  
  ClassDef(HEmcClusterSim,1) // EMC cluster data class
};

#endif /* !HEMCCLUSTER_H */
