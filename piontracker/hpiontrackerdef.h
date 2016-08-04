#ifndef PIONTRACKERDEF_H
#define PIONTRACKERDEF_H

#include "haddef.h"

const Cat_t catPionTrackerRaw = PIONTRACKER_OFFSET + 1;
const Cat_t catPionTrackerCal = PIONTRACKER_OFFSET + 2;
const Cat_t catPionTrackerHit = PIONTRACKER_OFFSET + 3;
const Cat_t catPionTrackerTrack = PIONTRACKER_OFFSET + 4;

const Int_t PIONTRACKER_MAX_MODULES		= 10;
const Int_t PIONTRACKER_MAX_COMPONENTS		= 128;

#endif /* !PIONTRACKERDEF_H */
