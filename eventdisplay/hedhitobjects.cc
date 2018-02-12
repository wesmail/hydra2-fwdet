#include "hedhitobjects.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hphysicsconstants.h"
#include "hcategorymanager.h"


#include "hgeantdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "richdef.h"
#include "tofdef.h"
#include "walldef.h"
#include "showerdef.h"
#include "emcdef.h"
#include "rpcdef.h"
#include "hparticledef.h"

#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"
#include "hwallhit.h"
#include "hwallhitsim.h"
#include "htofhit.h"
#include "htofhitsim.h"
#include "htofcluster.h"
#include "htofclustersim.h"
#include "hshowerhit.h"
#include "hshowerhitsim.h"
#include "hemccluster.h"
#include "hemcclustersim.h"
#include "hrichhit.h"
#include "hrichhitsim.h"
#include "hrichcal.h"
#include "hrpccluster.h"
#include "hrpcclustersim.h"
#include "hmetamatch2.h"
#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hparticlestructs.h"

#include "hgeantkine.h"
#include "hgeantrich.h"

#include "hgeomtransform.h"
#include "hgeomvector.h"
#include "hrichgeometrypar.h"
#include "hrich700digipar.h"
#include "hrich700data.h"
#include "hmdcsizescells.h"
#include "hwallgeompar.h"


#include "hedhelpers.h"


#include "TMath.h"
#include "TEveRGBAPalette.h"
#include "TEveManager.h"

#include <iostream>
using namespace std;


ClassImp(HEDVertex)
ClassImp(HEDSegment)
ClassImp(HEDMdcWire)
ClassImp(HEDRichHit)
ClassImp(HEDWallHit)
ClassImp(HEDWallPlane)
ClassImp(HEDTofHit)
ClassImp(HEDTofCluster)
ClassImp(HEDRpcCluster)
ClassImp(HEDShowerHit)
ClassImp(HEDEmcCluster)
ClassImp(HEDParticleCand)
ClassImp(HEDRichPadPlane)
ClassImp(HEDRichRing)
ClassImp(HEDRichHitPadPlane)
ClassImp(HEDRichCompound)
ClassImp(HEDRichGeantPadPlane)
ClassImp(HEDRichGeantMirror)
ClassImp(HEDGroup)
ClassImp(HEDGroup2D)

//-----------------------------------------------------------------
HEDVertex::HEDVertex():TEvePointSet(2)
{
    // create vertex point. No input needed. Takes
    // vertex from HEventHeader.

    HGeomVector pVertex;
    HEDTransform::calcVertexPoint(pVertex);
    SetElementName("EDVertex");
    SetTitle(Form("EDVertex: x=%5.3f,y=%5.3f,z=%5.3f",pVertex.X(),pVertex.Y(),pVertex.Z()));
    SetPoint(0, pVertex.X(),pVertex.Y(),pVertex.Z());
    SetPoint(1, pVertex.X(),pVertex.Y(),pVertex.Z()+0.01);  // just dummy to make it draw!
    SetMarkerColor(kMagenta);
    SetMarkerSize(1.5);
    SetPickable(kTRUE);
}
HEDVertex::~HEDVertex(){;}

void HEDVertex::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDSegment::HEDSegment(HMdcSegSim* seg):TEveLine(2)
{
    // create segment line (between middle planes of two MDCs)
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    HGeomVector p1,p2;
    HEDTransform::calcSegPointsLab(seg,p1,p2);
    HCategory* segCat= gHades->getCurrentEvent()->getCategory(catMdcSeg);
    Int_t index = -1;
    if(segCat) index = segCat->getIndex(seg);
    TString title = "EDSegment: \n";
    title += Form(" ind   = %i\n"     ,index);
    title += Form(" sec   = %i\n"     ,seg->getSec());
    title += Form(" IO    = %i\n"     ,seg->getIOSeg());
    title += Form(" Chi2  = %5.3f\n"  ,seg->getChi2());
    title += Form(" r     = %5.3f\n"  ,seg->getRprime());
    title += Form(" z     = %5.3f\n"  ,seg->getZprime());
    title += Form(" phi   = %5.3f\n"  ,seg->getPhi()  *TMath::RadToDeg());
    title += Form(" theta = %5.3f\n"  ,seg->getTheta()*TMath::RadToDeg());
    title += Form(" nLay  = %i 1th , %i 2nd\n",seg->getNLayers(0) ,seg->getNLayers(1) );
    title += Form(" nCell = %i 1th , %i 2nd\n",seg->getSumWires(0),seg->getSumWires(1));

    TString cname = seg->ClassName();
    if(cname.CompareTo("HMdcSegSim") == 0){

	Int_t n = seg->getNTracks();
 	title += Form(" nTr = %i \n",n);

	HCategory* kineCat= gHades->getCurrentEvent()->getCategory(catGeantKine);

	for(Int_t i = 0; i < n; i ++){
            Int_t tr = seg->getTrack(i);
	    Int_t parentTr = -1;
            Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine =0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
                parentTr = kine->getParentTrack();


		if(HPhysicsConstants::pid(kine->getID())) { cname = HPhysicsConstants::pid(kine->getID());}
		else                                      { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i x %i(%i) (%s,%6.2f MeV/c, parent %i)\n",tr,seg->getNTimes(i),seg->getNDigiTimes(i)
			  ,cname.Data(),mom, parentTr);
	}
    }

    title += Form(" p1(%5.3f,%5.3f,%5.3f)\n",p1.X(),p1.Y(),p1.Z());
    title += Form(" p2(%5.3f,%5.3f,%5.3f)\n",p2.X(),p2.Y(),p2.Z());

    SetElementName("EDSegment");
    SetTitle(title.Data());
    SetPoint(0, p1.X(),p1.Y(),p1.Z());
    SetPoint(1, p2.X(),p2.Y(),p2.Z());  // just dummy to make it draw!
    SetMarkerColor(kRed);
    SetLineColor(kRed);
    SetRnrLine(kTRUE);
    SetPickable(kTRUE);
}
HEDSegment::~HEDSegment(){;}

void HEDSegment::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDMdcWire::HEDMdcWire(HMdcCal1Sim* cal)
{
    // create wire line
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    nTimes = 0;
    Int_t s,m,l,c;
    if(cal){
        cal->getAddress(s,m,l,c);
        makeWire(s,m,l,c,cal);
    }
}
HEDMdcWire::HEDMdcWire(Int_t s,Int_t m,Int_t l,Int_t c)
{
    // create wire line
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    nTimes = 0;
    makeWire(s,m,l,c);
}


Bool_t HEDMdcWire::makeWire(Int_t s,Int_t m,Int_t l,Int_t c, HMdcCal1Sim* cal)
{
    HGeomVector p1,p2;
    Bool_t ok = HEDTransform::calcWirePointsLab (s,m,l,c,p1,p2);

    TString title = "EDMdcWire: \n";
    title += Form(" sec   = %i\n"     ,s);
    title += Form(" mod   = %i\n"     ,m);
    title += Form(" lay   = %i\n"     ,l);
    title += Form(" cell  = %i\n"     ,c);
    if(cal){
	title += Form(" t1     = %5.3f\n"   ,cal->getTime1());
        title += Form(" t2-t1  = %5.3f\n"   ,cal->getTime2() - cal->getTime1());

	TString cname = cal->ClassName();
	if(cname.CompareTo("HMdcCal1Sim") == 0){
 	    title += Form(" status  = %i\n" ,cal->getStatus());

	    HCategory* kineCat= gHades->getCurrentEvent()->getCategory(catGeantKine);

            //--------------------------------------------------------
	    // selected track
	    Int_t tr = cal->getNTrack1();
	    Int_t parentTr = -1;
            Float_t mom    = -1;

	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
                parentTr = kine->getParentTrack();

		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form(" tr = %i (%s,%6.2f MeV/c, parent %i)\n",tr,cname.Data(),mom, parentTr);
            //--------------------------------------------------------

            //--------------------------------------------------------
	    // list of tracks
	    Int_t n = cal->getNTracks();
	    title += Form(" ntr = %i\n",n);
	    for(Int_t i = 0; i < n; i++){

                tr = cal->getTrackFromList(i);
		if(kineCat && tr > 0) {
		    HGeantKine* kine = 0;
		    kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		    mom      = kine->getTotalMomentum();
		    parentTr = kine->getParentTrack();

		    if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		    else                                     { cname = "unknown"; }
		} else {
		    cname = "unknown";
		}
		title += Form("\t tr = %5i status = %i (%s,%6.2f MeV/c, parent %i)\n",
			      tr,cal->getStatusFromList(i),cname.Data(),mom, parentTr);
	    }
	    //--------------------------------------------------------

	} // sim case

    }
    title += Form(" p1(%5.3f,%5.3f,%5.3f)\n",p1.X(),p1.Y(),p1.Z());
    title += Form(" p2(%5.3f,%5.3f,%5.3f)\n",p2.X(),p2.Y(),p2.Z());

    SetElementName("EDMdcWire");
    SetTitle(title.Data());
    SetNextPoint(p1.X(),p1.Y(),p1.Z());
    SetNextPoint(p2.X(),p2.Y(),p2.Z());
    SetMarkerColor(kBlue);
    SetLineColor(kBlue);
    SetRnrLine(kTRUE);
    SetPickable(kTRUE);
    return ok;
}
HEDMdcWire::~HEDMdcWire(){;}

