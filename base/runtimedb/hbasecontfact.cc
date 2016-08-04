//*-- AUTHOR : Ilse Koenig
//*-- Modified last : 06/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HBaseContFact
//
//  Factory for the parameter containers in libHydra 
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hbasecontfact.h"
#include "hruntimedb.h"
#include "hspecgeompar.h"
#include "hmagnetpar.h"
#include "htrblookup.h"
#include "htrbnetaddressmapping.h"
#include "hslowpar.h"
#include "hemcsimulpar.h"
#include <iostream>
#include <iomanip>

ClassImp(HBaseContFact)

static HBaseContFact gHBaseContFact;

HBaseContFact::HBaseContFact() {
  // Constructor (called when the library is loaded)
  fName="BaseContFact";
  fTitle="Factory for parameter containers in libHydra";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);  
}

void HBaseContFact::setAllContainers() {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers in libHydra.
  containers->Add(new HContainer(
                    "SpecGeomPar",
                    "Geometry of cave, sectors and target",
                    "GeomProduction"));
  HContainer* cm=new HContainer(
                    "MagnetPar",
                    "Magnet current",
                    "MagnetCurrentSetValues");
  cm->addContext("MagnetCurrentReadbackValues");
  containers->Add(cm);  
  containers->Add(new HContainer(
                    "TrbLookup",
                    "Lookup table for TRB unpacker",
                    "TrbLookupProduction"));
  containers->Add(new HContainer(
                    "TrbnetAddressMapping",
                    "Mapping of subevent ids to TRB boards",
                    "Trb2Production"));

  containers->Add(new HContainer(
                    "SlowPar",
                    "slow control parameters",
                    "SlowParProduction"));
  containers->Add(new HContainer(
                    "EmcSimulPar",
                    "Emc simulation parameters",
                    "EmcSimulProduction"));
}

HParSet* HBaseContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name,"SpecGeomPar")==0) {
    return new HSpecGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"MagnetPar")==0) {
    return new HMagnetPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"TrbLookup")==0) {
    return new HTrbLookup(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"TrbnetAddressMapping")==0) {
    return new HTrbnetAddressMapping(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"SlowPar")==0) {
    return new HSlowPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"EmcSimulPar")==0) {
    return new HEmcSimulPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return 0;
} 
