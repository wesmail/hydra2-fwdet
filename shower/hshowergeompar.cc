//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
// HShowerGeomPar
//
// Standard geometry and alignment parameter container for the Shower
// (module positions, volume parameters of modules and wire planes)
//
// The internal geometry (frames, pads, wires) is store in parameter container
// HShowerGeometry
//
//////////////////////////////////////////////////////////////////////////////

#include "hshowergeompar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hgeomcompositevolume.h"

ClassImp(HShowerGeomPar)

HShowerGeomPar::HShowerGeomPar(const Char_t* name,
                                 const Char_t* title,
                                 const Char_t* context)
		 :HDetGeomPar(name,title,context,"Shower") {	
  // constructor calling the base class constructor with the detector name
}

Bool_t HShowerGeomPar::init(HParIo* input,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* inp=input->getDetParIo("HShowerParIo");
  if (inp) return (inp->init(this,set));
  return kFALSE;
}

Int_t HShowerGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HShowerParIo");
  if (out) return out->write(this);
  return -1;
}





