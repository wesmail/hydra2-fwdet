#ifndef HALIGNMENTGGEOM_H
#define HALIGNMENTGGEOM_H

#include"TObject.h"

class HAlignmentGData;
class HGeomVector;
class HGeomTransform;
class HAlignmentGGeom:public TObject
{
 public:
  HAlignmentGGeom();
 ~HAlignmentGGeom(); 
 void Clear();
 void Init();
 Float_t       CalcDistanceToPlane(HGeomVector, HGeomVector, HGeomVector);
 Float_t       CalcDistanceToPlane(HGeomVector, HGeomTransform);
 Float_t       CalcDistanceToLine(HGeomVector , HGeomVector, HGeomVector);
 HGeomVector   CalcVertex(HGeomVector, HGeomVector,HGeomVector, HGeomVector);
 HGeomVector   CalcIntersection(HGeomVector, HGeomVector,HGeomVector, HGeomVector);
 HGeomVector   CalcIntersection(HGeomVector,  HGeomVector, HGeomTransform);
 Float_t       CalcVectorDistance(HGeomVector, HGeomVector, HGeomVector);
 Float_t       GetTheta (const HGeomTransform &, const Float_t&, const Float_t&);
 Float_t       GetPhi   (const HGeomTransform &, const Float_t&, const Float_t&);



 private: 
 HAlignmentGData *alignData;

ClassDef(HAlignmentGGeom,0)
};
#endif
