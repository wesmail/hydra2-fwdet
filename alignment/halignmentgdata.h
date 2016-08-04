#ifndef HALIGNMENTGDATA_H
#define HALIGNMENTGDATA_H

#include "TObject.h"
#include "hgeomvector.h"
#include "hgeomtransform.h"

class HAlignmentGData:public TObject
{
 public:
  HAlignmentGData();
  ~HAlignmentGData() {;}
  static HAlignmentGData* Instance();
  static void DeleteInstance();
  
  Float_t        CalcDistanceToPlane (HGeomVector point);    //  distance from point to plane
  Float_t        CalcDistanceToLine  (HGeomVector point);    //  distance from point to line
  HGeomVector    CalcVertex();
  HGeomVector    CalcIntersection();
  void           SetPlane(HGeomVector, HGeomVector);
  void           SetPlane(HGeomTransform);
  void           SetLine(HGeomVector, HGeomVector);
  void           SetVertexLine1(HGeomVector, HGeomVector);
  void           SetVertexLine2(HGeomVector, HGeomVector);
  
 private:
  //Plane equation in normal form:  Ax+By+Cz+D=0;
  //Here plane is defined with its normal vector (Aplane, Bplane,Cplane) and
  //a dummy point on it (Xp,Yp,Zp)
  //Dplane defined as D=-(Aplane*Xp+Bplane*Yp+Cplane*Zp);
   
  Float_t           Aplane; 
  Float_t           Bplane;
  Float_t           Cplane;
  Float_t           Dplane;
  Float_t           Xp, Yp, Zp;
  HGeomVector       POINT;  
  HGeomVector       DIRPOINT;  //do not worry about normalization
  HGeomVector       vertexPoint1; 
  HGeomVector       vertexPointDir1;    
  HGeomVector       vertexPoint2; 
  HGeomVector       vertexPointDir2; 
  static HAlignmentGData   *oneInstance;

  ClassDef(HAlignmentGData,0)
    };   
#endif
    
