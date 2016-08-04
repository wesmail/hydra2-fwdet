#include "hwallhitsim.h"

// Created 18.06.00 by M.Golubeva
// Modified 11.8.2005 F. Krizek


ClassImp(HWallHitSim)

HWallHitSim::HWallHitSim(void) { clear(); }

void HWallHitSim::clear(void) {
    timeCal   = -200.;
    chargeCal = -200.;
    cell      = -1;
    nTrack1 = nTrack2 = -1;
}
