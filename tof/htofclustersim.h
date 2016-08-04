#ifndef HTOFCLUSTERSIM_H
#define HTOFCLUSTERSIM_H

#include "htofcluster.h"
#include "htofhitsim.h"

#define MAXPARTICIPANTS 3

class HTofClusterSim : public HTofCluster {
protected:
  Int_t nParticipants;
  Int_t nTrack1[MAXPARTICIPANTS];   // number of first Track in cluster participant
  Int_t nTrack2[MAXPARTICIPANTS];   // number of second Track in cluster participant

public:
  HTofClusterSim(void);
  HTofClusterSim(HTofHitSim *hit);
  HTofClusterSim(HTofHitSim *hit, Int_t cls, Float_t clpa, Float_t clpl);
  ~HTofClusterSim(void);
  void clear(void);

  void incNParticipants() {nParticipants++;}
  void setNParticipants(Int_t n) {nParticipants = n;}
  Int_t getNParticipants() {return nParticipants;}
  inline void setNTrack1(Int_t n) {if (nParticipants<=MAXPARTICIPANTS) nTrack1[nParticipants-1]=n;} //set first trk nb of participant
  inline void setNTrack2(Int_t n) {if (nParticipants<=MAXPARTICIPANTS) nTrack2[nParticipants-1]=n;} //set second trk nb of participant
  inline Int_t getNTrack1(Int_t i=0){
    if(i>=0 && i<nParticipants) return nTrack1[i];
    else return -1;
  } //get first trk nb of participant
  inline Int_t getNTrack2(Int_t i=0){
    if(i>=0 && i<nParticipants) return nTrack2[i];
    else return -1;
  } //get second trk nb of participant

  ClassDef(HTofClusterSim,1) // simulated cluster data level of TOF
};

#endif  /* HTOFCLUSTERSIM_H */
