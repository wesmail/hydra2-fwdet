#ifndef HGEOMTRANSFORM_H
#define HGEOMTRANSFORM_H

#include "hgeomvector.h"
#include "hgeomrotation.h"

class HGeomTransform : public TObject {
protected:
  HGeomRotation rot;   // rotation matrix, describing the orientation
  HGeomVector trans;   // translation vector, describing the position
public:
  HGeomTransform() {}
  inline HGeomTransform(const HGeomTransform& t);
  ~HGeomTransform() {}
  HGeomTransform& operator=(const HGeomTransform& t);
  const HGeomRotation& getRotMatrix() const { return rot; }
  const HGeomVector& getTransVector() const { return trans; } 
  void setRotMatrix(const HGeomRotation& r) { rot=r; }
  void setRotMatrix(const Double_t* a) { rot.setMatrix(a); }
  void setRotMatrix(const Float_t* a) { rot.setMatrix(a); }
  void setTransVector(const HGeomVector& t) { trans=t; }
  void setTransVector(const Double_t* a) { trans.setVector(a); }
  void setTransVector(const Float_t* a) { trans.setVector(a); }
  HGeomVector transFrom(const HGeomVector& p) const;
  HGeomVector transTo(const HGeomVector& p) const;
  void transFrom(const HGeomTransform&);
  void transTo(const HGeomTransform&);
  void invert(void);
  void clear();
  void print();
  inline void setTransform(const HGeomTransform& t);
  ClassDef(HGeomTransform,1) // basic geometry transformation class
};

inline HGeomTransform::HGeomTransform(const HGeomTransform& t): TObject(t) {
  rot=t.getRotMatrix();
  trans=t.getTransVector();
}


inline void HGeomTransform::setTransform(const HGeomTransform& t) {
  rot=t.getRotMatrix();
  trans=t.getTransVector();
}

#endif /* !HGEOMTRANSFORM_H */
