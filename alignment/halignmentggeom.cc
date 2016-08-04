//*--Author A.Rustamov
#include "halignmentggeom.h"
#include "halignmentgdata.h"
#include <iostream>

using namespace std;

ClassImp(HAlignmentGGeom)

  HAlignmentGGeom::HAlignmentGGeom()
{
  Init();
}

HAlignmentGGeom::~HAlignmentGGeom()
{
  Clear();
}

void HAlignmentGGeom::Clear()
{
  ;
}

void HAlignmentGGeom::Init()
{
  alignData=HAlignmentGData::Instance();
}

Float_t HAlignmentGGeom::CalcDistanceToPlane(HGeomVector point, HGeomVector pNormal, HGeomVector pPoint)
{
  // Calculates the distance from "point" to a plane 
  // defined by pNormal and pPoint; 
  
  alignData->SetPlane(pNormal, pPoint);
  return alignData->CalcDistanceToPlane(point);
}

Float_t HAlignmentGGeom::CalcDistanceToPlane(HGeomVector point, HGeomTransform trans)
{
  // Calculates the distance of a point to plane
  // defined by trans (3 Euler angles a position vector)
  
  alignData->SetPlane(trans);
  return alignData->CalcDistanceToPlane(point);
}


Float_t HAlignmentGGeom::CalcDistanceToLine(HGeomVector point, HGeomVector point1, HGeomVector point2 )
{
  // Claculates the distance between "point" and a line defined by two points: 
  // "point1" and "point2"  
  alignData->SetLine(point1, point2-point1);
  return alignData->CalcDistanceToLine(point);
}

HGeomVector HAlignmentGGeom::CalcVertex(HGeomVector point1, HGeomVector dir1, 
					HGeomVector point2, HGeomVector dir2)
{
  //Calculates vertex of two lines
  //Closest point to lines
  //Input should be the points and they direction vectors
  //There is no nedd to normalize direction vectors
  alignData->SetVertexLine1(point1, dir1);
  alignData->SetVertexLine2(point2, dir2);
  return alignData->CalcVertex();
}
HGeomVector HAlignmentGGeom::CalcIntersection(HGeomVector point1,  HGeomVector point2, 
					      HGeomVector pNormal, HGeomVector pPoint)
{
  //Calculates the intersection of a line
  //defined by "point1" and "point2"  with plane
  //defined by pNormal and pPoint
  alignData->SetLine(point1, point2-point1);
  alignData->SetPlane(pNormal, pPoint);
  return alignData->CalcIntersection();
}

HGeomVector HAlignmentGGeom::CalcIntersection(HGeomVector point1,  HGeomVector point2, 
					      HGeomTransform trans)
{
  //Calculates the intersection of a line
  //defined by "point1" and "point2"  with plane
  //defined by trans matrix
  alignData->SetLine(point1, point2-point1);
  alignData->SetPlane(trans);
  return alignData->CalcIntersection();
}

Float_t HAlignmentGGeom::CalcVectorDistance(HGeomVector dir, HGeomVector point1, HGeomVector point2)
{
  //calculates the vector distance between direction vector dir
  //and a direction vector of a line  defined with  point1 and point2
  //Be carefull with the order of point1 and point2
  //Vector distance depends on that, actually it should be defined in the same way
  //as dir
  HGeomVector DIR=(point2-point1);
  DIR/=DIR.length();
  return (dir-DIR).length();
}

Float_t HAlignmentGGeom::GetTheta(const HGeomTransform &trans, const Float_t& theta, const  Float_t& phi)
{
  //takes into account beam alignment
  //new polar angle is calculated 
  Float_t X=sin(theta)*cos(phi);
  Float_t Y=sin(theta)*sin(phi);
  Float_t Z=cos(theta);
  HGeomVector dir(X,Y,Z);
  HGeomRotation rot = trans.getRotMatrix();
  dir=  trans.transTo(dir) + rot.inverse()*trans.getTransVector();
  return atan2(sqrt(dir.getX()*dir.getX()+dir.getY()*dir.getY()), dir.getZ());
}

Float_t HAlignmentGGeom::GetPhi(const HGeomTransform& trans,const  Float_t& theta,const  Float_t& phi)
{
  //takes into account beam alignment
  //new athimuthal angle is calculated 
  Float_t X=sin(theta)*cos(phi);
  Float_t Y=sin(theta)*sin(phi);
  Float_t Z=cos(theta);
  HGeomVector dir(X,Y,Z);
  HGeomRotation rot=trans.getRotMatrix();
  dir=  trans.transTo(dir) + rot.inverse()*trans.getTransVector();
  return atan2(dir.getY(),dir.getX());
}


