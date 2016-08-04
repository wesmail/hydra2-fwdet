#ifndef HMDCCLUSINFSIM_H
#define HMDCCLUSINFSIM_H

#include "hmdcclusinf.h"

class HMdcClusInfSim : public HMdcClusInf {
protected:
  Short_t nTracks;        // num. of sim. tracks in chamber cluster
  Int_t listTr[5];        // list of tracks
  Short_t nTimes[5];      // num. of hits in chamber cluster from track
public:
  HMdcClusInfSim(void) {nTracks=-1;}
  ~HMdcClusInfSim() {}
  void setTracksList(Int_t nTr, const Int_t* list, const Short_t* nTm);
  void clear(void) {nTracks=-1;}
  virtual void print(void) const;
  Short_t getNTracks(void) const {return nTracks;}
  Int_t getTrack(Int_t indx) const {
    return (indx>=0 && indx<nTracks) ? listTr[indx] : -1;
  }
  Short_t getNTimesInTrack(Int_t indx) const {
    return (indx<0 || indx>=nTracks) ? 0:nTimes[indx];
  }
  
  ClassDef(HMdcClusInfSim,1)
};

#endif
