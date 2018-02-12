#include "hedhelpers.h"

#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hgeantdef.h"


#include "hmdcsizescells.h"
#include "hmdcgetcontainers.h"
#include "hmdckickplane.h"

#include "hrecevent.h"
#include "heventheader.h"
#include "hcategory.h"

#include "hmdccal1.h"
#include "hmdcseg.h"
#include "hwallhit.h"
#include "htofhit.h"
#include "hshowerhit.h"
#include "hemccluster.h"
#include "hrichhit.h"
#include "hrpccluster.h"
#include "hrichcal.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hgeanttof.h"
#include "hgeantrpc.h"
#include "hgeantshower.h"
#include "hgeantrich.h"
#include "hgeantwall.h"
#include "hparticlecand.h"

#include "hgeomtransform.h"
#include "hgeomvector.h"
#include "hgeomvector2.h"
#include "hgeomcompositevolume.h"

#include "hrich700digipar.h"
#include "hrichgeometrypar.h"
#include "hrichpadtab.h"
#include "hrichpad.h"
#include "hrichpadcorner.h"
#include "hrichframe.h"
#include "hrichframecorner.h"

#include "htofgeompar.h"
#include "hrpcgeompar.h"
#include "hwallgeompar.h"
#include "hshowergeometry.h"



#include "hcategorymanager.h"

#include "TMath.h"
#include "TEveFrameBox.h"
#include "TEveTrackPropagator.h"
#include "TEveTrack.h"
#include "TEveVSDStructs.h"
#include "TGeoSphere.h"

#include <iostream>
using namespace std;



ClassImp(HEDTransform)
ClassImp(HEDMakeContainers)
ClassImp(HEDMdcWireManager)


HGeomTransform* HEDTransform::richSecTrans    = 0;
HGeomTransform* HEDTransform::richMirrorTrans = 0;
TGeoSphere*     HEDTransform::richMirror      = 0;
Bool_t          HEDTransform::fisNewRich      = kFALSE;
Bool_t          HEDTransform::fisEmc          = kFALSE;

Float_t  HEDTransform::calcPhiToLab(Int_t sec)
{
    // returns the lower phi bound of the sector
    // in LAB [Deg].
    Float_t phi = 0.;
    if(sec < 5) { phi = (sec == 0)? 0. : sec * 60.; }
    else        { phi = -60.;   }

    return phi;
}

void HEDTransform::setRichSecTrans(Double_t x,Double_t y,Double_t z,Double_t rot1,Double_t rot2,Double_t rot3){
    // offset xyz + rotation in deg
    Double_t par[6] = {x,y,z,rot1,rot2,rot3};
    if(richSecTrans == 0) richSecTrans = new HGeomTransform();
    HMdcSizesCells::setTransform(par,*richSecTrans);
}
void HEDTransform::setRichMirrorTrans(Double_t x,Double_t y,Double_t z,Double_t rot1,Double_t rot2,Double_t rot3){
    // offset xyz + rotation in deg
    Double_t par[6] = {x,y,z,rot1,rot2,rot3};
    if(richMirrorTrans == 0) richMirrorTrans = new HGeomTransform();
    HMdcSizesCells::setTransform(par,*richMirrorTrans);
}

HGeomTransform* HEDTransform::getRichSecTrans(){
    // get Rich sector trans. If the trans has not been initialized
    // before a default transformation of -20 deg theta rotation
    // will be set.

    if(richSecTrans == 0){
	::Warning("getRichSecTrans()","Rich Sector trans was not initialized before! Use default....");
	HEDTransform::setRichSecTrans(0.,0.,0.,-20.,0.,0.); // padplane tilt is 20 Deg in theta
    }
    return richSecTrans;
}

HGeomTransform* HEDTransform::getRichMirrorTrans(){
    // get Rich Mirror trans. If the trans has not been initialized
    // before a default transformation 0 will be set.

    if(richMirrorTrans == 0){
	::Warning("getRichMirrorTrans()","Rich Mirror trans was not initialized before! Use default....");
	HEDTransform::setRichMirrorTrans(0.,0.,0.,0.,0.,0.); // padplane tilt is 20 Deg in theta
    }
    return richMirrorTrans;
}

Bool_t HEDTransform::calcSegPointsLab(HMdcSeg* seg ,HGeomVector& p1,HGeomVector& p2)
{
    // takes HMdcSeg object and calculates hits points on both
    // modules in Lab cordinates (in cm !)
    HMdcSizesCells* sizes         =  HMdcSizesCells::getExObject();
    HMdcGetContainers* containers =  HMdcGetContainers::getObject();
    if(sizes){
	if(seg){
	    Int_t sec = seg->getSec();
	    Int_t first = 0;
	    Double_t x,y,z;
	    if(seg->getIOSeg() == 1 ) first = 2;


            HMdcDetector* mdcDet = (HMdcDetector*) gHades->getSetup()->getDetector("Mdc");
	    if(!mdcDet) { cerr<<"helpers::calcSegPointsLab() : No Mdc Detector defined!"<<endl;
		exit(1);
	    }

	    if(!mdcDet->getModule(sec,first) || !mdcDet->getModule(sec,first+1) ){
		cerr<<"helpers::calcSegPointsLab() : At least one module in segment not defined in detector! Need Geometry!"<<endl;
                exit(1);
	    }


	    HMdcSizesCellsMod* module = &((*sizes)[sec][first]);
	    if(!module){
		cerr<<"helpers::calcSegPointsLab() : Received NULL pointer for HMdcSizesMod s = "<<sec<<" m = "<<first<<"!"<<endl;
		return kFALSE;
	    }
	    module->calcSegIntersec(seg->getZ(),seg->getR(),seg->getTheta(),seg->getPhi(),x,y,z);
            p1.setXYZ(x,y,z);
	    HGeomTransform trans;
	    containers->getLabTransSec(trans,sec,first);
	    p1 = trans.transFrom(p1);    // Mod to Lab coordinate system
            p1 *= TO_CM;                    // mm -> cm

	    module = &((*sizes)[sec][first+1]);
	    if(!module){
		cerr<<"helpers::calcSegPointsLab() : Received NULL pointer for HMdcSizesMod s = "<<sec<<" m = "<<first+1<<"!"<<endl;
		return kFALSE;
	    }
	    module->calcSegIntersec(seg->getZ(),seg->getR(),seg->getTheta(),seg->getPhi(),x,y,z);
            p2.setXYZ(x,y,z);
	    containers->getLabTransSec(trans,sec,first+1);
	    p2 = trans.transFrom(p2);    // Mod to Lab coordinate system
            p2 *= TO_CM;                  // mm -> cm

	} else {
	    cerr<<"helpers::calcSegPointsLab() : Received NULL pointer for HMdcSeg!"<<endl;
	    return kFALSE;
	}

    } else {
	cerr<<"helpers::calcSegPointsLab() : Received NULL pointer for HMdcSizesCells!"<<endl;
	exit(1);
    }
    return kTRUE;
}

