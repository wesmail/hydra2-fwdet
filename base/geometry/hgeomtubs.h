#ifndef HGEOMTUBS_H
#define HGEOMTUBS_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomTubs : public HGeomBasicShape {
public:
  HGeomTubs();
  ~HGeomTubs();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomTubs,0) // class for geometry shape TUBS
};

#endif  /* !HGEOMTUBS_H */
