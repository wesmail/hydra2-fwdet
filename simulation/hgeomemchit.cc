//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomEmcHit
//
// Class for EMC hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeomemchit.h"
#include "hgeomemc.h"
#include "hgeomnode.h"

ClassImp(HGeomEmcHit)

HGeomEmcHit::HGeomEmcHit(HGeomEmc* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomEmcHit::getIdType() {
  // Returns the idType for the current node
  if (!currentNode) return 0;
  return 750;
}
