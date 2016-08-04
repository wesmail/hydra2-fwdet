//*-- AUTHOR : Ilse Koenig
//*-- Created : 08/11/2004

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomFWallHit
//
// Class for hit definition of Forward Wall in GEANT
/////////////////////////////////////////////////////////////

#include "hgeomfwallhit.h"
#include "hgeomfwall.h"

ClassImp(HGeomFWallHit)

HGeomFWallHit::HGeomFWallHit(HGeomFWall* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomFWallHit::getIdType() {
  // Returns the idType
  return 400;
}
