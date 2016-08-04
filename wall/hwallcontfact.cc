//*-- AUTHOR : Ilse Koenig
//*-- Created : 17/01/2005
// Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HWallContFact
//
//  Factory for the parameter containers in libWall
//
/////////////////////////////////////////////////////////////

#include "hwallcontfact.h"
#include "hruntimedb.h"
#include "hwalllookup.h"
#include "hwalltrb2lookup.h"
#include "hwallcalpar.h"
#include "hwalldigipar.h"
#include "hwallgeompar.h"
#include "hwallrefwinpar.h"
#include "hwalleventplanepar.h"

ClassImp(HWallContFact)

static HWallContFact gHWallContFact;

HWallContFact::HWallContFact(void) {
  // Constructor (called when the library is loaded)
  fName="WallContFact";
  fTitle="Factory for parameter containers in libWall";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HWallContFact::setAllContainers(void) {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the Wall library.
  containers->Add(
    new HContainer("WallLookup",
                   "Unpacker lookup table for the Forward Wall",
                   "WallLookupProduction"));
  containers->Add(
    new HContainer("WallTrb2Lookup",
                   "Lookup table for the TRB2 unpacker of the Forward Wall",
                   "WallTrb2LookupProduction"));
  containers->Add(
    new HContainer("WallCalPar",
                   "Calibration parameters for Forward Wall",
                   "WallCalProduction"));
  containers->Add(
    new HContainer("WallDigiPar",
                   "Digitization parameters for Forward Wall",
                   "WallDigiProduction"));
  containers->Add(
    new HContainer("WallRefWinPar",
                   "Reference time windows parameters for Forward Wall",
                   "WallOneHitProduction"));
  containers->Add(
    new HContainer("WallEventPlanePar",
                   "EventPlane parameters for Forward Wall",
                   "WallEventPlaneProduction"));
  containers->Add(
    new HContainer("WallGeomPar",
                   "Geometry parameters of the Forward Wall",
                   "GeomProduction"));
}

HParSet* HWallContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name,"WallLookup")==0)
    return new HWallLookup(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"WallTrb2Lookup")==0)
    return new HWallTrb2Lookup(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"WallCalPar")==0)
    return new HWallCalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"WallDigiPar")==0)
    return new HWallDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"WallRefWinPar")==0)
    return new HWallRefWinPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"WallEventPlanePar")==0)
    return new HWallEventPlanePar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"WallGeomPar")==0)
    return new HWallGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  return 0;
}
