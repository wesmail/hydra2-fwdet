#ifndef HGEOMCONE_H
#define HGEOMCONE_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomCone : public HGeomBasicShape {
public:
  HGeomCone();
  ~HGeomCone();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomCone,0) // class for geometry shape CONE
};

#endif  /* !HGEOMCONE_H */
