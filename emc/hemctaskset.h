#ifndef HEMCTASKSET_H
#define HEMCTASKSET_H

#include "htaskset.h"

class HEmcTaskSet : public HTaskSet {
public:
  HEmcTaskSet(void) : HTaskSet() {}
  HEmcTaskSet(Text_t name[], Text_t title[]) : HTaskSet(name,title) {}
  ~HEmcTaskSet(void) {}
  HTask* make(const Char_t* select="", Option_t* option="");
  ClassDef(HEmcTaskSet,0) // Set of tasks for the EMC
};

#endif /* !HEMCTASKSET_H */