Bool_t HEDTransform::calcWirePointsLab (Int_t s,Int_t m,Int_t l,Int_t c,HGeomVector& p1,HGeomVector& p2)
{
    // calculates end points of a wire in Lab cordinates (in cm !)

    HMdcSizesCells* sizes         =  HMdcSizesCells::getExObject();
    HMdcGetContainers* containers =  HMdcGetContainers::getObject();
    if(sizes){
	HMdcDetector* mdcDet = (HMdcDetector*) gHades->getSetup()->getDetector("Mdc");
	if(!mdcDet) { cerr<<"helpers::calcWirePointsLab() : No Mdc Detector defined!"<<endl;
	    exit(1);
	}

	if(!mdcDet->getModule(s,m) ){
	    cerr<<"helpers::calcWirePointsLab() : module in segment not defined in detector! Need Geometry!"<<endl;
	    exit(1);
	}

	HMdcSizesCellsMod* module = &((*sizes)[s][m]);
	if(!module){
	    cerr<<"helpers::calcWirePointsLab() : Received NULL pointer for HMdcSizesMod s = "<<s<<" m = "<<m<<"!"<<endl;
	    return kFALSE;
	}
	HMdcSizesCellsLayer& layer = (*sizes)[s][m][l];

	Int_t ncell = layer.getNCells();
	if (c >= ncell ) {
	    p1.setXYZ(0,0,0);
            p2.setXYZ(0,0,0);
            return kFALSE;
	}
        const HGeomVector& p3 = *(layer[c].getWirePoint(0));
	const HGeomVector& p4 = *(layer[c].getWirePoint(1));
	p1 = p3;
        p2 = p4;

	HGeomTransform trans;
	containers->getLabTransSec(trans,s,m);
	p1 = trans.transFrom(p1);    // Mod to Lab coordinate system
	p1 *= TO_CM;                    // mm -> cm
	p2 = trans.transFrom(p2);    // Mod to Lab coordinate system
	p2 *= TO_CM;                  // mm -> cm


    } else {
	cerr<<"helpers::calcWirePointsLab() : Received NULL pointer for HMdcSizesCells!"<<endl;
	exit(1);
    }
    return kTRUE;
}


