#ifndef HGEOMPGON_H
#define HGEOMPGON_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomPgon : public HGeomBasicShape {
public:
  HGeomPgon();
  ~HGeomPgon();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomPgon,0) // class for geometry shape PGON
};

#endif  /* !HGEOMPGON_H */
