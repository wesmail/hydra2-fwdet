#ifndef __CREATEHADES__
#define __CREATEHADES__


#include "hades.h"
#include "hruntimedb.h"
#include "hdatasource.h"
#include "hrootsource.h"
#include "hldsource.h"
#include "hldfilesource.h"
#include "hldgrepfilesource.h"
#include "hldremotesource.h"
#include "hparora2io.h"
#include "hparasciifileio.h"
#include "hparrootfileio.h"
#include "htask.h"
#include "htaskset.h"

#include "hspectrometer.h"
#include "hstart2detector.h"
#include "htboxdetector.h"
#include "hrichdetector.h"
#include "hmdcdetector.h"
#include "htofdetector.h"
#include "hshowerdetector.h"
#include "hrpcdetector.h"
#include "hwalldetector.h"
#include "hpiontrackerdetector.h"

#include "hstarttaskset.h"
#include "hrichtaskset.h"
#include "hmdctaskset.h"
#include "htoftaskset.h"
#include "hwalltaskset.h"
#include "hshowertaskset.h"
#include "hrpctaskset.h"
#include "hpiontrackertaskset.h"
#include "hsplinetaskset.h"

#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"


#include "hmdcunpacker.h"
#include "hrichunpacker.h"
#include "hrpctrb2unpacker.h"
#include "htoftrb2unpacker.h"
#include "hstart2trb2unpacker.h"
#include "hstart2trb3unpacker.h"
#include "hpiontrackertrb3unpacker.h"
#include "hwalltrb2unpacker.h"
#include "hshowerunpacker.h"
#include "hlatchunpacker.h"
#include "htboxunpacker.h"

#include "hmdcsetup.h"
#include "hmdctimecut.h"
#include "hmdclayercorrpar.h"
#include "hmdctrackdset.h"
#include "hmdctrackfinder.h"
#include "hmdc34clfinder.h"
#include "hmdcidealtracking.h"
#include "hstart2calibrater.h"

#include "hmdcsizescells.h"
#include "hmdcgetcontainers.h"

#include "hedhelpers.h"

#include "TOrdCollection.h"

Bool_t initDetectorSetup();
Bool_t setDataSource (Int_t sourceType,TString inDir,TString inFile="", Int_t refId   = -1 ,TString eventbuilder = "lxhadeb02.gsi.de");
Bool_t setParamSource(TString paramSource,TString asciiParFile,TString rootParFile);