void HEDMdcWire::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDRichHit::HEDRichHit(HRichHitSim* hit,HParticleCandSim* cand):TEveLine(2)
{
    // create a line representing a RICH hit from
    // the Event vertex to MDCI taking theta and phi
    // from the RICH hit.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    HGeomVector p1,p2,pmirror;
    HEDTransform::calcRichLinePointLab(hit,p1,p2,cand);
    HEDTransform::calcRichMirrorHit(p1,p2,pmirror);

    HCategory* richHitCat = gHades->getCurrentEvent()->getCategory(catRichHit);
    Int_t index = -1;
    if(richHitCat) index = richHitCat->getIndex(hit);
    Float_t phi = -1,theta =-1;
    Int_t  sec  = hit->getSector();
    if(!cand) {
	phi   = hit->getPhi();
	theta = hit->getTheta();
    } else {
	phi   = cand->getPhi();
	theta = cand->getTheta();
    }
    if(sec < 5) {
	phi -= (sec*60.);
    } else {
	phi +=60.;
    }

    TString title = "EDRichHit: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" sec   = %i\n"   ,sec);
    title += Form(" phi   = %5.3f\n",phi);
    title += Form(" theta = %5.3f\n",theta);
    if(!hit->isNewRich()){
	title += Form(" hough = %i \n"  ,hit->getRingHouTra());
	title += Form(" ampl  = %i\n"   ,hit->getRingAmplitude());
	title += Form(" patMat= %i\n"   ,hit->getRingPatMat());
	title += Form(" padNr = %i\n"   ,hit->getRingPadNr());
	title += Form(" centr = %5.3f\n",hit->getCentroid());
    } else {
 	title += Form(" radius= %5.3f\n",hit->getRich700CircleRadius());
	title += Form(" chi2  = %6.3f\n",hit->getRich700CircleChi2());
	title += Form(" nCals = %i\n"   ,hit->getRich700NofRichCals() );
    }

    TString cname = hit->ClassName();
    if(cname.CompareTo("HRichHitSim") == 0){

	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
	Int_t tracks[3] = {hit->track1    ,hit->track2    ,hit->track3};
        Int_t weight[3] = {hit->weigTrack1,hit->weigTrack2,hit->weigTrack3};

	for(Int_t i = 0; i < 3; i ++){
            Int_t tr = tracks[i];
	    Int_t parentTr = -1;
            Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
                parentTr = kine->getParentTrack();
		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i x %i weight (%s,%6.2f MeV/c, parent %i)\n",tr,weight[i]
			  ,cname.Data(),mom,parentTr);
	}
    }

    title += Form(" p1(%5.3f,%5.3f,%5.3f)\n",p1.X(),p1.Y(),p1.Z());
    title += Form(" p2(%5.3f,%5.3f,%5.3f)\n",p2.X(),p2.Y(),p2.Z());

    SetElementName("EDRichHit");
    SetTitle(title.Data());
    SetPoint(0, p1.X(),p1.Y(),p1.Z());
    SetPoint(1, pmirror.X(),pmirror.Y(),pmirror.Z());
    SetMarkerColor(kYellow);
    SetLineColor(kYellow);
    SetRnrLine(kTRUE);
    SetPickable(kTRUE);
}
HEDRichHit::~HEDRichHit(){;}

void HEDRichHit::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDWallHit::HEDWallHit(HWallHitSim* hit):TEvePointSet(1)
{
    // creates a point at the position of the WALL hit.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    HGeomVector p;
    HEDTransform::calcWallHitPointLab(hit,p);

    HCategory* wallHitCat = gHades->getCurrentEvent()->getCategory(catWallHit);
    Int_t index = -1;
    if(wallHitCat) index = wallHitCat->getIndex(hit);

    TString title = "EDWallHit: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" addr  = cell=%i\n",hit->getCell());
    title += Form(" Time  = %5.3f\n",hit->getTime());
    title += Form(" Chrg  = %5.3f\n",hit->getCharge());
    title += Form(" Phi   = %5.3f\n",hit->getPhi());
    title += Form(" Theta = %5.3f\n",hit->getTheta());
    title += Form(" Dist  = %5.3f\n",hit->getDistance());

    TString cname = hit->ClassName();
    if(cname.CompareTo("HWallHitSim") == 0){

	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
	Int_t   tracks[2] = {hit->getNTrack1() ,hit->getNTrack2()};

	for(Int_t i = 0; i < 2; i ++){
            Int_t tr = tracks[i];
	    Int_t parentTr = -1;
            Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
                parentTr = kine->getParentTrack();
		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i (%s,%6.2f MeV/c, parent %i)\n",tr,cname.Data(),mom,parentTr);
	}
    }

    title += Form(" p(%5.3f,%5.3f,%5.3f)\n",p.X(),p.Y(),p.Z());

    SetElementName("EDWallHit");
    SetTitle(title.Data());
    SetPoint(0, p.X(),p.Y(),p.Z());
    SetMarkerColor(kMagenta);
    SetPickable(kTRUE);
}
HEDWallHit::~HEDWallHit(){;}

void HEDWallHit::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDWallPlane::HEDWallPlane()
: TEveQuadSet("Wallplane")
{
    // creates a TEveQuadSet object for a WallPlane
    // The object contains the HWallHits hits of the
    // event. The digits are filled by the charge.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    HWallGeomPar* fWallPar = (HWallGeomPar*) gHades->getRuntimeDb()->getContainer("WallGeomPar");
    if(fWallPar)
    {
	//--------------------------------------------------------------------
        // calc the frame box (has to be deleted in destructor!)
	box = HEDTransform::calcWallFrame();
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	TEveRGBAPalette *pal = new TEveRGBAPalette(0,50);
	SetOwnIds(kTRUE);  // quad deletes the id objects
        SetPalette(pal);
	SetFrame(box);     //
        Reset(TEveQuadSet::kQT_FreeQuad, kFALSE, 32);

	//--------------------------------------------------------------------
        // loop for the cal objects and fill the quad
	HCategory* wallHitCat = HCategoryManager::getCategory(catWallHit,2);
	if(wallHitCat){
	    Float_t coord[4*3]; // xyz 4 corner of pad
            HWallHit* hit;
	    for(Int_t i = 0; i < wallHitCat->getEntries(); i ++)
	    {
                hit = HCategoryManager::getObject(hit,wallHitCat,i);


		HEDTransform::calcWallCell(hit,coord);
		AddQuad(coord);
		QuadValue((Int_t)hit->getCharge());
		QuadId(new TNamed(Form("WallHitPlane_%i",i), "TNamed assigned to a quad as an indentifier."));
	    }
	}
	//--------------------------------------------------------------------
	RefitPlex();
    }
}
HEDWallPlane::~HEDWallPlane(){
    //if(box) delete box;
}

void HEDWallPlane::Print(){
    cout<<GetTitle()<<endl;
}

//-----------------------------------------------------------------


