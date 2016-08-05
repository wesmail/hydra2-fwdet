#ifndef HKALGEOMTOOLS_H_
#define HKALGEOMTOOLS_H_

#include "TObject.h"
class TVector3;

class HKalGeomTools : public TObject {

public:
    HKalGeomTools() { }

    virtual ~HKalGeomTools() { }

    static Double_t distance2Points           (const TVector3 &vec1, const TVector3 &vec2);

    static Double_t distancePointToPlane      (const TVector3 &point, const TVector3 &planeCenter, const TVector3 &planeNormal);

    static Double_t distancePointToLine       (TVector3 &pca, Int_t &pcaFlag, const TVector3 &point, const TVector3 &segPoint1, const TVector3 &segPoint2);

    static Double_t distancePointToSegment    (const TVector3 &point, const TVector3 &segPoint1, const TVector3 &segPoint2);

    static Double_t distancePointToSegment    (TVector3 &pca, Int_t &pcaFlag, const TVector3 &point, const TVector3 &segPoint1, const TVector3 &segPoint2);

    static Bool_t   findIntersectionLinePlane (TVector3 &pointIntersect, const TVector3 &pos, const TVector3 &dir,
                                               const TVector3 &planeCenter, const  TVector3 &planeNormal);

    static Bool_t   isPointOnPlane            (const TVector3 &point, const TVector3 &planeCenter, const TVector3 &planeNormal);

    static Double_t signedDistancePointToPlane(const TVector3 &point, const TVector3 &planeCenter, const TVector3 &planeNormal);

    static void     Track2ToLine              (TVector3 &Pfinal, TVector3 &Pwire, Int_t &Iflag,
                                               Double_t &Dist, Double_t &Length,
                                               const TVector3 &X1, const TVector3 &X2,
					       const TVector3 &w1, const TVector3 &w2);

    static void     TransModuleToSector       (Double_t &x, Double_t &y, Double_t &z, Int_t sector, Int_t module);

    static void     TransLayerToSector        (Double_t &x, Double_t &y, Double_t &z, Int_t sector, Int_t module, Int_t layer);

    ClassDef(HKalGeomTools, 0)
};

#endif /* HKALGEOMTOOLS_H_ */
