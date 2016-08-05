//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HPionTrackerContFact
//
//  Factory for the parameter containers in libPionTracker
//
/////////////////////////////////////////////////////////////

#include "hpiontrackercontfact.h"
#include "hruntimedb.h"
#include "hpiontrackertrb3lookup.h"
#include "hpiontrackercalpar.h"
#include "hpiontrackerhitfpar.h"
#include "hpiontrackertrackfpar.h"
#include "hpiontrackerbeampar.h"
#include "hpiontrackercalrunpar.h"
#include "hpiontrackermomrunpar.h"
#include "hpiontrackermompar.h"
#include "hpiontrackergeompar.h"

ClassImp(HPionTrackerContFact)

static HPionTrackerContFact gHPionTrackerContFact;

HPionTrackerContFact::HPionTrackerContFact() {
  // Constructor (called when the library is loaded)
  fName = "PionTrackerContFact";
  fTitle = "Factory for parameter containers in libPionTracker";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HPionTrackerContFact::setAllContainers() {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the PionTracker library.
  containers->Add(
    new HContainer("PionTrackerTrb3Lookup",
                   "Lookup table for the TRB3 unpacker of the PionTracker",
                   "PionTrackerTrb3LookupProduction"));
  containers->Add(
    new HContainer("PionTrackerCalPar",
                   "Calibration parameters of the PionTracker",
                   "PionTrackerCalParProduction"));
  containers->Add(
    new HContainer("PionTrackerHitFPar",
                   "Hit finder parameters of the PionTracker",
                   "PionTrackerHitFParProduction"));
  containers->Add(
    new HContainer("PionTrackerTrackFPar",
                   "Track finder parameters of the PionTracker",
                   "PionTrackerTrackFParProduction"));
  containers->Add(
    new HContainer("PionTrackerBeamPar",
                   "Beam parameters of the PionTracker",
                   "PionTrackerBeamParProduction"));
  containers->Add(
    new HContainer("PionTrackerGeomPar",
                   "Geometry parameters of the PionTracker",
                   "GeomProduction"));
  containers->Add(
    new HContainer("PionTrackerCalRunPar",
                   "Run by Run cal parameters of the PionTracker",
                   "PionTrackerCalRunParProduction"));
  containers->Add(
    new HContainer("PionTrackerMomPar",
                   "Mom calibration parameters of the PionTracker",
                   "PionTrackerMomParProduction"));

  containers->Add(
    new HContainer("PionTrackerMomRunPar",
                   "Run by Run mom parameters of the PionTracker",
                   "PionTrackerMomRunParProduction"));
}

HParSet* HPionTrackerContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name = c->GetName();
   if (0 == strncmp(name, "PionTrackerTrb3Lookup", strlen("PionTrackerTrb3Lookup")))
      return new HPionTrackerTrb3Lookup(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerCalPar", strlen("PionTrackerCalPar")))
      return new HPionTrackerCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerHitFPar", strlen("PionTrackerHitFPar")))
      return new HPionTrackerHitFPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerTrackFPar", strlen("PionTrackerTrackFPar")))
      return new HPionTrackerTrackFPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerBeamPar", strlen("PionTrackerBeamPar")))
      return new HPionTrackerBeamPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerCalRunPar", strlen("PionTrackerCalRunPar")))
      return new HPionTrackerCalRunPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerMomRunPar", strlen("PionTrackerMomRunPar")))
      return new HPionTrackerMomRunPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerMomPar", strlen("PionTrackerMomPar")))
      return new HPionTrackerMomPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "PionTrackerGeomPar", strlen("PionTrackerGeomPar")))
      return new HPionTrackerGeomPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   return 0;
}
