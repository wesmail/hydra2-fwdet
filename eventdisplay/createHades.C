#ifndef __CREATEHADES__
#define __CREATEHADES__


#include "hades.h"
#include "hruntimedb.h"
#include "hdatasource.h"
#include "hrootsource.h"
#include "hparora2io.h"
#include "hparasciifileio.h"
#include "hparrootfileio.h"
#include "htask.h"
#include "htaskset.h"
#include "hdst.h"
#include "hreconstructor.h"

#include "hspectrometer.h"
#include "hstart2detector.h"
#include "htboxdetector.h"
#include "hrichdetector.h"
#include "hmdcdetector.h"
#include "htofdetector.h"
#include "hshowerdetector.h"
#include "hrpcdetector.h"
#include "hwalldetector.h"

#include "hstarttaskset.h"
#include "hrichtaskset.h"
#include "hmdctaskset.h"
#include "htoftaskset.h"
#include "hshowertaskset.h"
#include "hrpctaskset.h"
#include "hsplinetaskset.h"
#include "hwalltaskset.h"

#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"

#include "hmdcsetup.h"
#include "hmdclayercorrpar.h"
#include "hmdctrackdset.h"
#include "hmdc34clfinder.h"
#include "hmdcidealtracking.h"

#include "hmdcsizescells.h"
#include "hmdcgetcontainers.h"

#include "hedhelpers.h"

#include "TOrdCollection.h"




