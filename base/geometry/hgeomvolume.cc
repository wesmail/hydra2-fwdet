//*-- AUTHOR : Ilse Koenig
//*-- Modified : 27/05/99

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomVolume
//
// Class to hold the basic geometry properties of a volume
// Each volume has a name, a pointer to a reference volume and
// a lab transformation.
// The reference volume defines the shape, the mother, the size
// and the transformation relative to the mother volume which
// is either the cave (for modules) or the detector (for the
// inner parts). 
// As an example:
// The Mdc modules built at GSI (plane 1 type) are all identical
// independent where the sit in the cave. This module type has a
// fixed coordinate system. The first layers in all these modules
// are identical and they have the same position in the module
// coordinate system.   
//
/////////////////////////////////////////////////////////////

#include "hgeomvolume.h"

ClassImp(HGeomVolume)

HGeomVolume::HGeomVolume(HGeomVolume& r) : TNamed(r) {
  // copy constructor
  points=0;
  nPoints = 0;
  fName=r.GetName();
  setVolumePar(r);
}

void HGeomVolume::setVolumePar(HGeomVolume& r) {
  // copies all volume parameters except the name
  shape=r.getShape();
  mother=r.getMother();
  Int_t n=r.getNumPoints();
  createPoints(n);
  for (Int_t i=0;i<nPoints;i++) setPoint(i,*(r.getPoint(i)));
  transform=r.getTransform();
}

void HGeomVolume::createPoints(const Int_t n) {
  // Creates n Points (objects of class HGeomVector).
  // If the array exists already and the size is different from n it is
  // deleted and recreated with the new size n.
  // If n==0 the points are deleted.
  if (n!=nPoints) {
    nPoints=n;
    if (n>0) {
      if (points!=0) {
        points->Delete();
        delete points;
      }
      points=new TObjArray(n);
      for(Int_t i=0;i<n;i++) points->AddAt(new HGeomVector(),i);
    }
    else {
      if (points) points->Delete();
      delete points;
      points=0;
    }
  }
}

void HGeomVolume::setPoint(const Int_t n,const Double_t x,
                           const Double_t y,const Double_t z) {
  // set the 3 values of the point with index n
  if (points && n<nPoints) {
    HGeomVector* v=(HGeomVector*)points->At(n);
    v->setX(x);
    v->setY(y);
    v->setZ(z);
  }
}

void HGeomVolume::setPoint(const Int_t n,const HGeomVector& p) {
  // sets point with index n by copying the 3 components of point p 
  if (points && n<nPoints) {
    HGeomVector& v=*((HGeomVector*)points->At(n));
    v=p;
  }
}

void HGeomVolume::clear() {
  // clears the volume
  // deletes the points
  shape="";
  mother="";
  if (points) points->Delete();
  delete points;
  points=0;
  nPoints=0;
  transform.clear();
}

void HGeomVolume::print() {
  // prints all parameters of a volume
  cout<<((const Char_t*)fName)<<"  "<<((const Char_t*)shape)<<"  "
      <<((const Char_t*)mother)<<'\n';
  if (points) {
    for (Int_t i=0;i<nPoints;i++)
        cout<<(*((HGeomVector*)points->At(i)));
  }
  transform.print();
  cout<<'\n';
}





