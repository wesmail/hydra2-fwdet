//*-- AUTHOR : Ilse Koenig
//*-- Created : 12/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomUserHit
//
// Class for hit definition in GEANT of user defined detector
//
/////////////////////////////////////////////////////////////

#include "hgeomuserhit.h"
#include "hgeomuser.h"
#include "hgeomnode.h"

ClassImp(HGeomUserHit)

HGeomUserHit::HGeomUserHit(HGeomUser* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomUserHit::getIdType() {
  // Returns the idType for the current node
  if (!currentNode) return 0;
  const Char_t* volumeName=currentNode->GetName();
  Int_t l1=(Int_t)(volumeName[1]-'0');
  return l1+150;
}
