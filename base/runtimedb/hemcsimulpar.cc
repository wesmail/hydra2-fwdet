#include "TString.h"
#include "hemcsimulpar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
using namespace std;

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////
//
//  HEmcSimulPar:
//  Container for the EMC parameters in the HGeant simulation
//
/////////////////////////////////////////////////////////////////////////

ClassImp(HEmcSimulPar)


HEmcSimulPar::HEmcSimulPar(const char* name,const char* title,const char* context)
           : HParCond(name,title,context) {
  // constructor
  numRows = 15;
  numCols = 17;
  lookupPmtType.Set(6*numRows*numCols);
  clear();
}

void HEmcSimulPar::clear(void) {
  // clears the container
  numPmtTypes = 0;
  lookupPmtType.Reset();
}

void HEmcSimulPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("numPmtTypes",  numPmtTypes);
  l->add("lookupPmtType",lookupPmtType);
}

Bool_t HEmcSimulPar::getParams(HParamList* l) {
  // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!(l->fill("numPmtTypes",  &numPmtTypes)))     return kFALSE;
  if (!(l->fill("lookupPmtType",&lookupPmtType)))   return kFALSE;   
  return kTRUE;
}

Bool_t HEmcSimulPar::getLookupArray(Int_t arraySize, Int_t* array) {
  // copies the lookup data into an array of the same size
  // (used in hgeant2/gemc/emcinit.F) 
  if (arraySize == lookupPmtType.GetSize()) {
    memcpy(array,lookupPmtType.GetArray(), arraySize*sizeof(Int_t));
    return kTRUE;
  }
  Error("getLookupArray", "Mismatch of array sizes (lookupPmtType: %i)",lookupPmtType.GetSize());
  return kFALSE;
}

void HEmcSimulPar::printLookupPmtType(void) {
  // prints the lookup data row wise
  // (17 values in one row, not 10 as in HParCond::printParams())
  Int_t i=0, k=0;
  while (k<lookupPmtType.GetSize()) {
    if (i==numCols) {
      cout<<" \\\n";
      i=0;
    }
    cout<<" "<<lookupPmtType[k];
    i++;
    k++;
  }
  cout<<endl;
}
