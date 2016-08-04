#ifndef HMDCCONTFACT_H
#define HMDCCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HMdcContFact : public HContFact {
private:
  void setAllContainers();
public:
  HMdcContFact();
  ~HMdcContFact() {}
  HParSet* createContainer(HContainer*); 
  ClassDef(HMdcContFact,0) // Factory for all MDC parameter containers
};

#endif  /* !HMDCCONTFACT_H */
