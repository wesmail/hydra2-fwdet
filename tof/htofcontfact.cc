//*-- AUTHOR : Ilse Koenig
//*-- Modified last : 20/02/2002 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HTofContFact
//
//  Factory for the parameter containers in libTof
//
/////////////////////////////////////////////////////////////

#include "htofcontfact.h"
#include "hruntimedb.h"
#include "htofcalpar.h"
#include "htofdigipar.h"
#include "htofgeompar.h"
#include "htoflookup.h"
#include "htoftrb2lookup.h"
#include "htofclusterfpar.h"
#include "htofwalkpar.h"

ClassImp(HTofContFact)

static HTofContFact gHTofContFact;

HTofContFact::HTofContFact() {
  // Constructor (called when the library is loaded)
  fName="TofContFact";
  fTitle="Factory for parameter containers in libTof";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HTofContFact::setAllContainers() {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the Tof library.
  containers->Add(
    new HContainer("TofCalPar",
                   "Calibration parameters for Tof",
                   "TofCalProduction"));
  containers->Add(
    new HContainer("TofDigiPar",
                   "TOF digitization parameters",
                   "TofDigiProduction"));
  containers->Add(
    new HContainer("TofGeomPar",
                   "Tof geometry parameters",
                   "GeomProduction"));
  containers->Add(
    new HContainer("TofLookup",
                   "Lookup table for Tof unpacker",
                   ""));
  containers->Add(
    new HContainer("TofTrb2Lookup",
                   "Lookup table for the TRB2 unpacker of the TOF detector",
                   "TofTrb2LookupProduction"));
  containers->Add(
    new HContainer("TofClusterFPar",
                   "Parameter container for the cluster finder",
                   "TofCluNormalBias"));
  containers->Add(
    new HContainer("TofWalkPar",
                   "Walk Calibration parameters for Tof",
                   "TofWalkParProduction"));
}

HParSet* HTofContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name,"TofCalPar")==0)
    return new HTofCalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofDigiPar")==0)
    return new HTofDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofGeomPar")==0)
    return new HTofGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofLookup")==0)
    return new HTofLookup(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofTrb2Lookup")==0)
    return new HTofTrb2Lookup(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofClusterFPar")==0)
    return new HTofClusterFPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"TofWalkPar")==0)
    return new HTofWalkPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  return 0;
}
