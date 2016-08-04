#ifndef HRPCCONTFACT_H
#define HRPCCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HRpcContFact : public HContFact {
private:
  void setAllContainers(void);
public:
  HRpcContFact(void);
  ~HRpcContFact(void) {}
  HParSet* createContainer(HContainer*);
  ClassDef(HRpcContFact,0) // Factory for parameter containers in libRpc
};

#endif  /* !HRPCCONTFACT_H */
