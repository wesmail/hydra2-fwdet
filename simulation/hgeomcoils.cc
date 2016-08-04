//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomCoils
//
// Class for the geometry of detector part Magnet
//
/////////////////////////////////////////////////////////////

#include "hgeomcoils.h"

ClassImp(HGeomCoils)

HGeomCoils::HGeomCoils() {
  // Constructor
  fName="coils";
  maxSectors=6;
  maxModules=1;
  strcpy(modName,"CKIV");
  strcpy(eleName,"C");
}
