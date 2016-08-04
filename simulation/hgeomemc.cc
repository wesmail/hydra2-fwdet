//*-- AUTHOR : Ilse Koenig
//*-- Created : 09/05/2006

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
// HGeomEmc
//
// Class for geometry of EMC
/////////////////////////////////////////////////////////////

#include "hgeomemc.h"
#include "hgeomnode.h"
#include "hgeommedia.h"
#include "hgeomemchit.h"

ClassImp(HGeomEmc)

HGeomEmc::HGeomEmc() {
  // Constructor
  fName="emc";
  maxSectors=6;
  maxModules=1;
  strcpy(modName,"GMOM");
  strcpy(eleName,"G");
  pHit=new HGeomEmcHit(this);
}
