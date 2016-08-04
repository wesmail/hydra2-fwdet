#include "hwallhit.h"

// Created 18.06.00 by M.Golubeva
// Modified 11.8.2005 F. Krizek


ClassImp(HWallHit)

HWallHit::HWallHit(void) { clear(); }


void HWallHit::fill(const Float_t t, const Float_t a, const Int_t c) {
    timeCal   = t;
    chargeCal = a;
    cell      = c;
}

void HWallHit::clear(void) {
    timeCal  = -200.;
    chargeCal= -200.;
    cell     = -1;
}
