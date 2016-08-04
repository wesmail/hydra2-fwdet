#ifndef HPIONTRACKERTASKSET_H
#define HPIONTRACKERTASKSET_H

#include "htaskset.h"

class HPionTrackerTaskSet : public HTaskSet {
private:
   Bool_t fSkipEvent;         // Switch to control skipping event w/o found reconstructed time. Default is kFALSE
   Bool_t fUsePionTrackerHit; // Turns on/off PionTrackerHitF task. Default is kTRUE
   Bool_t fUsePionTrackerTrack; // Turns on/off PionTrackerHitF task. Default is kTRUE

   void parseArguments(TString s1);
public:
   HPionTrackerTaskSet(void);
   HPionTrackerTaskSet(const Text_t name[], const Text_t title[]);
   ~HPionTrackerTaskSet(void) {}

   HTask *make(const Char_t *select = "", const Option_t *option = "");

   ClassDef(HPionTrackerTaskSet, 1) // Set of PionTracker tasks
};

#endif /* !HPIONTRACKERTASKSET_H */
