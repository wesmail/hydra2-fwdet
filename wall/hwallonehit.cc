#include "hwallonehit.h"

// Created by M.Golubeva 01.11.2006

ClassImp(HWallOneHit)

    HWallOneHit::HWallOneHit(void) { clear(); }

    void HWallOneHit::setAddress(const Int_t c) {      
       cell=c;
    }

void HWallOneHit::fill(const Float_t t, const Float_t a, const Int_t c) {
       timeHit=t;
       chargeHit=a;     
       cell=c;    
    }
    void HWallOneHit::getAddress(Int_t& c) {     
       c=cell;
    }

    void HWallOneHit::clear(void) {
       timeHit=-200;
       chargeHit=-200;      
       cell=-1;      
    }
