#ifndef HGEOMVERTEXFIT2_H
#define HGEOMVERTEXFIT2_H

#include "TObject.h"
#include "hgeommatrix.h"
#include "hgeomvector.h"
#include "hsymmat.h"

class HGeomVertexFit2 : public TObject {
private:
  HGeomMatrix fM; //Temporal matrix for calculations
protected:
  HGeomMatrix fSys; //LSM system inverse matrix
  HGeomVector fB; //LSM independent term
public:
  HGeomVertexFit2(void);
  ~HGeomVertexFit2(void);
  void addLine(const HGeomVector &r,const HGeomVector &alpha,HSymMat4 &cov,
	       Float_t zv,const Double_t w=1.0);
  void getVertex(HGeomVector &out);
  void reset(void);
  ClassDef(HGeomVertexFit2,1)
};

#endif
