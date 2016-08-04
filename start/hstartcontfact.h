#ifndef HSTARTCONTFACT_H
#define HSTARTCONTFACT_H

#include "hcontfact.h"

class HContainer;
class HParSet;

class HStartContFact : public HContFact {
private:
   void setAllContainers();
public:
   HStartContFact();
   ~HStartContFact() {}
   HParSet* createContainer(HContainer*);
   ClassDef(HStartContFact, 0) // Factory for parameter containers in libStart
};

#endif  /* !HSTARTCONTFACT_H */
