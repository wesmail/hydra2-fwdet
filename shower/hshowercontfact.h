#ifndef HSHOWERCONTFACT_H
#define HSHOWERCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HShowerContFact : public HContFact {
private:
  void setAllContainers();
public:
  HShowerContFact();
  ~HShowerContFact() {}
  HParSet* createContainer(HContainer*);
  ClassDef(HShowerContFact,0) // Factory for parameter containers in libShower
};

#endif  /* !HSHOWERCONTFACT_H */
