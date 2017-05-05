#ifndef __NEXTEVENT__
#define __NEXTEVENT__


//---------- hades etc -------------
#include "hades.h"
#include "hrecevent.h"
#include "heventheader.h"
#include "hcategory.h"
#include "hphysicsconstants.h"

#include "hgeomtransform.h"
#include "hmdcsizescells.h"

//---------- detector def ----------
#include "hgeantdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "richdef.h"
#include "tofdef.h"
#include "walldef.h"
#include "showerdef.h"
#include "emcdef.h"
#include "rpcdef.h"
#include "hparticledef.h"
//--------- data objects -----------
#include "hmetamatch2.h"
#include "hmdccal1sim.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdctrkcand.h"
#include "hsplinetrack.h"
#include "hrktrackB.h"
#include "hshowerhitsim.h"
#include "hemcclustersim.h"
#include "hwallhit.h"
#include "hwallhitsim.h"
#include "htofhit.h"
#include "htofhitsim.h"
#include "htofcluster.h"
#include "htofclustersim.h"
#include "hrichhit.h"
#include "hrichhitsim.h"
#include "hrichcalsim.h"
#include "hrichtrack.h"
#include "hrpccluster.h"
#include "hrpcclustersim.h"
#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hgeantrich.h"
#include "hgeantwall.h"

//--------- helpers -----------
#include "hgeomvector.h"
#include "hcategorymanager.h"
#include "hparticletracksorter.h"


#include "TROOT.h"
#include "TString.h"
#include "TSystem.h"
#include "TRandom.h"
#include "TStopwatch.h"
#include "TApplication.h"

#include "TEveManager.h"
#include "TEveScene.h"
#include "TEveElement.h"
#include "TEveLine.h"
#include "TEvePointSet.h"
#include "TEveQuadSet.h"
#include "TEveViewer.h"

#include "TEveTrackPropagator.h"
#include "TEveTrack.h"
#include "TEveVSDStructs.h"
//#include "TEvePathMark.h"   // 5.28

#include "TGLabel.h"
#include "TGLViewer.h"
#include "TGLSAViewer.h"
#include "TGFrame.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGButtonGroup.h"
#include "TGNumberEntry.h"
#include "TGWindow.h"
#include "TGLayout.h"
#include "TMarker.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <algorithm>

#include "hedhelpers.h"
#include "hedfield.h"
#include "hedhitobjects.h"
#include "hedgui.h"

#include "userFunc.C"     // selectLeptonsBeta(), rejectEvent() , isGoodEvent() is defined here

using namespace std;

//---------------------------------------------------------
//             NEXT EVENT
// This macro will load a new event into memory.
// It performs a call to Hades event loop. After
// running the event loop the full event is available
// in memory. The different detector hits can be selected
// by the user, tranformed and added to the event scene of
// Eve. All objects of the previous event scene will be
// destroyed.
//
//
// Class HEDEvtNavHandler  : event handler connect to GUI
//
// this Class keeps provides selectEvent() function connected
// to the "next Event" button. The function then calls
// nextEventLoop() or nextEvent() depending if the the loop
// box is checked.
// HEDEvtNavHandler holds the user defined TEveElementLists
// which are inserted in the Event Scene of TEveManager. The
// user has to clean and fill this lists inside nextEvent().
// The lists appear in "Eve" tab of the GUI in "Scenes/Event scene".
// The regions to be modified are marked \"## USER ACTION ##\"
//---------------------------------------------------------
class HEDEvtNavHandler
{
public:
    static HEDEvtNavHandler* gEDEvtNavHandler;

    static Bool_t isRealData;

    //################## USER ACTION ##########################
    // declare all list of elements
    // add as many as you need
    TEveElementList* vertexlist;
    HEDGroup*     sectors;
    HEDGroup*     particlecandNoLep;
    HEDGroup*     particlecandLep;
    HEDGroup*     particlecandHad;
    HEDGroup*     richpadplane;
    HEDGroup*     richpadplanecleaned;
    HEDGroup*     richhitpadplane;
    HEDGroup2D*   allMdcWires;
    HEDGroup2D*   allMdcWiresNotUsed;

    HEDMdcWireManager* wires;
    HEDGroup*            tofClusters;
    HEDGroup*            rpcClusters;
    HEDGroup*            showerHits;
    HEDGroup*            emcClusters;
    HEDGroup*            mdcSegments;
    TEveElementList*     wallHits;
    TEveElementList*     wallPlane;

    TEveElementList*     geantRichList;
    TEveElementList*     geantRichDirectList;

    TEveElementList*     geantList;
    TEveElementList*     primList;
    TEveElementList*     secoList;


    TEveTrackList*       simTrackList;
    TEveTrackList*       simTrackListPrim;
    TEveTrackList*       simTrackListSeco;

    map<Int_t, TEveTrackList*> mapPrim;
    map<Int_t, TEveTrackList*> mapSeco;

    static TEveTrackPropagator* g_prop ;

