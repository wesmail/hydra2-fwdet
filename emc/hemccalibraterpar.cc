#include "TString.h"
#include "hemccalibraterpar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
using namespace std;

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////
//
//  HEmcCalibraterPar:
//  Container for the EMC calibrater parameters
//
//  (Condition Style)
//
/////////////////////////////////////////////////////////////////////////

ClassImp(HEmcCalibraterPar)


HEmcCalibraterPar::HEmcCalibraterPar(const char* name,const char* title,const char* context)
           : HParCond(name,title,context) {
  // constructor
  clear();
}

void HEmcCalibraterPar::clear(void) {
  // clears the container
  fMatchWindowMin = 0.F;
  fMatchWindowMin = 0.F;
}

void HEmcCalibraterPar::setMatchWindow(Float_t min, Float_t max) {
  // sets the parameters
  fMatchWindowMin = min;
  fMatchWindowMax = max;
}

void HEmcCalibraterPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  TArrayF fMatchWindow;
  fMatchWindow.Set(2);
  fMatchWindow[0] = fMatchWindowMin;
  fMatchWindow[1] = fMatchWindowMax;
  l->add("fMatchWindow",      fMatchWindow);
}

Bool_t HEmcCalibraterPar::getParams(HParamList* l) {
  // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  TArrayF fMatchWindow;
  if (!(l->fill("fMatchWindow", &fMatchWindow)))    return kFALSE;

  if (fMatchWindow.GetSize() != 2)
  {
    Error("HEmcCalibraterPar::getParams(HParamList* l)",
              "Array size of fMatchWindow=%d does not fit to %d", fMatchWindow.GetSize(), 2);
    return kFALSE;
  }

  fMatchWindowMin = fMatchWindow[0];
  fMatchWindowMax = fMatchWindow[1];

  return kTRUE;
}