//-----------------------------------------------------------------
HEDTofHit::HEDTofHit(HTofHitSim* hit):TEvePointSet(1)
{
    // creates a point at the position of the TOF hit.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    HGeomVector p;
    HEDTransform::calcTofHitPointLab(hit,p);

    HCategory* tofHitCat = gHades->getCurrentEvent()->getCategory(catTofHit);
    Int_t index = -1;
    if(tofHitCat) index = tofHitCat->getIndex(hit);

    TString title = "EDTofHit: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" addr  = sec=%i,mod=%i,cell=%i\n",hit->getSector(),hit->getModule(),hit->getCell());
    title += Form(" Tof   = %5.3f\n",hit->getTof());
    title += Form(" Edep  = %5.3f\n",hit->getEdep());

    TString cname = hit->ClassName();
    if(cname.CompareTo("HTofHitSim") == 0){

	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
	Int_t   tracks[2] = {hit->getLeftNTrack() ,hit->getRightNTrack()};

	for(Int_t i = 0; i < 2; i ++){
            Int_t tr = tracks[i];
	    Int_t parentTr = -1;
            Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
                parentTr = kine->getParentTrack();
		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i (%s,%6.2f MeV/c, parent %i)\n",tr,cname.Data(),mom,parentTr);
	}
    }

    title += Form(" p(%5.3f,%5.3f,%5.3f)\n",p.X(),p.Y(),p.Z());

    SetElementName("EDTofHit");
    SetTitle(title.Data());
    SetPoint(0, p.X(),p.Y(),p.Z());
    SetMarkerColor(kYellow);
    SetPickable(kTRUE);
}
HEDTofHit::~HEDTofHit(){;}

void HEDTofHit::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDTofCluster::HEDTofCluster(HTofClusterSim* hit):TEvePointSet(1)
{
    // creates a point at the position of the TOF cluster.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    HGeomVector p;
    HEDTransform::calcTofHitPointLab(hit,p);

    HCategory* tofClusterCat = gHades->getCurrentEvent()->getCategory(catTofCluster);
    Int_t index = -1;
    if(tofClusterCat) index = tofClusterCat->getIndex(hit);

    TString title = "EDTofCluster: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" addr  = sec=%i,mod=%i,cell=%i\n",hit->getSector(),hit->getModule(),hit->getCell());
    title += Form(" Tof   = %5.3f\n",hit->getTof());
    title += Form(" Edep  = %5.3f\n",hit->getEdep());
    title += Form(" clSize= %i\n"   ,hit->getClusterSize());

    TString cname = hit->ClassName();
    if(cname.CompareTo("HTofClusterSim") == 0){

	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);

	for(Int_t i = 0; i < hit->getNParticipants(); i ++){

	    Int_t   tracks[2] = {hit->getNTrack1(i),hit->getNTrack2(i)};
	    title += Form(" participant %i :\n",i);
	    for(Int_t j = 0; j < 2; j ++){

		Int_t tr = tracks[j];
		Int_t parentTr = -1;
		Float_t mom    = -1;
		if(kineCat && tr > 0 ){
		    HGeantKine* kine = 0;
		    kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		    if(kine){
			mom      = kine->getTotalMomentum();
			parentTr = kine->getParentTrack();
			if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
			else                                     { cname = "unknown"; }
		    } else {
			Error("HEDTofCluster()","Zero pointer retrieved for kine object. track=%i,sec=%i,mod=%i,cell=%i",
			      tr,hit->getSector(),hit->getModule(),hit->getCell());
			cout<<"cl size "<<hit->getClusterSize()<<" participants "<<hit-> getNParticipants()<<endl;
			hit->Dump();
		    }
		} else {
		    cname = "unknown";
		}
		title += Form("\t tr = %i (%s,%6.2f MeV/c, parent %i)\n",tr,cname.Data(),mom,parentTr);
	    } // hits
	}// participants
    }
    title += Form(" p(%5.3f,%5.3f,%5.3f)\n",p.X(),p.Y(),p.Z());

    SetElementName("EDTofCluster");
    SetTitle(title.Data());
    SetPoint(0, p.X(),p.Y(),p.Z());
    SetMarkerColor(kYellow);
    SetPickable(kTRUE);
}
HEDTofCluster::~HEDTofCluster(){;}

void HEDTofCluster::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------



//-----------------------------------------------------------------
HEDRpcCluster::HEDRpcCluster(HRpcClusterSim* hit):TEvePointSet(1)
{
    // creates a point at the position of the RPC cluster.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    HGeomVector p;
    HEDTransform::calcRpcClustPointLab(hit,p);

    HCategory* rpcClusterCat = gHades->getCurrentEvent()->getCategory(catRpcCluster);
    Int_t index = -1;
    if(rpcClusterCat) index = rpcClusterCat->getIndex(hit);

    TString title = "EDRpcCluster: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" addr  = sec=%i,col=(%i,%i),cell=(%i,%i)\n",hit->getSector(),hit->getColumn1(),hit->getColumn2(),hit->getCell1(),hit->getCell2());
    title += Form(" Tof   = %5.3f\n",hit->getTof());
    title += Form(" charge= %5.3f\n",hit->getCharge());
    title += Form(" clSize= %i\n"   ,hit->getClusterType());

    TString cname = hit->ClassName();
    if(cname.CompareTo("HRpcClusterSim") == 0){
	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
	Int_t  tracks[4];
        Bool_t  atBox[4];
	hit->getTrackList  (&tracks[0]);
        hit->getIsAtBoxList(&atBox[0]);
	for(Int_t i = 0; i < 4; i ++){
	    Int_t tr = tracks[i];
	    Int_t parentTr = -1;
	    Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
		parentTr = kine->getParentTrack();
		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i, atBox %i, (%s,%6.2f MeV/c, parent %i)\n",tr,(Int_t)atBox[i],cname.Data(),mom,parentTr);
	} // hits
    }

    title += Form(" p(%5.3f,%5.3f,%5.3f)\n",p.X(),p.Y(),p.Z());

    SetElementName("EDRpcCluster");
    SetTitle(title.Data());
    SetPoint(0, p.X(),p.Y(),p.Z());
    SetMarkerColor(kGreen);
    SetPickable(kTRUE);
}
HEDRpcCluster::~HEDRpcCluster(){;}