Bool_t HEDTransform::calcSegKickPlanePointLab(HMdcSeg* seg,HGeomVector& p)
{
    // takes HMdcSeg object and calculates hit point on
    // kickplane in Lab cordinates (in cm !)
    HMdcSizesCells* sizes         =  HMdcSizesCells::getExObject();
    HMdcGetContainers* containers =  HMdcGetContainers::getObject();
    if(sizes){
	if(seg){
	    Int_t sec = seg->getSec();
	    Int_t first = 0;
	    Double_t x1,y1,z1;
	    Double_t x2,y2,z2;
	    if(seg->getIOSeg() == 1 ) first = 2;

	    HMdcDetector* mdcDet = (HMdcDetector*) gHades->getSetup()->getDetector("Mdc");
	    if(!mdcDet) { cerr<<"helpers::calcSegKickPlanePointLab() : No Mdc Detector defined!"<<endl;
		exit(1);
	    }

	    if(!mdcDet->getModule(sec,first) || !mdcDet->getModule(sec,first+1) ){
		cerr<<"helpers::calcSegKickPlanePointLab() : At least one module in segment not defined in detector! Need Geometry!"<<endl;
		exit(1);
	    }

	    HMdcSizesCellsMod* module = &((*sizes)[sec][first]);
	    if(!module){
		cerr<<"helpers::calcSegKickPlanePointLab() : Received NULL pointer for HMdcSizesMod s = "<<sec<<" m = "<<first<<"!"<<endl;
		return kFALSE;
	    }
	    module->calcSegIntersec(seg->getZ(),seg->getR(),seg->getTheta(),seg->getPhi(),x1,y1,z1);

	    module = &((*sizes)[sec][first+1]);
	    if(!module){
		cerr<<"helpers::calcSegKickPlanePointLab() : Received NULL pointer for HMdcSizesMod s = "<<sec<<" m = "<<first+1<<"!"<<endl;
		return kFALSE;
	    }
	    module->calcSegIntersec(seg->getZ(),seg->getR(),seg->getTheta(),seg->getPhi(),x2,y2,z2);

            HMdcKickPlane plane;
	    Double_t x,y,z;
            plane.calcIntersection(x1,y1,z1,x2,y2,z2,x,y,z);

	    p.setXYZ(x,y,z);
	    HGeomTransform trans;
	    containers->getLabTransSec(trans,sec);
	    p = trans.transFrom(p);    // Mod to Lab coordinate system
            p *= TO_CM;               // mm -> cm

	} else {
	    cerr<<"helpers::calcSegKickPlanePointLab() : Received NULL pointer for HMdcSeg!"<<endl;
	    return kFALSE;
	}

    } else {
	cerr<<"helpers::calcSegKickPlanePointLab() : Received NULL pointer for HMdcSizesCells!"<<endl;
	exit(1);
    }
    return kTRUE;
}
Bool_t HEDTransform::calcWallHitPointLab(HWallHit* hit ,HGeomVector& p)
{
    // calc WALL hit point in LAB ccordinate sys (cm!)
    if(hit) {
	Float_t x,y,z;
	hit->getXYZLab(x,y,z);
	p.setXYZ(x,y,z);
	p *= TO_CM;
    } else {
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HEDTransform::calcTofHitPointLab(HTofHit* hit ,HGeomVector& p)
{
    // calc TOF hit point in LAB ccordinate sys (cm!)
    if(hit) {
	Float_t x,y,z;
	hit->getXYZLab(x,y,z);
	p.setXYZ(x,y,z);
	p *= TO_CM;
    } else {
	return kFALSE;
    }
    return kTRUE;
}


Bool_t HEDTransform::calcShowerHitPointLab(HShowerHit* hit ,HGeomVector& p)
{
    // calc SHOWER hit point in LAB ccordinate sys (cm!)
    if(hit) {
	Float_t x,y,z;
	hit->getLabXYZ(&x,&y,&z);
	p.setXYZ(x,y,z);
	p *= TO_CM;
    } else {
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HEDTransform::calcEmcClusterPointLab(HEmcCluster* hit ,HGeomVector& p)
{
    // calc EMC Cluster point in LAB ccordinate sys (cm!)
    if(hit) {
	Float_t x,y,z;
	hit->getXYZLab(x,y,z);
	p.setXYZ(x,y,z);
	p *= TO_CM;
    } else {
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HEDTransform::calcRpcClustPointLab(HRpcCluster* hit ,HGeomVector& p)
{
    // calc RPC cluster point in LAB ccordinate sys (cm!)
    HMdcSizesCells* sizes         =  HMdcSizesCells::getExObject();
    HMdcGetContainers* containers =  HMdcGetContainers::getObject();

    if(sizes&&hit) {
        p.setXYZ(hit->getXSec(),hit->getYSec(),hit->getZSec());
	HGeomTransform trans;
	containers->getLabTransSec(trans,hit->getSector());

	p = trans.transFrom(p);  // Sec to Lab coordinate system
	p *= TO_CM;            // mm -> cm

    } else {
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HEDTransform::calcRichGeantPadplanePointLab(Int_t s,HGeomVector& p)
{
    // calc RICH geant point (on Pad plane) in LAB ccordinate sys (cm!)
    HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
    if(fRichGeometryPar){

	p *= TO_CM;
        p.setY(p.getY() + fRichGeometryPar->getSectorShift() / cos(20*TMath::DegToRad()) );

	//--------------------------------------------------------------------
	// get transformation RICH padplane -> sector sys
	HGeomTransform& trans = *HEDTransform::getRichSecTrans();
	//--------------------------------------------------------------------

	//Lab system: all sectors in Lab
	HEDTransform::calcRichPadPlaneToLab(s,p,trans);

    } else {
	return kFALSE;
    }
    return kTRUE;
}


Bool_t HEDTransform::calcMdcGeantLayerPointLab (Int_t s,Int_t m,Int_t l,HGeomVector& p)
{
    // calculates points on MDC layer to Lab cordinates (in cm !)

    HMdcSizesCells* sizes         =  HMdcSizesCells::getExObject();
    HMdcGetContainers* containers =  HMdcGetContainers::getObject();
    if(sizes){

	HMdcDetector* mdcDet = (HMdcDetector*) gHades->getSetup()->getDetector("Mdc");
	if(!mdcDet) { cerr<<"helpers::calcMdcGeantLayerPointLab() : No Mdc Detector defined!"<<endl;
	    exit(1);
	}

	if(!mdcDet->getModule(s,m) ){
	    cerr<<"helpers::calcMdcGeantLayerPointLab() : module in segment not defined in detector! Need Geometry!"<<endl;
	    exit(1);
	}

	HMdcSizesCellsMod* module = &((*sizes)[s][m]);
	if(!module){
	    cerr<<"helpers::calcLayerPointLab() : Received NULL pointer for HMdcSizesMod s = "<<s<<" m = "<<m<<"!"<<endl;
	    return kFALSE;
	}

	HGeomTransform trans;
	containers->getLabTransGeantLayer(trans,s,m,l);
	p = trans.transFrom(p);    // Layer to Lab coordinate system
	p *= TO_CM;                // mm -> cm

    } else {
	cerr<<"helpers::calcLayerPointLab() : Received NULL pointer for HMdcSizesCells!"<<endl;
	exit(1);
    }
    return kTRUE;
}


Bool_t HEDTransform::calcWallGeantPointLab(Int_t c,HGeomVector& p)
{
    // calc WALL geant point (Module system) in LAB ccordinate sys (cm!)

    HWallGeomPar* fWallGeometry=(HWallGeomPar *)(gHades->getRuntimeDb()->getContainer("WallGeomPar"));


    if(fWallGeometry) {

	HModGeomPar*    module   = fWallGeometry->getModule(0);
	HGeomTransform &trans    = module->getLabTransform();
	HGeomVolume*    rodVol   = module->getRefVolume()->getComponent(c);
	HGeomTransform &rodTrans = rodVol->getTransform();

	p = rodTrans.transFrom(p);  // transform to module coordinate system
	p = trans.transFrom(p);     // transform from module to LAB system

	p *= TO_CM;    // mm -> cm

    } else {
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HEDTransform::calcTofGeantPointLab(Int_t s,Int_t m,Int_t c,HGeomVector& p)
{
    // calc RPC geant point (Module system) in LAB ccordinate sys (cm!)

    HTofGeomPar* fTofGeometry=(HTofGeomPar *)(gHades->getRuntimeDb()->getContainer("TofGeomPar"));


    if(fTofGeometry) {

	HModGeomPar*    module   = fTofGeometry->getModule(s,m);
	HGeomTransform &trans    = module->getLabTransform();
	HGeomVolume*    rodVol   = module->getRefVolume()->getComponent(c);
	HGeomTransform &rodTrans = rodVol->getTransform();

	p = rodTrans.transFrom(p);  // transform to module coordinate system
	p = trans.transFrom(p);     // transform from module to LAB system

	p *= TO_CM;    // mm -> cm

    } else {
	return kFALSE;
    }
    return kTRUE;
}


Bool_t HEDTransform::calcShowerGeantPointLab(Int_t s,Int_t m,HGeomVector& p)
{
    // calc SHOWER geant point (Module system) in LAB ccordinate sys (cm!)

    HShowerGeometry* fShowerGeometry=(HShowerGeometry *)(gHades->getRuntimeDb()->getContainer("ShowerGeometry"));


    if(fShowerGeometry) {
	HLocation loc;
	loc.set(2,0,0);
	loc[0] = s;
        loc[1] = m;

	HGeomVector local  = p;
	HGeomVector2 lab   = p;
	fShowerGeometry->transVectToLab(loc,local,lab);
        p  = lab;
	p *= TO_CM;    // mm -> cm

    } else {
	return kFALSE;
    }
    return kTRUE;
}



Bool_t HEDTransform::calcRpcGeantPointLab(Int_t s,HGeomVector& p)
{
    // calc RPC geant point (Module system) in LAB ccordinate sys (cm!)
    HRpcGeomPar* pRpcGeometry   = (HRpcGeomPar*)(gHades->getRuntimeDb()->getContainer("RpcGeomPar"));
    if(pRpcGeometry) {

	//Lab system: all sectors in Lab
	HGeomTransform& TransRpc2Lab = pRpcGeometry->getModule(s,0)->getLabTransform();
	p = TransRpc2Lab.transFrom(p);
	p *= TO_CM;    // mm -> cm

    } else {
	return kFALSE;
    }
    return kTRUE;
}


Bool_t HEDTransform::calcVertexPoint(HGeomVector& p)
{
    // calc vertex point in LAB coordinate sys (cm!)
    if(gHades){
	HVertex& vertex=gHades->getCurrentEvent()->getHeader()->getVertex();
	p = vertex.getPos();
        if(p.getZ() == -1000.) p.setXYZ(0.,0.,0); // no vertex calulated !
	p *= TO_CM;            // mm -> cm
    } else {
     return kFALSE;
    }
    return kTRUE;
}

Bool_t HEDTransform::calcRichLinePointLab(HRichHit* hit,HGeomVector& p1,HGeomVector& p2,HParticleCand* cand){
    // calculates Line from event vertex to MDCI
    // with theta, phi from richhit. The points
    // of the line are in LAB coordiante sys (cm!)

    HMdcSizesCells* sizes         =  HMdcSizesCells::getExObject();
    HMdcGetContainers* containers =  HMdcGetContainers::getObject();
    if(sizes&&containers){
	if(hit){
	    HVertex& vertex = gHades->getCurrentEvent()->getHeader()->getVertex();
	    p1              = vertex.getPos();
	    if(p1.getZ() == -1000.) p1.setXYZ(0.,0.,0); // no vertex calulated !

	    Double_t theta =-1, phi =-1;

	    Int_t sec      = hit->getSector();
	    if(!cand){
		theta = hit->getTheta()*TMath::DegToRad();
		phi   = hit->getPhi();
	    } else {
		theta = cand->getTheta()*TMath::DegToRad();
		phi   = cand->getPhi();
	    }
	     if(sec < 5) {
                   phi -= (sec*60.);
	    } else {
                   phi +=60.;
	    }
	    phi *= TMath::DegToRad();

	    HMdcDetector* mdcDet = (HMdcDetector*) gHades->getSetup()->getDetector("Mdc");
	    if(!mdcDet) { cerr<<"helpers::calcRichLinePointLab() : No Mdc Detector defined!"<<endl;
		exit(1);
	    }

	    if(!mdcDet->getModule(sec,0) ){
		cerr<<"helpers::calcRichLinePointLab() : module in segment not defined in detector! Need Geometry!"<<endl;
		exit(1);
	    }

	    HGeomTransform transSec;
	    containers->getLabTransSec(transSec,sec);
            p1 = transSec.transTo(p1);    // Lab to sec coordinate system

	    p2.setX(p1.X() + cos(phi)*sin(theta));
	    p2.setY(p1.Y() + sin(phi)*sin(theta));
	    p2.setZ(p1.Z() + cos(theta));

	    Int_t first = 0;


	    HMdcSizesCellsMod* module = &((*sizes)[sec][first]);
	    if(!module){
		cerr<<"helpers::calcRichLinePointLab() : Received NULL pointer for HMdcSizesMod s = "<<sec<<" m = "<<first<<"!"<<endl;
		return kFALSE;
	    }
	    Double_t x,y,z;
	    module->calcIntersection(p1.X(),p1.Y(),p1.Z(),p2.X(),p2.Y(),p2.Z(),x,y,z);
	    p2.setXYZ(x,y,z);

	    HGeomTransform trans;
	    containers->getLabTransSec(trans,sec,first);

	    p2 = trans.transFrom(p2);   // Sec to Lab coordinate system
	    p2 *= TO_CM;              // mm -> cm
	    p1  = vertex.getPos();
	    if(p1.getZ() == -1000.) p1.setXYZ(0.,0.,0); // no vertex calulated !
	    p1 *= TO_CM;

	} else {
	    cerr<<"helpers::calcRichLinePointLab() : Received NULL pointer for HMdcSeg!"<<endl;
	    return kFALSE;
	}

    } else {
	cerr<<"helpers::calcRichLinePointLab() : Received NULL pointer for HMdcSizesCells!"<<endl;
	exit(1);
    }
    return kTRUE;

}

TEveFrameBox* HEDTransform::calcRichSectorFrame(Int_t sec,HGeomTransform& trans)
{
    // calc the RICH frame of sector sec in LAB coordinate sys (cm!)
    // transformation trans provides the theta rotation and offset to
    // go to sector system. Phi rotation by sector gives LAB sys.
    HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
    if(fRichGeometryPar)
    {
	HRichFrame  &fFrame = *fRichGeometryPar->getFramePar();
        Float_t secphi = HEDTransform::calcPhiToLab(sec)*TMath::DegToRad();

	Float_t cosPhi = cos(secphi);
        Float_t sinPhi = sin(secphi);

	//--------------------------------------------------------------------
	HRichFrameCorner* frcorner;
	Float_t coordCorner[7*3];

	HGeomVector p;
 	for(Int_t i = 0; i < 7; i ++){
	    frcorner = fFrame.getCorner(i);
	    p.setXYZ( frcorner->getX(),frcorner->getY(),0);
	    p = trans.transFrom(p); // to sector sys
            // to LAB sys
	    coordCorner[i*3+0] =  p.X() * cosPhi + p.Y() * -sinPhi;
	    coordCorner[i*3+1] =  p.X() * sinPhi + p.Y() *  cosPhi;
	    coordCorner[i*3+2] =  p.Z();
	}
	//--------------------------------------------------------------------

	TEveFrameBox* box = new TEveFrameBox();
	box->SetQuadByPoints(coordCorner,7);
	box->SetFrameColor(kGray);
	return box;
    }
    return 0;
}

TEveFrameBox* HEDTransform::calcWallFrame()
{
    // calc the WALL frame in LAB coordinate sys (cm!)

    HWallGeomPar* fWallGeomPar = (HWallGeomPar*) gHades->getRuntimeDb()->getContainer("WallGeomPar");
    if(fWallGeomPar)
    {
	//--------------------------------------------------------------------
	HModGeomPar*    module   = fWallGeomPar->getModule(0);
	HGeomTransform &trans    = module->getLabTransform();

        HGeomVector p(-880,-880,0)  ; // lower right
        p = trans.transFrom(p);
        p *= TO_CM;    // mm -> cm

	TEveFrameBox* box = new TEveFrameBox();
	box->SetAAQuadXY(p.X(), p.Y(), p.Z(), 2*88., 2*88.);
	box->SetFrameColor(kGray);
	return box;
    }
    return 0;
}

Bool_t HEDTransform::calcWallCell(HWallHit* hit,Float_t* coord /* [12], xyz 4 corners*/)
{

    // calc 4 front corner points of a given WALL cell

    HWallGeomPar* fWallGeomPar = (HWallGeomPar*) gHades->getRuntimeDb()->getContainer("WallGeomPar");
    if(fWallGeomPar)
    {
	Int_t c = hit->getCell();

	HModGeomPar*    module   = fWallGeomPar->getModule(0);
	HGeomTransform &trans    = module->getLabTransform();
	HGeomVolume*    rodVol   = module->getRefVolume()->getComponent(c);
	HGeomTransform &rodTrans = rodVol->getTransform();

	for(Int_t i = 0; i < 4; i ++){
            HGeomVector p(*rodVol->getPoint(i));
	    p = rodTrans.transFrom(p);  // transform to module coordinate system
	    p = trans.transFrom(p);     // transform from module to LAB system
	    p *= TO_CM;    // mm -> cm

	    coord[i*3+0] =  p.X();
	    coord[i*3+1] =  p.Y();
	    coord[i*3+2] =  p.Z();
	}

	return kTRUE;
    }

    return kFALSE;
}

Bool_t HEDTransform::calcRichPadPlaneToLab(Int_t sec,HGeomVector& p, HGeomTransform& trans)
{
    // calc space coordinates of a RichPad in LAB coordinate sys
    // transformation trans provides the theta rotation and offset to
    // go to sector system. Phi rotation by sector gives LAB sys.

    HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
    if(fRichGeometryPar)
    {
        Float_t secphi = HEDTransform::calcPhiToLab(sec)*TMath::DegToRad();
        Float_t cosPhi = cos(secphi);
        Float_t sinPhi = sin(secphi);

	//--------------------------------------------------------------------
	p = trans.transFrom(p); // to sector sys

	// to LAB sys
	p.setXYZ( p.X() * cosPhi + p.Y() * -sinPhi,
		  p.X() * sinPhi + p.Y() *  cosPhi,
		  p.Z());
	return kTRUE;
    }
    return kFALSE;
}

Bool_t HEDTransform::calcRichPadSector(HRichCal* cal, HGeomTransform& trans, Float_t* coord /* [12], xyz 4 corners*/)
{
    // calc space coordinates of a RichPad in LAB coordinate sys (!cm)
    // transformation trans provides the theta rotation and offset to
    // go to sector system. Phi rotation by sector gives LAB sys.


    HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
    if(fRichGeometryPar)
    {
        Int_t sec = cal->getSector();
	HRichPadTab &fPads  = *fRichGeometryPar->getPadsPar();
        Float_t secphi = HEDTransform::calcPhiToLab(sec)*TMath::DegToRad();
        Float_t cosPhi = cos(secphi);
        Float_t sinPhi = sin(secphi);

	//--------------------------------------------------------------------
        HRichPad* pad = fPads.getPad((UInt_t)cal->getCol(),(UInt_t)cal->getRow());
	if(pad && pad->getCornersNr() == 4)
	{
            HGeomVector p;
	    HRichPadCorner*   padcorner;
	    for(Int_t i = 0; i < 4; i ++){
		padcorner       =  pad->getCorner(i);
		p.setXYZ(-padcorner->getX(),padcorner->getY(),0);
		p = trans.transFrom(p); // to sector sys

                // to LAB sys
		coord[i*3+0] =  p.X() * cosPhi + p.Y() * -sinPhi;
		coord[i*3+1] =  p.X() * sinPhi + p.Y() *  cosPhi;
		coord[i*3+2] =  p.Z();
	    }
	    return kTRUE;
	}
    }
    return kFALSE;
}

Bool_t HEDTransform::calcRichMirrorHit(const HGeomVector& p1, const HGeomVector& p2, HGeomVector& pout)
{
    // calculates hit point of a line (represented by p1,p2, LAB [cm])
    // with the RICH mirror. Result is returned in pout (LAB [cm])
    //
    // To calculate a line-sphere intersection  we need:
    //    * A ray with a known point of origin offset, and
    //      direction vector dir.
    //    * a sphere with a known centre at a point c and a
    //      known radius r
    // Given the above mentioned sphere, a point p lies on the surface
    // of the sphere if
    // (p-c)*(p-c) = r^2
    // Given a line with a point of origin offset, and a direction vector dir:
    // line(t) = offset + t*dir  t > 0
    // setting both equal
    // (offset + t*dir - c)*(offset + t*dir - c) = r^2
    // (dir*dir)*t^2 + 2*(offset-c)*dir*t + (offset-c)*(offset-c) - r^2 = 0
    //  == > At2 + Bt + C = 0
    // where
    //   A = dir*dir
    //   B = 2(offset-c)*dir
    //   C =  (offset-c)*(offset-c) - r^2
    // solve using quadratic formular
    // no positive real root mean no intersection
    //
    // Simplification:
    // If a sphere is centred at origin, a point p lies on a sphere
    // of radius r2 if
    // p * p = r^2
    // we can find the t at which the (transformed) line
    // (shifted by center vector c) intersects the sphere by
    // (offset1 + t*dir) * (offset1 + t*dir) = r^2
    // quadratic form :
    // At2 + Bt + C = 0
    // which now has coefficients:
    //    A = dir*dir
    //    B = 2(dir*offset1)
    //    C = offset1*offset1 - r^2
    //
    // There are two possible solutions to the quadratic equation:
    // t_0 = (-B -\sqrt(B^2 - 4AC)) / 2A
    // t_1 = (-B +\sqrt(B^2 - 4AC)) / 2A
    //
    // if the discriminant is negative, i.e. (B2 - 4AC) < 0,
    // line has missed the sphere.
    // To avoid poor numeric precision when B ~ \sqrt(B^2 - 4AC)
    // t0 and t1 can be rewritten:
    // t_0 = q/A
    // t_1 = C/q
    // where
    // q = (-B + \sqrt(B^2 - 4AC)) / 2       B < 0
    //     (-B - \sqrt(B^2 - 4AC)) / 2       otherwise.
    //
    // The calculated hit point has to shifted back finally


    HGeomVector center, centerReal;
    center    .setXYZ(0,0,0); // system of sphere
    centerReal.setXYZ(0,0,0);

    //--------------------------------------------------
    // get Mirror parameters
    if(richMirrorTrans){
	centerReal = richMirrorTrans->transFrom(center); // LAB
    } else {
	::Error("calcRichMirrorHit()","richMirrorTrans == 0!");
	return kFALSE;
    }
    Float_t r = 0;

    if(richMirror){
        r = richMirror->GetRmin(); // min radius of sphere
    } else {
	::Error("calcRichMirrorHit()","richMirror == 0!");
	return kFALSE;
    }
    //--------------------------------------------------



    //--------------------------------------------------
    // line equation
    // line (t) = offset + t*dir
    HGeomVector dir,offset;
    Float_t t;
    dir    = p1 - p2;
    offset = p1 - dir; // make sure we are behind the sphere
    offset = offset - centerReal; // now line in sphere system
    //--------------------------------------------------


    //Compute A, B and C coefficients
    Float_t a = dir.scalarProduct(dir);
    Float_t b = 2 * dir.scalarProduct(offset);
    Float_t c = offset.scalarProduct(offset) - (r * r);
    //Find discriminant
    Float_t disc = b * b - 4 * a * c;

    // if discriminant is negative there are no
    // real roots, so return false as line misses sphere
    if (disc < 0) {
	::Warning("calcRichMirrorHit()","disc<0!");
	return kFALSE;
    }

    // compute q as described above
    Float_t distSqrt = sqrt(disc);
    Float_t q;
    if (b < 0) { q = (-b - distSqrt)/2.0; }
    else       { q = (-b + distSqrt)/2.0; }

    // compute t0 and t1
    Float_t t0 = q / a;
    Float_t t1 = c / q;

    // make sure t0 is smaller than t1
    if (t0 > t1) {
        // if t0 is bigger than t1 swap them around
        Float_t temp = t0;
        t0 = t1;
        t1 = temp;
    }

    // if t1 is less than zero, the object is
    // in the line's negative direction and
    // consequently the line misses the sphere
    if (t1 < 0) {
        ::Warning("calcRichMirrorHit()","t1 < 0, missed the sphere!");
	return kFALSE; }

    // if t0 is less than zero, the intersection
    // point is at t1
    if (t0 < 0)  {
        t = t1;
    } else {
	// else the intersection point is at t0
	t = t0;
    }

    // calculate hit point on sphere
    dir *= t;
    pout  = offset + dir;
    pout += centerReal;  // sphere to LAB

    return kTRUE;
}

//----------------------------------------------------------------


//----------------------------------------------------------------
TEveTrack* HEDTransform::createParticle(Int_t pid,
					Double_t vx,Double_t vy,Double_t vz,
					Double_t px,Double_t py,Double_t pz,
					TEveTrackPropagator* prop)
{
    Int_t sign       = HPhysicsConstants::charge(pid);
    TEveRecTrack *rc = new TEveRecTrack();

    Double_t scaleX = 0.1;   // [mm->cm  ]
    Double_t scaleP = 0.001; // [MeV->GeV]
    rc->fV.Set(vx*scaleX,vy*scaleX,vz*scaleX);
    rc->fP.Set(px*scaleP,py*scaleP,pz*scaleP);
    rc->fSign = sign;

    TEveTrack* track = new TEveTrack(rc, prop);
    track->SetName(HPhysicsConstants::pid(pid) ? Form("ID %s", HPhysicsConstants::pid(pid)) : "unknown");

    TString title="";
    title +="-------------------------------\n";
    TString cname;
    if(HPhysicsConstants::pid(pid) && pid >= 0){ cname = HPhysicsConstants::pid(pid);}
    else                                       { cname = "unknown"; }


    title += Form(" %s (momentum %5.3f : (%5.3f %5.3f %5.3f) MeV/c, vertex : %5.3f %5.3f %5.3f mm )\n"
		  ,track->GetName()
		  ,px
		  ,py
		  ,pz
		  ,sqrt(px*px+py*py+pz*pz)
		  ,px
		  ,py
		  ,pz );
    track->SetTitle(title);
    return track;

}
//----------------------------------------------------------------

//----------------------------------------------------------------
TEveTrack* HEDTransform::createKineParticle(HGeantKine* kine,TEveTrackPropagator* prop)
{
    if(kine)
    {
	Float_t vx,vy,vz;
	Float_t px,py,pz;
	Int_t  id =  kine->getID();

	kine->getVertex(vx,vy,vz);
	kine->getMomentum(px,py,pz);

	Int_t sign       = HPhysicsConstants::charge(id);
	TEveRecTrack *rc = new TEveRecTrack();

	Double_t scaleX = 0.1;   // [mm->cm  ]
	Double_t scaleP = 0.001; // [MeV->GeV]
	rc->fV.Set(vx*scaleX,vy*scaleX,vz*scaleX);
	rc->fP.Set(px*scaleP,py*scaleP,pz*scaleP);
	rc->fSign = sign;

	TEveTrack* track = new TEveTrack(rc, prop);
	track->SetName(HPhysicsConstants::pid(id) ? Form("ID %s", HPhysicsConstants::pid(id)) : "unknown");
        Int_t hitInd = -1;


	//----------------------------------------------------------------
	hitInd = kine->getFirstMdcHit();

	if(hitInd >= 0)
	{
	    HCategory* catMdc = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcGeantRaw);
	    if(catMdc)
	    {
		HGeomVector p;
		HGeantMdc* mdc = 0;
		while ( (mdc = HCategoryManager::getObject(mdc,catMdc,hitInd)) ){

		    Int_t l  = mdc->getLayer();
		    hitInd   = mdc->getNextHitIndex();
		    if(l != 6) {
			Float_t x,y,tof,mom;
			mdc->getHit(x,y,tof,mom);
			p.setX(x);
			p.setY(y);
			p.setZ(0);

			if(HEDTransform::calcMdcGeantLayerPointLab(mdc->getSector(),mdc->getModule(),mdc->getLayer(),p))
			{
			    TEvePathMark* pm = new TEvePathMark(TEvePathMark::kDaughter);    // 5.22
			    pm->fV.Set(p.getX(),p.getY(),p.getZ());
			    pm->fTime = tof;
			    track->AddPathMark((TEvePathMark&)*pm);
			}
		    }
		}
	    }
	} // end loop over mdc
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	hitInd = kine->getFirstRpcHit();
	if(hitInd >= 0)
	{

	    HCategory* catRpc = (HCategory*)gHades->getCurrentEvent()->getCategory(catRpcGeantRaw);

	    if(catRpc)
	    {
		Float_t geaTof    = 0.;
		Float_t geaX      = 0.;
		Float_t geaY      = 0.;
		Float_t geaZ      = 0.;
		HGeomVector p;

		HGeantRpc* rpc = 0;
		while ( (rpc = HCategoryManager::getObject(rpc,catRpc,hitInd)) ){

		    hitInd = rpc->getNextHitIndex();
		    if(rpc->getDetectorID() < 0) continue; //Hit in virtual volume EBOX
		    Int_t s = rpc->getSector();
                    rpc->getCellAverage(geaX, geaY, geaZ, geaTof); // [mm], [ns], [MeV] in module ref system
		    p.setXYZ(geaX, geaY, geaZ);

		    if(HEDTransform::calcRpcGeantPointLab(s,p)){

			TEvePathMark* pm = new TEvePathMark(TEvePathMark::kDaughter);    // 5.22
			pm->fV.Set(p.getX(),p.getY(),p.getZ());
			pm->fTime = geaTof;
			track->AddPathMark((TEvePathMark&)*pm);
		    }
		}
	    }
	}
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	hitInd = kine->getFirstShowerHit();
	if(hitInd >= 0)
	{

	    HCategory* catShower = (HCategory*)gHades->getCurrentEvent()->getCategory(catShowerGeantRaw);

	    if(catShower)
	    {
		Float_t geaEner   = 0.;
		Float_t geaX      = 0.;
		Float_t geaY      = 0.;
		Float_t geaBeta   = 0.;
		HGeomVector p;

		HGeantShower* shower = 0;
		while ( (shower = HCategoryManager::getObject(shower,catShower,hitInd)) ){

		    hitInd = shower->getNextHitIndex();
		    Int_t s = shower->getSector();
                    Int_t m = shower->getModule();

                    shower->getHit(geaEner, geaX, geaY, geaBeta);

		    p.setXYZ(geaX, geaY,0.);

		    if(HEDTransform::calcShowerGeantPointLab(s,m,p)){

			TEvePathMark* pm = new TEvePathMark(TEvePathMark::kDaughter);    // 5.22
			pm->fV.Set(p.getX(),p.getY(),p.getZ());
			//pm->fTime = geaTof;   // does not exist
			track->AddPathMark((TEvePathMark&)*pm);
		    }
		}
	    }
	}
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	hitInd = kine->getFirstTofHit();
	if(hitInd >= 0)
	{

	    HCategory* catTof = (HCategory*)gHades->getCurrentEvent()->getCategory(catTofGeantRaw);

	    if(catTof){
		HGeomVector p;
		Float_t geaTof  = 0.;
		Float_t geaEner = 0.;
		Float_t geaX    = 0.;
		Float_t geaY    = 0.;     // used by Tofino only
		Float_t geaMom  = 0.;
                Float_t trackLen= 0.;

		HGeantTof* tof = 0;
		while ( (tof = HCategoryManager::getObject(tof,catTof,hitInd)) )
		{
		    hitInd = tof->getNextHitIndex();

		    Int_t m = tof->getModule();
		    if (m > 21 || m < 14) continue;   // this is a Tofino, skip it!
		    m = 21 - m;                       // Tof modules in HGeant: (0->21) = (in->out)

		    Int_t s = tof->getSector();
		    Int_t c = tof->getCell();
		    c = 7 - c;                       // reverse also order of rods in Tof module

                    tof->getHit(geaEner,geaX,geaY,geaTof,geaMom,trackLen);


		    p.setXYZ(geaX, 0., 0.);

		    if(HEDTransform::calcTofGeantPointLab(s,m,c,p)){

			TEvePathMark* pm = new TEvePathMark(TEvePathMark::kDaughter);    // 5.22
			pm->fV.Set(p.getX(),p.getY(),p.getZ());
			pm->fTime = geaTof;
			track->AddPathMark((TEvePathMark&)*pm);
		    }
		}
	    }
	}
	//----------------------------------------------------------------


	//----------------------------------------------------------------
	hitInd = kine->getFirstWallHit();
	if(hitInd >= 0)
	{

	    HCategory* catWall = (HCategory*)gHades->getCurrentEvent()->getCategory(catWallGeantRaw);

	    if(catWall){
		HGeomVector p;
		Float_t geaTof  = 0.;
		Float_t geaEner = 0.;
		Float_t geaX    = 0.;
		Float_t geaY    = 0.;     // used by Tofino only
		Float_t geaMom  = 0.;
                Float_t trackLen= 0.;

		HGeantWall* wall = 0;
		while ( (wall = HCategoryManager::getObject(wall,catWall,hitInd)) )
		{
		    hitInd = wall->getNextHitIndex();

                    wall->getHit(geaEner,geaX,geaY,geaTof,geaMom,trackLen);


		    p.setXYZ(geaX,geaY, 0.);
                    Int_t c = wall->getCell();
		    if(HEDTransform::calcWallGeantPointLab(c,p)){

			TEvePathMark* pm = new TEvePathMark(TEvePathMark::kDaughter);    // 5.22
			pm->fV.Set(p.getX(),p.getY(),p.getZ());
			pm->fTime = geaTof;
			track->AddPathMark((TEvePathMark&)*pm);
		    }
		}
	    }
	}
	//----------------------------------------------------------------





	TString title="";
	title +="-------------------------------\n";
	TString cname;
	if(HPhysicsConstants::pid(id) && id >= 0){ cname = HPhysicsConstants::pid(id);}
	else                                     { cname = "unknown"; }


	title += Form(" tr = %i (%s , %5.3f MeV/c, parent %i grandparent %i)\n"
		      ,kine->getTrack()
		      ,cname.Data()
		      ,kine->getTotalMomentum()
		      ,kine->getParentTrack()
		      ,kine->getParentTrack() );
	track->SetTitle(title);
        return track;
    }

    return 0;

}
//----------------------------------------------------------------

//----------------------------------------------------------------
HEDMdcWireManager::HEDMdcWireManager()
{
    // Helpers class to collect statistics
    // about Mdc wires in the event.
    // Usage:
    //
    // HEDMdcWireManager manager;
    // manager.fill(); // for each event
    // manager.isUsedNtimes(sec,mod,lay,cell)
    // returns usage count of the wire
    // (-1 if nothing filled,
    //   0 for existing cal object
    //  >0 used by segment (after fit)
    // )
}
HEDMdcWireManager::~HEDMdcWireManager()
{
}

void HEDMdcWireManager::clear(){

    // clear the count table per wire
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    for(Int_t l = 0; l < 6; l ++){
		for(Int_t c = 0; c < 220; c ++){
		    wires[s][m][l][c] = -1;
		}
	    }
	}
    }

}
Int_t HEDMdcWireManager::fill(){

    // fill the count table per wire
    // Loops over cal1 and segment category
    // to get the list of all wires and
    // how often a wires has been used in
    // the event. Needs to be called one time
    // per event before one can use
    // isUsedNtimes(s,m,l,c)
    clear();
    Int_t n = fillAllWires();
    fillSegmentWires();
    return n;
}
Int_t HEDMdcWireManager::fillAllWires()
{
    // loop over Mdc Cal1 cat and make a list of all
    // wires in the current event. the initial count
    // of the wire is set to 0

    HCategory* mdcCal1Cat = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcCal1);
    Int_t nwires = 0;
    if(mdcCal1Cat){
	HMdcCal1* cal;
        Int_t s,m,l,c;
	Int_t size = mdcCal1Cat->getEntries();
	for(Int_t i = 0; i < size; i ++){
	    cal = HCategoryManager::getObject(cal,mdcCal1Cat,i);
	    if(cal){
		cal->getAddress(s,m,l,c);
		wires[s][m][l][c] ++;
                nwires ++;
	    }
	}
    }
    return nwires;
}

Int_t HEDMdcWireManager::fillSegmentWires()
{
    // loop over MdcSegment cat and count up
    // each wire which has been used in segments.
    // Non fitted wires are ignored.

    HCategory* mdcSegCat = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcSeg);
    Int_t nwires = 0;
    if(mdcSegCat){
	HMdcSeg* seg;
        Int_t s,m=0,l,c,io,ncell;
	Int_t size = mdcSegCat->getEntries();
	for(Int_t i = 0; i < size; i ++){
	    seg = HCategoryManager::getObject(seg,mdcSegCat,i);
	    if(seg){
		s  = seg->getSec();
                io = seg->getIOSeg();
		for(Int_t lay = 0; lay < 12; lay ++)
		{
		    if(io == 0 ){ m = ( lay < 6 ) ? 0 : 1;}
		    if(io == 1 ){ m = ( lay < 6 ) ? 2 : 3;}

		    if(lay >= 6 ) l = lay - 6;
                    else          l = lay;

		    ncell = seg->getNCells(lay);

		    for(Int_t n = 0; n < ncell; n ++){
			c = seg->getCell(lay,n);
			wires[s][m][l][c] ++;
                        nwires ++;
		    }
		}
	    }
	}
    }
    return nwires;
}
Int_t HEDMdcWireManager::isUsedNtimes(Int_t s,Int_t m,Int_t l,Int_t c)
{
    // return how often a wire has been used
    // in the event (by segment)
    // fill() should be called before.
    // returns usage count of the wire
    // (-1 if nothing filled,
    //   0 for existing cal object
    //  >0 used by segment (after fit)
    // )
    return wires[s][m][l][c];
}
//----------------------------------------------------------------


//----------------------------------------------------------------
Bool_t HEDMakeContainers::init(void){

    HRuntimeDb* rtdb = gHades->getRuntimeDb();

    rtdb->getContainer("MdcTrackGFieldPar");
    rtdb->getContainer("MagnetPar");

    rtdb->getContainer("MdcRawStruct");
    rtdb->getContainer("MdcGeomStruct");
    rtdb->getContainer("MdcLookupRaw");
    rtdb->getContainer("MdcLookupGeom");
    rtdb->getContainer("MdcLayerCorrPar");
    rtdb->getContainer("MdcLayerGeomPar");
    rtdb->getContainer("MdcGeomPar");
    rtdb->getContainer("SpecGeomPar");
    if(HEDTransform::isNewRich()){
        rtdb->getContainer("Rich700DigiMapPar");
    } else {
	rtdb->getContainer("RichGeometryParameters");
    }
    rtdb->getContainer("RpcGeomPar");
    if(!HEDTransform::isEmc())rtdb->getContainer("ShowerGeometry");
    rtdb->getContainer("TofGeomPar");
    rtdb->getContainer("WallGeomPar");

    return kTRUE;
}
Bool_t HEDMakeContainers::reinit(void){

    HMdcSizesCells* fSizesCells = HMdcSizesCells::getObject();
    Bool_t retval = fSizesCells->initContainer();
    if(!retval) {
	Error("reinit()","EVENT DISPLAY : createHades() INIT OF HMdcSizesCells FAILED ! ###########");
	return kFALSE;
    }
    HMdcGetContainers::getObject();


    return kTRUE;
}
//----------------------------------------------------------------
