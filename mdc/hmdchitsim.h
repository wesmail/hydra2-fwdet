#ifndef HMDCHITSIM_H
#define HMDCHITSIM_H

#include "hmdchit.h"

class HMdcHitSim : public HMdcHit {
  protected:
    Int_t nTracks;        // number of tracks in the list of tracks
    Int_t listTracks[5];  // list of tracks
    UChar_t nTimes[5];    // num. of drift times from each track
    Int_t status;         // status of hit (e.g. 1 if ok)
  public:
    HMdcHitSim(void){clear();}
    ~HMdcHitSim(void) {;}
    void  clear(void);
    void  setStatus(Int_t f) { status=f; }
    void  setNTracks(Int_t nTr, const Int_t* listTr, const UChar_t* nTm);
    void  setNumNoiseWires(Int_t n);
    Int_t calcNTracks(void);
    
    inline Int_t   getStatus(void) const        {return status;}
    inline Int_t   getNTracks(void) const       {return nTracks;}
    inline Int_t   getTrack(Int_t n) const;
    inline UChar_t getNTimes(Int_t n) const;
    inline Int_t   getNumNoiseWires(void) const;

    void print(void);

  ClassDef(HMdcHitSim,1) // simulated hit on a MDC 
};

inline Int_t HMdcHitSim::getTrack(Int_t n) const {
  return (n>=0 && n<nTracks) ? listTracks[n] : -1;
}

inline UChar_t HMdcHitSim::getNTimes(Int_t n) const {
  return (n>=0 && n<nTracks) ? nTimes[n]:-1;
}

inline Int_t HMdcHitSim::getNumNoiseWires(void) const {
  return (listTracks[4]==-99) ? nTimes[4] : 0;
}
 
#endif  /* HMDCHITSIM_H */
