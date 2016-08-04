//*-- Created : 26/02/2014 by I.Koenig
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////
//
//  HStart2DigiPar:
//  Container for the START digitization parameters used in HStart2HitFSim
//
//  (Condition Style)
//
//////////////////////////////////////////////////////////////////////////

#include "TString.h"
#include "hstart2digipar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
using namespace std;

ClassImp(HStart2DigiPar)

HStart2DigiPar::HStart2DigiPar(const char* name,const char* title,const char* context)
           : HParCond(name,title,context) {
  // constructor
  clear();
}

void HStart2DigiPar::clear(void) {
  // clears the container
  sigmaT = sigmaE = deltaTimeE = minEloss = timeNoHit = 0.F;
}

void HStart2DigiPar::fill(Float_t sT, Float_t sE, Float_t dt, Float_t mE, Float_t t0) {
  // sets the parameters
  sigmaT       = sT;
  sigmaE       = sE;
  deltaTimeE   = dt; 
  minEloss     = mE;
  timeNoHit    = t0;
}

void HStart2DigiPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("sigmaT",       sigmaT);
  l->add("sigmaE",       sigmaE);
  l->add("deltaTimeE",   deltaTimeE);
  l->add("minEloss",     minEloss);
  l->add("timeNoHit",    timeNoHit);
}

Bool_t HStart2DigiPar::getParams(HParamList* l) {
  // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!(l->fill("sigmaT",       &sigmaT)))       return kFALSE;
  if (!(l->fill("sigmaE",       &sigmaE)))       return kFALSE;
  if (!(l->fill("deltaTimeE",   &deltaTimeE)))   return kFALSE;
  if (!(l->fill("minEloss",     &minEloss)))     return kFALSE;
  if (!(l->fill("timeNoHit",    &timeNoHit)))    return kFALSE;
  return kTRUE;
}