    Int_t eventCt;
    //#########################################################

private:
    HEDEvtNavHandler(){
	gEDEvtNavHandler = this;


	//################## USER ACTION ##########################

	// create all list of elements
	// which have been declared. They
	// have to added to event scene
        // cleared in ech event


	vertexlist              = new TEveElementList("vertex","vertex");
	sectors                 = new HEDGroup     ("sectors"            ,"sectors"            ,6  ,"Sector");
	particlecandNoLep       = new HEDGroup     ("particlecandNoLep"  ,"particlecandNoLep"  ,6  ,"particlecandNoLep");
	particlecandLep         = new HEDGroup     ("particlecandLep"    ,"particlecandLep"    ,6  ,"particlecandLep");
	particlecandHad         = new HEDGroup     ("particlecandHad"    ,"particlecandHad"    ,6  ,"particlecandHad");
        richpadplane            = new HEDGroup     ("richpadplane"       ,"richpadplane"       ,6  ,"richpadplane");
        richpadplanecleaned     = new HEDGroup     ("richpadplanecleaned","richpadplanecleaned",6  ,"richpadplanecleaned");
        richhitpadplane         = new HEDGroup     ("richhitpadplane"    ,"richhitpadplane"    ,6  ,"richhitpadplane");
	allMdcWires             = new HEDGroup2D   ("allMdcwires"        ,"allMdcwires"        ,6,4,"Sector","Module");
        allMdcWiresNotUsed      = new HEDGroup2D   ("allMdcwiresNotUsed" ,"allMdcwiresNotUsed" ,6,4,"Sector","Module");

	tofClusters  = new HEDGroup     ("tofclusters"  ,"tofclusters"  ,6,"tofclusters");
	rpcClusters  = new HEDGroup     ("rpcclusters"  ,"rpcclusters"  ,6,"rpcclusters");
	showerHits   = new HEDGroup     ("showerHits"   ,"showerHits"   ,6,"showerHits");
	emcClusters  = new HEDGroup     ("emcClusters"  ,"emcClusters"  ,6,"emcClusters");
	mdcSegments  = new HEDGroup     ("mdcSegments"  ,"mdcSegments"  ,6,"mdcSegments");
        wallHits     = new TEveElementList("wallHits"   ,"wallHits");
	wallPlane    = new TEveElementList("wallPane"   ,"wallPlane");
	
        wires = new HEDMdcWireManager();

	//---------------------------------------------------------
	// track list for simulated particles for HGeantKine
	// propagated by Eve

	HPhysicsConstants::setDefaultGraphic(kMagenta,1);
	HPhysicsConstants::setGraphic(14,kGray    ,1); // proton
	HPhysicsConstants::setGraphic(13,kGreen-2 ,1); // neutron
	HPhysicsConstants::setGraphic(8 ,kRed-9   ,1); // pi+
	HPhysicsConstants::setGraphic(9 ,kBlue-9  ,1); // pi-
	HPhysicsConstants::setGraphic(7 ,kYellow  ,1); // pi0

	HPhysicsConstants::setGraphic(5 ,kRed     ,1); // mu-
	HPhysicsConstants::setGraphic(6 ,kBlue    ,1); // mu+

	HPhysicsConstants::setGraphic(2 ,kRed     ,1); // e+
	HPhysicsConstants::setGraphic(3 ,kBlue    ,1); // e-
	HPhysicsConstants::setGraphic(1 ,kWhite   ,1); // gamma

	HPhysicsConstants::setGraphic(10,kGreen   ,1); // K0L
	HPhysicsConstants::setGraphic(11,kGreen-7 ,1); // K+
	HPhysicsConstants::setGraphic(12,kGreen-6 ,1); // K-
	HPhysicsConstants::setGraphic(16,kGreen-5 ,1); // KOS

	HPhysicsConstants::setGraphic(45,kOrange  ,1); // d
	HPhysicsConstants::setGraphic(46,kOrange-3,1); // t



	geantList    = new TEveElementList("Geant","Geant");
        primList     = new TEveElementList("Primaries","Primaries");
        secoList     = new TEveElementList("Secondaries","Secondaries");

	simTrackList = new TEveTrackList();
	TEveTrackPropagator* prop = simTrackList->GetPropagator();
        prop->SetFitDaughters(kTRUE);
	prop->SetRnrDaughters(kTRUE);
	prop->SetFitReferences(kTRUE);
	prop->SetRnrReferences(kTRUE);
	prop->SetStepper(TEveTrackPropagator::kRungeKutta);
	prop->SetMaxZ(700);  // max dist in z [cm]
	prop->SetMaxR(300);  // max dist in R [cm]
	prop->SetMaxOrbs(3); // max number of loops (low momenta particles can spiral almost for ever)


        // REpresentation of Reference points
	(prop->RefPMAtt()).SetMarkerStyle(20);
        (prop->RefPMAtt()).SetMarkerColor(kRed);
        (prop->RefPMAtt()).SetMarkerSize(1.0);

	simTrackList->SetName("RK Propagator");
	simTrackList->SetLineColor(kMagenta);
	prop->SetMagFieldObj(gEDField);
	g_prop = prop;

        simTrackListPrim = new TEveTrackList();
	simTrackListPrim->SetName("others");
	simTrackListPrim->SetLineColor(kMagenta);

	simTrackListSeco = new TEveTrackList();
	simTrackListSeco->SetName("others");
	simTrackListSeco->SetLineColor(kMagenta);


	Int_t pidPrim[] = {2,3, 7,8,9, 10,11,12,16, 13,14};
	for(UInt_t i = 0; i< sizeof(pidPrim)/sizeof(Int_t); i++){
             mapPrim[pidPrim[i]] = new TEveTrackList(HPhysicsConstants::pid(pidPrim[i]));
             mapPrim[pidPrim[i]]->SetLineColor(HPhysicsConstants::lineColor(pidPrim[i]));
	}
	Int_t pidSeco[] = {1, 2,3, 5,6, 7,8,9, 10,11,12,16, 13,14, 45,46};

	for(UInt_t i = 0; i < sizeof(pidSeco)/sizeof(Int_t); i++){
             mapSeco[pidSeco[i]] = new TEveTrackList(HPhysicsConstants::pid(pidSeco[i]));
             mapSeco[pidSeco[i]]->SetLineColor(HPhysicsConstants::lineColor(pidSeco[i]));
	}

	geantRichList       = new TEveElementList("geantRich"      ,"geantRich");
        geantRichDirectList = new TEveElementList("geantDirectRich","geantDirectRich");



	//---------------------------------------------------------


	TEveScene* eveEv = gEve->GetEventScene();

	// add all list of elements to event scene
	// they will appear in "Eve" tab of the GUI
	// in "Scenes/Event scene" with the defined
        // names.
        Bool_t sectorsTopLevel = kFALSE; // kTRUE = add gropups that they appear under sector top level

	gEve->AddElement(vertexlist       ,eveEv);

	if(sectorsTopLevel) {
	    gEve->AddElement(sectors          ,eveEv);

	    for(Int_t s = 0;s < 6; s ++){
		sectors->getList(s)->AddElement(particlecandLep         ->getList(s));
		sectors->getList(s)->AddElement(particlecandNoLep       ->getList(s));
		sectors->getList(s)->AddElement(particlecandHad         ->getList(s));
		sectors->getList(s)->AddElement(richpadplane            ->getList(s));
		sectors->getList(s)->AddElement(richpadplanecleaned     ->getList(s));
		sectors->getList(s)->AddElement(richhitpadplane         ->getList(s));
	    }
	} else {
	    gEve->AddElement(particlecandLep        ,eveEv);
	    gEve->AddElement(particlecandNoLep      ,eveEv);
	    gEve->AddElement(particlecandHad        ,eveEv);
	    gEve->AddElement(richpadplane           ,eveEv);
	    gEve->AddElement(richpadplanecleaned    ,eveEv);
	    gEve->AddElement(richhitpadplane        ,eveEv);
	}
	gEve->AddElement(allMdcWires      ,eveEv);
	gEve->AddElement(allMdcWiresNotUsed,eveEv);
	allMdcWires ->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default
        allMdcWiresNotUsed->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default

        gEve->AddElement(tofClusters      ,eveEv);
        gEve->AddElement(rpcClusters      ,eveEv);
	if(!HEDTransform::isEmc()) gEve->AddElement(showerHits       ,eveEv);
        else                       gEve->AddElement(emcClusters      ,eveEv);
        gEve->AddElement(mdcSegments      ,eveEv);
        gEve->AddElement(wallHits         ,eveEv);
        gEve->AddElement(wallPlane        ,eveEv);
	tofClusters->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default
        rpcClusters->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default
        showerHits->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default
        emcClusters->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default
        mdcSegments->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default
        //wallHits->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default

	//---------------------------------------------------------
        // GEANT part
	gEve->AddElement(geantList,eveEv);     // main list
	gEve->AddElement(primList,geantList);  // main primary   list
        gEve->AddElement(secoList,geantList);  // main secondary list

        // add all primary lists
	for(UInt_t i = 0; i < sizeof(pidPrim)/sizeof(Int_t); i++){
	    gEve->AddElement(mapPrim[pidPrim[i]] ,primList);
	}
        gEve->AddElement(simTrackListPrim,primList);

        // add all secondaries lists
	for(UInt_t i = 0; i < sizeof(pidSeco)/sizeof(Int_t); i++){
	    gEve->AddElement(mapSeco[pidSeco[i]] ,secoList);
	}
        gEve->AddElement(simTrackListSeco,secoList);

	mapPrim[13]->SetRnrSelfChildren(kFALSE,kFALSE); // do not show by default

	gEve->AddElement(geantRichList      ,eveEv);
	gEve->AddElement(geantRichDirectList,eveEv);
	//---------------------------------------------------------

	//#########################################################
        eventCt=0;
    }
public:
    static HEDEvtNavHandler* getHandler(){
        // Singleton
	if(gEDEvtNavHandler == 0){
             gEDEvtNavHandler = new HEDEvtNavHandler();
	}
        return gEDEvtNavHandler;
    }
    void selectEvent(){
	// this function is connected to "next Event" button
	// calls nextEventLoop() or nextEvent() depending
        // if the loop checkbox has been set
	HEDSetup* setup = HEDSetup::getEDSetup();
	if(setup->fCheckTimed->IsOn() ){ // loop events
	    nextEventLoop();
	} else {
           nextEvent();                  // one event
	}
    }