Bool_t createHades(Int_t   datasource,
		   TString inputDir,
		   TString inputFile,
		   Int_t   refId,
		   TString eventbuilder,
		   TString paramSource,
		   TString asciiParFile,
		   TString rootParFile
		  )
{

    cout<<"-----------------------------------------------------------------------------"<<endl;
    cout<<"EVENT DISPLAY : INPUT FILE: "<<inputDir.Data()<<inputFile.Data()<<endl;
    cout<<"-----------------------------------------------------------------------------"<<endl;
    Hades* myHades = new Hades();
    gHades->setTreeBufferSize(8000);
    gHades->setQuietMode(2);
    gHades->makeCounter(1);
    HRuntimeDb* rtdb = gHades->getRuntimeDb();

    TString beamtime="apr12";

    // ------------ Set input data file: NO NEED TO CHANGE  --------------
    if(!setDataSource(datasource,inputDir,inputFile,refId,eventbuilder))  exit(1); // 0 = hld, 1 = hld grep 2= hldremote
    // ----------Add detectors to the setup: NO NEED TO CHANGE -----------
    if(!initDetectorSetup())                                 exit(1);
    //--------------------------------------------------------------------
    //  PARAMETER SOURCES
    if(!setParamSource(paramSource,asciiParFile,rootParFile))exit(1);
    //--------------------------------------------------------------------

    Int_t mdcUnpackers   [12] = {0x1100,0x1110,0x1120,0x1130,0x1140,0x1150,0x1000,0x1010,0x1020,0x1030,0x1040,0x1050};
    Int_t rpcUnpackers   [3]  = {0x8400,0x8410,0x8420}; //
    Int_t startUnpackers [1]  = {0x8800};        // CTS
    //Int_t startUnpackersTrb3 [2]  = {0x8880,0x8890};  // start+hodo
    Int_t startUnpackersTrb3 [1]  = {0x8890};  // hodo
    Int_t pionTrackerUnpackers[2] = {0x8900,0x8910};
    Int_t tofUnpackers   [1]  = {0x8600};        //
    Int_t wallUnpackers  [1]  = {0x8700};        //
    Int_t showerUnpackers[6]  = {0x3200,0x3210,0x3220,0x3230,0x3240,0x3250};        //
    Int_t richUnpackers  [3]  = {0x8300,0x8310,0x8320};


    HldSource* source = (HldSource*)gHades->getDataSource();


    Bool_t disableTimeRef = kFALSE;
    
    for(UInt_t i=0; i<(sizeof(wallUnpackers)/sizeof(Int_t)); i++)
    {
	HWallTrb2Unpacker* wallUnpacker=new HWallTrb2Unpacker(wallUnpackers[i]);
	wallUnpacker->setQuietMode();
	wallUnpacker->setQuietMode();
	wallUnpacker->removeTimeRef();
	wallUnpacker->shiftTimes(7200.);
	source->addUnpacker( wallUnpacker);
    }
   
    source->addUnpacker( new HLatchUnpacker(startUnpackers[0]) );
    source->addUnpacker( new HTBoxUnpacker(startUnpackers[0]) );

    for(UInt_t i=0; i<(sizeof(richUnpackers)/sizeof(Int_t)); i++){
	source->addUnpacker( new HRichUnpacker(richUnpackers[i]) );
    }



    for(UInt_t i=0; i<(sizeof(mdcUnpackers)/sizeof(Int_t)); i++) {
	HMdcUnpacker* mdc_unpacker = new HMdcUnpacker(  mdcUnpackers[i], kTRUE );
	mdc_unpacker->setQuietMode(kTRUE);

	mdc_unpacker->setFillDataWord(kFALSE);
	mdc_unpacker->setFillOepStatus(kTRUE);
	mdc_unpacker->setPersistencyDataword(kTRUE);
	mdc_unpacker->setPersistencyOepStatusData(kFALSE);
	//mdc_unpacker->setStandaloneMode(kTRUE,1);
	source->addUnpacker( (HMdcUnpacker*)mdc_unpacker );
    }


    for(UInt_t i=0; i<(sizeof(showerUnpackers)/sizeof(Int_t)); i++) {
	source->addUnpacker( new HShowerUnpacker(showerUnpackers[i]) );
    }

    for(UInt_t i=0; i<(sizeof(tofUnpackers)/sizeof(Int_t)); i++){
	HTofTrb2Unpacker *tofUnp = new HTofTrb2Unpacker(tofUnpackers[i]);
	tofUnp->setDebugFlag(0);
	//tofUnp->setQuietMode();
	//tofUnp->setcorrectINL();
	tofUnp->removeTimeRef();
	tofUnp->shiftTimes(7200.);
	source->addUnpacker( tofUnp );
    }
    /*
    for(UInt_t i=0; i<(sizeof(startUnpackers)/sizeof(Int_t)); i++){
	HStart2Trb2Unpacker *startUnp = new HStart2Trb2Unpacker(startUnpackers[i]);
	startUnp->setDebugFlag(0);
	//startUnp->setQuietMode();
	if(disableTimeRef)startUnp->disableTimeRef(); // only cosmics
	source->addUnpacker( startUnp );
    }
    */
    for(UInt_t i=0; i<(sizeof(startUnpackersTrb3)/sizeof(Int_t)); i++){
	HStart2Trb3Unpacker *startUnp = new HStart2Trb3Unpacker(startUnpackersTrb3[i]);
	startUnp->setDebugFlag(0);
	startUnp->setQuietMode();
	startUnp->setCTSId(0x8000);
	source->addUnpacker( startUnp );
    }

    for(UInt_t i=0; i<(sizeof(pionTrackerUnpackers)/sizeof(Int_t)); i++){
	HPionTrackerTrb3Unpacker *pionTrackerUnp = new HPionTrackerTrb3Unpacker(pionTrackerUnpackers[i]);
	pionTrackerUnp->setDebugFlag(0);
	pionTrackerUnp->setQuietMode();
	source->addUnpacker( pionTrackerUnp );
    }

    for(UInt_t i=0; i<(sizeof(rpcUnpackers)/sizeof(Int_t)); i++){

	HRpcTrb2Unpacker *rpcTrb2Unpacker = new HRpcTrb2Unpacker(rpcUnpackers[i]);
	//rpcTrb2Unpacker->setQuietMode();
	rpcTrb2Unpacker->setDebugFlag(0);
	rpcTrb2Unpacker->storeSpareChannelsData();
	source->addUnpacker(rpcTrb2Unpacker);
    }


    //--------------------------------------------------------------------
    // ALIGNMENT OF WIRE PLANES
    HMdcLayerCorrPar* fMdcLayerCorrPar =  (HMdcLayerCorrPar*)rtdb->getContainer("MdcLayerCorrPar");
      fMdcLayerCorrPar->setDefaultPar();
      fMdcLayerCorrPar->setStatic();
    //--------------------------------------------------------------------

    // ----------- Build TASK SETS (using H***TaskSet::make) -------------
    HStartTaskSet        *startTaskSet        = new HStartTaskSet();
    HRichTaskSet         *richTaskSet         = new HRichTaskSet();
    HRpcTaskSet          *rpcTaskSet          = new HRpcTaskSet();
    HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
    HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
    HWallTaskSet         *wallTaskSet         = new HWallTaskSet();
    HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet();
    mdcTaskSet->setVersionDeDx(1); // 0 = no dEdx, 1 = HMdcDeDx2
    HPionTrackerTaskSet  *piontrackerTaskSet  = new HPionTrackerTaskSet();

    Int_t nLayers[6][4] = {
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6},
	{6,6,5,6} };
    Int_t nLevel[4] = {10,50000,10,5000};

    HMdcSetup* mysetup = (HMdcSetup*)rtdb->getContainer("MdcSetup");
    rtdb->initContainers(refId);

    mysetup->setStatic();

    mysetup->getMdcCommonSet()->setAnalysisLevel(4);                // fit
    mysetup->getMdcCalibrater1Set()->setMdcCalibrater1Set(1, 0);    // 1 = NoStartandCal, 2 = StartandCal, 3 = NoStartandNoCal ::  0 = noTimeCut, 1 = TimeCut
    mysetup->getMdcTrackFinderSet()->setIsCoilOff(kFALSE);          // field is on
    mysetup->getMdcTrackFinderSet()->setNLayers(nLayers[0]);
    mysetup->getMdcTrackFinderSet()->setNLevel(nLevel);
    mysetup->getMdc12FitSet()->setMdc12FitSet(2,1,0,kFALSE,kFALSE); // tuned fitter, seg

    HTask *startTasks;
    //startTasks          = startTaskSet       ->make("",""); //nostarthit
    startTasks          = startTaskSet       ->make("real","nostarthit");
    HTask *richTasks    = richTaskSet        ->make("real","");
    HTask *tofTasks     = tofTaskSet         ->make("real");
    HTask *wallTasks    = wallTaskSet        ->make("real", "noeventplane");
    HTask *rpcTasks     = rpcTaskSet         ->make("real");
    HTask *showerTasks  = showerTaskSet      ->make("real","lowshowerefficiency");
    //HTask *mdcTasks     = mdcTaskSet         ->make("","hit,NoStartAndCal,NoTimeCuts,MagnetOff");
    HTask *mdcTasks     = mdcTaskSet         ->make("rtdb","");
    HTask *piontrackerTasks   = piontrackerTaskSet ->make("real","nopiontrackertrack");

     //  ----------- MDC ------------------------------------------------------
     //HTask *mdcTasks           = mdcTaskSet         ->make("rtdb","");
     HMdcTrackFinder* trackfinder = HMdcTrackDSet::getMdcTrackFinder();
     if(trackfinder) {
     //trackfinder->setMetaMatchFlag(kFALSE,kFALSE); // no metamatching, no metamacth plots
     }

    //----------------SPLINE and RUNGE TACKING----------------------------------------
    HSplineTaskSet         *splineTaskSet       = new HSplineTaskSet("","");
    HTask *splineTasks=splineTaskSet->make("","spline,runge");

    //-----------------------------------------------------------------------
    //HParticleCandFiller  *pParticleCandFiller = new HParticleCandFiller("particlecandfiller","particlecandfiller","debug,goodseg0,goodseg1,goodmeta,goodrk");
    HParticleCandFiller   *pParticleCandFiller = new HParticleCandFiller  ("particlecandfiller","particlecandfiller","");
    HParticleTrackCleaner *pParticleCleaner    = new HParticleTrackCleaner("particlecleaner"   ,"particlecleaner");
    HParticleEvtInfoFiller *pParticleEvtInfo   = new HParticleEvtInfoFiller("particleevtinfo"  ,"particleevtinfo");

    //------------------------ Master task set --------------------------

    HTaskSet *masterTaskSet = gHades->getTaskSet("all");


    //######################### SPECIAL #############################
    HStart2Calibrater::setCorrection(kFALSE) ; // kTRUE(default) = use HStartCalRunPar corrections, for online no corrections


    HMdcTimeCut* timecut = (HMdcTimeCut*)rtdb->getContainer("MdcTimeCut");
    timecut->setIsUsed(kFALSE);  // time cut not used in HMdcCalibrater1 , but Tracking will use it
    //###############################################################

    masterTaskSet->add(startTasks);
    masterTaskSet->add(tofTasks);
    masterTaskSet->add(wallTasks);
    masterTaskSet->add(rpcTasks);
    masterTaskSet->add(richTasks);
    masterTaskSet->add(showerTasks);
    masterTaskSet->add(mdcTasks);
    masterTaskSet->add(piontrackerTasks);


    masterTaskSet->add(splineTasks);
    masterTaskSet->add(pParticleCandFiller);
    masterTaskSet->add(pParticleCleaner);
    masterTaskSet->add(pParticleEvtInfo);



    masterTaskSet->isTimed(kTRUE);

    HStart2Calibrater::setCorrection(kFALSE);  // if calibration is available .... change it

    //--------------------------------------------------------------------
    if (!gHades->init())
    {
	cerr<<"EVENT DISPLAY : ERROR IN INIT, EXITING! #####################################"<<endl;
	delete myHades;
	return kFALSE;
    }
    //--------------------------------------------------------------------

    HMdcTrackDSet::setTrackParam(beamtime);


    return kTRUE;
}



