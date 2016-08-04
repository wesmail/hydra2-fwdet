//*-- AUTHOR : Ilse Koenig
//*-- Created : 27/11/2015

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomFwDet
//
// Class for geometry of the new forward detector
//
/////////////////////////////////////////////////////////////

#include "hgeomfwdet.h"
#include "hgeomfwdethit.h"

ClassImp(HGeomFwDet)

HGeomFwDet::HGeomFwDet() {
  // Constructor
  fName="fwdet";
  maxSectors=0;
  maxModules=9;
  pHit=new HGeomFwDetHit(this);
}
