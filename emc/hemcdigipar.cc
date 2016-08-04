#include "TString.h"
#include "hemcdigipar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
using namespace std;

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////
//
//  HEmcDigiPar:
//  Container for the RPC digitization parameters
//
//  (Condition Style)
//
/////////////////////////////////////////////////////////////////////////

ClassImp(HEmcDigiPar)


HEmcDigiPar::HEmcDigiPar(const char* name,const char* title,const char* context)
           : HParCond(name,title,context) {
  // constructor
  clear();
}

void HEmcDigiPar::clear(void) {
  // clears the container
  sigmaT = phot2E = sigmaEIntern = sigmaEReal = 0.F;
}

void HEmcDigiPar::fill(Float_t sT, Float_t p2e, Float_t seI, Float_t seR) {
  // sets the parameters
  sigmaT       = sT;
  phot2E       = p2e; 
  sigmaEIntern = seI;
  sigmaEReal   = seR;
}

void HEmcDigiPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("sigmaT",      sigmaT);
  l->add("phot2E",      phot2E);
  l->add("sigmaEIntern",sigmaEIntern);
  l->add("sigmaEReal",  sigmaEReal);
}

Bool_t HEmcDigiPar::getParams(HParamList* l) {
  // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!(l->fill("sigmaT",      &sigmaT)))       return kFALSE;
  if (!(l->fill("phot2E",      &phot2E)))       return kFALSE;
  if (!(l->fill("sigmaEIntern",&sigmaEIntern))) return kFALSE;
  if (!(l->fill("sigmaEReal",  &sigmaEReal)))   return kFALSE;
  return kTRUE;
}
