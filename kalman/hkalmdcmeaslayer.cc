//*--- Author: Erik Krebs

#include "hkalmdcmeaslayer.h"

// from ROOT
#include "TCollection.h" // for kIterForward declaration
#include "TVector3.h"

// from kalman
#include "hkaldef.h"
using namespace Kalman;

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(HKalMdcMeasLayer)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// This models a measurement layer within a mini drift chamber. The geometry
// of the layer is defined in class HKalPlane.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalMdcMeasLayer::HKalMdcMeasLayer(const TVector3 &center, const TVector3 &normal, const HKalMixture *matIn, const HKalMixture *matOut,
        Int_t mod, Int_t sec, Int_t lay, Int_t nLayMod, Double_t catDist) : HKalPlane(center, normal) {
    // Creates a new Mdc measurement layer.
    // center:  a point on the layer
    // normal:  normal vector of the layer
    // matIn:   material inside the layer
    // matOut:  material outside the layer
    // mod:     module where the layer is located
    // sec:     sector
    // lay:     layer number in Mdc
    // catDist: the thickness of the mdc layer, i.e. the distance between the two cathodes, in mm

    materialInside  = (HKalMixture*) matIn;
    materialOutside = (HKalMixture*) matOut;
    module          = mod;
    sector          = sec;
    layer           = lay;
    nLayersInModule = nLayMod;
    thickness       = catDist;
}

HKalMdcMeasLayer::HKalMdcMeasLayer(const TVector3 &origin, const TVector3 &u, const TVector3 &v, const HKalMixture *matIn, const HKalMixture *matOut,
        Int_t mod, Int_t sec, Int_t lay, Int_t nLayMod, Double_t catDist) : HKalPlane(origin, u, v) {
    // Creates a new Mdc measurement layer.
    // origin:  origin of the plane's coordinate system
    // u, v:    two orthogonal vectors defining the plane's coordinate system. The axis u points in
    //          direction of the signal wire which fired. Axis v in direction of the drift radius.
    // matIn:   material inside the layer
    // matOut:  material outside the layer
    // mod:     module where the layer is located
    // sec:     sector
    // lay:     layer number in Mdc
    // catDist: the thickness of the mdc layer, i.e. the distance between the two cathodes, in mm

    materialInside  = (HKalMixture*) matIn;
    materialOutside = (HKalMixture*) matOut;
    module          = mod;
    sector          = sec;
    layer           = lay;
    nLayersInModule = nLayMod;
    thickness       = catDist;
}

HKalMdcMeasLayer::~HKalMdcMeasLayer() {
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

Double_t HKalMdcMeasLayer::getDistToModBound(Bool_t dir) const {
    Double_t dist = getThickness() / 2.;
    if(dir == kIterForward) {
        dist += (nLayersInModule - 1 - layer) * getThickness();
    } else {
	dist += layer * getThickness();
    }
    return dist;
}

Bool_t HKalMdcMeasLayer::isInsideMod(const TVector3 &point) const {
    Double_t dist = signedDistanceToPlane(point);
    //cout<<"signed dist from isInsideMod: "<<dist<<endl;
    if(TMath::Abs(dist) < getDistToModBound((dist > 0.))) {
        return kTRUE;
    }
    return kFALSE;
}

Bool_t HKalMdcMeasLayer::isInside(const TVector3 &point) const {
    // Checks if point is located inside the measurement chamber or outside.
    return (distanceToPlane(point) < getThickness()/2.);
}

void HKalMdcMeasLayer::print(Option_t* opt) const {
    // Prints information about the measurement layer.
    // opt: Determines what information about the object will be printed.
    // If opt contains:
    // "Geo": print information about the geometry and location in Hades detector (module, sector, ..)
    // "Mat": print materials of the layer.
    // If opt is empty all of the above will be printed.

    TString stropt(opt);
    cout<<"**** Measurement layer of module "<<getModule()<<", sector "<<getSector()
		        <<" and layer "<<getLayer()<<": ****"<<endl;
    if(stropt.Contains("Geo", TString::kIgnoreCase) || stropt.IsNull()) {
	HKalPlane::print();
        cout<<"Thickness of layer is "<<getThickness()<<" mm."<<endl;
    }
    if(stropt.Contains("Mat", TString::kIgnoreCase) || stropt.IsNull()) {
        cout<<"Material inside: "<<endl;
        materialInside->print();
        cout<<endl;
        cout<<"Material outside: "<<endl;
        materialOutside->print();
    }
    cout<<endl;
    cout<<"**** End printout of measurement layer. ****"<<endl;
}

Float_t HKalMdcMeasLayer::getA(Bool_t inside) const {
    return (inside ? materialInside->GetA() : materialOutside->GetA());
}

Float_t HKalMdcMeasLayer::getZ(Bool_t inside) const {
    return (inside ? materialInside->GetZ() : materialOutside->GetZ());
}

Float_t HKalMdcMeasLayer::getDensity(Bool_t inside) const {
    return (inside ? materialInside->GetDensity() : materialOutside->GetDensity());
}

Float_t HKalMdcMeasLayer::getRadLength(Bool_t inside) const {
    return (inside ? materialInside->GetRadLength() : materialOutside->GetRadLength());
}

Float_t HKalMdcMeasLayer::getExcitationEnergy(Bool_t inside) const {
    return (inside ? materialInside->GetExciteEner() : materialOutside->GetExciteEner());
}

const HKalMixture* HKalMdcMeasLayer::getMaterialPtr(Bool_t inside) const {
    return (inside ? materialInside : materialOutside);
}

