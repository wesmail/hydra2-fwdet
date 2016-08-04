#ifndef __CREATEHADES__
#define __CREATEHADES__


#include "hades.h"
#include "hruntimedb.h"
#include "hdatasource.h"
#include "hrootsource.h"
#include "hldfilesource.h"
#include "hparora2io.h"
#include "hparasciifileio.h"
#include "hparrootfileio.h"
#include "htask.h"
#include "htaskset.h"
#include "hdst.h"


#include "hrichunpacker.h"
#include "hmdcunpacker.h"
#include "hshowerunpacker.h"
#include "htoftrb2unpacker.h"
#include "hrpctrb2unpacker.h"
#include "hstart2trb2unpacker.h"
#include "hlatchunpacker.h"
#include "htboxunpacker.h"
#include "hwallunpacker.h"

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
#include "hwalltaskset.h"
#include "hsplinetaskset.h"

#include "hparticlestart2hitf.h"
#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"
#include "hparticlevertexfind.h"
#include "hmdcdedx2maker.h"

#include "hmdcsetup.h"
#include "hmdclayercorrpar.h"
#include "hmdctrackdset.h"
#include "hmdc34clfinder.h"
#include "hmdcidealtracking.h"
#include "hstart2calibrater.h"

#include "hmdcsizescells.h"
#include "hmdcgetcontainers.h"
#include "htrbnetaddressmapping.h"


#include "hedhelpers.h"
#include "HSelectedEvents.h"

#include "TOrdCollection.h"





