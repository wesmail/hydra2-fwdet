#ifndef HEMCCONTFACT_H
#define HEMCCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HEmcContFact : public HContFact {
private:
  void setAllContainers(void);
public:
  HEmcContFact(void);
  ~HEmcContFact(void) {}
  HParSet* createContainer(HContainer*);
  ClassDef(HEmcContFact,0) // Factory for parameter containers in libEmc
};

#endif  /* !HEMCCONTFACT_H */