    Int_t nextEvent()
    {
	// this function loads one event into memory and
	// fills the graphics objects which should be
	// displayed in the GUI. This is the "User playground"

	//-----------------------------------------------------
	if(isRealData)
	{
	    if(eventCt == 0) { // in real data the first event is empty
		gHades->eventLoop(2);
		eventCt++;
		return 0;
	    }

	    while( isGoodEvent() == kFALSE ) { // get the next high mult trigger  (userFunc.C)
		cout<<"#########  skipped event "<<gHades->getCurrentEvent()->getHeader()->getEventSeqNumber()<<endl;
		gHades->eventLoop(1);
	    }
	}
	//-----------------------------------------------------

	TStopwatch timer;
	timer.Reset();
	timer.Start();

        Bool_t drawParticleCand = kTRUE;
        Bool_t drawRichPadPlane = kTRUE;
        Bool_t drawRichCompound = kTRUE;
        Bool_t drawMdcWires     = kTRUE;
        Bool_t drawKine         = kTRUE;

	Bool_t drawTofClusters = kTRUE;
        Bool_t drawRpcClusters = kTRUE;
        Bool_t drawShowerHits  = kTRUE;
        Bool_t drawEmcClusters = kTRUE;
        Bool_t drawMdcSegments = kTRUE;
        Bool_t drawWallHits    = kTRUE;
        Bool_t drawWallPlane   = kTRUE;

	Bool_t savePics        = kFALSE;           // to work over ssh : put the line OpenGL.SavePicturesViaFBO: off  to your .rootrc
	TString picturepath    = "./auto_pics";
        TString picturemask    = "event_%i.png";  // convert -delay 500 -loop 0 *.png animation.gif   (~ 5 sec per pic, endless)

	// ROOT 5.26 : clear annotions (if any at all)
	//gEve->GetViewers()->DeleteAnnotations();

	TEveScene*	eveEv = gEve->GetEventScene();
	if(eveEv){
            // clean up last event


	    //################## USER ACTION ##########################
	    // cleanup all list of elements
            // don't destroy sectors !
	    vertexlist         ->DestroyElements();
	    particlecandNoLep  ->DestroyElements();
	    particlecandLep    ->DestroyElements();
	    particlecandHad    ->DestroyElements();
	    richpadplane       ->DestroyElements();
	    richpadplanecleaned->DestroyElements();
            richhitpadplane    ->DestroyElements();
	    allMdcWires        ->DestroyElements();
            allMdcWiresNotUsed ->DestroyElements();
            tofClusters        ->DestroyElements();
            rpcClusters        ->DestroyElements();
            showerHits         ->DestroyElements();
            emcClusters        ->DestroyElements();
            mdcSegments        ->DestroyElements();
            wallHits           ->DestroyElements();
            wallPlane          ->DestroyElements();


	    //---------------------------------------------------------
            // GEANT part

	    map<Int_t,TEveTrackList*>::iterator iter;
	    for( iter = mapPrim.begin(); iter != mapPrim.end(); ++iter ) {
		iter->second->DestroyElements();
	    }
            simTrackListPrim->DestroyElements();

	    for( iter = mapSeco.begin(); iter != mapSeco.end(); ++iter ) {
		iter->second->DestroyElements();
	    }
            simTrackListSeco->DestroyElements();


	    geantRichList      ->DestroyElements();
	    geantRichDirectList->DestroyElements();
	    //---------------------------------------------------------

	    //#########################################################

	}

	Int_t seqNumber;
	Int_t runNumber;

	if(gHades)
	{
	    //----------break if last event is reached-------------
	    if(!gHades->eventLoop(1)) return -1;
	    //----------------looping data-------------------------
            Bool_t isSimulation = kFALSE;
	    Bool_t badEvent    = kTRUE;
	    Int_t nCandLep     = -1;
	    Int_t nCandLepBest = -1;
	    Int_t nCandHad     = -1;
	    Int_t nCandHadBest = -1;

	    //--------------------------CONFIGURATION PARTICLE SORTER----------------------
	    //At begin of the program (outside the event loop)
	    HParticleTrackSorter sorter;
	    sorter.init();                                        // get catgegory pointers etc...
	    //--------------------------END CONFIGURATION PARTICLE SORTER-------------------

	    while (badEvent==kTRUE){

		sorter.cleanUp();

		sorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);      // reset all flags for flags (0-28) ,reject,used,lepton
		nCandLep     = sorter.fill(selectLeptonsBeta);   // fill only good leptons   selectLeptonsSharp2Mom(
	        nCandLepBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsLepton);
		nCandHad     = sorter.fill(selectHadrons);       // fill only good hadrons (already marked good leptons will be skipped)
		nCandHadBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsHadron);

