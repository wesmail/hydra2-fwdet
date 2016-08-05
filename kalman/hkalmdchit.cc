
// from ROOT
#include "TRotation.h"

// from hydra
#include "hmdcsizescells.h"
#include "hgeomvector.h"

#include "hkalmdchit.h"

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(HKalMdcHit)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// A hit point is a measurement vector.
// KalMdcHit provides the position vector of the hit with errors and the
// measurement layer of the Mini Drift Chamber where the hit was registered.
// The input for the position vector should already be in the sector coordinate
// system. The class itself won't do the transformation from layer to sector
// coordinates.
// It is recommended to make the hit vector two-dimensional. This will reduce
// the calculation time for the Kalman Filter. Since the hit must be on the
// surface of a measurement layer the third hit coordinate can be calculated.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalMdcHit::HKalMdcHit(Int_t dim)
: TObject(), hitVec(dim), errVec(dim), fErr(dim, dim), weightsHist(5) {
    // Creates an empty hit object.
    // dim: dimension of hit vector

    measLayer = NULL;
}

HKalMdcHit::HKalMdcHit(Double_t *hit, Double_t *err, const HKalMdcMeasLayer &layer, Int_t dim,
        const TVector3 &impact, Kalman::kalHitTypes type, Double_t w, Int_t cell)
: TObject(), hitVec(dim), errVec(dim), fErr(dim, dim), weightsHist(5) {
      // Creates a measurement hit with arrays as input for the coordinates.
      // hit:   Measurements vector (only the first dim coordinates will be used).
      // err:   The error vector (only the first dim coordinates will be used)
      // layer: The measurement layer were the hit was detected.
      // dim:   The dimension of the hit vector.
      // impact: 3-dimensional hit vector.
      // type:   2D hit or wire hit.
      // w:      Weight of this hit (used for competing hits).
      // cell:   Drift cell number.

    for(Int_t i = 0; i < dim; i++) {
        hitVec(i) = hit[i];
        errVec(i) = err[i];
        fErr(i,i) = errVec(i) * errVec(i);
    }
    measLayer = (HKalMdcMeasLayer*)&layer;
    hitVec3   = impact;
    hitType   = type;
    weight    = w;
    driftCell = cell;

    driftTime    = -1.;
    driftTimeErr = -1.;
    angle        = 0.F;
    t1           = -1.F;
    t2           = -1.F;
    tTof         = -1.F;
    tTofCal1     = -1.F;
    tWireOffset  = -1.F;
    chi2         = -1.;

    virtLayer.setPlane(measLayer->getCenter(), measLayer->getNormal());

    if(cell >= 0) {
	HMdcSizesCells     *fSizesCells     = HMdcSizesCells::getExObject();
	if(fSizesCells == NULL) {
	    Warning("HKalMdcHit()", "HMdcSizesCells is NULL.");
	} else {
	    if(measLayer->getSector() >= 0 && measLayer->getSector() <= 5 &&
	       measLayer->getModule() >= 0 && measLayer->getModule() <= 3 &&
	       measLayer->getLayer() >= 0 && measLayer->getLayer() <= 6) {
		HMdcSizesCellsLayer &fSizesCellsLayer = (*fSizesCells)[measLayer->getSector()][measLayer->getModule()][measLayer->getLayer()];
		if(&fSizesCellsLayer == NULL) {
		    Warning("HKalMdcHit()", "No SizesCellsLayer object found.");
		    exit(1);
		}
		if(driftCell >= 0 && driftCell < fSizesCellsLayer.getNCells()) {
		    HGeomVector &wire1 = fSizesCellsLayer[driftCell].getWirePnt1();
		    wirePoint1.SetXYZ(wire1.X(), wire1.Y(), wire1.Z());
		    HGeomVector &wire2 = fSizesCellsLayer[driftCell].getWirePnt2();
		    wirePoint2.SetXYZ(wire2.X(), wire2.Y(), wire2.Z());
		} else {
		    Warning("HKalMdcHit()", Form("%d is not a valid drift cell number.", driftCell));
		}
	    } else {
		Warning("HKalMdcHit()", "Sector %i, Module %i, Layer %i not found in HMdcSizesCells.", measLayer->getSector(), measLayer->getModule(), measLayer->getLayer());
	    }
	}
    }
}

