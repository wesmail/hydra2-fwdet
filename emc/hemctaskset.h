#ifndef HEMCTASKSET_H
#define HEMCTASKSET_H

#include "htaskset.h"

class HEmcTaskSet : public HTaskSet {
protected:
  Bool_t doEmcCal;      // calibrater (exp), digitizer (sim)
  Bool_t doEmcClusF;

public:
  HEmcTaskSet(void) : HTaskSet(), doEmcCal(kTRUE), doEmcClusF(kTRUE) {}
  HEmcTaskSet(Text_t name[], Text_t title[]) : HTaskSet(name,title) {}
  ~HEmcTaskSet(void) {}
  HTask* make(const Char_t* select="", Option_t* option="");

protected:
  void parseArguments(TString s);

  ClassDef(HEmcTaskSet,0) // Set of tasks for the EMC
};

#endif /* !HEMCTASKSET_H */
