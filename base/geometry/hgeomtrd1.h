#ifndef HGEOMTRD1_H
#define HGEOMTRD1_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomTrd1 : public HGeomBasicShape {
private:
  HGeomRotation intrinsicRot;
public:
  HGeomTrd1();
  ~HGeomTrd1();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
           const HGeomTransform&,const HGeomTransform&);
  ClassDef(HGeomTrd1,0) // class for geometry shape TRD1
};

#endif  /* !HGEOMTRD1_H */
