#ifndef HGEOMMATRIX_H
#define HGEOMMATRIX_H

#include "TObject.h"
#include "hgeomvector.h"

class HGeomMatrix : public TObject {
protected:
  Double_t fM[9];
public:
  HGeomMatrix(void);
  ~HGeomMatrix(void);
  Double_t &operator()(Int_t i,Int_t j) { return fM[i*3+j]; }
  Double_t det(void);
  HGeomVector operator*(HGeomVector &v);
  HGeomMatrix &operator/=(Double_t d);
  ClassDef(HGeomMatrix,0)
};

#endif
