#ifndef HPIONTRACKERCAL_H
#define HPIONTRACKERCAL_H

#include "hpiontrackerraw.h"

class HPionTrackerCal : public HPionTrackerRaw 
{
 public:
   static  Int_t InvalidAdc() {return -10000;}

  ClassDef(HPionTrackerCal, 1) // calibrated data of PionTracker
};

#endif /* ! HPIONTRACKERCAL_H */
