//*-- AUTHOR : Ilse Koenig
//*-- Created : 12/12/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomStart
//
// Class for geometry of Start/Veto detector
//
/////////////////////////////////////////////////////////////

#include "hgeomstart.h"
#include "hgeomstarthit.h"

ClassImp(HGeomStart)

HGeomStart::HGeomStart() {
  // Constructor
  fName="start";
  strcpy(modName,"V");
  strcpy(eleName,"V");
  maxSectors=0;
  maxModules=1;
  pHit=new HGeomStartHit(this);
}
