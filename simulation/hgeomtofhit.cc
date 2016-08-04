//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomTofHit
//
// Class for TOF/TOFINO hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeomtofhit.h"
#include "hgeomtof.h"
#include "hgeomnode.h"

ClassImp(HGeomTofHit)

HGeomTofHit::HGeomTofHit(HGeomTof* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomTofHit::getIdType() {
  // Returns the idType for the current node
  if (!currentNode) return 0;
  const Char_t* volumeName=currentNode->GetName();
  Int_t l1=(Int_t)(volumeName[1]-'0');
  Int_t l2=(Int_t)(volumeName[2]-'0');
  return 100+10*l1+l2;
}
