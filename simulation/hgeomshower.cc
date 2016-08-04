//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomShower
//
// Class for geometry of SHOWER
//
/////////////////////////////////////////////////////////////

#include "hgeomshower.h"
#include "hgeomshowerhit.h"

ClassImp(HGeomShower)

HGeomShower::HGeomShower() {
  // Constructor
  fName="shower";
  maxSectors=6;
  maxKeepinVolumes=1;
  maxModules=3;
  pHit=new HGeomShowerHit(this);
}

const Char_t* HGeomShower::getKeepinName(Int_t s,Int_t) {
  // Returns the name of the keepin volume in sector s
  sprintf(keepinName,"SHK%i",s+1);
  return keepinName;
}

const Char_t* HGeomShower::getModuleName(Int_t m) {
  // Returns the name of module m
  sprintf(modName,"SH%iM",m+1);
  return modName;
}

const Char_t* HGeomShower::getEleName(Int_t m) {
  // Returns the element name in module m
  sprintf(eleName,"S%i",m+1);
  return eleName;
}
