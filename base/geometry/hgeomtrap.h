#ifndef HGEOMTRAP_H
#define HGEOMTRAP_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomTrap : public HGeomBasicShape {
private:
  HGeomRotation intrinsicRot;
public:
  HGeomTrap();
  ~HGeomTrap();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
            const HGeomTransform&,const HGeomTransform&);
  ClassDef(HGeomTrap,0) // class for geometry shape TRAP
};

#endif  /* !HGEOMTRAP_H */
