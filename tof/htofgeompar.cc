//*-- AUTHOR : Ilse Koenig
//*-- Createded : 10/03/2000

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HTofGeomPar
//
// Container class for the basic TOF geometry parameters
//
//////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "htofgeompar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HTofGeomPar)

HTofGeomPar::HTofGeomPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
            : HDetGeomPar(name,title,context,"Tof") {
  // constructor calling the base class constructor with the detector name
}

Bool_t HTofGeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HTofParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HTofGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HTofParIo");
  if (out) return out->write(this);
  return -1;
}
