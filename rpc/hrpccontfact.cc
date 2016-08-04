//*-- AUTHOR :  Pablo Cabanelas
//*-- Created : 11/10/2005
//*-- Modified: 11/09/2006
//*-- Modified: 18/08/2007

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HRpcContFact
//
//  Factory for the parameter containers in libRpc
//
/////////////////////////////////////////////////////////////

#include "hrpccontfact.h"
#include "hruntimedb.h"
#include "hrpcdigipar.h"
#include "hrpcwtoqpar.h"
#include "hrpccalpar.h"
#include "hrpccellstatuspar.h"
#include "hrpcgeomcellpar.h"
#include "hrpchitfpar.h"
#include "hrpcclusfpar.h"
#include "hrpcgeompar.h"
#include "hrpctrb2lookup.h"
#include "hrpcslewingpar.h"
#include "hrpctimepospar.h"
#include "hrpcchargeoffsetpar.h"




ClassImp(HRpcContFact)

  static HRpcContFact gHRpcContFact;

HRpcContFact::HRpcContFact(void) {
  // Constructor (called when the library is loaded)
  fName="RpcContFact";
  fTitle="Factory for parameter containers in libRpc";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HRpcContFact::setAllContainers(void) {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the Rpc library.

  containers->Add(
		  new HContainer("RpcDigiPar",
				 "Digitization parameters for the Rpc Detector",
				 "RpcDigiProduction"));
  containers->Add(
		  new HContainer("RpcWtoQPar",
				 "Width to Charge conversion parameters for the Rpc Detector",
				 "RpcWtoQProduction"));
  containers->Add(
		  new HContainer("RpcCalPar",
				 "Calibration parameters for the Rpc Detector",
				 "RpcCalProduction"));
  containers->Add(
		  new HContainer("RpcCellStatusPar",
				 "Cell status parameters for the Rpc Detector",
				 "RpcCellStatusProduction"));
  containers->Add(
		  new HContainer("RpcHitFPar",
				 "Hit finder parameters for the Rpc Detector",
				 "RpcHitFProduction"));
  containers->Add(
		  new HContainer("RpcClusFPar",
				 "Cluster finder parameters for the Rpc Detector",
				 "RpcClusFProduction"));
  containers->Add(
		  new HContainer("RpcGeomCellPar",
				 "Geometry parameters of cells for the Rpc Detector",
				 "RpcGeomCellProduction"));
  containers->Add(
		  new HContainer("RpcGeomPar",
				 "Geometry parameters of the Rpc Detector",
				 "GeomProduction"));
  containers->Add(
      new HContainer("RpcTrb2Lookup",
                     "Lookup table for the TRB2 unpacker of the RPC detector",
		     "RpcTrb2LookupProduction"));
  containers->Add(
		  new HContainer("RpcTimePosPar",
				 "Time-position correction parameters for the Rpc Detector",
				 "RpcTimePosParProduction"));
  containers->Add(
		  new HContainer("RpcSlewingPar",
				 "Slewing correciton parameters for the Rpc Detector",
				 "RpcSlewingParProduction"));
  containers->Add(
		  new HContainer("RpcChargeOffsetPar",
				 "Correction of RPC charge offsets",
				 "RpcChargeOffsetParProduction"));




}

HParSet* HRpcContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name,"RpcDigiPar")==0)
    return new HRpcDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcWtoQPar")==0)
    return new HRpcWtoQPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcCalPar")==0)
    return new HRpcCalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcCellStatusPar")==0)
    return new HRpcCellStatusPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcHitFPar")==0)
    return new HRpcHitFPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcClusFPar")==0)
    return new HRpcClusFPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcGeomCellPar")==0)
    return new HRpcGeomCellPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcGeomPar")==0)
    return new HRpcGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcTrb2Lookup")==0)
    return new HRpcTrb2Lookup(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcTimePosPar")==0)
      return new HRpcTimePosPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcSlewingPar")==0)
      return new HRpcSlewingPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"RpcChargeOffsetPar")==0)
      return new HRpcChargeOffsetPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());


  return 0;
}

