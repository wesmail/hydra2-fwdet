using namespace std;
#include "hshowerwiretab.h"
#include "hparamlist.h"
#include <iostream> 
#include <iomanip>
#include <stdlib.h>
#include <math.h>

ClassImp(HShowerWireTab)

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HShowerWireTab
// Table of local coordinates of wires
//
/////////////////////////////////////////////////////////////

HShowerWireTab::HShowerWireTab() {
//creating empty table
  nWires = 89;
  wiresArr.Set(89);
  reset();
}

void HShowerWireTab::reset() {
//clearing data 
   fDistWire = 0.0f;
   wiresArr.Reset(0.f);
}

void HShowerWireTab::putParams(HParamList* l) {
  // puts the parameters into the list
  if (!l) return;
  l->add(Form("fDistWire_%i",m_nModuleID),fDistWire);
  l->add(Form("pWireYPositions_%i",m_nModuleID),wiresArr);
}

Bool_t HShowerWireTab::getParams(HParamList* l) {
  // gets the parameters from the list
  if (!l) return kFALSE;
  if (!l->fill(Form("fDistWire_%i",m_nModuleID),&fDistWire)) return kFALSE;
  if (!l->fill(Form("pWireYPositions_%i",m_nModuleID),&wiresArr)) return kFALSE;
  if (wiresArr.GetSize()!=nWires) {
    Error("getParams(HParamList*)","Only %i wire positions read instead of %i",
          wiresArr.GetSize(),nWires);
  }
  return kTRUE;
}

Int_t HShowerWireTab::lookupWire(Float_t y) {
  //return number of wire at position y
  Int_t nWireNr = -1;
  Float_t pos=0.f;
  for(Int_t j = 0; j < nWires; j++) {	 
    pos = wiresArr.At(j);
    if ((y >= pos - fDistWire) && (y < pos + fDistWire)) {
      nWireNr = j;
      break;
    }
  }
  return nWireNr;
}
