//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomTarget
//
// Class for geometry of Target
//
/////////////////////////////////////////////////////////////

#include "hgeomtarget.h"

ClassImp(HGeomTarget)

HGeomTarget::HGeomTarget() {
  // Constructor
  fName="target";
  strcpy(modName,"T");
  strcpy(eleName,"T");
  maxSectors=0;
  maxModules=1;
}
