//*-- AUTHOR : Ilse Koenig
//*-- Createded : 10/03/2000

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HRpcGeomPar
//
// Container class for the basic RPC geometry parameters
//
//////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hrpcgeompar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hrpcdetector.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HRpcGeomPar)

HRpcGeomPar::HRpcGeomPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
            : HDetGeomPar(name,title,context,"Rpc") {
  // constructor calling the base class constructor with the detector name
  HRpcDetector* det=(HRpcDetector*)(gHades->getSetup()->getDetector("Rpc"));
  if (det) {
    maxCells=det->getMaxCells();
  } else {
    maxCells=0;
    Error("HRpcGeomPar","RPC detector not defined in setup");
  }
}

Bool_t HRpcGeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HRpcParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HRpcGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HRpcParIo");
  if (out) return out->write(this);
  return -1;
}

Int_t HRpcGeomPar::getCompNum(const TString& name) {
  // returns the cell index retrieved from the cell name EGxx
  Int_t column=(Int_t)(name[2]-'0')-1;
  Int_t cell=(Int_t)(name[3]-'0')-1;
  if (cell<=9) return column*maxCells+cell;
  else return column*maxCells+(cell-7);
}
