#ifndef HGEOMELTU_H
#define HGEOMELTU_H

#include "hgeombasicshape.h"

class HGeomVolume;

class HGeomEltu : public HGeomBasicShape {
public:
  HGeomEltu();
  ~HGeomEltu();
  TArrayD* calcVoluParam(HGeomVolume*);
  void calcVoluPosition(HGeomVolume*,
          const HGeomTransform&,const HGeomTransform&);
  Int_t readPoints(fstream*,HGeomVolume*);   
  Bool_t writePoints(fstream*,HGeomVolume*);   
  void printPoints(HGeomVolume* volu);
  ClassDef(HGeomEltu,0) // class for geometry shape ELTU
};

#endif  /* !HGEOMELTU_H */
