#ifndef HFWDETCONTFACT_H
#define HFWDETCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HFwDetContFact : public HContFact {
private:
  void setAllContainers(void);
public:
  HFwDetContFact(void);
  ~HFwDetContFact(void) {}
  HParSet* createContainer(HContainer*);
  ClassDef(HFwDetContFact,0) // Factory for parameter containers in libFwdet
};

#endif  /* !HFWDETCONTFACT_H */
