#ifndef HGEOMTUBE_H
#define HGEOMTUBE_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomTube : public HGeomBasicShape {
public:
  HGeomTube();
  ~HGeomTube();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomTube,0) // class for geometry shape TUBE
};

#endif  /* !HGEOMTUBE_H */
