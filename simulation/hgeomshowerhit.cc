//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomShowerHit
//
// Class for SHOWER hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeomshowerhit.h"
#include "hgeomshower.h"
#include "hgeomnode.h"

ClassImp(HGeomShowerHit)

HGeomShowerHit::HGeomShowerHit(HGeomShower* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomShowerHit::getIdType() {
  // Returns the idType for the current node
  if (!currentNode) return 0;
  const Char_t* volumeName=currentNode->GetName();
  Int_t l1=(Int_t)(volumeName[1]-'0');
  return 50+l1;
}
