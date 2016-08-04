//*-- Author  : Pablo Cabanelas
//*-- Created : 29/04/10

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////
//
//  HRpcWtoQPar:
//  Container for the RPC width to charge conversion parameters
//
//  (Condition Style)
//
/////////////////////////////////////////////////////////////////////////

#include "TString.h"
#include "hrpcwtoqpar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
using namespace std;

ClassImp(HRpcWtoQPar)

// constructor
HRpcWtoQPar::HRpcWtoQPar(const char* name,const char* title,
		const char* context) : HParCond(name,title,context) {
	clear();
}

// clears the container
void HRpcWtoQPar::clear() {
	fPar0 = 0.;
	fPar1 = 0.;
	fPar2 = 0.;
	fPar3 = 0.;
	fPar4 = 0.;
	fPar5 = 0.;
	ftdc2time = 0.;
	status=kFALSE;
	resetInputVersions();
}

// puts all parameters to the parameter list, which is used by the io
void HRpcWtoQPar::putParams(HParamList* l) {
	if (!l) return;
	l->add("fPar0",fPar0);
	l->add("fPar1",fPar1);
	l->add("fPar2",fPar2);
	l->add("fPar3",fPar3);
	l->add("fPar4",fPar4);
	l->add("fPar5",fPar5);
	l->add("ftdc2time",ftdc2time);
}

// gets all parameters from the parameter list, which is used by the io
Bool_t HRpcWtoQPar::getParams(HParamList* l) {
	if (!l) return kFALSE;
	if (!(l->fill("fPar0",&fPar0)))   return kFALSE;
	if (!(l->fill("fPar1",&fPar1)))   return kFALSE;
	if (!(l->fill("fPar2",&fPar2)))   return kFALSE;
	if (!(l->fill("fPar3",&fPar3)))   return kFALSE;
	if (!(l->fill("fPar4",&fPar4)))   return kFALSE;
	if (!(l->fill("fPar5",&fPar5)))   return kFALSE;
	if (!(l->fill("ftdc2time",&ftdc2time)))   return kFALSE;
	return kTRUE;
}
