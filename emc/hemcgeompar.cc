//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HEmcGeomPar
//
// Container class for the basic EMC geometry parameters
//
//////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hemcgeompar.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HEmcGeomPar)

HEmcGeomPar::HEmcGeomPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
            : HDetGeomPar(name,title,context,"Emc") {
  // constructor calling the base class constructor with the detector name
}

Bool_t HEmcGeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HEmcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HEmcGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HEmcParIo");
  if (out) return out->write(this);
  return -1;
}

Int_t HEmcGeomPar::getCompNum(const TString& name) {
  // returns the cell index retrieved from the cell name GLEAxxx
  Int_t length=name.Length();
  switch (length) {
    case 5:
      return ((Int_t)(name[4]-'0')-1);
    case 6:
      return ((Int_t)(name[4]-'0')*10+(Int_t)(name[5]-'0')-1);
    case 7:
      return ((Int_t)(name[4]-'0')*100+(Int_t)(name[5]-'0')*10+(Int_t)(name[6]-'0')-1);
  }
  return(-1);
}
