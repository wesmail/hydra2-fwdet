//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomCave
//
// Class for the geometry of the detector part CAVE
//
/////////////////////////////////////////////////////////////

#include "hgeomcave.h"
#include "hgeomnode.h"
#include "hgeomshapes.h"
#include "hgeombasicshape.h"
#include "hgeommedia.h"
#include "hgeommedium.h"

ClassImp(HGeomCave)

HGeomCave::HGeomCave() {
  // Constructor
  fName="cave";
  name="CAVE";
  maxModules=1;
}

Bool_t HGeomCave::read(fstream& fin,HGeomMedia* media) {
  // Reads the geometry from file
  if (!media) return kFALSE;
  const Int_t maxbuf=256;
  Char_t buf[maxbuf];
  HGeomNode* volu=0;
  HGeomMedium* medium;
  Bool_t rc=kTRUE;
  do {
    fin.getline(buf,maxbuf);
    if (buf[0]!='\0' && buf[0]!='/' && !fin.eof()) {
      if (strcmp(buf,name)==0) {
        volu=new HGeomNode;
        volu->SetName(buf);
        volu->setVolumeType(kHGeomTopNode);
        volu->setActive();
        fin.getline(buf,maxbuf);
        TString shape(buf);
        HGeomBasicShape* sh=pShapes->selectShape(shape);
        if (sh) volu->setShape(sh);
        else rc=kFALSE;
        fin.getline(buf,maxbuf);
        medium=media->getMedium(buf);
        if (!medium) {
          medium=new HGeomMedium();
          media->addMedium(medium);
        }
        volu->setMedium(medium);
        Int_t n=0;
        if (sh) n=sh->readPoints(&fin,volu);
        if (n<=0) rc=kFALSE;
      } else rc=kFALSE;
    }
  } while (rc && !volu && !fin.eof()); 
  if (volu && rc) {
    volumes->Add(volu);
    masterNodes->Add(new HGeomNode(*volu));
  } else {
    delete volu;
    volu=0;
    rc=kFALSE;
  }
  return rc;
}

void HGeomCave::addRefNodes() {
  // Adds the reference node
  HGeomNode* volu=getVolume("CAVE");
  if (volu) masterNodes->Add(new HGeomNode(*volu));
}

void HGeomCave::write(fstream& fout) {
  // Writes the geometry to file
  fout.setf(ios::fixed,ios::floatfield);
  HGeomNode* volu=getVolume(name);
  if (volu) {
    HGeomBasicShape* sh=volu->getShapePointer();
    HGeomMedium* med=volu->getMedium();
    if (sh&&med) {
      fout<<volu->GetName()<<'\n'<<sh->GetName()<<'\n'<<med->GetName()<<'\n';
      sh->writePoints(&fout,volu);
    }
  }
}

void HGeomCave::print() {
  // Prints the geometry
  HGeomNode* volu=getVolume(name);
  if (volu) {
    HGeomBasicShape* sh=volu->getShapePointer();
    HGeomMedium* med=volu->getMedium();
    if (sh&&med) {
      cout<<volu->GetName()<<'\n'<<sh->GetName()<<'\n'<<med->GetName()<<'\n';
      sh->printPoints(volu);
    }
  }
}
