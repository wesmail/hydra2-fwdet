#ifndef HGEOMPCON_H
#define HGEOMPCON_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomPcon : public HGeomBasicShape {
public:
  HGeomPcon();
  ~HGeomPcon();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomPcon,0) // class for geometry shape PCON
};

#endif  /* !HGEOMPCON_H */
