//*-- AUTHOR : A. Rustamov

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HMdcTrackGContFact
//
//  Factory for the parameter containers in libmdctrackG
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hmdctrackgcontfact.h"
#include "hruntimedb.h"
#include "hparset.h"
#include "hparamlist.h"
#include "hcontfact.h"
#include "hmdctrackgcorrpar.h"
#include "hmdctrackgfieldpar.h"
#include "hmetamatchpar.h"
#include "henergylosscorrpar.h"
#include <iostream> 
#include <iomanip>

ClassImp(HMdcTrackGContFact)

static HMdcTrackGContFact gHMdcTrackGContFact;

HMdcTrackGContFact::HMdcTrackGContFact() {
  // Constructor (called when the library is loaded)
  fName ="MdcTrackGContFact";
  fTitle="Factory for parameter containers in libMdcTrackG";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);  
}

void HMdcTrackGContFact::setAllContainers() {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the MdcTrackG library.
  containers->Add(new HContainer("MdcTrackGCorrPar",
			         "parameters for Splinemomentum fit",
                                 "MdcTrackGCorrParProduction"));
  containers->Add(new HContainer("MdcTrackGFieldPar",
				 "Fieldmap",
                                 "MdcTrackGFieldParProduction"));
  HContainer* cm=new HContainer("MetaMatchPar",
				"Parameters for HMetaMatchF",
                                "MetaMatchProductionCuts");
  cm->addContext("MetaMatchSharpCuts");
  cm->addContext("MetaMatchWideCuts");
  containers->Add(cm);    
  containers->Add(new HContainer("EnergyLossCorrPar",
				 "Energy losses correction parameters",
                                 "EnergyLossCorrParProduction"));
}

HParSet* HMdcTrackGContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name,"MdcTrackGCorrPar")==0)
      return new HMdcTrackGCorrPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcTrackGFieldPar")==0)
      return new HMdcTrackGFieldPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MetaMatchPar")==0)
      return new HMetaMatchPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());  
  if (strcmp(name,"EnergyLossCorrPar")==0)
      return new HEnergyLossCorrPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  return 0;
} 
