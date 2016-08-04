//*-- AUTHOR : Ilse Koenig
//*-- Modified last : 03/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HStartContFact
//
//  Factory for the parameter containers in libStart
//
/////////////////////////////////////////////////////////////

#include "hruntimedb.h"
#include "hstart2calpar.h"
#include "hstart2hitfpar.h"
#include "hstart2trb2lookup.h"
#include "hstart2calrunpar.h"
#include "hstart2geompar.h"
#include "hstart2digipar.h"
#include "hstart2trb3lookup.h"
#include "hstart2trb3calpar.h"
#include "hstartcontfact.h"

ClassImp(HStartContFact)

static HStartContFact gHStartContFact;

HStartContFact::HStartContFact()
{
   // Constructor (called when the library is loaded)
   fName = "StartContFact";
   fTitle = "Factory for parameter containers in libStart";
   setAllContainers();
   HRuntimeDb::instance()->addContFactory(this);
}

void HStartContFact::setAllContainers()
{
   // Creates the Container objects with all accepted contexts and adds them to
   // the list of containers for the Start library.
   containers->Add(
      new HContainer("Start2Trb2Lookup",
                     "Lookup table for Start2 TRB unpacker",
                     "Start2Trb2LookupProduction"));
   containers->Add(
      new HContainer("Start2Calpar",
                     "Calibration parameters for Start2",
                     "Start2CalparProduction"));
   containers->Add(
      new HContainer("Start2HitFPar",
                     "Start2 Hit Finder Parameters",
                     "Start2HitFParProduction"));
   containers->Add(
      new HContainer("Start2CalRunPar",
                     "Start2 offset corrections per run Parameters",
                     "Start2CalRunParProduction"));
   containers->Add(
      new HContainer("Start2GeomPar",
                     "Geometry parameters of the Start Detector",
                     "GeomProduction"));
   containers->Add(
      new HContainer("Start2DigiPar",
                     "Digitization parameters of the Start detector",
                     "Start2DigiProduction"));
   containers->Add(
      new HContainer("Start2Trb3Lookup",
                     "Lookup table for Start2 TRB3 unpacker",
                     "Start2Trb3LookupProduction"));
   containers->Add(
      new HContainer("Start2Trb3Calpar",
                     "TRB3 TDC calibration parameters of the Start detector",
                     "Trb3CalparProduction"));
}

HParSet* HStartContFact::createContainer(HContainer* c)
{
   // Calls the constructor of the corresponding parameter container.
   // For an actual context, which is not an empty string and not the default context
   // of this container, the name is concatinated with the context.
   const Char_t* name = c->GetName();
   if (strcmp(name, "Start2Trb2Lookup") == 0)
      return new HStart2Trb2Lookup(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (strcmp(name, "Start2Calpar") == 0)
      return new HStart2Calpar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "Start2HitFPar", strlen("Start2HitFPar")))
      return new HStart2HitFPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "Start2CalRunPar", strlen("Start2CalRunPar")))
      return new HStart2CalRunPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "Start2GeomPar", strlen("Start2GeomPar")))
      return new HStart2GeomPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "Start2DigiPar", strlen("Start2DigiPar")))
      return new HStart2DigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (strcmp(name, "Start2Trb3Lookup") == 0)
      return new HStart2Trb3Lookup(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "Start2Trb3Calpar", strlen("Start2Trb3Calpar")))
      return new HStart2Trb3Calpar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   return 0;
}
