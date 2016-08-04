//*-- AUTHOR : Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomStartHit
//
// Class for START hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeomstarthit.h"
#include "hgeomstart.h"
#include "hgeomnode.h"

ClassImp(HGeomStartHit)

HGeomStartHit::HGeomStartHit(HGeomStart* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomStartHit::getIdType() {
  // Returns the idType for the current node
  if (!currentNode) return 0;
  return 740;
}
