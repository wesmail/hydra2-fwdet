//*-- AUTHOR : Ilse Koenig
//*-- Modified : 11/11/2003 by Ilse Koenig
//*-- Modified : 14/05/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
// HGeomBrik
//
// class for the GEANT shape BOX
// 
// The intrisic coordinate system of a BOX in GEANT/ROOT has the
// same orientation as the volume described by its 8 points
// in the technical coordinate system 
// 
/////////////////////////////////////////////////////////////

#include "hgeombrik.h"
#include "hgeomvolume.h"
#include "hgeomvector.h"

ClassImp(HGeomBrik)

HGeomBrik::HGeomBrik() {
  // constructor
  fName="BOX ";
  nPoints=8;
  nParam=3;
  param=new TArrayD(nParam);
}


HGeomBrik::~HGeomBrik() {
  // destructor
  if (param) {
    delete param;
    param=0;
  }
  if (center) {
    delete center;
    center=0;
  }
  if (position) {
    delete position;
    position=0;
  }
}


TArrayD* HGeomBrik::calcVoluParam(HGeomVolume* volu) {
  // calculates the parameters needed to create the shape 
  if (!volu) return 0;
  HGeomVector v=*(volu->getPoint(5)) - *(volu->getPoint(3));
  v.abs();
  v*=(1/20.);
  for(Int_t i=0;i<nParam;i++) param->AddAt(v(i),i);
  return param;
} 


void HGeomBrik::calcVoluPosition(HGeomVolume* volu,
          const HGeomTransform& dTC,const HGeomTransform& mTR) {
  // calls the function posInMother(...) to calculate the position of the
  // volume in its mother 
  HGeomVector trans=*(volu->getPoint(5)) + *(volu->getPoint(3));
  trans*=0.5;
  center->clear();
  center->setTransVector(trans);
  posInMother(dTC,mTR);
}





