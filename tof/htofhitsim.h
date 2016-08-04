#ifndef HTOFHITSIM_H
#define HTOFHITSIM_H

#include "htofhit.h"

class HTofHitSim : public HTofHit {
protected:
  Int_t nTrack1;   // number of Track for first hit
  Int_t nTrack2;   // number of Track for second hit

public:
  HTofHitSim(void) : nTrack1(-1), nTrack2(-1) {}
  ~HTofHitSim(void) {}
  void clear(void);
  inline void setNTrack1(const Int_t n) {nTrack1=n;} //set trk nb of hit 1
  inline void setNTrack2(const Int_t n) {nTrack2=n;} //set trk nb of hit 2
  inline void setLeftNTrack(const Int_t n) {nTrack1=n;} //set trk nb of hit 1
  inline void setRightNTrack(const Int_t n) {nTrack2=n;} //set trk nb of hit 2
  inline Int_t getNTrack1(void) const {return nTrack1;} //get trk nb of hit 1
  inline Int_t getNTrack2(void) const {return nTrack2;} //get trk nb of hit 2 
  inline Int_t getLeftNTrack(void) const {return nTrack1;} //get trk nb of hit 1
  inline Int_t getRightNTrack(void) const {return nTrack2;} //get trk nb of hit 2
  ClassDef(HTofHitSim,1) // simulated hit data level of TOF
};


class HTofHitSimTmp : public HTofHitSim {
public:
    ClassDef(HTofHitSimTmp,1) // simulated hit data level of TOF
};


#endif  /* HTOFHITSIM_H */
