#ifndef __HPARTICLECONTFACT_H__
#define __HPARTICLECONTFACT_H__

#include "hcontfact.h"

class HParSet;
class HContainer;

class HParticleContFact : public HContFact {
private:
    void setAllContainers();
public:
    HParticleContFact();
    ~HParticleContFact() {}
    HParSet* createContainer(HContainer*);
    ClassDef(HParticleContFact,0) // Factory for all Particle parameter containers
};

#endif  /* !__HPARTICLECONTFACT_H__ */