HKalMdcHit::HKalMdcHit(const HKalMdcHit &hit)
: TObject(), hitVec(hit.getDimension()), errVec(hit.getDimension()), fErr(hit.getDimension(), hit.getDimension()), weightsHist(5) {
      // Copy constructor.

    hitVec    = hit.getHitVec();
    errVec    = hit.getErrVec();
    fErr      = hit.getErrMat();

    measLayer = const_cast<HKalMdcMeasLayer*>(&(hit.getMeasLayer()));
    hit.getHitVec3(hitVec3);
    hitType   = hit.getHitType();
    weight    = hit.getWeight();
    driftCell = hit.getCell();

    driftTime    = hit.getDriftTime();
    driftTimeErr = hit.getDriftTimeErr();
    angle        = hit.getImpactAngle();
    t1           = hit.getTime1();
    t2           = hit.getTime2();
    tTof         = hit.getTimeTof();
    tTofCal1     = hit.getTimeTofCal1();
    tWireOffset  = hit.getTimeWireOffset();
    chi2         = hit.getChi2();

    virtLayer.setPlane(hit.getVirtPlane().getCenter(), hit.getVirtPlane().getNormal());
    hit.getWirePts(wirePoint1, wirePoint2);
}


HKalMdcHit::~HKalMdcHit() {
}

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------

void HKalMdcHit::setHitVec(const TVectorD &newhit) {
    // Replace the hit vector with the coordinates stored in the parameter newhit.
    // The dimension of the object's hit vector dimension will not be changed.

    Int_t dim = getDimension();
    for(Int_t i = 0; i < dim; i++) {
        hitVec(i) = newhit[i];
    }
}

void HKalMdcHit::setHitVec(const TVector3 &newhit) {
    // Replace the hit vector with the coordinates stored in the parameter newhit.
    // The dimension of the object's hit vector dimension will not be changed.

    Int_t dim = getDimension();
    for(Int_t i = 0; i < dim; i++) {
        hitVec(i) = newhit[i];
    }
}


//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

Bool_t HKalMdcHit::areCompetitors(const HKalMdcHit &hit1, const HKalMdcHit &hit2) {
    // Checks if hit1 and hit2 are competitors, i.e. hits on the same measurement layer.

    Int_t mod1 = hit1.getMeasLayer().getModule();
    Int_t sec1 = hit1.getMeasLayer().getSector();
    Int_t lay1 = hit1.getMeasLayer().getLayer();

    Int_t mod2 = hit2.getMeasLayer().getModule();
    Int_t sec2 = hit2.getMeasLayer().getSector();
    Int_t lay2 = hit2.getMeasLayer().getLayer();

    if(mod1 == mod2 && sec1 == sec2 && lay1 == lay2) {
        return kTRUE;
    }
    return kFALSE;
}


Int_t HKalMdcHit::Compare(const TObject *obj) const {
    // Implements the Compare function from TObject parent.
    // Returns  1 if this hit's weight is greater than obj's weight.
    // Returns -1 if this hit's weight is smaller than obj's weight.
    // Return   0 if weights are equal.

#ifdef kalDebug
    if(!obj->InheritsFrom("HKalMdcHit")) {
        Warning("Compare()", "Parameter is not of class HKalMdcHit.");
        return 0;
    }
#endif

    if(getWeight() < ((const HKalMdcHit*)obj)->getWeight()) {
        return -1;
    } else if(getWeight() > ((const HKalMdcHit*)obj)->getWeight()) {
        return 1;
    }
    return 0;
}