void HEDRpcCluster::Print(){
    cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDShowerHit::HEDShowerHit(HShowerHitSim* hit):TEvePointSet(1)
{
    // creates a point at the position of the SHOWER hit.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    HGeomVector p;
    HEDTransform::calcShowerHitPointLab(hit,p);


    TString cname = hit->ClassName();
    Bool_t isSim  = kFALSE ;
    if(cname.CompareTo("HShowerHitSim") == 0) isSim = kTRUE;

    HCategory* showerHitCat = gHades->getCurrentEvent()->getCategory(catShowerHit);

    Int_t index = -1;
    if(showerHitCat) index = showerHitCat->getIndex(hit);

    TString title = "EDShowerHit: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" addr  = sec=%i,mod=%i,col=%i,row=%i\n",hit->getSector(),hit->getModule(),hit->getCol(),hit->getRow());
    title += Form(" sum   = %5.3f,%5.3f,%5.3f\n",hit->getSum(0),hit->getSum(1),hit->getSum(2));
    title += Form(" charge= %5.3f\n",hit->getCharge());

    if(isSim){

	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
	for(Int_t i = 0; i < hit->getNTracks(); i ++){
            Int_t tr = hit->getTrack(i);
	    Int_t parentTr = -1;
            Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
                parentTr = kine->getParentTrack();
		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i (%s,%6.2f MeV/c, parent %i)\n",tr,cname.Data(),mom,parentTr);
	}
    }

    title += Form(" p(%5.3f,%5.3f,%5.3f)\n",p.X(),p.Y(),p.Z());

    SetElementName("EDShowerHit");
    SetTitle(title.Data());
    SetPoint(0, p.X(),p.Y(),p.Z());
    SetMarkerColor(kCyan);
    SetPickable(kTRUE);
}
HEDShowerHit::~HEDShowerHit(){;}

void HEDShowerHit::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDEmcCluster::HEDEmcCluster(HEmcClusterSim* hit):TEvePointSet(1)
{
    // creates a point at the position of the SHOWER hit.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.
    HGeomVector p;
    HEDTransform::calcEmcClusterPointLab(hit,p);

    TString cname = hit->ClassName();
    Bool_t isSim  = kFALSE ;
    if(cname.CompareTo("HEmcClusterSim") == 0) isSim = kTRUE;

    HCategory* emcClusterCat = gHades->getCurrentEvent()->getCategory(catEmcCluster);

    Int_t index = -1;
    if(emcClusterCat) index = emcClusterCat->getIndex(hit);

    TString title = "EDEmcCluster: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" addr  = sec=%i,cell=%i\n",hit->getSector(),hit->getCell());
    title += Form(" time  = %5.3f\n",hit->getTime());
    title += Form(" energy= %5.3f\n",hit->getEnergy());
    title += Form(" ncells= %i\n"   ,hit->getNCells());
    title += Form(" active= %i\n"   ,hit->ifActive());
    title += Form(" used  = %i\n"   ,hit->isUsedInParticleCand());
    title += Form(" match = %i\n"   ,hit->getNMatchedCells()>0? 1: 0);
    title += Form(" theta = %5.3f\n",hit->getTheta());
    title += Form(" phi   = %5.3f\n",hit->getPhi());

    if(isSim){

	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
	for(Int_t i = 0; i < hit->getNTracks(); i ++){
	    Int_t tr = hit->getTrack(i);
	    Int_t parentTr = -1;
	    Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
		parentTr = kine->getParentTrack();
		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i (%s,%6.2f MeV/c, parent %i)\n",tr,cname.Data(),mom,parentTr);
	}
    }

    title += Form(" p(%5.3f,%5.3f,%5.3f)\n",p.X(),p.Y(),p.Z());

    SetElementName("EDEmcCluster");
    SetTitle(title.Data());
    SetPoint(0, p.X(),p.Y(),p.Z());
    SetMarkerColor(kCyan);
    SetPickable(kTRUE);
}
HEDEmcCluster::~HEDEmcCluster(){;}

void HEDEmcCluster::Print(){
    cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDParticleCand::HEDParticleCand(HParticleCandSim* cand)
{
    // creates a compound object for a HParticleCand.
    // The object contains the detector hits of the
    // candidate and a line connecting all points.
    // For the bending inside the magnetc field a
    // additional hitpoint at the kickplane is used.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    TString cname = cand->ClassName();
    Bool_t isSim  = kFALSE ;
    if(cname.CompareTo("HParticleCandSim") == 0) isSim = kTRUE;
    HCategory* mdcSegCat      = HCategoryManager::getCategory(catMdcSeg,2);
    HCategory* richHitCat     = HCategoryManager::getCategory(catRichHit,2);
    HCategory* tofHitCat      = HCategoryManager::getCategory(catTofHit,2);
    HCategory* tofClusterCat  = HCategoryManager::getCategory(catTofCluster,2);
    HCategory* rpcClusterCat  = HCategoryManager::getCategory(catRpcCluster,2);

    HCategory* showerHitCat = HCategoryManager::getCategory(catShowerHit,2);
    HCategory* emcClusterCat= HCategoryManager::getCategory(catEmcCluster,2);

    Float_t x,y,z;
    TEveLine* track = new TEveLine();

    //-----------------------------------------------------------------
    if(richHitCat && cand->getRichInd() >= 0) {
	HRichHitSim* hit     = 0;
        HEDRichHit* edrichhit = 0;
	hit = HCategoryManager::getObject(hit,richHitCat,cand->getRichInd());
	edrichhit = new HEDRichHit(hit,cand);
	edrichhit->SetLineColor(kYellow);

        //--------------------------------------------------------------
        // draw line from mirror hit to pad plane
	TEveLine* lineToPadPlane = new TEveLine(2);
	lineToPadPlane->SetElementName("EDLineRichPadPlaneToShell");
	lineToPadPlane->SetTitle("EDLineRichPadPlaneToShell");
        edrichhit->GetPoint(1,x,y,z);       // mirror hit
	lineToPadPlane->SetPoint(1,x,y,z);

	Float_t x,y,z;
	HGeomVector p;
	if(hit->isNewRich()){
	    hit->getLabXYZ(&x,&y,&z);
	    p.setXYZ(x,y,z);
            p *= TO_CM;
	} else {
	    hit->getXY(&x,&y);
	    x*=-1;
	    p.setXYZ(x,y,0);

	    HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
	    if(fRichGeometryPar)
	    {
		//--------------------------------------------------------------------
		// get transformation RICH padplane -> sector sys
		HGeomTransform& trans = *HEDTransform::getRichSecTrans();
		HEDTransform::calcRichPadPlaneToLab(cand->getSector(),p,trans);
		//--------------------------------------------------------------------
	    }
	}

	lineToPadPlane->SetPoint(0,p.X(),p.Y(),p.Z());  // pad plane hit

	lineToPadPlane->SetMarkerColor(kYellow);
	lineToPadPlane->SetLineColor(kYellow);
	lineToPadPlane->SetLineStyle(7);
	lineToPadPlane->SetRnrLine(kTRUE);
	lineToPadPlane->SetPickable(kTRUE);
        //--------------------------------------------------------------

	AddElement(edrichhit);
	AddElement(lineToPadPlane);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if(mdcSegCat) {
	HMdcSegSim* seg  = 0;
	HEDSegment* edseg = 0;

	seg = HCategoryManager::getObject(seg,mdcSegCat,cand->getInnerSegInd());
	edseg = new HEDSegment(seg);
        edseg->SetLineColor(kRed);

	TEveLine* lineToRichShell = new TEveLine(2);
	lineToRichShell->SetElementName("EDLineSegToRichShell");
	lineToRichShell->SetTitle("EDLineSegToRichShell");

	edseg->GetPoint(0,x,y,z);
	HGeomVector p1(x,y,z);
        edseg->GetPoint(1,x,y,z);
	HGeomVector p2(x,y,z);
	HGeomVector pout;

	HEDTransform::calcRichMirrorHit(p1,p2,pout);

	lineToRichShell->SetPoint(1,p1.X(),p1.Y(),p1.Z());
	lineToRichShell->SetPoint(0,pout.X(),pout.Y(),pout.Z());
	lineToRichShell->SetMarkerColor(kYellow);
	lineToRichShell->SetLineColor(kYellow);
	lineToRichShell->SetLineStyle(7);
	lineToRichShell->SetRnrLine(kTRUE);
	lineToRichShell->SetPickable(kTRUE);

	AddElement(lineToRichShell);

	AddElement(edseg);

        edseg->GetPoint(0,x,y,z);
	track->SetNextPoint(x,y,z);

	edseg->GetPoint(1,x,y,z);
	track->SetNextPoint(x,y,z);

	HGeomVector p;
	HEDTransform::calcSegKickPlanePointLab(seg,p);

	track->SetNextPoint(p.X(),p.Y(),p.Z());

	if(cand->getOuterSegInd()>= 0){
	    seg = HCategoryManager::getObject(seg,mdcSegCat,cand->getOuterSegInd());
	    edseg = new HEDSegment(seg);
	    edseg->SetLineColor(kBlue);
	    AddElement(edseg);

	    edseg->GetPoint(0,x,y,z);
	    track->SetNextPoint(x,y,z);

	    edseg->GetPoint(1,x,y,z);
	    track->SetNextPoint(x,y,z);
	}
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if(rpcClusterCat && cand->getRpcInd() >= 0) {
	HRpcClusterSim* hit        = 0;
        HEDRpcCluster* edrpccluster = 0;
	hit = HCategoryManager::getObject(hit,rpcClusterCat,cand->getRpcInd());
	edrpccluster = new HEDRpcCluster(hit);
	edrpccluster->SetMarkerColor(kGreen);
	AddElement(edrpccluster);

	edrpccluster->GetPoint(0,x,y,z);
	track->SetNextPoint(x,y,z);

    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if(showerHitCat && cand->getShowerInd() >= 0) {
	HShowerHitSim* hit     = 0;
        HEDShowerHit* edshowerhit = 0;
	hit = HCategoryManager::getObject(hit,showerHitCat,cand->getShowerInd());
	edshowerhit = new HEDShowerHit(hit);
	edshowerhit->SetMarkerColor(kCyan);
	AddElement(edshowerhit);
	edshowerhit->GetPoint(0,x,y,z);
	track->SetNextPoint(x,y,z);
    }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if(emcClusterCat && cand->getEmcInd() >= 0) {
	HEmcClusterSim* hit     = 0;
        HEDEmcCluster* edemccluster = 0;
	hit = HCategoryManager::getObject(hit,emcClusterCat,cand->getEmcInd());
	edemccluster = new HEDEmcCluster(hit);
	edemccluster->SetMarkerColor(kCyan);
	AddElement(edemccluster);
	edemccluster->GetPoint(0,x,y,z);
	track->SetNextPoint(x,y,z);
    }
    //-----------------------------------------------------------------


    //-----------------------------------------------------------------
    if(tofHitCat && cand->getTofHitInd() >= 0) {
	HTofHitSim* hit    = 0;
        HEDTofHit* edtofhit = 0;
	hit = HCategoryManager::getObject(hit,tofHitCat,cand->getTofHitInd());
	edtofhit = new HEDTofHit(hit);
	edtofhit->SetMarkerColor(kYellow);
	AddElement(edtofhit);
	edtofhit->GetPoint(0,x,y,z);
	track->SetNextPoint(x,y,z);
      }
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    if(tofClusterCat && cand->getTofClstInd() >= 0) {
	HTofClusterSim* hit        = 0;
        HEDTofCluster* edtofcluster = 0;
	hit = HCategoryManager::getObject(hit,tofClusterCat,cand->getTofClstInd());
	edtofcluster = new HEDTofCluster(hit);
	edtofcluster->SetMarkerColor(kYellow);
	AddElement(edtofcluster);
	edtofcluster->GetPoint(0,x,y,z);
	track->SetNextPoint(x,y,z);
    }
    //-----------------------------------------------------------------



    TString title = "EDParticleCand: \n";
    title += Form(" isUsed = %i, isLepton = %i\n",(Int_t) cand->isFlagBit(Particle::kIsUsed),(Int_t) cand->isFlagBit(Particle::kIsLepton));
    title += Form(" ind        = %i\n"   ,cand->getIndex());
    title += Form(" sec        = %i\n"   ,cand->getSector());
    title += Form(" sys        = %i\n"   ,cand->getSystem());
    title += Form(" ringCorr   = %i (seg), %i (RK)\n",(Int_t)cand->isRichMatch(kIsRICHMDC),(Int_t)cand->isRichMatch(kIsRICHRK));
    title += Form(" pid        = %i\n"   ,cand->getPID());
    title += Form(" mdcDedx    = %5.3f\n",cand->getMdcdEdx());
    title += Form(" tofDedx    = %5.3f\n",cand->getTofdEdx());
    title += Form(" charge     = %i\n"   ,cand->getCharge());
    title += Form(" beta       = %5.3f\n",cand->getBeta());
    title += Form(" mom        = %5.3f\n",cand->getMomentum());
    title += Form(" mass       = %5.3f\n",(cand->getMass2() >= 0)? sqrt(cand->getMass2()): -1);
    title += Form(" phi        = %5.3f\n",cand->getPhi());
    title += Form(" theta      = %5.3f\n",cand->getTheta());
    title += Form(" r          = %5.3f\n",cand->getR());
    title += Form(" z          = %5.3f\n",cand->getZ());
    title += Form(" chi2       = %5.3f\n",cand->getChi2());
    title += Form(" richPhi    = %5.3f\n",cand->getRichPhi());
    title += Form(" richTheta  = %5.3f\n",cand->getRichTheta());
    title += Form(" distMeta   = %5.3f\n",cand->getDistanceToMetaHit());
    title += Form(" seg1Chi2   = %5.3f\n",cand->getInnerSegmentChi2());
    title += Form(" seg2Chi2   = %5.3f\n",cand->getOuterSegmentChi2());
    title += Form(" angToFit   = %5.3f\n",cand->getAngleToNearbyFittedInner());
    title += Form(" angToNoFit = %5.3f\n",cand->getAngleToNearbyUnfittedInner());
    title += Form(" metaQA     = %5.3f\n",cand->getMetaMatchQuality());
    if(!HEDTransform::isEmc()){
	title += Form(" metaQAShr  = %5.3f\n",cand->getMetaMatchQualityShower());
    } else {
	title += Form(" metaQAEmc  = %5.3f\n",cand->getMetaMatchQualityEmc());
	title += Form(" emcTime    = %5.3f\n",cand->getEmcTime());
	title += Form(" emcEnergy  = %5.3f\n",cand->getEmcEnergy());
    }
    title += Form(" sel meta   = %i\n"   ,cand->getSelectedMeta());
    title += Form(" ringQA     = %5.3f\n",cand->getRichInd() >= 0 ? cand->getRichMatchingQuality(): -1);
    if(!HEDTransform::isEmc())
    {
	title += Form(" hit Ind    = rich %i, seg1 %i, seg2 %i, tofhit %i, tofClst %i, rpc %i, shower %i,\n"
		      ,cand->getRichInd()
		      ,cand->getInnerSegInd()
		      ,cand->getOuterSegInd()
		      ,cand->getTofHitInd()
		      ,cand->getTofClstInd()
		      ,cand->getRpcInd()
		      ,cand->getShowerInd()
		     );
    } else {
	title += Form(" hit Ind    = rich %i, seg1 %i, seg2 %i, tofhit %i, tofClst %i, rpc %i, ecal %i,\n"
		      ,cand->getRichInd()
		      ,cand->getInnerSegInd()
		      ,cand->getOuterSegInd()
		      ,cand->getTofHitInd()
		      ,cand->getTofClstInd()
		      ,cand->getRpcInd()
		      ,cand->getEmcInd()
		     );
    }


    if(isSim) {
	title +="-------------------------------\n";
	if(HPhysicsConstants::pid(cand->getGeantPID()) && cand->getGeantPID()>= 0){ cname = HPhysicsConstants::pid(cand->getGeantPID());}
	else                                                                      { cname = "unknown"; }
	Float_t mom = sqrt(cand->getGeantxMom()*cand->getGeantxMom() +
			   cand->getGeantyMom()*cand->getGeantyMom() +
			   cand->getGeantzMom()*cand->getGeantzMom() ) ;


	title += Form(" tr = %i (%s , %5.3f MeV/c, parent %i grandparent %i)\n"
		      ,cand->getGeantTrack()
		      ,cname.Data()
		      ,mom
		      ,cand->getGeantParentTrackNum()
		      ,cand->getGeantGrandParentTrackNum() );

    if(!HEDTransform::isEmc())
    {
	title += Form(" isInDet  = rich %i, seg1 %i, seg2 %i, tof %i rpc %i, shower %i\n"
		      , (Int_t)cand->isInDetector(kIsInRICH)
                      , (Int_t)cand->isInDetector(kIsInInnerMDC)
                      , (Int_t)cand->isInDetector(kIsInOuterMDC)
                      , (Int_t)cand->isInDetector(kIsInTOF)
                      , (Int_t)cand->isInDetector(kIsInRPC)
                      , (Int_t)cand->isInDetector(kIsInSHOWER)
		     );
    } else {
	title += Form(" isInDet  = rich %i, seg1 %i, seg2 %i, tof %i rpc %i, ecal %i\n"
		      , (Int_t)cand->isInDetector(kIsInRICH)
                      , (Int_t)cand->isInDetector(kIsInInnerMDC)
                      , (Int_t)cand->isInDetector(kIsInOuterMDC)
                      , (Int_t)cand->isInDetector(kIsInTOF)
                      , (Int_t)cand->isInDetector(kIsInRPC)
                      , (Int_t)cand->isInDetector(kIsInEMC)
		     );
    }
	title += Form(" isGhost  = total %i seg1 %i seg2 %i\n"
		      ,(Int_t)cand->isGhostTrack()
		      ,(Int_t)cand->isInDetector(kIsInnerGhost)
                      ,(Int_t)cand->isInDetector(kIsOuterGhost)
		     );
    }

    track->SetElementName("EDParticleCand");
    track->SetTitle(title.Data());
    track->SetLineColor(kRed);
    track->SetMarkerColor(kRed);
    track->SetRnrLine(kTRUE);
    track->SetPickable(kTRUE);

    AddElement(track);

    SetElementName("EDParticleCand");
    SetTitle(title.Data());
    SetMainColor(kRed);
    SetPickable(kTRUE);
}
HEDParticleCand::~HEDParticleCand(){
}

void HEDParticleCand::Print(){
   cout<<GetTitle()<<endl;
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDRichPadPlane::HEDRichPadPlane(Int_t sec,Int_t cleaned)
: TEveQuadSet(Form("Rich_sector_%i_%i",sec,cleaned))
{
    // creates a TEveQuadSet object for a RichPadPlane (sector)
    // The object contains the HRichCalHits hits of the
    // event. The digits are filled by the charge.
    // cleaned == 0 (default) : fill all pads
    // cleaned == 1  : fill all cleaned pads  (single 10, high 20 sector 30)
    // cleaned == 2  : fill all pads but not cleaned
    //
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
    if(fRichGeometryPar)
    {
	//--------------------------------------------------------------------
        // get transformation RICH padplane -> sector sys
	HGeomTransform& trans = *HEDTransform::getRichSecTrans();
	//--------------------------------------------------------------------



	//--------------------------------------------------------------------
        // cal the frame box (has to be deleted in destructor!)
	box = HEDTransform::calcRichSectorFrame(sec,trans);
	//--------------------------------------------------------------------
	TEveRGBAPalette *pal = new TEveRGBAPalette(0,50);
	SetOwnIds(kTRUE);  // quad deletes the id objects
        SetPalette(pal);
	SetFrame(box);     //
        Reset(TEveQuadSet::kQT_FreeQuad, kFALSE, 32);

	//--------------------------------------------------------------------
        // loop for the cal objects and fill the quad
	HCategory* richCalCat = HCategoryManager::getCategory(catRichCal,2);
	if(richCalCat){
	    Float_t coord[4*3]; // xyz 4 corner of pad
            HRichCal* cal;
	    for(Int_t i = 0; i < richCalCat->getEntries(); i ++)
	    {
                cal = HCategoryManager::getObject(cal,richCalCat,i);
		HEDTransform::calcRichPadSector(cal,trans,coord);

		if(cleaned==0) {
		    AddQuad(coord);
		    QuadValue((Int_t)cal->getCharge());
		    QuadId(new TNamed(Form("all_sector_%i_%i",sec,i), "TNamed assigned to a quad as an indentifier."));

		}
		if(cleaned==1){

		    if(cal->getIsCleanedSingle() ||
		       cal->getIsCleanedHigh()   ||
		       cal->getIsCleanedSector())
		    {
			AddQuad(coord);
			if     (cal->getIsCleanedSingle()) { QuadValue(10); }
			else if(cal->getIsCleanedHigh  ()) { QuadValue(20); }
			else if(cal->getIsCleanedSector()) { QuadValue(30); }
			QuadId(new TNamed(Form("cut_sector_%i_%i",sec,i), "TNamed assigned to a quad as an indentifier."));

		    }
		}
		if(cleaned==2) {
		    if(!cal->getIsCleanedSingle() &&
		       !cal->getIsCleanedHigh() &&
		       !cal->getIsCleanedSector())
		    {
			AddQuad(coord);
			QuadValue((Int_t)cal->getCharge());
			QuadId(new TNamed(Form("not_cut_sector_%i_%i",sec,i), "TNamed assigned to a quad as an indentifier."));

		    }
		}
	    }
	}
	//--------------------------------------------------------------------
	RefitPlex();
    }
}
HEDRichPadPlane::~HEDRichPadPlane(){
    //if(box) delete box;
}

void HEDRichPadPlane::Print(){
    cout<<GetTitle()<<endl;
}

//-----------------------------------------------------------------
HEDRich700PadPlane::HEDRich700PadPlane()
: TEveQuadSet(Form("Rich700_padplane"))
{
    // creates a TEveQuadSet object for a Rich700PadPlane
    // The object contains the HRichCalHits hits of the
    // event.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    HRich700DigiPar* fRich700DigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
    if(fRich700DigiPar)
    {
	TEveRGBAPalette *pal = new TEveRGBAPalette(0,50);
	SetOwnIds(kTRUE);  // quad deletes the id objects
        SetPalette(pal);
        Reset(TEveQuadSet::kQT_FreeQuad, kFALSE, 32);

	//--------------------------------------------------------------------
        // loop for the cal objects and fill the quad
	HCategory* richCalCat = HCategoryManager::getCategory(catRichCal,2);
	if(richCalCat){
	    Float_t coord[4*3]; // xyz 4 corner of pad
            HRichCal* cal;
	    for(Int_t i = 0; i < richCalCat->getEntries(); i ++)
	    {
                cal = HCategoryManager::getObject(cal,richCalCat,i);
                Int_t col    = cal->getCol();
                Int_t row    = cal->getRow();
                Int_t pmtid  = fRich700DigiPar->getPMTId(col,row);
                Float_t w    = fRich700DigiPar->getPmtSize();
                Int_t  npix  = fRich700DigiPar->getNPixelInRow();
		Float_t pixw = w/npix;
                w/=2;



		HRich700PmtData* data = fRich700DigiPar ->getPMTData(pmtid);
		if(data){
		    //    +y   row 0 at max y
		    //     |
		    // +x--|   col 0 at max x
		    //
		    // (fx+w,fy+w) 1     2
                    //                x(fx,fy)
		    //             4     3
                    //
		    Int_t indx = npix-1 - (col%npix);  // local pixel index running along x in ascending order
                    Int_t indy = npix-1 - (row%npix);  // local pixel index running along y in ascending order

                    // 1
		    coord [0] = (data->fX+w- ((indx  )*pixw))*TO_CM;
                    coord [1] = (data->fY+w- ((indy  )*pixw))*TO_CM;
                    coord [2] = (data->fZ)*TO_CM;
                    // 2
		    coord [3] = (data->fX+w- ((indx+1)*pixw))*TO_CM;
                    coord [4] = (data->fY+w- ((indy  )*pixw))*TO_CM;
                    coord [5] = (data->fZ)*TO_CM;
                    // 3
		    coord [6] = (data->fX+w- ((indx+1)*pixw))*TO_CM;
                    coord [7] = (data->fY+w- ((indy+1)*pixw))*TO_CM;
                    coord [8] = (data->fZ)*TO_CM;
                    // 4
		    coord [9]  = (data->fX+w- ((indx  )*pixw))*TO_CM;
                    coord [10] = (data->fY+w- ((indy+1)*pixw))*TO_CM;
                    coord [11] = (data->fZ)*TO_CM;


		    AddQuad(coord);
		    QuadValue(10);
		    QuadId(new TNamed(Form("all_sector"), "TNamed assigned to a quad as an indentifier."));

		}
	    }
	}
	//--------------------------------------------------------------------
	RefitPlex();
    }
}
HEDRich700PadPlane::~HEDRich700PadPlane(){
}

void HEDRich700PadPlane::Print(){
    cout<<GetTitle()<<endl;
}

//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDRichRing::HEDRichRing(HRichHitSim* hit):TEveLine()
{
    // create a line representing a RICH ring on
    // the pad plane.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    HCategory* richHitCat = gHades->getCurrentEvent()->getCategory(catRichHit);
    Int_t index = -1;
    if(richHitCat) index = richHitCat->getIndex(hit);

    Int_t  sec  = hit->getSector();
    Float_t phi   = hit->getPhi();
    if(sec < 5) {
	phi -= (sec*60.);
    } else {
	phi +=60.;
    }

    TString title = "EDRichRing: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" sec   = %i\n"   ,sec);
    title += Form(" phi   = %5.3f\n",phi);
    title += Form(" theta = %5.3f\n",hit->getTheta());
    if(!hit->isNewRich())
    {
	title += Form(" hough = %i \n"  ,hit->getRingHouTra());
	title += Form(" ampl  = %i\n"   ,hit->getRingAmplitude());
	title += Form(" patMat= %i\n"   ,hit->getRingPatMat());
	title += Form(" padNr = %i\n"   ,hit->getRingPadNr());
	title += Form(" centr = %5.3f\n",hit->getCentroid());
	title += Form(" radius= %5.3f\n",hit->getRadius());
    } else {
 	title += Form(" radius= %5.3f\n",hit->getRich700CircleRadius());
	title += Form(" chi2  = %6.3f\n",hit->getRich700CircleChi2());
	title += Form(" nCals = %i\n"   ,hit->getRich700NofRichCals() );
    }

    Float_t R = 1., dW = 0, dH = 5;
    Int_t n = 10;

    Float_t x,y,z;
    HGeomVector p;

    if(hit->isNewRich())
    {
	hit->getLabXYZ(&x,&y,&z);

	if(hit->getRich700CircleRadius() > 1.) R = hit->getRich700CircleRadius(); // minimum radius

	for(Int_t i=0;i<=n;i++)
	{
	    p.setXYZ(x + (R+dW) * TMath::Cos(TMath::TwoPi()*i/n),
		     y + (R+dW) * TMath::Sin(TMath::TwoPi()*i/n),
		     z+dH);
            p *= TO_CM;
	    SetNextPoint(p.X(),p.Y(),p.Z());
	}

    } else {
	HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
	if(fRichGeometryPar)
	{
	    //--------------------------------------------------------------------
	    // get transformation RICH padplane -> sector sys
	    HGeomTransform& trans = *HEDTransform::getRichSecTrans();
	    //--------------------------------------------------------------------

	    hit->getXY(&x,&y);
	    x*=-1;

	    if(hit->getRadius() > 1.) R = hit->getRadius(); // minimum radius



	    for(Int_t i=0;i<=n;i++)
	    {
		p.setXYZ(x + (R+dW) * TMath::Cos(TMath::TwoPi()*i/n),
			 y + (R+dW) * TMath::Sin(TMath::TwoPi()*i/n),
			 dH);
		HEDTransform::calcRichPadPlaneToLab(sec,p,trans);
		SetNextPoint(p.X(),p.Y(),p.Z());
	    }
	}
    }



    SetElementName("EDRichRing");
    SetTitle(title.Data());
    SetMarkerColor(kRed);
    SetLineColor(kRed);
    SetRnrLine(kTRUE);
    SetSmooth(kTRUE);
    SetPickable(kTRUE);
}
HEDRichRing::~HEDRichRing(){;}

void HEDRichRing::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDRichHitPadPlane::HEDRichHitPadPlane(HRichHitSim* hit) : TEvePointSet(1)
{
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    HCategory* richHitCat = gHades->getCurrentEvent()->getCategory(catRichHit);
    Int_t index = -1;
    if(richHitCat) index = richHitCat->getIndex(hit);

    Int_t  sec  = hit->getSector();
    Float_t phi   = hit->getPhi();
    if(sec < 5) {
	phi -= (sec*60.);
    } else {
	phi +=60.;
    }

    Float_t x,y,z;

    HGeomVector p;
    if(hit->isNewRich()){
	hit->getLabXYZ(&x,&y,&z);
	p.setXYZ(x,y,z);
        p *= TO_CM;
    } else {

	hit->getXY(&x,&y);
	x*=-1;

	p.setXYZ(x,y,0);

	HRichGeometryPar* fRichGeometryPar = (HRichGeometryPar*) gHades->getRuntimeDb()->getContainer("RichGeometryParameters");
	if(fRichGeometryPar)
	{
	    //--------------------------------------------------------------------
	    // get transformation RICH padplane -> sector sys
	    HGeomTransform& trans = *HEDTransform::getRichSecTrans();
	    HEDTransform::calcRichPadPlaneToLab(sec,p,trans);
	    //--------------------------------------------------------------------
	}
    }

    TString title = "EDRichHit: \n";
    title += Form(" ind   = %i\n"   ,index);
    title += Form(" sec   = %i\n"   ,sec);
    title += Form(" phi   = %5.3f\n",phi);
    title += Form(" theta = %5.3f\n",hit->getTheta());
    if(!hit->isNewRich())
    {
	title += Form(" hough = %i \n"  ,hit->getRingHouTra());
	title += Form(" ampl  = %i\n"   ,hit->getRingAmplitude());
	title += Form(" patMat= %i\n"   ,hit->getRingPatMat());
	title += Form(" padNr = %i\n"   ,hit->getRingPadNr());
	title += Form(" centr = %5.3f\n",hit->getCentroid());
    } else {
	title += Form(" radius= 5.3%f\n",hit->getRich700CircleRadius());
	title += Form(" chi2  = %6.3f\n",hit->getRich700CircleChi2());
	title += Form(" nCals = %i\n"   ,hit->getRich700NofRichCals() );
    }


    TString cname = hit->ClassName();
    if(cname.CompareTo("HRichHitSim") == 0){

	HCategory* kineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
	Int_t   tracks[3] = {hit->track1    ,hit->track2    ,hit->track3};
        Int_t weight[3] = {hit->weigTrack1,hit->weigTrack2,hit->weigTrack3};

	for(Int_t i = 0; i < 3; i ++){
            Int_t tr = tracks[i];
	    Int_t parentTr = -1;
            Float_t mom    = -1;
	    if(kineCat && tr > 0 ){
		HGeantKine* kine = 0;
		kine = HCategoryManager::getObject(kine,kineCat,tr-1);
		mom      = kine->getTotalMomentum();
                parentTr = kine->getParentTrack();
		if(HPhysicsConstants::pid(kine->getID())){ cname = HPhysicsConstants::pid(kine->getID());}
		else                                     { cname = "unknown"; }
		if(i == 0) { // choose some markers for differnt cases
		    if(parentTr > 0 )                             { SetMarkerStyle(kPlus);} // secondary
		    if(kine->getID() > 3 ){ // no lep or gamma
			SetMarkerStyle(kPlus);
		        SetMarkerSize(2);
		    }// no lepton
		}
	    } else {
		cname = "unknown";
	    }
	    title += Form("\t tr = %i x %i weight (%s,%6.2f MeV/c, parent %i)\n",tr,weight[i]
			  ,cname.Data(),mom,parentTr);

	}
    }

    title += Form(" p(%5.3f,%5.3f,%5.3f)\n",p.X(),p.Y(),p.Z());

    SetElementName("EDRichHitPadPlane");
    SetTitle(title.Data());
    SetPoint(0, p.X(),p.Y(),p.Z());
    SetMarkerColor(kYellow);
    SetPickable(kTRUE);

}
HEDRichHitPadPlane::~HEDRichHitPadPlane(){;}

void HEDRichHitPadPlane::Print()
{
    cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDRichGeantPadPlane::HEDRichGeantPadPlane(HGeantKine* kine,Int_t select,HGeantRichDirect* geadir) : TEvePointSet()
{
    // Works for sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.


    //----------------------------------------------------------------
    Int_t hitInd = -1;

    if(select == 0 && kine)
    {
	hitInd = kine->getFirstRichHit();
	if(hitInd >= 0)
	{
	    HCategory* catRichPho = (HCategory*)gHades->getCurrentEvent()->getCategory(catRichGeantRaw);
	    if(catRichPho)
	    {
		HGeomVector p;
		HGeantRichPhoton* rich = 0;
		while ( (rich = HCategoryManager::getObject(rich,catRichPho,hitInd)) ){

		    if(!HEDTransform::isNewRich()){
			Int_t s  = rich->getSector();

			p.setX( -rich->getX());
			p.setY( rich->getY());
			p.setZ(0);

			if(HEDTransform::calcRichGeantPadplanePointLab(s,p))
			{
			    SetNextPoint(p.getX(),p.getY(),p.getZ());
			}
		    } else {

			HRich700DigiPar* fRich700DigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
			if(fRich700DigiPar){
                           HRich700PmtData* data =  fRich700DigiPar->getPMTData(rich->getPmtId());
			   if(data){
			       p.setXYZ(rich->getY()+data->fX,rich->getX()+data->fY,data->fZ);
                               p *= TO_CM;
			       SetNextPoint(p.getX(),p.getY(),p.getZ());
			   }
			}
		    }
		    hitInd   = rich->getNextHitIndex();
		} // end loop over rich photon
	    }
	}
    }

    if(select == 1 && geadir)
    {
        HCategory* catKine    = (HCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);

	if( catKine) kine = HCategoryManager::getObject(kine,catKine,geadir->getTrack()-1);

	HGeomVector p;

	if(!HEDTransform::isNewRich()){
	    Int_t s  = geadir->getSector();
	    p.setX( -geadir->getX());
	    p.setY(  geadir->getY());
	    p.setZ(0);

	    if(HEDTransform::calcRichGeantPadplanePointLab(s,p))
	    {
		SetNextPoint(p.getX(),p.getY(),p.getZ());
	    }
	} else {
	    HRich700DigiPar* fRich700DigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
	    if(fRich700DigiPar){
		HRich700PmtData* data =  fRich700DigiPar->getPMTData(geadir->getPmtId());
		if(data){
		    p.setXYZ(geadir->getY()+data->fX,geadir->getX()+data->fY,data->fZ);
		    p *= TO_CM;
		    SetNextPoint(p.getX(),p.getY(),p.getZ());
		}
	    }

	}
    }
    //----------------------------------------------------------------

    TString title = "EDRichGeantPadPlane: \n";

    Int_t  id = 1000;
    Float_t mom = -1;
    Int_t parentTrack = 0;
    Int_t grandparentTrack = 0;
    Int_t track = 0;
    if(kine){
	id    = kine->getID();
        track = kine->getTrack();
	mom   = kine->getTotalMomentum();
	parentTrack = kine->getParentTrack();

	if(kine->getParentTrack() > 0){
	    HCategory* catKine = (HCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
	    if(catKine){
		HGeantKine* parent = 0;
		parent = HCategoryManager::getObject(parent,catKine,kine->getParentTrack()-1);
		if(parent){
		    grandparentTrack = parent->getParentTrack();
		}
	    }
	}
    }
    TString cname;
    if(HPhysicsConstants::pid(id) && id >= 0){ cname = HPhysicsConstants::pid(id);}
    else                                     { cname = "unknown"; }





    title += Form(" tr = %i (%s , %5.3f MeV/c, parent %i grandparent %i)\n"
		      ,track
		      ,cname.Data()
		      ,mom
		      ,parentTrack
		      ,grandparentTrack );

    SetElementName("EDRichGeantPadPlane");
    SetTitle(title.Data());
    SetMarkerColor(kYellow);
    SetPickable(kTRUE);

}
HEDRichGeantPadPlane::~HEDRichGeantPadPlane(){;}

void HEDRichGeantPadPlane::Print()
{
    cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDRichGeantMirror::HEDRichGeantMirror(HGeantRichMirror* mirror) : TEvePointSet()
{
    // Works for sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.


    //----------------------------------------------------------------
    TString title;
    if(mirror)
    {
        HGeomVector p;
	Float_t x,y,z;
	//Int_t s  = mirror->getSector();
        Int_t trNum,id;
	mirror->getHit(x,y,z);
        mirror->getTrack(trNum, id);

	p.setXYZ(x,y,z);
        p *= 0.1; // mm -> cm
	SetNextPoint(p.getX(),p.getY(),p.getZ());


    title = "EDRichGeantMirror: \n";

    HGeantKine* kine = 0;

    Float_t ptot = 0;


    Int_t parentTrack = 0;
    Int_t grandparentTrack = 0;
    kine = HCategoryManager::getObject(kine,catGeantKine,trNum-1);
    if(kine){
	ptot          = kine->getTotalMomentum();
	parentTrack   = kine->getParentTrack();

	if(kine->getParentTrack() > 0){
	    HCategory* catKine = (HCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
	    if(catKine){
		HGeantKine* parent = 0;
		parent = HCategoryManager::getObject(parent,catKine,kine->getParentTrack()-1);
		if(parent){
		    grandparentTrack = parent->getParentTrack();
		}
	    }
	}
    }

    TString cname;
    if(HPhysicsConstants::pid(id) && id >= 0){ cname = HPhysicsConstants::pid(id);}
    else                                     { cname = "unknown"; }




    title += Form(" tr = %i (%s , %5.3f MeV/c, parent %i grandparent %i)\n"
		      ,trNum
		      ,cname.Data()
		      ,ptot
		      ,parentTrack
		      ,grandparentTrack);

    }
    SetElementName("EDRichGeantMirror");
    SetTitle(title.Data());
    SetMarkerColor(kYellow);
    SetPickable(kTRUE);

}
HEDRichGeantMirror::~HEDRichGeantMirror(){;}

void HEDRichGeantMirror::Print()
{
    cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//-----------------------------------------------------------------
HEDRichCompound::HEDRichCompound(HRichHitSim* hit)
{
    // create a compound object consisting a RICH hit and
    // ring on the pad plane.
    // Works for real and sim objects. The object info
    // will be displayed in TEve my mouse over events in
    // the GL Viewer and the EveBrowser of the Event scene.

    HEDRichHitPadPlane* edrichhitplane = new HEDRichHitPadPlane(hit);
    AddElement(edrichhitplane);
    TString title = edrichhitplane->GetTitle();

    HGeomVector p1,p2,pmirror;

    HEDTransform::calcRichLinePointLab(hit,p1,p2);
    HEDTransform::calcRichMirrorHit(p1,p2,pmirror);

    Float_t x,y,z;
    edrichhitplane->GetPoint(0,x,y,z);

    TEveLine* hitmirr = new TEveLine(2);
    hitmirr->SetElementName("EDRichHitMirror");
    hitmirr->SetTitle(edrichhitplane->GetTitle());

    hitmirr->SetPoint(0,x,y,z);
    hitmirr->SetPoint(1,pmirror.X(),pmirror.Y(),pmirror.Z());
    hitmirr->SetMarkerColor(kYellow);
    hitmirr->SetLineColor(kYellow);
    hitmirr->SetLineStyle(7);
    hitmirr->SetRnrLine(kTRUE);
    hitmirr->SetPickable(kTRUE);

    AddElement(hitmirr);

    HEDRichRing* edrichring = new HEDRichRing(hit);
    AddElement(edrichring);


    SetElementName("EDRichCompound");
    SetTitle(title.Data());
    SetPickable(kTRUE);
}
HEDRichCompound::~HEDRichCompound(){;}

void HEDRichCompound::Print(){
   cout<<GetTitle()<<endl;
}
//-----------------------------------------------------------------

//----------------------------------------------------------------
HEDGroup::HEDGroup(const Char_t* name,const Char_t* title, Int_t n, const Char_t* subname)
: TEveElementList(name,title)
{
    // List of n TEveElementLists with name/title subname_i
    // Useful for creating hirachical structures
    size = n;
    list = new TEveElementList* [size];
    for(Int_t i = 0; i < size; i ++){
	list[i] = new TEveElementList(Form("%s_%i",subname,i),Form("%s_%i",subname,i));
	TEveElement::AddElement((TEveElement*)list[i]);
    }
}
HEDGroup::~HEDGroup(){
    for(Int_t n = 0; n < size; n ++){
	list[n]->DestroyElements();
        delete list[n];
    }
}
//----------------------------------------------------------------

//----------------------------------------------------------------
HEDGroup2D::HEDGroup2D(const Char_t* name,const Char_t* title, Int_t n1,Int_t n2, const Char_t* subname1, const Char_t* subname2)
: TEveElementList(name,title)
{
    // 2 dim List of n1xn2 TEveElementLists with name/title
    // subname1_i subname2_i. Useful for creating hirachical
    // structures

    size1 = n1;
    size2 = n2;

    list1 = new TEveElementList* [size1];

    list2 = new TEveElementList** [size1];
    for(Int_t i = 0; i < size1; i ++){
	list2[i] = new TEveElementList* [size2];
    }

    for(Int_t i = 0; i < size1; i ++){
	list1[i] = new TEveElementList(Form("%s_%i",subname1,i),Form("%s_%i",subname1,i));
	TEveElement::AddElement((TEveElement*)list1[i]);
	for(Int_t j = 0; j < size2; j ++){
	    list2[i][j] = new TEveElementList(Form("%s_%i",subname2,j),Form("%s_%i",subname2,j));
	    list1[i]->AddElement((TEveElement*)list2[i][j]);
	}
    }
}
HEDGroup2D::~HEDGroup2D(){
    for(Int_t i = 0; i < size1; i ++){
	for(Int_t j = 0; j < size2; j ++){
	    list2[i][j]->DestroyElements();
	    delete list2[i][j];
	}
	delete list1[i];
    }
}
//----------------------------------------------------------------
