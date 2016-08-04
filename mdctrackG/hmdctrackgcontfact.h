#ifndef HMDCTRACKGCONTFACT_H
#define HMDCTRACKGCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

/** Factory for all MdcTrackG parameter containers.
*/

class HMdcTrackGContFact : public HContFact {
private:
  void setAllContainers();
public:
  HMdcTrackGContFact();
  ~HMdcTrackGContFact() {}
  HParSet* createContainer(HContainer*); 
  ClassDef(HMdcTrackGContFact,0) // Factory for all MdcTrackG parameter containers
};

#endif  /* !HMDCTRACKGCONTFACT_H */
