//*-- AUTHOR : Ilse Koenig
//*-- Created : 16/11/2015

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HFwDetContFact
//
//  Factory for the parameter containers in libFwDet
//
/////////////////////////////////////////////////////////////

#include "hfwdetcontfact.h"
#include "hruntimedb.h"
#include "hfwdetgeompar.h"
#include "hfwdetstrawgeompar.h"
#include "hfwdetstrawdigipar.h"
#include "hfwdetscindigipar.h"
#include "hfwdetrpcgeompar.h"
#include "hfwdetrpcdigipar.h"
#include "hfwdetrpchitfinderpar.h"
#include "hfwdetvectorfinderpar.h"

ClassImp(HFwDetContFact);

static HFwDetContFact gHFwDetContFact;

HFwDetContFact::HFwDetContFact()
{
    // Constructor (called when the library is loaded)
    fName = "FwDetContFact";
    fTitle = "Factory for parameter containers in libFwdet";
    setAllContainers();
    HRuntimeDb::instance()->addContFactory(this);
}

HFwDetContFact::~HFwDetContFact()
{
}

void HFwDetContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the FwDet library.
    containers->Add(
        new HContainer("FwDetGeomPar",
            "Geometry parameters of the Forward Detector",
            "GeomProduction"));
    containers->Add(
        new HContainer("FwDetStrawGeomPar",
            "Geometry parameters of the Straw Forward Detector",
            "StrawGeomProduction"));
    containers->Add(
        new HContainer("FwDetRpcGeomPar",
            "Geometry parameters of the RPC Forward Detector",
            "RpcGeomProduction"));

    containers->Add(
        new HContainer("FwDetStrawDigiPar",
            "FwDetStraw digitization parameters",
            "FwDetStrawDigiProduction"));
    containers->Add(
        new HContainer("FwDetScinDigiPar",
            "FwDetScin digitization parameters",
            "FwDetScinDigiProduction"));
    containers->Add(
        new HContainer("FwDetRpcDigiPar",
            "FwDetRpc digitization parameters",
            "FwDetRpcDigiProduction"));

    containers->Add(
        new HContainer("FwDetRpcHitFinderPar",
            "FwDetRpc digitization parameters",
            "FwDetRpcDigiProduction"));

    containers->Add(
        new HContainer("FwDetVectorFinderPar",
            "FwDetStraw vector finder parameters",
            "FwDetStrawVectorFinderProduction"));
}

HParSet* HFwDetContFact::createContainer(HContainer* c)
{
    // Calls the constructor of the corresponding parameter container.
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.
    const Char_t* name=c->GetName();
    if (strcmp(name,"FwDetGeomPar") == 0)
        return new HFwDetGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    if (strcmp(name,"FwDetStrawGeomPar") == 0)
        return new HFwDetStrawGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    if (strcmp(name,"FwDetRpcGeomPar") == 0)
        return new HFwDetRpcGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());

    if (strcmp(name,"FwDetStrawDigiPar") == 0)
        return new HFwDetStrawDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    if (strcmp(name,"FwDetScinDigiPar") == 0)
        return new HFwDetScinDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    if (strcmp(name,"FwDetRpcDigiPar") == 0)
        return new HFwDetRpcDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());

    if (strcmp(name,"FwDetRpcHitFinderPar") == 0)
        return new HFwDetRpcHitFinderPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());

    if (strcmp(name,"FwDetVectorFinderPar") == 0)
        return new HFwDetVectorFinderPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());

    return 0;
}