Bool_t createHades()
{

    TString inputFile  = "/hera/hades/apr12/108/be1210817112103.hld";

    Int_t refId        = -1;
    TString beamtime = "apr12";
    Bool_t doExtendedFit     = kTRUE; // switch on/off fit for initial params of segment fitter (10 x slower!)
    Bool_t doStartCorrection = kTRUE;  // kTRUE (default)=  use run by run start corrections

    TString asciiParFile="";
    //TString rootParFile = "";
    TString rootParFile  = "/cvmfs/hades.gsi.de/param/real/apr12/allParam_APR12_gen2_14122012.root";

    TString paramSource   = "root"; // oracle, ascii, root

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

    HDst::setupSpectrometer(beamtime,mdcMods,"rich,mdc,tof,rpc,shower,wall,tbox,start");
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

    HDst::setDataSource(0,"",inputFile, -1 ,"");
    // datasource 0 = hld, 1 = hldgrep 2 = hldremote, 3 root
    // like "lxhadeb02.gsi.de"  needed by dataosoure = 2
    // inputDir needed by dataosoure = 1,2
    // inputFile needed by dataosoure = 1,3

    HDst::setupUnpackers(beamtime,"rich,mdc,tof,rpc,shower,wall,tbox,latch,start");
    // beamtime apr12
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,latch,start


    // ----------- Build TASK SETS (using H***TaskSet::make) -------------

    HStartTaskSet        *startTaskSet        = new HStartTaskSet();
    HRichTaskSet         *richTaskSet         = new HRichTaskSet();
    HRpcTaskSet          *rpcTaskSet          = new HRpcTaskSet();
    HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
    HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
    HWallTaskSet         *wallTaskSet         = new HWallTaskSet();
    HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet();
    mdcTaskSet->setVersionDeDx(1); // 0 = no dEdx, 1 = HMdcDeDx2

    refId=gHades->getDataSource()->getCurrentRunId();

    HMdcSetup* mysetup = (HMdcSetup*)rtdb->getContainer("MdcSetup");
    HTrbnetAddressMapping* trbnetmap = (HTrbnetAddressMapping*)rtdb->getContainer("TrbnetAddressMapping");
    rtdb->initContainers(refId);
    mysetup->setStatic();
    trbnetmap->setStatic();

    mysetup->getMdcCommonSet()->setAnalysisLevel(4);                // fit
    mysetup->getMdcCalibrater1Set()->setMdcCalibrater1Set(1, 1);    // 1 = NoStartandCal, 2 = StartandCal, 3 = NoStartandNoCal ::  0 = noTimeCut, 1 = TimeCut
    mysetup->getMdcTrackFinderSet()->setIsCoilOff(kFALSE);          // field is on
    mysetup->getMdcTrackFinderSet()->setNLayers(nLayers[0]);
    mysetup->getMdcTrackFinderSet()->setNLevel(nLevel);
    mysetup->getMdc12FitSet()->setMdc12FitSet(2,1,0,kFALSE,kFALSE); // tuned fitter, seg


    HTask *startTasks         = startTaskSet       ->make("","");
    HTask *richTasks          = richTaskSet        ->make("real","");
    HTask *tofTasks           = tofTaskSet         ->make("real");
    HTask *rpcTasks           = rpcTaskSet         ->make("real");
    HTask *wallTasks          = wallTaskSet        ->make("real");
    HTask *showerTasks        = showerTaskSet      ->make("","lowshowerefficiency");
    HTask *mdcTasks           = mdcTaskSet->make("rtdb","");

    //----------------SPLINE and RUNGE TACKING----------------------------------------
    HSplineTaskSet         *splineTaskSet       = new HSplineTaskSet("","");
    HTask *splineTasks=splineTaskSet->make("","spline,runge");

    //-----------------------------------------------------------------------
    HParticleStart2HitF    *pParticleStart2HitF = new HParticleStart2HitF   ("particlehitf"      ,"particlehitf","");
    //HParticleCandFiller  *pParticleCandFiller = new HParticleCandFiller("particlecandfiller","particlecandfiller","debug,goodseg0,goodseg1,goodmeta,goodrk");
    HParticleCandFiller   *pParticleCandFiller  = new HParticleCandFiller   ("particlecandfiller","particlecandfiller","");
    HParticleTrackCleaner *pParticleCleaner     = new HParticleTrackCleaner ("particlecleaner"   ,"particlecleaner");
    HParticleVertexFind    *pParticleVertexFind = new HParticleVertexFind   ("particlevertexfind","particlevertexfind","");
    HParticleEvtInfoFiller *pParticleEvtInfo    = new HParticleEvtInfoFiller("particleevtinfo"   ,"particleevtinfo");

    //------------------------ Master task set --------------------------

    // filling of MDC de/dx ...
    HMdcDeDx2Maker::setFillCase(2); // 0 (default)=combined, 1=combined+seg, 2=combined+seg+mod


    HSelectedEvents* selection = new HSelectedEvents("HSeletedEvents","HSelectedEvents");
    //selection->setInputFile("seqNumber.txt"); // comment this line to do no selection

    HTaskSet *masterTaskSet = gHades->getTaskSet("all");
    masterTaskSet->add(selection);
    masterTaskSet->add(startTasks);
    masterTaskSet->add(tofTasks);
    masterTaskSet->add(wallTasks);
    masterTaskSet->add(rpcTasks);
    masterTaskSet->add(pParticleStart2HitF); // run before wall,mdc,sline,candfiller
    masterTaskSet->add(richTasks);
    masterTaskSet->add(showerTasks);
    masterTaskSet->add(mdcTasks);
    masterTaskSet->add(splineTasks);
    masterTaskSet->add(pParticleCandFiller);
    masterTaskSet->add(pParticleCleaner);
    masterTaskSet->add(pParticleVertexFind); // run after track cleaning
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

    HTaskSet* set = (HTaskSet*) gHades->getTaskSet("all")->getSetOfTask()->FindObject("Mdc");
    if(set && set->getSetOfTask()->FindObject("mdc.hitf")) hasTracking = kTRUE;

    if(!hasTracking){
        set = (HTaskSet*) gHades->getTaskSet("all")->getSetOfTask()->FindObject("Mdc");
        if(set && set->getSetOfTask()->FindObject("mdc.hitf")) hasTracking = kTRUE;
    }

    if(hasTracking){
	cout<<"EVENT DISPLAY : TRACKING IS RUNNING #########################################"<<endl;
    }

    if(hasTracking){                      // is Tracking running ?
	cout<<"EVENT DISPLAY : SETTING TRACKING PARAMETERS FOT AU+AU @ 1.23 AGEV (APR12) ############"<<endl;
	//--------------------------------------------------------------------
	//  special settings
	HMdcTrackDSet::setTrackParam(beamtime);

	//--------------------------------------------------------------------
	// find best initial params for segment fit (takes long!)
	if(doExtendedFit) {
	    HMdcTrackDSet::setCalcInitialValue(1);  // -  1 : for all clusters 2 : for not fitted clusters
	}
	//--------------------------------------------------------------------
    }
    //HMdcDeDx2Maker::setFillCase(2);                      // 0 (default)=combined, 1=combined+seg, 2=combined+seg+mod
    HStart2Calibrater::setCorrection(doStartCorrection); // kTRUE (default)=  use
    //--------------------------------------------------------------------

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

