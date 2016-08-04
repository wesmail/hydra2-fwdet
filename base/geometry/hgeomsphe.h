#ifndef HGEOMSPHE_H
#define HGEOMSPHE_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomSphe : public HGeomBasicShape {
public:
  HGeomSphe();
  ~HGeomSphe();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomSphe,0) // class for geometry shape SPHE
};

#endif  /* !HGEOMSPHE_H */
