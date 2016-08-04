#ifndef HSPLINEPAR_H
#define HSPLINEPAR_H
#include "TObject.h"
#include <iostream>
class HSplinePar:public TObject
{
 private:
  // HGeomVector segpoints[4]; //may be for future
  Float_t xPoints[52];         //x coordinate of the points used in spline
  Float_t yPoints[52];         //y coordinate of the points used in spline
  Float_t zPoints[52];        
  //Double_t *XZ, *XZP; //may be fo future     
  //Double_t *YZ, *YZP; //may be for future         
 public:
  HSplinePar();
  //HSplinePar(Int_t, Int_t);
  virtual ~HSplinePar();
  void init(Int_t, Int_t);
  void clear();

  void remove(Double_t *);
  void remove(Float_t *);
  void setSplinePoints(Float_t *, Float_t *, Float_t *);
  void getSplinePoints(Float_t *, Float_t *, Float_t *);

  ClassDef(HSplinePar,1)
};
#endif
