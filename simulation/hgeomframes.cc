//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomFrames
//
// Class for geometry of support structure
//
/////////////////////////////////////////////////////////////

#include "hgeomframes.h"

ClassImp(HGeomFrames)

HGeomFrames::HGeomFrames() {
  // Constructor
  fName="frames";
  strcpy(modName,"F");
  strcpy(eleName,"F");
  maxSectors=0;
  maxModules=1;
}
