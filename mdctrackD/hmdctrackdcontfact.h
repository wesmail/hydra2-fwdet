#ifndef HMDCTRACKDCONTFACT_H
#define HMDCTRACKDCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HMdcTrackDContFact : public HContFact {
private:
  void setAllContainers();
public:
  HMdcTrackDContFact();
  ~HMdcTrackDContFact() {}
  HParSet* createContainer(HContainer*);
  ClassDef(HMdcTrackDContFact,0) // Factory for all Dubna tracking parameter containers
};

#endif  /* !HMDCTRACKDCONTFACT_H */
