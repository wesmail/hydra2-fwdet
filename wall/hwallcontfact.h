#ifndef HWALLCONTFACT_H
#define HWALLCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HWallContFact : public HContFact {
private:
  void setAllContainers(void);
public:
  HWallContFact(void);
  ~HWallContFact(void) {}
  HParSet* createContainer(HContainer*);
  ClassDef(HWallContFact,0) // Factory for parameter containers in libWall
};

#endif  /* !HWALLCONTFACT_H */
