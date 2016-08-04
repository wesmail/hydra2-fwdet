//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomRich
//
// Class for geometry of RICH
//
/////////////////////////////////////////////////////////////

#include "hgeomrich.h"
#include "hgeomnode.h"
#include "hgeommedia.h"
#include "hgeomrichhit.h"

ClassImp(HGeomRich)

HGeomRich::HGeomRich() {
  // Constructor
  fName="rich";
  strcpy(modName,"RICH");
  strcpy(eleName,"R");
  maxSectors=0;
  maxModules=1;
  pHit=new HGeomRichHit(this);
}

Bool_t HGeomRich::read(fstream& fin,HGeomMedia* media) {
  // Reads the geometry from file
  Bool_t rc=HGeomSet::read(fin,media);
  if (rc) addRefNodes();
  return rc;  
}

void HGeomRich::addRefNodes() {
  // Adds the reference node
  HGeomNode* volu=getVolume("RICH");
  if (volu) {
    masterNodes->Add(new HGeomNode(*volu));
  }
  volu=getVolume("RTAM");
  if (volu) {
    volu->setVolumeType(kHGeomRefNode);
    masterNodes->Add(new HGeomNode(*volu));
  }
}
