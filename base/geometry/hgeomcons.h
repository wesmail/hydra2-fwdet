#ifndef HGEOMCONS_H
#define HGEOMCONS_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomCons : public HGeomBasicShape {
public:
  HGeomCons();
  ~HGeomCons();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomCons,0) // class for geometry shape CONS
};

#endif  /* !HGEOMCONS_H */
