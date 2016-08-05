
#include "hmdcsizescells.h"

#include "hkalgeomtools.h"

// from ROOT
#include "TGeoTorus.h"
#include "TMath.h"
#include "TVector3.h"

#include <iostream>
using namespace std;

ClassImp(HKalGeomTools)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// Provides static functions to solve some geometric problems.
//
///////////////////////////////////////////////////////////////////////////////

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

Double_t HKalGeomTools::distance2Points(const TVector3 &vec1, const TVector3 &vec2) {
    // Calculates the distance between two points.
    TVector3 distV = vec1 - vec2;
    return distV.Mag();
}

Double_t HKalGeomTools::distancePointToPlane(const TVector3 &point, const TVector3 &planeCenter, const TVector3 &planeNormal) {
    // Calculates the distance of vector pos to the plane.

    // Plane equation in Hesse Normal Form: center * normal - d = 0
    // Distance of point to plane = point * normal - d
    return TMath::Abs(signedDistancePointToPlane(point, planeCenter, planeNormal));
}

Double_t HKalGeomTools::distancePointToLine(TVector3 &pca, Int_t &pcaFlag, const TVector3 &point, const TVector3 &segPoint1, const TVector3 &segPoint2) {
    // Calculates the distance between a test point and a line defined by two points.
    // This function also find the point on the segment that is closest to the test point.
    // For method used see:
    // http://www.geometrictools.com/Documentation/DistancePointLine.pdf
    //
    // return value: Distance of the test point to the segment.
    // output parameters:
    // pca:          The point on the line that is closest to the test point.
    // iFlag:        =0 if the PCA is on the segment,
    //               =1 the PCA is one of the segment end points.
    // input parameters:
    // point:        Find the distance of this test point to the line segment.
    // segPoint1, segPoint2: The two points defining the line segment.

    if(segPoint1 == segPoint2) {
	::Warning("distancePointToLine()", "Both line points are equal.");
        return (point - segPoint1).Mag();
    }

    TVector3 segDir = segPoint2 - segPoint1;
    TVector3 diff   = point - segPoint1;
    Double_t segPar = segDir * diff / (segDir * segDir);
    Double_t dist   = 0.;

    if(segPar > 1. || segPar <= 0.) {
        pcaFlag = 1;
    } else {
        pcaFlag = 0;
    }
    pca = segPoint1 + segPar * segDir;
    dist = (diff - segPar * segDir).Mag();
    return dist;
}

Double_t HKalGeomTools::distancePointToSegment(const TVector3 &point, const TVector3 &segPoint1, const TVector3 &segPoint2) {
    // Calculates the distance between a test point and a line segment.
    // For method used see:
    // http://www.geometrictools.com/Documentation/DistancePointLine.pdf
    //
    // input:
    // point: the test point
    // segPoint1, segPoint2: The two points defining the line segment.

    TVector3 pca;
    Int_t iFlag;
    return distancePointToSegment(pca, iFlag, point, segPoint1, segPoint2);
}

Double_t HKalGeomTools::distancePointToSegment(TVector3 &pca, Int_t &pcaFlag, const TVector3 &point, const TVector3 &segPoint1, const TVector3 &segPoint2) {
    // Calculates the distance between a test point and a line segment defined by two points.
    // This function also find the point on the segment that is closest to the test point.
    // For method used see:
    // http://www.geometrictools.com/Documentation/DistancePointLine.pdf
    //
    // return value: Distance of the test point to the segment.
    // output parameters:
    // pca:          The point on the line segment that is closest to the test point.
    // iFlag:        =0 if the PCA is on the segment,
    //               =1 the PCA is one of the segment end points.
    // input parameters:
    // point:        Find the distance of this test point to the line segment.
    // segPoint1, segPoint2: The two points defining the line segment.

    if(segPoint1 == segPoint2) {
	::Warning("distancePointToSegment()", "Both segment end points are equal.");
        return (point - segPoint1).Mag();
    }

    TVector3 segDir = segPoint2 - segPoint1;
    TVector3 diff   = point - segPoint1;
    Double_t segPar = segDir * diff / (segDir * segDir);
    Double_t dist   = 0.;
    pcaFlag           = 0;
    if(segPar > 0.) {
        // 0 < segPar < 1
        if(segPar < 1.) {
            dist = (diff - segPar * segDir).Mag();
            pca  = segPoint1 + segPar * segDir;
        } else { // segPar >= 1
	    dist  = (diff - segDir).Mag();
            pca   = segPoint2;
            pcaFlag = 1;
        }
    } else { // segPar <= 0
	dist  = diff.Mag();
        pca   = segPoint1;
        pcaFlag = 1;
    }
    return dist;
}