void HKalMdcHit::getWirePts(TVector3 &wire1, TVector3 &wire2) const {
    // Return the two wire end points for this hit.

    wire1.SetXYZ(wirePoint1.X(), wirePoint1.Y(), wirePoint1.Z());
    wire2.SetXYZ(wirePoint2.X(), wirePoint2.Y(), wirePoint2.Z());
}

void HKalMdcHit::print(const Option_t *opt) const {
    // Prints information about the hit object.
    // opt: Determines what information about the object will be printed.
    // If opt contains:
    // "Hit": print hit and error vectors.
    // "Lay": print geometry information about the hit's measurement layer.
    // "Mat": print material information about the hit's measurement layer.
    // If opt is empty all of the above will be printed.

    TString stropt(opt);

    Int_t pre = cout.precision();
    // Print hit coordinates and errors.
    cout<<endl;
    cout<<"****Properties of hit object:****"<<endl;
    if(stropt.Contains("Hit", TString::kIgnoreCase) || stropt.IsNull()) {
        Int_t dim = getDimension();
        cout<<"Dimension of hit vector: "<<dim<<endl;
        for(Int_t i = 0; i < dim; i++) {
            Double_t x  = hitVec(i);
            Double_t dx = errVec(i);
            cout << "x  [" << i << "] = " << setw(8) << setprecision (5) << x;
            cout << "dx [" << i << "] = " << setw(6) << setprecision (2) << dx;
            cout << resetiosflags(ios::showpoint) << endl;
        }
    }
    if(measLayer) {
        cout<<"Hit is in measurement layer: "<<endl;
        measLayer->print(opt);
    } else {
        cout<<"No measurement layer defined."<<endl;
    }
    if(stropt.Contains("Lay", TString::kIgnoreCase) || stropt.IsNull()) {
	cout<<"Properties of virtual plane:"<<endl;
	virtLayer.print();
    }
    cout<<"****End of hit print.****"<<endl;
    cout<<setprecision(pre)<<endl;
}

void HKalMdcHit::setHitAndErr(const TVectorD &newhit, const TVectorD &newerr) {
    // Replace the hit and error vectors with the coordinates stored in the parameters
    // newhit and newerr.
    // The dimension of the object's hit vector dimension will not be changed.

    Int_t dim = getDimension();
    for(Int_t i = 0; i < dim; i++) {
        hitVec(i) = newhit[i];
        errVec(i) = newerr[i];
        fErr(i,i) = errVec(i) * errVec(i);
    }
}

void HKalMdcHit::setHitAndErr(const TVector3 &newhit, const TVector3 &newerr) {
    // Replace the hit and error vectors with the coordinates stored in the parameters
    // newhit and newerr.
    // The dimension of the object's hit vector dimension will not be changed.

    Int_t dim = getDimension();
    for(Int_t i = 0; i < dim; i++) {
        hitVec(i) = newhit[i];
        errVec(i) = newerr[i];
        fErr(i,i) = newerr[i] * newerr[i];
    }
}

void HKalMdcHit::transformHit(const TRotation &transMat) {
    // Transforms the hit vector using a rotation matrix.
    // The error vector will not be transformed.

    hitVec3   .Transform(transMat);
    wirePoint1.Transform(transMat);
    wirePoint2.Transform(transMat);
    if(getHitType() == Kalman::kSegHit) {
        setHitVec(hitVec3);
    }
}

void HKalMdcHit::transformLayer(const TRotation &transMat) {
    // Transforms the hit's measurement layer using a rotation matrix.

    measLayer->transform(transMat);
    virtLayer.transform(transMat);
}


void HKalMdcHit::getHitVec3(TVector3& hit3) const {
    // Returns the position of this hit.

    hit3.SetXYZ(hitVec3.X(), hitVec3.Y(), hitVec3.Z());
}

