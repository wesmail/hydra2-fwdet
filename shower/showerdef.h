#ifndef __ShowerDEF_H
#define __ShowerDEF_H

#include "haddef.h"


#define SH0WER_MAX_TRACK 8    // max number of geant tracks stored to HShowerHitTrack/HShowerHitTofTrack

const Cat_t catShowerRaw       = SHOWER_OFFSET;
const Cat_t catShowerHit       = SHOWER_OFFSET+1;
const Cat_t catShowerCal       = SHOWER_OFFSET+2;
const Cat_t catShowerGeantWire = SHOWER_OFFSET+3;
const Cat_t catShowerPID       = SHOWER_OFFSET+4;
const Cat_t catShowerTrack     = SHOWER_OFFSET+5;
const Cat_t catShowerRawMatr   = SHOWER_OFFSET+6;
const Cat_t catShowerHitHdr    = SHOWER_OFFSET+7;
//const Cat_t catShowerHitTrack  = SHOWER_OFFSET+8;  // obsolete
const Cat_t catShowerPIDTrack  = SHOWER_OFFSET+9;
const Cat_t catShowerHitTof      = SHOWER_OFFSET+14; // 78
const Cat_t catShowerHitTofTrack = SHOWER_OFFSET+15; // 79 

#endif