Bool_t initDetectorSetup()
{
    // Detectors setup configuration
    Int_t richMods[1]   = {1};

    Int_t mdcMods[6][4] = {
	{1,1,1,1},
	{1,1,1,1},
	{1,1,1,1},
	{1,1,1,1},
	{1,1,1,1},
	{1,1,1,1} };

    Int_t tofMods   [22] = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    Int_t rpcMods   [1]  = {1};
    Int_t showerMods[3]  = {1,2,3};
    //Int_t startMods [10] = {1,1,1,1,1,0,0,0,0,0};     // mod=4 added for apr12
    Int_t startMods [10] = {1,0,0,0,1,0,0,0,0,0};      // may14
    Int_t piontrackerMods [10] = {1,1,1,1,0,0,0,0,0,0};// may14
    Int_t wallMods  [1]  = {1};
    Int_t nTrigMods[]    = {1};

    HSpectrometer* spec = gHades->getSetup();

    spec->addDetector(new HTBoxDetector);
    spec->addDetector(new HStart2Detector);
    spec->addDetector(new HRichDetector);
    spec->addDetector(new HMdcDetector);
    spec->addDetector(new HTofDetector);
    spec->addDetector(new HRpcDetector);
    spec->addDetector(new HWallDetector);
    spec->addDetector(new HPionTrackerDetector);
    spec->addDetector(new HShowerDetector);


    spec->getDetector("TBox")->setModules(-1,nTrigMods);
    spec->getDetector("Start")->setModules(-1,startMods);
    spec->getDetector("PionTracker")->setModules(-1,piontrackerMods);
    spec->getDetector("Wall")->setModules(-1,wallMods);

    for (Int_t is=0; is<6; is++) {
	spec->getDetector("Rich")  ->setModules(is,richMods);
	spec->getDetector("Mdc")   ->setModules(is,mdcMods[is]);
	spec->getDetector("Tof")   ->setModules(is,tofMods);
	spec->getDetector("Rpc")   ->setModules(is,rpcMods);
	spec->getDetector("Shower")->setModules(is,showerMods);
    }

    return kTRUE;
}

