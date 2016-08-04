//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomMdcHit
//
// Class for MDC hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeommdchit.h"
#include "hgeommdc.h"
#include "hgeomnode.h"

ClassImp(HGeomMdcHit)

HGeomMdcHit::HGeomMdcHit(HGeomMdc* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomMdcHit::getIdType() {
  // Returns the idType for the current node
  if (!currentNode) return 0;
  const Char_t* volumeName=currentNode->GetName();
  Int_t l1=(Int_t)(volumeName[1]-'0');
  Int_t l2=(Int_t)(volumeName[3]-'0');
  if(volumeName[2]=='C'&&volumeName[3]=='4') l2 = 7;   // this is the 4th cathode plane
  return 10*l1+l2;
}
