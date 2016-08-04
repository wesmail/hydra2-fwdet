//*--Author A.Rustamov

#include "halignmentgdata.h"
#include "hgeomvertexfit.h"
#include "hgeomrotation.h"
#include "hgeomtransform.h"
#include <iostream>

 using namespace std;

ClassImp(HAlignmentGData)

// This object is keeping always one plane, and one line
// Here plane is defined with its normal vector (Aplane, Bplane,Cplane) and
// a dummy point on it (Xp,Yp,Zp)
// Aplane*X+Bplane*Y+Cplane*z+Dplane=0
// Dplane defined as D=-(Aplane*Xp+Bplane*Yp+Cplane*Zp);

HAlignmentGData* HAlignmentGData::oneInstance = NULL;

HAlignmentGData::HAlignmentGData()
{
  ;
}
 
HAlignmentGData* HAlignmentGData::Instance()
{
  if(oneInstance) return oneInstance;
  else
    {
      oneInstance = new HAlignmentGData();
      return oneInstance;
    }
}

void HAlignmentGData::DeleteInstance()
{
  if(oneInstance)
    {
      delete oneInstance;
      oneInstance = NULL;
    }
}

Float_t HAlignmentGData::CalcDistanceToPlane (HGeomVector point)
{
  HGeomVector normal(Aplane, Bplane, Cplane);
  Float_t dist=(normal.scalarProduct(point) + Dplane)/normal.length();

  return dist;
}

HGeomVector HAlignmentGData::CalcIntersection ()
{
  HGeomVector normal(Aplane, Bplane, Cplane); 
  Float_t t = -1.*(normal.scalarProduct(POINT)+Dplane)/normal.scalarProduct(DIRPOINT);
  Float_t xInt = POINT.getX() + t*DIRPOINT.getX();
  Float_t yInt = POINT.getY() + t*DIRPOINT.getY();
  Float_t zInt = POINT.getZ() + t*DIRPOINT.getZ();
  return HGeomVector(xInt, yInt, zInt);
}

Float_t HAlignmentGData::CalcDistanceToLine(HGeomVector point)
{
  HGeomVector dist=((point - POINT).vectorProduct(DIRPOINT));
  return dist.length(); 
}

HGeomVector HAlignmentGData::CalcVertex()  
{
  HGeomVertexFit fit;
  HGeomVector Vertex(-100.,-100.,-100.);
  fit.reset();
  fit.addLine(vertexPoint1,vertexPointDir1);
  fit.addLine(vertexPoint2,vertexPointDir2);
  fit.getVertex(Vertex);
  return Vertex;	
}

void HAlignmentGData::SetPlane(HGeomVector pNormal, HGeomVector pPoint)
{
  Aplane = pNormal.getX();
  Bplane = pNormal.getY();
  Cplane = pNormal.getZ();
  Xp = pPoint.getX();
  Yp = pPoint.getY();
  Zp = pPoint.getZ();
  Dplane = -1.*(pNormal.scalarProduct(pPoint));
}

void  HAlignmentGData::SetPlane(HGeomTransform trans)
{
  const HGeomRotation& rot = trans.getRotMatrix();
  const HGeomVector&   shift = trans.getTransVector();
  HGeomVector pNormal(rot(2),rot(5),rot(8));
  HGeomVector pPoint = shift;
  SetPlane(pNormal,pPoint);
}

void HAlignmentGData::SetLine(HGeomVector point, HGeomVector dirPoint)
{
  POINT = point;
  DIRPOINT = dirPoint;
  DIRPOINT/= DIRPOINT.length();
}

void HAlignmentGData::SetVertexLine1(HGeomVector point1, HGeomVector dir1)
{
  vertexPoint1    = point1;
  vertexPointDir1 = dir1;
  vertexPointDir1/= vertexPointDir1.length();  
}
void HAlignmentGData::SetVertexLine2(HGeomVector point2, HGeomVector dir2)
{
  vertexPoint2   = point2;
  vertexPointDir2 = dir2;
  vertexPointDir2/= vertexPointDir2.length();
}
