#ifndef HTOFCONTFACT_H
#define HTOFCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HTofContFact : public HContFact {
private:
  void setAllContainers();
public:
  HTofContFact();
  ~HTofContFact() {}
  HParSet* createContainer(HContainer*);
  ClassDef(HTofContFact,0) // Factory for parameter containers in libTof
};

#endif  /* !HTOFCONTFACT_H */
