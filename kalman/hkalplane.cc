
// from ROOT
#include "TMath.h"

// from hydra
#include "hkaldef.h"
#include "hkalgeomtools.h"
#include "hkalplane.h"

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(HKalPlane)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// A class that implements a planar surface of infinite dimension. The plane
// is defined either by a point on the plane and its normal vector which is a unit
// vector or alternatively by a point and two perpendicular axis on the plane.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalPlane::HKalPlane(const TVector3 &center, const TVector3 &normal) : TObject() {
    // Creates a plane defined by a point on the plane and its normal vector.
    // Hesse Normal Form: center * normal - d = 0
    // center: a point on the plane.
    // normal: normal vector of the plane.

    setPlane(center, normal);
}

HKalPlane::HKalPlane(const TVector3 &origin, const TVector3 &u, const TVector3 &v) : TObject() {
    // Creates a plane defined by two axis u and v on the plane defining its coordinate system
    // and the origin of the plane's coordinate system.

    setPlane(origin, u, v);
}

HKalPlane::~HKalPlane() {
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

Double_t HKalPlane::distanceToPlane(const TVector3 &point) const {
    // Calculates the distance of vector pos to the plane.

    return HKalGeomTools::distancePointToPlane(point, getCenter(), getNormal());
}

Bool_t HKalPlane::findIntersection(TVector3 &pointIntersect, const TVector3 &pos, const TVector3 &dir) const {
    // Finds the intersection point of a straight line with this plane.
    // pointIntersect: the intersection point (return value).
    // t:              line parameter to reach the plane (return value)
    // pos:            a point on the straight line.
    // dir:            direction of the straight line.

    return HKalGeomTools::findIntersectionLinePlane(pointIntersect, pos, dir, getCenter(), getNormal());
}

Bool_t HKalPlane::isOnSurface(const TVector3 &point) const {
    // Checks if point is on the plane.

    return HKalGeomTools::isPointOnPlane(point, getCenter(), getNormal());
}

void HKalPlane::print(Option_t* opt) const {
    // Print center and normal vectors of this plane.
    // opt: print options (not used)

    cout<<"**** Properties of plane: ****"<<endl;
    cout<<"Center of layer: "<<endl;
    getCenter().Print();
    cout<<"Normal of layer:"<<endl;
    getNormal().Print();
    cout<<"Axis U:"<<endl;
    getAxisU().Print();
    cout<<"Axis V:"<<endl;
    getAxisV().Print();
    cout<<"**** End print of plane ****"<<endl;
}

Double_t HKalPlane::signedDistanceToPlane(const TVector3 &point) const {
    // Calculates the distance of vector pos to the plane.
    // The signed distance is positive if the origin of the coordinate system and the
    // test point are on opposite sides of the plane and negative if they are on the same side.

    return HKalGeomTools::signedDistancePointToPlane(point, getCenter(), getNormal());
}

void HKalPlane::transform(const TRotation &transMat) {
    // Transforms the plane using a rotation matrix.

    vCenter.Transform(transMat);
    vNormal.Transform(transMat);
    vAxisU .Transform(transMat);
    vAxisV .Transform(transMat);
}

Bool_t HKalPlane::setPlane(const TVector3 &origin, const TVector3 &normal) {
    // Set the plane using a point on the plane and the plane's normal vector.

#if kalDebug > 0
    if(normal.Mag() == 0.) {
	Error("setPlane()", "Normal vector of plane has length zero.");
        return kFALSE;
    }
#endif

    vCenter = origin;
    vNormal = normal.Unit();
    // Make sure the normal vector points from (0,0,0) to the plane.
    if(vCenter * vNormal < 0.) {
        vNormal *= -1.;
    }

    // Define two orthogonal axis u and v that lie in the plane.
    if(vNormal.X() != 0. && vNormal.Y() != 0.) {
	vAxisU.SetXYZ(-vNormal.Y(), vNormal.X(), 0.);
        vAxisU.SetMag(1.);
    } else {
	vAxisU.SetXYZ(1., 0., 0.); // Normal vector is parallel to z-Axis.
    }
    vAxisV  = vNormal.Cross(vAxisU);
    vAxisV.SetMag(1.);

    return kTRUE;
}

Bool_t HKalPlane::setPlane(const TVector3 &origin, const TVector3 &u, const TVector3 &v) {
    // Sets the two axis u and v on the plane defining its coordinate system
    // and the origin of the plane's coordinate system.

    Bool_t axisOrth = kTRUE;
    if(TMath::Abs(u * v) > 1.e-3) {
#if kalDebug > 0
	Double_t angle = u.Angle(v) * TMath::RadToDeg();
	Warning("setPlane()",
		Form("The plane's axis u and v are not orthogonal. The Angle between them is %f degrees.", angle));
#endif
        axisOrth = kFALSE;
    }

    vCenter = origin;
    vAxisU  = u.Unit();
    vAxisV  = v.Unit();
    vNormal = u.Cross(v);
    vNormal = vNormal.Unit();
    // Make sure the normal vector points from (0,0,0) to the plane.
    if(vCenter * vNormal < 0.) {
        vNormal *= -1.;
    }
    return axisOrth;
}
