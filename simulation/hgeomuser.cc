//*-- AUTHOR : Ilse Koenig
//*-- Created : 12/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomUser
//
// Class for user defined detector
//
/////////////////////////////////////////////////////////////

#include "hgeomuser.h"
#include "hgeomuserhit.h"

ClassImp(HGeomUser)

HGeomUser::HGeomUser() {
  // Constructor
  fName="user";
  maxSectors=0;
  maxModules=9;
  pHit=new HGeomUserHit(this);
}

const Char_t* HGeomUser::getModuleName(Int_t m) {
  // Return the module name in plane m
  sprintf(modName,"U%iKI",m+1);
  return modName;
}

const Char_t* HGeomUser::getEleName(Int_t m) {
  // Return the element name in plane m
  sprintf(eleName,"U%i",m+1);
  return eleName;
}