Bool_t createHades()
{

    TString inputFile  = "/hera/hades/dstsim/apr12/hgeant/bmax10/white/e-/Au_Au_1230MeV_1000evts_1_1.root";
    // following : apr12 sim (urqmd + 1 close pair (e+e-, 250MeV/c) per event )
    //TString inputFile  = "/hera/hades/user/kempter/eventdisplay/apr12_closepair_0_3deg.root";
    //TString inputFile  = "/hera/hades/user/kempter/eventdisplay/apr12_closepair_5deg.root";
    //TString inputFile  = "/hera/hades/user/kempter/eventdisplay/apr12_closepair_7deg.root";
    //TString inputFile  = "/hera/hades/user/kempter/eventdisplay/apr12_closepair_9deg.root";

    Int_t refId        = 12001;  // apr12sim_mediumfieldalign_auau;
    TString beamtime = "apr12";
    TString asciiParFile ="";
    Bool_t doExtendedFit     = kTRUE; // switch on/off fit for initial params of segment fitter (10 x slower!)
    Bool_t doStartCorrection = kTRUE;  // kTRUE (default)=  use run by run start corrections

    TString rootParFile  = "/cvmfs/hades.gsi.de/param/sim/apr12/allParam_APR12_sim_run_12001_shower2_08112012.root";
    TString paramSource  = "oracle"; // oracle, ascii, root

    cout<<"-----------------------------------------------------------------------------"<<endl;
    cout<<"EVENT DISPLAY : INPUT FILE: "<<inputFile.Data()<<endl;
    cout<<"-----------------------------------------------------------------------------"<<endl;
    Hades* myHades = new Hades();
    gHades->setTreeBufferSize(8000);
    gHades->setQuietMode(2);
    gHades->makeCounter(1);
    HRuntimeDb* rtdb = gHades->getRuntimeDb();


    Int_t mdcMods[6][4]=
    { {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1} };

    // recommendations from Vladimir+Olga
    // according to params from 28.04.2011
    Int_t nLayers[6][4] = {
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6} };
    Int_t nLevel[4] = {10,50000,10,5000};

    HDst::setupSpectrometer(beamtime,mdcMods,"rich,mdc,tof,rpc,shower,wall");
    // beamtime mdcMods_apr12, mdcMods_full
    // Int_t mdcset[6][4] setup mdc. If not used put NULL (default).
    // if not NULL it will overwrite settings given by beamtime
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,start

    HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,"now");
    //HDst::setupParameterSources("oracle",asciiParFile,rootParFile,"now"); // use to create param file
    // parsource = oracle,ascii,root (order matters)
    // if source is "ascii" a ascii param file has to provided
    // if source is "root" a root param file has to provided
    // The histDate paramter (default "now") is used wit the oracle source

    HDst::setDataSource(3,"",inputFile, refId  ,"");
    // datasource 0 = hld, 1 = hldgrep 2 = hldremote, 3 root
    // like "lxhadeb02.gsi.de"  needed by dataosoure = 2
    // inputDir needed by dataosoure = 1,2
    // inputFile needed by dataosoure = 1,3



    //--------------------------------------------------------------------
    // ALIGNMENT OF WIRE PLANES
    //HMdcLayerCorrPar* fMdcLayerCorrPar =  (HMdcLayerCorrPar*)rtdb->getContainer("MdcLayerCorrPar");
    //  fMdcLayerCorrPar->setDefaultPar();
    //fMdcLayerCorrPar->setStatic();
    //--------------------------------------------------------------------

    // ----------- Build TASK SETS (using H***TaskSet::make) -------------

    HRichTaskSet         *richTaskSet         = new HRichTaskSet();
    HRpcTaskSet          *rpcTaskSet          = new HRpcTaskSet();
    HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
    HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
    HWallTaskSet         *wallTaskSet         = new HWallTaskSet();
    HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet();
    mdcTaskSet->setVersionDeDx(1); // 0 = no dEdx, 1 = HMdcDeDx2


    HTask *richTasks          = richTaskSet        ->make("simulation","noiseon");
    //HTask *richTasks          = richTaskSet        ->make("simulation","noiseoff");
    HTask *tofTasks           = tofTaskSet         ->make("simulation");
    HTask *rpcTasks           = rpcTaskSet         ->make("simulation");
    HTask *wallTasks          = wallTaskSet        ->make("simulation");
    HTask *showerTasks        = showerTaskSet      ->make("","simulation,magneton,hit");


    //  ----------- MDC ------------------------------------------------------
    HMdcSetup* mysetup = (HMdcSetup*)rtdb->getContainer("MdcSetup");
    mysetup->getMdcCommonSet()->setAnalysisLevel(4);                // fit
    mysetup->getMdcCalibrater1Set()->setMdcCalibrater1Set(1, 1);    // 1 = NoStartandCal, 2 = StartandCal, 3 = NoStartandNoCal ::  0 = noTimeCut, 1 = TimeCut
    mysetup->getMdcTrackFinderSet()->setIsCoilOff(kFALSE);          // field is on
    mysetup->getMdcTrackFinderSet()->setNLayers(nLayers[0]);
    mysetup->getMdcTrackFinderSet()->setNLevel(nLevel);
    mysetup->getMdc12FitSet()->setMdc12FitSet(2,1,0,kFALSE,kFALSE); // tuned fitter, seg


    rtdb->initContainers(refId);
    mysetup->setStatic();
    //setup->getMdcCommonSet()->setAnalysisLevel(1);



    HTask *mdcTasks           = mdcTaskSet         ->make("rtdb","");
    HMdcIdealTracking *mdcidealtrackTasks       =  new HMdcIdealTracking("idealtrack","idealtrack");
    // mdcidealtrackTasks->fillParallelCategories();

    //----------------SPLINE and RUNGE TACKING----------------------------------------
    HSplineTaskSet         *splineTaskSet       = new HSplineTaskSet("","");
    HTask *splineTasks=splineTaskSet->make("","spline,runge");

    //-----------------------------------------------------------------------
    //HParticleCandFiller  *pParticleCandFiller = new HParticleCandFiller("particlecandfiller","particlecandfiller","debug,goodseg0,goodseg1,goodmeta,goodrk");
    HParticleCandFiller   *pParticleCandFiller = new HParticleCandFiller  ("particlecandfiller","particlecandfiller","");
    HParticleTrackCleaner *pParticleCleaner    = new HParticleTrackCleaner("particlecleaner"   ,"particlecleaner");
    HParticleEvtInfoFiller *pParticleEvtInfo   = new HParticleEvtInfoFiller("particleevtinfo"  ,"particleevtinfo",beamtime);

    //------------------------ Master task set --------------------------

    HTaskSet *masterTaskSet = gHades->getTaskSet("simulation");

    masterTaskSet->add(tofTasks);

    masterTaskSet->add(rpcTasks);

    masterTaskSet->add(richTasks);
    masterTaskSet->add(showerTasks);
    masterTaskSet->add(mdcTasks);
    masterTaskSet->add(wallTasks);
    //masterTaskSet->add(mdcidealtrackTasks);

    masterTaskSet->add(splineTasks);
    masterTaskSet->add(pParticleCandFiller);
    masterTaskSet->add(pParticleCleaner);
    masterTaskSet->add(pParticleEvtInfo);


    //--------------------------------------------------------------------
    // Get Parameter containers for transformations
    // This taks is needed to get the parameter
    // containers for coordinate transformations
    // of the detector hits for the event display
    masterTaskSet->add(new HEDMakeContainers());
    //--------------------------------------------------------------------

    masterTaskSet->isTimed(kTRUE);

    Bool_t hasTracking = kFALSE;

    HTaskSet* set = (HTaskSet*) gHades->getTaskSet("simulation")->getSetOfTask()->FindObject("Mdc");
    if(set && set->getSetOfTask()->FindObject("mdc.hitf")) hasTracking = kTRUE;

    if(!hasTracking){
        set = (HTaskSet*) gHades->getTaskSet("real")->getSetOfTask()->FindObject("Mdc");
        if(set && set->getSetOfTask()->FindObject("mdc.hitf")) hasTracking = kTRUE;
    }

    if(hasTracking){
	cout<<"EVENT DISPLAY : TRACKING IS RUNNING #########################################"<<endl;
	cout<<"EVENT DISPLAY : SETTING TRACKING PARAMETERS FOT AU+AU @ 1.23 AGEV (APR12) ############"<<endl;
	HMdcTrackDSet::setTrackParam(beamtime);
	//--------------------------------------------------------------------
	// find best initial params for segment fit (takes long!)
	if(doExtendedFit) {
	    HMdcTrackDSet::setCalcInitialValue(1);  // -  1 : for all clusters 2 : for not fitted clusters
	}
	//--------------------------------------------------------------------
    }

    //--------------------------------------------------------------------
    if (!gHades->init())
    {
	cerr<<"EVENT DISPLAY : ERROR IN INIT, EXITING! #####################################"<<endl;
	delete myHades;
	return kFALSE;
    }
    //--------------------------------------------------------------------


    return kTRUE;
}

#endif

