#ifndef HWALLHITSIM_H
#define HWALLHITSIM_H

#include "TObject.h"
#include "hwallhit.h"

//F. Krizek 11.8.2005

class HWallHitSim : public HWallHit {
protected:
    Int_t nTrack1;   // number of Track for first hit
    Int_t nTrack2;   // number of Track for second hit
public:
    HWallHitSim(void);
    ~HWallHitSim(void) {;}

    inline void  setNTrack1(const Int_t n) { nTrack1 = n;    } //set trk nb of hit 1
    inline void  setNTrack2(const Int_t n) { nTrack2 = n;    } //set trk nb of hit 2
    inline Int_t getNTrack1(void) const    { return nTrack1; } //get trk nb of hit 1
    inline Int_t getNTrack2(void) const    { return nTrack2; } //get trk nb of hit 2


    void clear(void);
    ClassDef(HWallHitSim,1)  // WALL detector calibrated data
};

#endif /* ! HWALLHITSIM_H */
