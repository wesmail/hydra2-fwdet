//*-- AUTHOR : Ilse Koenig
//*-- Modified : 22/06/2003 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HGeomShapes
//
// Class to manage the GEANT/ROOT geometry shapes
//
// The 3D geometry in ROOT uses the ROOT library geom.
//
// This class holds a list of shape classes (type HGeomBasicShape). They are
// used by all detectors when reading or writing geometry parameter containers
// to ascii files or to create an event display. This class is instantiated in
// the class HSpectrometer inside the function createDetGeomPar(Text_t* name).
// Every geometry parameter container gets a pointer to it.
//
// The individual shape classes are accessible with the selectShape(...)
// functions. The functions readPoints(...) and writePoints(...) use it
// internally.
//
// In the current version the following shapes are implemented:
//     BOX  TRAP  TRD1  PGON  PCON TUBE TUBS CONE CONS SPHE  ELTU
//
///////////////////////////////////////////////////////////////////////////////

#include"hgeomshapes.h"
#include "hgeomvolume.h"
#include "hgeombasicshape.h"
#include "hgeombrik.h"
#include "hgeomtrap.h"
#include "hgeomtrd1.h"
#include "hgeompgon.h"
#include "hgeompcon.h"
#include "hgeomtube.h"
#include "hgeomtubs.h"
#include "hgeomcone.h"
#include "hgeomcons.h"
#include "hgeomsphe.h"
#include "hgeomeltu.h"

ClassImp(HGeomShapes) 

HGeomShapes::HGeomShapes() {
  // constructor creates empty list of shapes
  shapes=new TList();
}


HGeomShapes::~HGeomShapes() {
  // destructor deletes all shapes
  shapes->Delete();
  delete shapes;
}


HGeomBasicShape* HGeomShapes::selectShape(HGeomVolume * volu) {
  // returns a pointer to the shape used in the given volume
  // calls internally selectShape(TString&) with the name of the shape
  // returns NULL if the corresponding shape class is not implemented 
  const TString& name(volu->getShape());
  return selectShape(name);
}


HGeomBasicShape* HGeomShapes::selectShape(const TString& name) {
  // returns a pointer to the shape given by name
  // creates a shape object and adds it to the list of shapes if
  // not existing
  // returns NULL if the corresponding shape class is not implemented 
  TString allShapes[11]={"BOX ","TRAP","TRD1","PGON","PCON","TUBE","TUBS",
                         "CONE","CONS","SPHE","ELTU"};
  TString sName(name);
  if (sName.Length()==3) sName+=" ";
  HGeomBasicShape* s=(HGeomBasicShape*)shapes->FindObject(sName);
  if (s) return s;
  Int_t no=-1;
  for(Int_t i=0;i<11;i++) {if (sName.CompareTo(allShapes[i])==0) no=i;}
  switch(no) {
    case 0:  {s= new HGeomBrik();  break;}
    case 1:  {s= new HGeomTrap();  break;}
    case 2:  {s= new HGeomTrd1();  break;}
    case 3:  {s= new HGeomPgon();  break;}
    case 4:  {s= new HGeomPcon();  break;}
    case 5:  {s= new HGeomTube();  break;}
    case 6:  {s= new HGeomTubs();  break;}
    case 7:  {s= new HGeomCone();  break;}
    case 8:  {s= new HGeomCons();  break;}
    case 9:  {s= new HGeomSphe();  break;}
    case 10: {s= new HGeomEltu();  break;}
    default: {
      Error("selectShape","shape %s not implemented",name.Data());
    }
  }
  if (s) shapes->Add(s);
  return s;
}
 
 
Int_t HGeomShapes::readPoints(fstream* pFile,HGeomVolume* volu) {
  // reads the points of the given volume from the Ascii file
  // returns the number of points read
  // returns 0 if if the corresponding shape class is not implemented
  HGeomBasicShape* s=selectShape(volu);
  if (s) return s->readPoints(pFile,volu);
  else return 0;
}

  
Bool_t HGeomShapes::writePoints(fstream* pFile,HGeomVolume* volu) {
  // writes the points of the given volume to the Ascii file
  // return kFALSE if the corresponding shape class is not implemented
  HGeomBasicShape* s=selectShape(volu);
  if (s) return s->writePoints(pFile,volu);
  else return kFALSE;
}


void HGeomShapes::printPoints(HGeomVolume* volu) {
  // writes the points of the given volume to the Ascii file
  // return kFALSE if the corresponding shape class is not implemented
  HGeomBasicShape* s=selectShape(volu);
  if (s) s->printPoints(volu);
}
