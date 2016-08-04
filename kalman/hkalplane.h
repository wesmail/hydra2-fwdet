#ifndef HKalPlane_h
#define HKalPlane_h

// from ROOT
#include "TObject.h"
#include "TVector3.h"

class HKalPlane : public TObject {

 private:
    TVector3 vCenter; //! A point on the plane.
    TVector3 vNormal; //! Normal vector of the plane.
    TVector3 vAxisU;
    TVector3 vAxisV;

 public:

    HKalPlane() {  }

    HKalPlane(const TVector3 &center, const TVector3 &normal);

    HKalPlane(const TVector3 &origin, const TVector3 &u, const TVector3 &v);

    virtual ~HKalPlane();

    virtual Double_t distanceToPlane       (const TVector3 &point) const;

    virtual Bool_t   findIntersection      (TVector3 &pointIntersect, const TVector3 &pos, const TVector3 &dir) const;

    virtual Bool_t   isOnSurface           (const TVector3 &point) const;

    virtual void     print                 (Option_t* opt="") const;

    virtual Double_t signedDistanceToPlane (const TVector3 &point) const;

    virtual void     transform             (const TRotation &transMat);

    virtual TVector3 const& getCenter() const { return vCenter; }

    virtual TVector3 const& getNormal() const { return vNormal; }

    virtual TVector3 const& getAxisU () const { return vAxisU;  }

    virtual TVector3 const& getAxisV () const { return vAxisV;  }

    virtual Bool_t          setPlane (const TVector3 &origin, const TVector3 &normal);

    virtual Bool_t          setPlane (const TVector3 &origin, const TVector3 &u, const TVector3 &v);

    ClassDef(HKalPlane, 0)
};


#endif // HKalPlane_h
