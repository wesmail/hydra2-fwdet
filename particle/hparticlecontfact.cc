
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HParticleContFact
//
//  Factory for the parameter containers in libParticle
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hparticlecontfact.h"
#include "hruntimedb.h"
#include "hparticlecandfillerpar.h"
#include "hparticlebtpar.h"


#include <iostream>
#include <iomanip>

ClassImp(HParticleContFact)

    static HParticleContFact gHParticleContFact;

HParticleContFact::HParticleContFact() {
    // Constructor (called when the library is loaded)
    fName="ParticleContFact";
    fTitle="Factory for parameter containers in libParticle";
    setAllContainers();
    HRuntimeDb::instance()->addContFactory(this);
}

void HParticleContFact::setAllContainers() {
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the Particle library.
    HContainer* fillerpar= new HContainer("ParticleCandFillerPar",
					  "parameters for HParticleCandFiller","ParticleCandFillerParProduction");
    HContainer* btpar    = new HContainer("ParticleBtPar",
					  "parameters for HParticleBt","ParticleBtParProduction");
    //fillerpar->addContext("");
    containers->Add(fillerpar);
    containers->Add(btpar);
}

HParSet* HParticleContFact::createContainer(HContainer* c) {
    // Calls the constructor of the corresponding parameter container.
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.
    const Char_t* name=c->GetName();
    if (strcmp(name,"ParticleCandFillerPar")==0)
	return new HParticleCandFillerPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    if (strcmp(name,"ParticleBtPar")==0)
	return new HParticleBtPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    return 0;
}
