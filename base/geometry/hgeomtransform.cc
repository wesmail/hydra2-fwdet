//*-- AUTHOR : Ilse Koenig
//*-- Modified : 16/06/99

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HGeomTransform
//
// Class to hold the orientation (rotation matrix) and the position
// (translation vector) of a coordinate system (system 2) relative to a
// reference coordinate system (system 1)
// It provides member functions to transform a vector or a point and an other
// coordinate system from its own coordinate system into the reference
// coordinate system and vice versa.
// Instances of this class can e.g. hold the lab or detector transformation of
// a geometry volume (see class HGeomVolume)
//
// Inline functions:
//
// HGeomTransform()
//    The default constructor creates an identity transformation  
// HGeomTransform(HGeomTransform& t)
//    copy constructor
// void setTransform(const HGeomTransform& t)
//    copies the given transformation
// void setRotMatrix(const HGeomRotation& r)
//    copies the given rotation matrix
// void setRotMatrix(const Double_t* a)
//    creates a rotation matrix taking an Double_t array with 9 components 
// void setTransVector(const HGeomVector& t)
//    copies the given translation vector
// void setTransVector(const Double_t* a)
//    creates a translation vector taking an Double_t array with 6 components
// const HGeomRotation& getRotMatrix() const
//    returns the rotation matrix
// const HGeomVector& getTransVector() const
//    returns the  translation vector
//
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hgeomtransform.h"
#include <iostream> 
#include <iomanip>
#include <math.h>

ClassImp(HGeomTransform)

HGeomTransform& HGeomTransform::operator=(const HGeomTransform& t)  {
  rot=t.getRotMatrix();
  trans=t.getTransVector();

  return *this;
}


HGeomVector HGeomTransform::transFrom(const HGeomVector& p) const {
  // Transforms a vector (point) given in its own coordinate
  // system (2) into the reference coordinate system (1)
  // e.g. v2 is a vector (point) in the detector coordinate system;
  // it can be transformed to a vector v2 the lab system with
  //    HGeomVector v2=mo.transFrom(v1)
  // where mo is the coordinate system of the mother
  return rot*p+trans;
}

HGeomVector HGeomTransform::transTo(const HGeomVector& p) const {
  // Transforms a vector (point) given in the reference system (1)
  // into the local coordinate system (2)
  // e.g. v1 is a vector (point) in the lab system; it can be transformed to
  // a vector v2 the detector coordinate system with
  //    HGeomVector v2=mo.transTo(v1)
  // where mo is the coordinate system of the mother
  return rot.inverse()*(p-trans);
}

void HGeomTransform::transTo(const HGeomTransform& s) {
  // Transforms the coordinate system into the coordinate system
  // described by s. Both transformations must have the same reference
  // system e.g. the lab system
  // This function is e.g. used to transform a daughter coordinate system
  // with a transformation relative to the lab into the detector coordinate
  // system.
  const HGeomRotation& rm=s.getRotMatrix();
  HGeomRotation rt(rm.inverse());
  if (rm.diff2(rot)<0.000001) rot.setUnitMatrix();
  else rot.transform(rt);
  trans-=s.getTransVector();
  trans=rt*trans;
  //  trans.round(3); // rounds to 3 digits (precision 1 micrometer)
}

void HGeomTransform::transFrom(const HGeomTransform& s) {
  // Transforms the coordinate system described by s into the local
  // coordinate system
  // This function is e.g. used to transform a daughter coordinate system
  // with a transformation relative to its mother into the lab system.
  // e.g. daughterDetTransform.transFrom(motherLabTransform)
  const HGeomRotation& r=s.getRotMatrix();
  rot.transform(r);
  trans=r*trans;
  trans+=s.getTransVector();
}

void HGeomTransform::clear() {
  trans.clear();
  rot.setUnitMatrix();
}

void HGeomTransform::print() {
  rot.print();
  trans.print();
}

void HGeomTransform::invert(void) {
  rot.invert();
  trans = rot*trans;
  trans *= -1.;
}
