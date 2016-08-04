//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomRichHit
//
// Class for RICH hit definition in GEANT
//
/////////////////////////////////////////////////////////////

#include "hgeomrichhit.h"
#include "hgeomrich.h"

ClassImp(HGeomRichHit)

HGeomRichHit::HGeomRichHit(HGeomRich* p) : HGeomHit(p) {
  // Constructor
}

Int_t HGeomRichHit::getIdType() {
  // Returns the idType
  return 1;
}
