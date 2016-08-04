//*-- AUTHOR : Ilse Koenig
//*-- Modified last : 21/01/2002 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HMdcTrackDContFact
//
//  Factory for the parameter containers in libMdcTrackD
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hmdctrackdcontfact.h"
#include "hruntimedb.h"
#include "hmdctrackfitpar.h"
#include "hmdcdedx2.h"
#include "hmdclayercorrpar.h"
#include <iostream> 
#include <iomanip>

ClassImp(HMdcTrackDContFact)

static HMdcTrackDContFact gHMdcTrackDContFact;

HMdcTrackDContFact::HMdcTrackDContFact() {
  // Constructor (called when the library is loaded)
  fName="MdcTrackDContFact";
  fTitle="Factory for parameter containers in libMdcTrackD";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HMdcTrackDContFact::setAllContainers() {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the MdcTrackD library.
  containers->Add(
    new HContainer("MdcTrackFitPar",
                   "parameters for DUBNA fitter",
                   "MdcTrackFitParProduction"));


  HContainer* dedx2= new HContainer("MdcDeDx2",
                                   "parameters for Mdc dedx calculation (new)",
                                   "MdcDeDxProduction");
  containers->Add(dedx2);
  
  HContainer* layrCorr = new HContainer("MdcLayerCorrPar",
                                        "parameters for Mdc layer correction",
                                        "MdcLayerCorrProduction");
  containers->Add(layrCorr);

}

HParSet* HMdcTrackDContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name,"MdcTrackFitPar")==0)
    return new HMdcTrackFitPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcDeDx2")==0)
    return new HMdcDeDx2(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcLayerCorrPar")==0)
    return new HMdcLayerCorrPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  return 0;
}
