#ifndef HGEOMVERTEXFIT_H
#define HGEOMVERTEXFIT_H

#include "TObject.h"
#include "hgeommatrix.h"
#include "hgeomvector.h"

class HGeomVertexFit : public TObject {
private:
  HGeomMatrix fM; //Temporal matrix for calculations
protected:
  HGeomMatrix fSys; //LSM system inverse matrix
  HGeomVector fB; //LSM independent term
public:
  HGeomVertexFit(void);
  ~HGeomVertexFit(void);
  void addLine(const HGeomVector &r,const HGeomVector &alpha,
	       const Double_t w=1.0);
  void getVertex(HGeomVector &out);
  void reset(void);
  ClassDef(HGeomVertexFit,0)
};

#endif