Bool_t setDataSource(Int_t sourceType,
		     TString inDir,
		     TString inFile,
		     Int_t refId,
		     TString eventbuilder
		    )
{

    if( sourceType == 0 ){
	HldFileSource* source = new HldFileSource;
	source->setDirectory((Text_t*) inDir.Data());
	source->addFile((Text_t*) inFile.Data(),refId);
	gHades->setDataSource(source);
    } else if ( sourceType == 1 ){
	Int_t GrepInterval   = 5 ; // look for new file each n seconds
	Int_t GrepTimeOffset = 10; // at least n seconds old
	Int_t GrepFileOffset = 1;  // latest - GrepFileOffset file (minimum 0)
	HldGrepFileSource* source = new HldGrepFileSource(inDir,"Grep",GrepInterval,refId,GrepTimeOffset,GrepFileOffset);
	gHades->setDataSource(source);
    } else if ( sourceType == 2 ){
	HldRemoteSource* source = new HldRemoteSource(eventbuilder.Data());
	source->setRefId(refId);
	gHades->setDataSource(source);
    } else {
	cout<<"Error setDataSource(): Unknown source type = "<<sourceType<<"!"<<endl;
	return kFALSE;
    }
    return kTRUE;
}

Bool_t setParamSource(TString paramSource,TString asciiParFile,TString rootParFile)
{
    if(paramSource == "" ||
       (!paramSource.Contains("ROOT")  &&
	!paramSource.Contains("ASCII") &&
	!paramSource.Contains("ORACLE")
       )
      ){
	cout<<"Error: setDataSource(): Unknown parameter source = "<<paramSource.Data()<<"!"<<endl;
	return kFALSE;
    }
    HRuntimeDb *rtdb = gHades->getRuntimeDb();

    if (paramSource.Contains("ROOT"))
    {
	cout<<"READING ROOT :"<<rootParFile.Data()<<endl;
	HParRootFileIo *input1=new HParRootFileIo;
	input1->open(((Char_t *)rootParFile.Data()),"READ");
	if(paramSource.Contains("ASCII") == 0) rtdb->setFirstInput(input1);
	else                                   rtdb->setSecondInput(input1);
    }


    if (paramSource.Contains("ASCII"))
    {
	cout<<"READING ASCII :"<<asciiParFile.Data()<<endl;
	HParAsciiFileIo *input2 = new HParAsciiFileIo();
	input2->open((Text_t*)asciiParFile.Data(),"in");
	rtdb->setFirstInput(input2);
    }

    // ----- SECOND PARAM INPUT FOR PID

    if (paramSource.Contains("ORACLE"))
    {
        cout<<"READING ORACLE"<<endl;
	HParOra2Io* ora=new HParOra2Io;
	ora->open();
	ora->setHistoryDate("now");
	rtdb->setSecondInput(ora);
    }

    return kTRUE;
}














#endif


