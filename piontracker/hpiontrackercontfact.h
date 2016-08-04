#ifndef HPIONTRACKERCONTFACT_H
#define HPIONTRACKERCONTFACT_H

#include "hcontfact.h"

class HContainer;
class HParSet;

class HPionTrackerContFact : public HContFact {
private:
   void setAllContainers();
public:
   HPionTrackerContFact();
   ~HPionTrackerContFact() {}
   HParSet* createContainer(HContainer*);
   ClassDef(HPionTrackerContFact, 0) // Factory for parameter containers in libPionTracker
};

#endif  /* !HPIONTRACKERCONTFACT_H */