Bool_t HKalGeomTools::findIntersectionLinePlane(TVector3 &pointIntersect,
						const TVector3 &pos, const TVector3 &dir,
						const TVector3 &planeCenter, const  TVector3 &planeNormal) {
    // Finds the intersection point of a straight line with any plane.
    // pointIntersect: the intersection point (return value).
    // pos:            a point on the straight line.
    // dir:            direction of the straight line.
    // planeCenter:    a point on the plane.
    // planeNormal:    normal vector of the plane.

    Double_t denom = planeNormal.Dot(dir);
    if (denom != 0.0) {
        Double_t t = ((planeCenter.x() - pos.x()) * planeNormal.x() +
		      (planeCenter.y() - pos.y()) * planeNormal.y() +
		      (planeCenter.z() - pos.z()) * planeNormal.z()) / denom;

        pointIntersect = pos + (t * dir);
        return kTRUE;
    } else {
	::Warning("findIntersection()", "No intersection point found : (plane || track)");
        return kFALSE;
    }
    return kFALSE;
}

Bool_t HKalGeomTools::isPointOnPlane(const TVector3 &point, const TVector3 &planeCenter, const TVector3 &planeNormal){
    // Checks if point is on the plane.
    Double_t dist = distancePointToPlane(point, planeCenter, planeNormal);
    return ((TMath::Abs(dist) < 1.e-10) ? kTRUE : kFALSE);
}

Double_t HKalGeomTools::signedDistancePointToPlane(const TVector3 &point, const TVector3 &planeCenter, const TVector3 &planeNormal) {
    // Calculates the distance of a test point to the plane.
    // The signed distance is positive if the origin of the coordinate system and the test point
    // are on opposite sides of the plane and negative if they are on the same side.
    //
    // input parameters:
    // point: test point
    // planeCenter: a point on the plane
    // planeNormal: normal vector of the plane


    // Plane equation in Hesse Normal Form: center * normal - d = 0
    // Distance of point to plane = point * normal - d
    if(planeCenter * planeNormal < 0.) { // Normal vector of the plane is pointing in the wrong direction.
	return (point - planeCenter) * planeNormal.Unit() * -1.;
    }
    return (point - planeCenter) * planeNormal.Unit();
}

