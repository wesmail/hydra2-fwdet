//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomSectors
//
// Class for geometry of Sectors
//
/////////////////////////////////////////////////////////////

#include "hgeomsectors.h"
#include "hgeomnode.h"
#include "hgeommedia.h"

ClassImp(HGeomSectors)

HGeomSectors::HGeomSectors() {
  // Constructor
  fName="sect";
  maxModules=6;
}

const Char_t* HGeomSectors::getModuleName(Int_t m) {
  // Return the sector name
  sprintf(modName,"SEC%i",m+1);
  return modName;
}  

Bool_t HGeomSectors::read(fstream& fin,HGeomMedia* media) {
  // Redas the geometry from file
  Bool_t rc=HGeomSet::read(fin,media);
  if (rc) addRefNodes();
  return rc;
}

void HGeomSectors::addRefNodes() {
  // Adds the reference nodes
  TListIter iter(volumes);
  HGeomNode* volu;
  while((volu=(HGeomNode*)iter.Next())) {
    volu->setVolumeType(kHGeomRefNode);
    volu->setActive();
    masterNodes->Add(new HGeomNode(*volu));
  }
}

