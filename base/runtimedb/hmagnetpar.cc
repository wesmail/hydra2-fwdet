//*-- AUTHOR : Ilse Koenig
//*-- Created : 15/10/2004

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////
//
//  HMagnetPar
//
//  Parameter container for magnet current  
//
///////////////////////////////////////////////////////////////////////////

#include "hmagnetpar.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"
#include <math.h>

ClassImp(HMagnetPar)

HMagnetPar::HMagnetPar(const Char_t* name,
                       const Char_t* title,
                       const Char_t* context)
           : HParCond(name,title,context) {
  // Constructor
  maxCurrent=3465;
  current=0;
}

Float_t HMagnetPar::getScalingFactor(void) {
  // Calculates the scaling factor
  return ((Float_t)current/maxCurrent);
}

Bool_t HMagnetPar::init(HParIo* inp,Int_t* set) {
 // initializes the container from an input using the spectrometer interface class
  HDetParIo* input=inp->getDetParIo("HCondParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HMagnetPar::write(HParIo* output) {
  // writes the container to an output using the spectrometer interface class
  HDetParIo* out=output->getDetParIo("HCondParIo");
  if (out) return out->write(this);
  return kFALSE;
}

void HMagnetPar::clear(void) {
  // Clears the container
  current=0;
  status=kFALSE;
  resetInputVersions();
  changed=kFALSE;
}
 
void HMagnetPar::putParams(HParamList* l) {
  // Puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("current",current);
}
 
Bool_t HMagnetPar::getParams(HParamList* l) {
  // Gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!l->fill("current",&current)) return kFALSE;
  return kTRUE;
}
