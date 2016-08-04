//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HEmcContFact
//
//  Factory for the parameter containers in libEmc
//
/////////////////////////////////////////////////////////////

#include "hemccontfact.h"
#include "hruntimedb.h"
#include "hemctrb3lookup.h"
#include "hemccalpar.h"
#include "hemcgeompar.h"
#include "hemccellgeompar.h"
#include "hemcdigipar.h"

ClassImp(HEmcContFact)

  static HEmcContFact gHEmcContFact;

HEmcContFact::HEmcContFact(void) {
  // Constructor (called when the library is loaded)
  fName="EmcContFact";
  fTitle="Factory for parameter containers in libEmc";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HEmcContFact::setAllContainers(void) {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the Emc library.
  containers->Add(
    new HContainer("EmcTrb3Lookup",
                   "Lookup table for Emc TRB3 unpacker",
                   "EmcTrb3LookupProduction"));
  containers->Add(
    new HContainer("EmcCalPar",
                   "Calibration parameters of the Emc Detector",
                   "EmcCalParProduction"));
  containers->Add(
    new HContainer("EmcGeomPar",
		   "Geometry parameters of the Emc Detector",
		   "GeomProduction"));
  containers->Add(
    new HContainer("EmcCellGeomPar",
		   "Emc cell geometry parameters",
		   "GeomProduction"));
  containers->Add(
    new HContainer("EmcDigiPar",
		   "Emc digitization parameters",
		   "EmcDigiProduction"));
}

HParSet* HEmcContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name, "EmcTrb3Lookup") == 0)
    return new HEmcTrb3Lookup(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  if (strcmp(name, "EmcCalPar") == 0)
    return new HEmcCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
  if (strcmp(name,"EmcGeomPar")==0)
    return new HEmcGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"EmcCellGeomPar")==0)
    return new HEmcCellGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"EmcDigiPar")==0)
    return new HEmcDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  return 0;
}

