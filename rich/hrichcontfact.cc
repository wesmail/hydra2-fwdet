//////////////////////////////////////////////////////////////////////////////
//
// @(#)hydraTrans/richNew:$Id: $
//
//*-- Author  : Ilse Koenig <i.koenig@gsi.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichContFact
//
//  Factory for the parameter containers in libRich
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichanalysispar.h"
#include "hrichcalpar.h"
#include "hrichcontfact.h"
#include "hrichdigitisationpar.h"
#include "hrichgeometrypar.h"
#include "hrichmappingpar.h"
#include "hrichthresholdpar.h"
#include "hrich700digipar.h"
#include "hrich700ringfinderpar.h"
#include "hruntimedb.h"

#include <string.h>

using namespace std;

ClassImp(HRichContFact)

static HRichContFact gHRichContFact;  // instantiated when libRich is loaded

HRichContFact::HRichContFact()
{
   // Constructor (called when the library is loaded)
   fName = "RichContFact";
   fTitle = "Factory for parameter containers in libRich";
   setAllContainers();
   HRuntimeDb::instance()->addContFactory(this);
}

void
HRichContFact::setAllContainers()
{
   // Creates the Container objects with all accepted contexts and adds them to
   // the list of containers for the Rich library.

   containers->Add(new HContainer("RichAnalysisParameters",
                                  "Rich Analysis Parameters",
                                  "RichAnaNormalBias"));

   containers->Add(new HContainer("RichCalPar",
                                  "Rich Calibration Parameters",
                                  "RichPrimaryCalibration"));

   containers->Add(new HContainer("RichDigitisationParameters",
                                  "Rich Digitisation Parameters",
                                  "RichDigiProduction"));

   containers->Add(new HContainer("RichGeometryParameters",
                                  "Rich Geometry Parameters",
                                  "RichGeometryParProduction"));

   containers->Add(new HContainer("RichMappingParameters",
                                  "Rich Mapping Parameters",
                                  "RichMappingParProduction"));

   containers->Add(new HContainer("RichThresholdParameters",
                                  "Rich Threshold Parameters",
                                  ""));

   containers->Add(new HContainer("Rich700DigiPar",
                                  "Rich Digitizer Parameters",
                                  ""));

   containers->Add(new HContainer("Rich700RingFinderPar",
                                  "Rich RingFinder Parameters",
                                  ""));

}

HParSet*
HRichContFact::createContainer(HContainer* c)
{
   // Calls the constructor of the corresponding parameter container.
   // For an actual context, which is not an empty string and not the default context
   // of this container, the name is concatinated with the context.
   const Char_t* name = c->GetName();
   if (0 == strncmp(name, "RichAnalysisParameters", strlen("RichAnalysisParameters")))
      return new HRichAnalysisPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "RichCalPar", strlen("RichCalPar")))
      return new HRichCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "RichDigitisationParameters", strlen("RichDigitisationParameters")))
      return new HRichDigitisationPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "RichGeometryParameters", strlen("RichGeometryParameters")))
      return new HRichGeometryPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "RichMappingParameters", strlen("RichMappingParameters")))
      return new HRichMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "RichThresholdParameters", strlen("RichThresholdParameters")))
      return new HRichThresholdPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "Rich700DigiPar", strlen("Rich700DigiPar")))
      return new HRich700DigiPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   if (0 == strncmp(name, "Rich700RingFinderPar", strlen("Rich700RingFinderPar")))
      return new HRich700RingFinderPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
   return 0;
}
