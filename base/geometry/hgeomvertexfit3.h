#ifndef HGEOMVERTEXFIT3_H
#define HGEOMVERTEXFIT3_H

#include "TObject.h"
#include "hgeommatrix.h"
#include "hgeomvector.h"
#include "hsymmat.h"

class HGeomVertexFit3 : public TObject {
private:
  HGeomMatrix fM; //Temporal matrix for calculations
protected:
  HGeomMatrix fSys; //LSM system inverse matrix
  HGeomVector fB; //LSM independent term
public:
  HGeomVertexFit3(void);
  ~HGeomVertexFit3(void);
  void addLine(const HGeomVector &r,const HGeomVector &alpha,
	       const Double_t w=1.0);
  void getVertex(HGeomVector &out);
  void reset(void);
  ClassDef(HGeomVertexFit3,1)
};

#endif
