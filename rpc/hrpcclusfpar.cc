//*-- Author  : Pablo Cabanelas
//*-- Created : 09/06/2010

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////
//
//  HRpcClusFPar:
//  Container for the RPC cluster finder parameters
//
//  (Condition Style)
//
/////////////////////////////////////////////////////////////////////////

#include "TString.h"
#include "hrpcclusfpar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
using namespace std;

ClassImp(HRpcClusFPar)

// constructor
HRpcClusFPar::HRpcClusFPar(const char* name,const char* title,
		const char* context) : HParCond(name,title,context) {
	clear();
}

// clears the container
void HRpcClusFPar::clear() {
	fNsigmasX  = 0.;
	fNsigmasT  = 0.;
	status=kFALSE;
	resetInputVersions();
}

// puts all parameters to the parameter list, which is used by the io
void HRpcClusFPar::putParams(HParamList* l) {
	if (!l) return;
	l->add("fNsigmasX",fNsigmasX);
	l->add("fNsigmasT",fNsigmasT);
}

// gets all parameters from the parameter list, which is used by the io
Bool_t HRpcClusFPar::getParams(HParamList* l) {
	if (!l) return kFALSE;
	if (!(l->fill("fNsigmasX",&fNsigmasX)))   return kFALSE;
	if (!(l->fill("fNsigmasT",&fNsigmasT)))   return kFALSE;
	return kTRUE;
}