		badEvent = rejectEvent(); // userFunc.C
                if (badEvent) if(!gHades->eventLoop(1)) return -1;
	    }



	    //################## USER ACTION ##########################
	    HCategory* mdcSegCat          = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcSeg);
	    HCategory* mdcCal1Cat         = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcCal1);
	    HCategory* wallHitCat         = (HCategory*) gHades->getCurrentEvent()->getCategory(catWallHit);
	    HCategory* tofHitCat          = (HCategory*) gHades->getCurrentEvent()->getCategory(catTofHit);
	    HCategory* tofClustCat        = (HCategory*) gHades->getCurrentEvent()->getCategory(catTofCluster);
	    HCategory* rpcClustCat        = (HCategory*) gHades->getCurrentEvent()->getCategory(catRpcCluster);
	    HCategory* emcClusterCat      = (HCategory*) gHades->getCurrentEvent()->getCategory(catEmcCluster);
	    HCategory* showerHitCat       = (HCategory*) gHades->getCurrentEvent()->getCategory(catShowerHit);
	    HCategory* richHitCat         = (HCategory*) gHades->getCurrentEvent()->getCategory(catRichHit);
            HCategory* richTrackCat       = (HCategory*) gHades->getCurrentEvent()->getCategory(catRichTrack);
            HCategory* particleCandCat    = (HCategory*) gHades->getCurrentEvent()->getCategory(catParticleCand);
            HCategory* kineCat            = (HCategory*) gHades->getCurrentEvent()->getCategory(catGeantKine);
            HCategory* geantWallCat       = (HCategory*) gHades->getCurrentEvent()->getCategory(catWallGeantRaw);
            HCategory* geantMdcCat        = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcGeantRaw);
            HCategory* geantRichMirrorCat = (HCategory*) gHades->getCurrentEvent()->getCategory(catRichGeantRaw+2);
            HCategory* geantRichDirectCat = (HCategory*) gHades->getCurrentEvent()->getCategory(catRichGeantRaw+1);

            if(kineCat) isSimulation = kTRUE;

	    seqNumber = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
            runNumber = gHades->getCurrentEvent()->getHeader()->getEventRunNumber();

	    //---------------------------------------------------------
	    HGeomVector pVertex;
	    HEDTransform::calcVertexPoint(pVertex); // for print later
	    vertexlist->AddElement(new HEDVertex());
	    //---------------------------------------------------------

	    //---------------------------------------------------------
            // RICH
	    if(!HEDTransform::isNewRich()&&drawRichPadPlane){
		for(Int_t s = 0 ; s < 6; s ++){
		    // clean : 0 (all pads), 1 : cleaned (removed pads), 2 : kept pads after cleaning
		    richpadplane       ->AddElement(s,new HEDRichPadPlane(s,2)); // rich pads fired on cal level
	            richpadplanecleaned->AddElement(s,new HEDRichPadPlane(s,1)); // rich pads fired on cal level but cleanded
	        }
	    } else {
		richpadplane       ->AddElement(0,new HEDRich700PadPlane()); // rich pads fired on cal level
	    }
	    if(drawRichCompound && richHitCat){
                HRichHitSim* richhit;
		HEDRichCompound* edrichcompound;
		Int_t size = richHitCat->getEntries();
		if(size>0){
		    cout<<"-----------------------------------------------------------------"<<endl;
        	    cout<<"reconstruted RICH Hits :"<<endl;
		}
		for(Int_t i = 0; i < size; i ++){
		    richhit = HCategoryManager::getObject(richhit,richHitCat,i);
		    if(richhit){
                        Int_t s = richhit->getSector();
			edrichcompound = new HEDRichCompound(richhit);
			edrichcompound ->SetLineColor(kRed);          // ring
			edrichcompound ->SetColorMirrorHit(kMagenta); // line to mirror
			edrichcompound ->SetColorPadPlaneHit(kYellow);// hit marker
                        if(richhit->getRadius() < 0) edrichcompound ->SetLineStyle(7);
			richhitpadplane->AddElement(s,edrichcompound);
			Int_t   tracks[3] = {-1,-1,-1};
                        Int_t   weight[3] = {-1,-1,-1};

			if(isSimulation) {
			    tracks[0] = richhit->track1;
                            tracks[1] = richhit->track2;
                            tracks[2] = richhit->track3;
                            weight[0] = richhit->weigTrack1;
			    weight[1] = richhit->weigTrack2;
			    weight[2] = richhit->weigTrack3;

			}

			cout<<"index "<<i<<" sector "<<s<<" ("<<setw(3)<<right<<tracks[0]<<","<<weight[0] <<") ("<<setw(3)<<right<<tracks[1]<<","<<weight[1]<<") ("<<setw(3)<<right<<tracks[2]<<","<<weight[2]<<")"<<endl;
		    }
		}
		if(size>0){
		    cout<<"-----------------------------------------------------------------"<<endl;
		}
	    }
            //---------------------------------------------------------

	    //---------------------------------------------------------
	    if(drawMdcWires && mdcCal1Cat){
 		HMdcCal1Sim* cal;
                HEDMdcWire* edmdcwire;
                Int_t s,m,l,c;
		Int_t size = mdcCal1Cat->getEntries();

		wires->fill(); // make wire usage statistics for this event

		for(Int_t i = 0; i < size; i ++){
		    cal = HCategoryManager::getObject(cal,mdcCal1Cat,i);
		    if(cal){
                        cal->getAddress(s,m,l,c);
			Int_t ntimes = wires->isUsedNtimes(s,m,l,c);


			if(ntimes >= 1) {
			    edmdcwire = new HEDMdcWire(cal);
			    //if(ntimes == 0) edmdcwire ->SetLineColor(kOrange);   // changed unused by segment fitter
			    //if(ntimes == 0) edmdcwire ->SetLineColor(kRed-2);   // unused by segment fitter
			    if(ntimes == 1) edmdcwire ->SetLineColor(kBlue);    // unique wire
			    if(ntimes > 1 ) edmdcwire ->SetLineColor(kMagenta); // multiple used
			    allMdcWires->AddElement(s,m,edmdcwire);
			}
			if(ntimes == 0) {
			    edmdcwire = new HEDMdcWire(cal);
			    edmdcwire ->SetLineColor(kOrange);   // changed unused by segment fitter
			    allMdcWiresNotUsed->AddElement(s,m,edmdcwire);
			}
		    }
		}
	    }
 	    //---------------------------------------------------------

	    //---------------------------------------------------------
	    if(drawTofClusters && tofClustCat){
		HTofClusterSim* clust;
		HEDTofCluster* edtofcluster;
		Int_t size = tofClustCat->getEntries();

		for(Int_t i = 0; i < size; i ++){
		    clust = HCategoryManager::getObject(clust,tofClustCat,i);
		    if(clust){
			edtofcluster = new HEDTofCluster(clust);
			//edtofcluster ->SetMarkerColor(kRed-2);
			tofClusters->AddElement(clust->getSector(),edtofcluster);
		    }
		}
	    }
 	    //---------------------------------------------------------

	    //---------------------------------------------------------
	    if(drawRpcClusters && rpcClustCat){
		HRpcClusterSim* clust;
		HEDRpcCluster* edrpccluster;
		Int_t size = rpcClustCat->getEntries();

		for(Int_t i = 0; i < size; i ++){
		    clust = HCategoryManager::getObject(clust,rpcClustCat,i);
		    if(clust){
			edrpccluster = new HEDRpcCluster(clust);
			//edrpccluster ->SetMarkerColor(kRed-2);
			rpcClusters->AddElement(clust->getSector(),edrpccluster);
		    }
		}
	    }
 	    //---------------------------------------------------------

	    //---------------------------------------------------------
	    if(drawShowerHits && showerHitCat){
		HShowerHitSim* hit;
		HEDShowerHit* edshowerhit;
		Int_t size = showerHitCat->getEntries();

		for(Int_t i = 0; i < size; i ++){
		    hit = HCategoryManager::getObject(hit,showerHitCat,i);
		    if(hit){
			edshowerhit = new HEDShowerHit(hit);
			//edshowerhit ->SetMarkerColor(kRed-2);
			showerHits->AddElement(hit->getSector(),edshowerhit);
		    }
		}
	    }
 	    //---------------------------------------------------------

	    //---------------------------------------------------------
	    if(drawEmcClusters && emcClusterCat){
		HEmcClusterSim* hit;
		HEDEmcCluster* edemccluster;
		Int_t size = emcClusterCat->getEntries();

		for(Int_t i = 0; i < size; i ++){
		    hit = HCategoryManager::getObject(hit,emcClusterCat,i);
		    if(hit){
			edemccluster = new HEDEmcCluster(hit);
			//edemccluster ->SetMarkerColor(kRed-2);
			emcClusters->AddElement(hit->getSector(),edemccluster);
		    }
		}
	    }
 	    //---------------------------------------------------------

	    //---------------------------------------------------------
	    if(drawWallHits && wallHitCat){
		HWallHitSim* hit;
		HEDWallHit* edwallhit;
		Int_t size = wallHitCat->getEntries();

		for(Int_t i = 0; i < size; i ++){
		    hit = HCategoryManager::getObject(hit,wallHitCat,i);
		    if(hit){
			edwallhit = new HEDWallHit(hit);
			//edwallhit ->SetMarkerColor(kRed-2);
			wallHits->AddElement(edwallhit);
		    }
		}
	    }
 	    //---------------------------------------------------------

 	    //---------------------------------------------------------
	    if(drawWallPlane){
		wallPlane->AddElement(new HEDWallPlane()); // wall cells fired on hit level
	    }
  	    //---------------------------------------------------------

	    //---------------------------------------------------------
	    if(drawMdcSegments && mdcSegCat){
		HMdcSegSim* seg;
		HEDSegment* edsegment;
		Int_t size = mdcSegCat->getEntries();

		for(Int_t i = 0; i < size; i ++){
		    seg = HCategoryManager::getObject(seg,mdcSegCat,i);
		    if(seg){
			edsegment = new HEDSegment(seg);
			//edshowerhit ->SetMarkerColor(kRed-2);
			mdcSegments->AddElement(seg->getSec(),edsegment);
		    }
		}
	    }
 	    //---------------------------------------------------------


	    //---------------------------------------------------------
	    if(drawParticleCand && particleCandCat){

		HParticleCandSim* cand;
		Int_t size = particleCandCat->getEntries();
		for(Int_t i = 0; i < size; i ++){
		    cand = HCategoryManager::getObject(cand,particleCandCat,i);
		    if(cand){
			HEDParticleCand* edcand = 0;
			Int_t s = cand->getSector();

			// Fill all objects. Group them into different lists
			// Groups get different colors.
                        /*
                        if( cand->isFlagAND(4,
					    Particle::kIsAcceptedHitInnerMDC,
					    Particle::kIsAcceptedHitOuterMDC,
					    Particle::kIsAcceptedHitMETA,
					    Particle::kIsAcceptedRK
					   ) && cand->getBeta()<0.9) continue;      // reject slow fully reconstructed particles
                        */
			//---------------------------------------------------------
			// Candidates with matched Ring (wide window),
                        // but not accepted as lepton by HParticleTrackSorter
                        // Includes incomplete tracks (eg. no outer MDC or META)
			if(cand->getRichInd() >= 0 && !cand->isFlagBit(kIsLepton)){
                            edcand = new HEDParticleCand(cand);
			    edcand->SetLineColor(kMagenta);
			    particlecandNoLep->AddElement(s,edcand);
			}
			//---------------------------------------------------------

			//---------------------------------------------------------
                        // Candidates accepted as leptons by HParticleTrackSorter
                        // Full tracks with RICH and accepted after sorting
			if(cand->isFlagBit(kIsLepton)){
			    edcand = new HEDParticleCand(cand);
			    edcand->SetLineColor(kRed);
			    particlecandLep->AddElement(s,edcand);
			}
			//---------------------------------------------------------
			// Candidates accepted by HParticleTrackSorter
                        // which do not belong to the groups above
                        // Full tracks, no incomplete tracks here
			if(!cand->isFlagBit(kIsLepton) &&
                           !(cand->getRichInd() >= 0 && !cand->isFlagBit(kIsLepton)) &&
			   cand->isFlagBit(kIsUsed)){
			    edcand = new HEDParticleCand(cand);
			    // selcted no rings
			    edcand->SetLineColor(kGreen);
			    particlecandHad->AddElement(s,edcand);
			}
			//---------------------------------------------------------

			//---------------------------------------------------------
			// Ghost tracks get dshed lines
                        if(isSimulation && edcand && cand->isGhostTrack()) edcand->SetLineStyle(7);
			//---------------------------------------------------------
		    }
		} // end loop particlecand

	    } // end particlecand
	    //---------------------------------------------------------



	    //---------------------------------------------------------
            Bool_t doSelectSecondaries=kFALSE;
            Float_t vertexDistCat = 500;
            Float_t pzCut         = 0;
	    if(drawKine && kineCat){
		HGeantKine* kine;
		Int_t size = kineCat->getEntries();
		for(Int_t i = 0; i < size; i ++){
		    kine = HCategoryManager::getObject(kine,kineCat,i);
		    if(kine){
			Int_t  id =  kine->getID();

			if(doSelectSecondaries&&kine->getParentTrack()>0){
			    Float_t vx,vy,vz;
			    Float_t px,py,pz;
			    kine->getVertex(vx,vy,vz);

			    if(sqrt(vx*vx+vy*vy+vz*vz)>vertexDistCat) continue;

			    kine->getMomentum(px,py,pz);

			    if(pz<pzCut) continue;
			}

			TEveTrack* track = HEDTransform::createKineParticle(kine,simTrackList->GetPropagator());

			track->SetLineColor(simTrackList->GetLineColor()); // default color
                        track->SetLineColor(simTrackList->GetLineColor()); // default color

			track->SetLineColor(HPhysicsConstants::lineColor(id));   // color settings are done at the beginning of the macro
                        track->SetLineWidth(2);
                        if(kine->getParentTrack() > 0) track->SetLineStyle(7); // secondaries

			if(kine->getParentTrack() > 0)
			{ // secondaries
                           // if(kine->getPz() > 0 ) continue; // only backward flying particles
                            map<Int_t,TEveTrackList*>::iterator iter;
			    iter = mapSeco.find(id);

                            if(iter != mapSeco.end()) iter->second->AddElement(track);
                            else                      simTrackListSeco->AddElement(track);
			}
			else { // primaries

                            map<Int_t,TEveTrackList*>::iterator iter;
			    iter = mapPrim.find(id);

                            if(iter != mapPrim.end()) iter->second->AddElement(track);
                            else                      simTrackListPrim->AddElement(track);
			}
			track->MakeTrack();

			if(kine->getFirstRichHit()!=-1){
                            Int_t select = 0; // 0=photon, 1=direct
			    HEDRichGeantPadPlane* richGea = new HEDRichGeantPadPlane(kine,select);
			    richGea->SetMarkerColor(kYellow);
                            richGea->SetMarkerSize(0.2);
                            geantRichList->AddElement(richGea);

			}

		    }
		}
		if(geantRichDirectCat){
		    Int_t select = 1; // 0=photon, 1=direct
		    HGeantRichDirect* direct=0;
		    Int_t size = geantRichDirectCat->getEntries();
		    for(Int_t i = 0; i < size; i ++){
			direct = HCategoryManager::getObject(direct,geantRichDirectCat,i);
			if(direct){
			    HEDRichGeantPadPlane* richGea = new HEDRichGeantPadPlane(0,select,direct);
			    richGea->SetMarkerColor(kCyan+1);
			    richGea->SetMarkerStyle(kPlus);
                            richGea->SetMarkerSize(0.2);
                            geantRichDirectList->AddElement(richGea);
			}
		    }
		}
		if(geantRichMirrorCat){
		    HGeantRichMirror* mirror=0;
		    Int_t size = geantRichMirrorCat->getEntries();
		    for(Int_t i = 0; i < size; i ++){
			mirror = HCategoryManager::getObject(mirror,geantRichMirrorCat,i);
			if(mirror){
			    HEDRichGeantMirror* richMir = new HEDRichGeantMirror(mirror);
			    richMir->SetMarkerColor(kMagenta);
			    geantRichList->AddElement(richMir);
			}
		    }
		}
	    }
	    //---------------------------------------------------------



	    cout<<"-----------------------------------------------------------------"<<endl;
	    cout<<"EventRun Number : "<<runNumber<<endl;
	    cout<<"EventSeq Number : "<<seqNumber<<endl;
	    cout<<"Vertex [cm] = "<<pVertex.X()<<", "<<pVertex.Y()<<", "<<pVertex.Z()<<endl;
	    if(richHitCat)  { cout<<setw(5)<<richHitCat->getEntries()  <<" RICH HITS "   <<endl; }
	    if(mdcSegCat)   { cout<<setw(5)<<mdcSegCat->getEntries()   <<" MDC SEGMENTS "<<endl; }
	    if(tofHitCat)   { cout<<setw(5)<<tofHitCat->getEntries()   <<" TOF HITS "    <<endl; }
	    if(tofClustCat) { cout<<setw(5)<<tofClustCat->getEntries() <<" TOF CLUSTER " <<endl; }
	    if(rpcClustCat) { cout<<setw(5)<<rpcClustCat->getEntries() <<" RPC CLUSTER " <<endl; }
	    if(showerHitCat){ cout<<setw(5)<<showerHitCat->getEntries()<<" SHOWER HITS " <<endl; }
	    if(emcClusterCat){ cout<<setw(5)<<emcClusterCat->getEntries()<<" EMC CLUSTERS " <<endl; }
	    if(wallHitCat)  { cout<<setw(5)<<wallHitCat->getEntries()  <<" WALL HITS " <<endl; }
	    cout<<"nCandLep     = "<<nCandLep
		<<"\nnCandLepBest = "<<nCandLepBest
		<<"\nnCandHad     = "<<nCandHad
		<<"\nnCandHadBest = "<<nCandHadBest
		<<endl;

	    //#########################################################

	} // end hades
	//---------------------------------------------------------


	gEve->Redraw3D(kFALSE,kFALSE);
	if(savePics){

	    if(gSystem->AccessPathName(picturepath.Data())) { // create dir if not existing
                 gSystem->MakeDirectory(picturepath.Data());
	    }


	    TGLViewer* viewer = gEve->GetDefaultGLViewer();
	    TEveViewer* eviewer = gEve->GetDefaultViewer();
	    if(viewer){
		TString format = picturepath + "/" + picturemask;
		gSystem->ProcessEvents();

		eviewer->GetGLViewer()->SavePicture(Form(format.Data(),seqNumber));
                eviewer->GetGLViewer()->SavePicture("viewer.gif+500");
	        cout<<"Saved Picture : for event "<<seqNumber  <<", "<< Form(format.Data(),seqNumber) <<endl;
	    }
	}
	timer.Stop();
	cout<<"REAL time="<<timer.RealTime()<<",CPU time="<<timer.CpuTime()<<endl;
	return 0;
    }

    void nextEventLoop()
    {
	// loop until loop checkbox is unchecked
        // or nextEvent() return non ZERO
	HEDSetup* setup = HEDSetup::getEDSetup();
        Int_t returnval = 0;
	while(setup->fCheckTimed->IsOn() && returnval == 0){
	    Int_t sleep = setup->fNumberLoopTime->GetTGNumberEntry()->GetIntNumber();
	    for(Int_t i = 0; i <= 2*sleep; i ++){ // do not freeze GUI (Too lazy to do Threads ...)
		gSystem->Sleep(500); // 2n time 0.5s
		gSystem->ProcessEvents();
	    }
	    returnval = nextEvent();
            gSystem->ProcessEvents();
	}
    }

};


HEDEvtNavHandler*     HEDEvtNavHandler::gEDEvtNavHandler = 0;
TEveTrackPropagator*  HEDEvtNavHandler::g_prop           = 0;
Bool_t                HEDEvtNavHandler::isRealData       = kFALSE;
#endif // __NEXTEVENT__