void HKalGeomTools::Track2ToLine(TVector3 &Pfinal, TVector3 &Pwire, Int_t &Iflag,
                  Double_t &Dist, Double_t &Length,
                  const TVector3 &X1, const TVector3 &X2,
                  const TVector3 &w1, const TVector3 &w2) {
    // Code from GEANE.
    //
    // Closest approach to a line from 2 GEANE points
    //
    // METHOD: the nearest points on the two lines
    //         x1,x2 and w1,w2 is found.
    //         The method is described in:
    //  http://softsurfer.com/Archive/algorithm_0106/algorithm_0106.htm
    //  http://www.geometrictools.com/Documentation/DistanceLine3Line3.pdf.
    //
    // INPUT: x1, x2   closest appoach GEANE points
    //        w1, w2   points of the line  (wire)  to approach
    //
    // OUTPUT: Pfinal  point of closest approach on the track
    //         Pwire    point of closest approach on the wire
    //         Dist    distance between Pfian and w1
    //         Length  arc length to add to the GEANE length of x1
    //         Iflag   =1 when Pwire is outside [w1,w2]
    //                 In this case, when w1 and w2 are the extremes
    //                 of the wire, the user could remake the procedure
    //                 by calling Track3ToPoint or Track2ToPoint, where the
    //                 Point is w1 or w2;
    //                 = 2 when the two lines are parallel and the solution
    //                 does not exists.
    //
    // Authors: Andrea Fontana and Alberto Rotondi 20 MAy 2007
    //


    TVector3 x21, x32, w21;
    TVector3 xw1, xw2;

    Double_t a1, b1, c1, d1, e1, t1, s1;
    Double_t Delta1;

    Double_t Eps = 1.E-08;

    Iflag =0;

    // line-line distance
    x21 = X2-X1;
    w21 = w2-w1;

    xw1 = X1-w1;
    xw2 = X2-w1;

    a1 =  x21.Mag2();
    b1 =  x21.Dot(w21);
    c1 =  w21.Mag2();
    d1 =  xw1.Dot(x21);
    e1 =  xw1.Dot(w21);

    Delta1 = a1*c1-b1*b1;

    if(Delta1 > Eps) {
      t1 = (a1*e1-b1*d1)/Delta1;
      s1 = (b1*e1-c1*d1)/Delta1;

      Pfinal = (X1 + x21*s1);
      Pwire  = (w1 + w21*t1);
      Length = s1*x21.Mag();
      Dist= (Pfinal-Pwire).Mag();
    }
    else {
      // lines are parallel, no solution does exist
      Pfinal.SetXYZ(0.,0.,0.);
      Pwire.SetXYZ(0.,0.,0.);
      Dist=0.;
      Length=0.;
      Iflag = 2;
      return;
    }
    // flag when the point on the wire is outside (w1,w2)
    if((((Pwire[0]<w1[0] && Pwire[0]<w2[0]) || (w2[0]<Pwire[0] && w1[0]<Pwire[0]))
        && (fabs(Pwire[0]-w1[0]) > 1e-11 && fabs(Pwire[0]- w2[0]) > 1e-11))
       || (((Pwire[1]<w1[1] && Pwire[1]<w2[1]) || (w2[1]<Pwire[1] && w1[1]<Pwire[1]))
           && (fabs(Pwire[1]-w1[1]) > 1e-11 && fabs(Pwire[1]- w2[1]) > 1e-11))
       || (((Pwire[2]<w1[2] && Pwire[2]<w2[2]) || (w2[2]<Pwire[2] && w1[2]<Pwire[2]))
           && (fabs(Pwire[2]-w1[2]) > 1e-11 && fabs(Pwire[2]- w2[2]) > 1e-11)))
      {
        Iflag=1;
      }
}

void HKalGeomTools::TransModuleToSector(Double_t &x, Double_t &y, Double_t &z,
					Int_t sector, Int_t module) {
    HMdcSizesCells* fSizesCells = HMdcSizesCells::getExObject();  // check if is already there
    if(fSizesCells) {
	((*fSizesCells)[sector][module]).transFrom(x,y,z); // transform to sector coordinate system
    } else {
	::Error("TransModuleToSector()", "No HMdcSizesCells.");
    }
}

void HKalGeomTools::TransLayerToSector(Double_t &x, Double_t &y, Double_t &z,
				       Int_t sector, Int_t module, Int_t layer) {
    HMdcSizesCells* fSizesCells = HMdcSizesCells::getExObject();  // check if is already there
    if(fSizesCells) {
	((*fSizesCells)[sector][module][layer]).transFrom(x,y,z); // transform to sector coordinate system
    } else {
	::Error("TransLayerToSector()", "No HMdcSizesCells.");
    }
}
