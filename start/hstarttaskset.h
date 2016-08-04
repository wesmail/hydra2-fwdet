#ifndef HSTARTTASKSET_H
#define HSTARTTASKSET_H

#include "htaskset.h"

class HStartTaskSet : public HTaskSet {

private:
   Bool_t fSkipEvent;     // Switch to control skipping event w/o found start time. Default is kFALSE
   Bool_t fUseStartHit;   // Turns on/off Start2HitF task

private:
   void parseArguments(TString s1);

public:
   HStartTaskSet(void) :
      HTaskSet(),
      fSkipEvent(kFALSE),
      fUseStartHit(kTRUE) {}
   HStartTaskSet(const Text_t name[], const Text_t title[]) :
      HTaskSet(name, title),
      fSkipEvent(kFALSE),
      fUseStartHit(kTRUE) {}
   ~HStartTaskSet(void) {}

// function to be used for derived task sets (HRichTaskSet, etc.)
   HTask *make(const Char_t *select = "real", const Option_t *option = "");

   ClassDef(HStartTaskSet, 1) // Set of tasks
};

#endif /* !HSTARTTASKSET_H */
