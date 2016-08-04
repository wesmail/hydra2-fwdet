#ifndef HGEOMBRIK_H
#define HGEOMBRIK_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomBrik : public HGeomBasicShape {
public:
  HGeomBrik();
  ~HGeomBrik();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  ClassDef(HGeomBrik,0) // class for geometry shape BOX or BRIK
};

#endif  /* !HGEOMBRIK_H */
