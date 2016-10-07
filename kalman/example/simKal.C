#include "hades.h"
#include "hruntimedb.h"
#include "htask.h"
#include "hevent.h"
#include "hcategory.h"
#include "hdst.h"
#include "htime.h"
#include "hsrckeeper.h"

#include "hkaldef.h"
#include "hkalfiltertask.h"

#include "hstarttaskset.h"
#include "hrichtaskset.h"
#include "hmdctaskset.h"
#include "htoftaskset.h"
#include "hrpctaskset.h"
#include "hshowertaskset.h"
#include "hwalltaskset.h"
#include "hsplinetaskset.h"

#include "hparticlevertexfind.h"
#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"
#include "hparticlebt.h"
#include "hparticlet0reco.h"
#include "hqamaker.h"

#include "haddef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "showerdef.h"
#include "rpcdef.h"
#include "tofdef.h"
#include "walldef.h"



#include "hmdcsetup.h"
#include "hmdclayercorrpar.h"
#include "hmdcdedx2maker.h"
#include "hmdcdigitizer.h"
#include "hmdcidealtracking.h"
#include "hrichdigitizer.h"
#include "hmdctrackdset.h"
#include "hmdctrkcand.h"
#include "hmdc12fit.h"
#include "hmetamatchF2.h"
#include "hstart2hitfsim.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TStopwatch.h"
#include "TDatime.h"

// Possible to make your own PID function for the Kalman filter.
static Int_t customPid(HMdcTrkCand* cand) {
    return 14;
}

void simKal(
	    TString inFile="/lustre/nyx/hades/dstsim/apr12/hgeant/bmax10/new/w/Au_Au_1230MeV_1000evts_1_1.root",
	    TString outDir="./sim",
	    Int_t nEvents=100,
	    Int_t startEvt=0) {

    const Int_t counterstep = 100;

    new Hades;
    TStopwatch timer;
    gHades->setTreeBufferSize(8000);
    gHades->makeCounter(counterstep);
    gHades->setBeamTimeID(Particle::kApr12);
    //gHades->getSrcKeeper()->addSourceFile("analysisDST.cc");
    //gHades->getSrcKeeper()->addSourceFile("sendScript.sh");


    //####################################################################
    //######################## CONFIGURATION #############################
    printf("Setting configuration...+++\n");

    HRuntimeDb *rtdb = gHades -> getRuntimeDb();
    Int_t refId = 12001; // apr12sim_mediumfieldalign_auau;
    TString beamtime="apr12"; // used for spectrometer setup and track params
    //-------------- Default Settings for the File names -----------------

    if(!outDir.EndsWith("/")) outDir+="/";
    TString outDirQA = outDir+"/qa/";

    TString outFileSuffix = "_dst_" + beamtime + ".root";

    TString asciiParFile = "";
    //TString rootParFile = "./allParam_APR12_sim_run_12001_gen8_01062016.root";
    TString rootParFile = "/cvmfs/hades.gsi.de/param/sim/apr12/allParam_APR12_sim_run_12001_gen8_27012016.root";
    TString paramSource = "root"; // root, ascii, oracle
    //TString paramRelease = "APR12SIM_dst_gen8"; // 16082015
    TString paramRelease = "now"; //"now"


    Bool_t kParamFile        = kFALSE;
    Bool_t doExtendedFit     = kTRUE; // switch on/off fit for initial params of segment fitter (10 x slower!)
    Bool_t doMetaMatch       = kTRUE;  // default : kTRUE, kFALSE switch off metamatch in clusterfinder
    Bool_t doMetaMatchScale  = kTRUE;
    Bool_t useWireStat       = kTRUE;
    Float_t metaScale        = 1.5;
    Bool_t doTree            = kTRUE;
    Bool_t doMDCDeltaElectron = kTRUE;
    Bool_t doRICHDeltaElectron= kFALSE; // kFALSE : suppress inserted delta electrons


    //--------------------------------------------------------------------
    // switch off unwanted categories in the poutput
    Cat_t notPersistentCat[] =
    {
	//catRich, catMdc, catShower, catTof, catTracks,
	catRichRaw, catRichHitHdr,
        catRichTrack,
	//catRichDirClus,
	//catRichHit,
	//catRichCal,
	catMdcRaw, //catMdcCal1, catMdcCal2, catMdcHit,
	catMdcClusInf,/*catMdcSeg,*/ catMdcTrkCand, catMdcRawEventHeader,

	catShowerCal, catShowerPID, catShowerHitHdr, catShowerRaw,
	catShowerHit,
        catShowerGeantWire,catShowerRawMatr,catShowerTrack,

	catTofRaw,
	catTofHit,
	catTofCluster,

	catRpcRaw,
	catRpcHit,
	catRpcCluster,
	catRpcCal,

	/*catRKTrackB,*/catSplineTrack,
	//catMetaMatch,
	//catParticleCandidate, catParticleEvtInfo,
        catParticleMdc,
	catWallRaw, catWallOneHit, catWallCal
    };
    //--------------------------------------------------------------------

    //####################################################################
    //####################################################################


    //------------- Operations on the filenames --------------------------
    TString rootSuffix =".root";
    TString nFile;    // needed to build outputfile
    TString dirname;  // needed for datasource
    TString filename; // needed for datasource
    TString outFile;

    Int_t sourcetype = 3; // root source

    if(inFile.Contains(",")){ // comma seperated list for geant merge source
	sourcetype = 4;
	inFile.ReplaceAll(" ","");
	TObjArray* ar = inFile.Tokenize(",");
	TString firstfile;
	if(ar){
	    if(ar->GetEntries()>0) {
		firstfile = ((TObjString*)ar->At(0))->GetString();
	    }
	    delete ar;
	}
	nFile     = gSystem->BaseName(firstfile.Data());
	filename  = inFile;
	dirname   = "";

    }  else {  // root source

	nFile     = gSystem->BaseName(inFile.Data());
	filename  = gSystem->BaseName(inFile.Data());
	dirname   = gSystem->DirName(inFile.Data());
    }
    if (nFile.EndsWith(rootSuffix)) nFile.ReplaceAll(rootSuffix,"");
    outFile  = outDir+"/"+nFile+outFileSuffix;
    outFile.ReplaceAll("//", "/");

    if(gSystem->AccessPathName(outDir.Data()) != 0){
	cout<<"Creating output dir :"<<outDir.Data()<<endl;
	gSystem->Exec(Form("mkdir -p %s",outDir.Data()));
    }
    if(gSystem->AccessPathName(outDirQA.Data()) != 0){
	cout<<"Creating output qadir :"<<outDirQA.Data()<<endl;
	gSystem->Exec(Form("mkdir -p %s",outDirQA.Data()));
    }
    //--------------------------------------------------------------------




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

    HDst::setupSpectrometer(beamtime,mdcMods,"start,rich,mdc,tof,rpc,shower,wall");
    // beamtime mdcMods_apr12, mdcMods_full
    // Int_t mdcset[6][4] setup mdc. If not used put NULL (default).
    // if not NULL it will overwrite settings given by beamtime
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,start

    HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,paramRelease);
    //HDst::setupParameterSources("oracle",asciiParFile,rootParFile,"now"); // use to create param file
    // parsource = oracle,ascii,root (order matters)
    // if source is "ascii" a ascii param file has to provided
    // if source is "root" a root param file has to provided
    // The histDate paramter (default "now") is used wit the oracle source

    cout<<"dir="<<dirname<<", file="<<filename<<", refid="<<refId<<endl;

    HDst::setDataSource(sourcetype,dirname,filename,refId); // Int_t sourceType,TString inDir,TString inFile,Int_t refId, TString eventbuilder"
    // datasource 0 = hld, 1 = hldgrep 2 = hldremote, 3 root, 4 geantmerge
    // like "lxhadeb02.gsi.de"  needed by dataosoure = 2
    // inputDir needed by dataosoure = 1,2
    // inputFile needed by dataosoure = 1,3
    // for datasource 4 inputFile is a comma seprated list
    // "file1_with_path,file2_with_path,file3_with_path"


    if(kParamFile) {

	TDatime time;
        TString paramfilename= Form("allParam_APR12_sim_run_%i_gen8_%02i%02i%i",refId,time.GetDay(),time.GetMonth(),time.GetYear());  // without .root

	if(gSystem->AccessPathName(Form("%s.root",paramfilename.Data())) == 0){
	    gSystem->Exec(Form("rm -f %s.root",paramfilename.Data()));
	}
	if(gSystem->AccessPathName(Form("%s.log",paramfilename.Data())) == 0){
	    gSystem->Exec(Form("rm -f %s.log" ,paramfilename.Data()));
	}

	if (!rtdb->makeParamFile(Form("%s.root",paramfilename.Data()),"apr12sim","20-MAR-2012 00:00:00","20-MAR-2012 00:00:01")) {
	    delete gHades;
	    exit(1);
	}
    }

    //--------------------------------------------------------------------
    // ----------- Build TASK SETS (using H***TaskSet::make) -------------
    HStartTaskSet        *startTaskSet        = new HStartTaskSet();
    HRichTaskSet         *richTaskSet         = new HRichTaskSet();
    HRpcTaskSet          *rpcTaskSet          = new HRpcTaskSet();
    HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
    HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
    HWallTaskSet         *wallTaskSet         = new HWallTaskSet();
    HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet();
    //    mdcTaskSet->setVersionDeDx(1); // 0 = no dEdx, 1 = HMdcDeDx2


   // HMdcSetup* mysetup = (HMdcSetup*)rtdb->getContainer("MdcSetup");
    rtdb->initContainers(refId);
   // mysetup->setStatic();

    //mysetup->getMdcCommonSet()->setIsSimulation(1);                 // fit
    //mysetup->getMdcCommonSet()->setAnalysisLevel(4);                // fit
    //mysetup->getMdcTrackFinderSet()->setIsCoilOff(kFALSE);          // field is on
    //mysetup->getMdcTrackFinderSet()->setNLayers(nLayers[0]);
    //mysetup->getMdcTrackFinderSet()->setNLevel(nLevel);
    //mysetup->getMdc12FitSet()->setMdc12FitSet(2,1,0,kFALSE,kFALSE); // tuned fitter, seg

    HTask *startTasks         = startTaskSet       ->make("simulation");
    HTask *richTasks          = richTaskSet        ->make("simulation","noiseon");
    HTask *tofTasks           = tofTaskSet         ->make("simulation");
    HTask *wallTasks          = wallTaskSet        ->make("simulation");
    HTask *rpcTasks           = rpcTaskSet         ->make("simulation");
    HTask *showerTasks        = showerTaskSet      ->make("","simulation,lowshowerefficiency");
    Bool_t idealTracking = kFALSE;
    Bool_t fillParallel  = kFALSE;
    if(idealTracking) {
	mdcTaskSet->useIdealTracking(fillParallel); // must be before mdcTaskSet->make(...)
    }
    HTask *mdcTasks           = mdcTaskSet         ->make("rtdb","");

    if(idealTracking) {
	HMdcTrackDSet::getMdcIdealTracking()->setResolutionX(0.F, 0, 0, 0);
	HMdcTrackDSet::getMdcIdealTracking()->setResolutionY(0, 0, 0, 0);
    }

    HMdcDigitizer* digi = mdcTaskSet->getDigitizer();

    digi->setDeltaElectronUse(doMDCDeltaElectron,kFALSE,109,-750.,600.,20.);
    digi->setDeltaElectronMinMomCut(2.,2.,4.5,2.,2.,4.5);  // take care of glass mirrors in sec 2+5
    digi->setTimeCutUse(kTRUE);

    HRichDigitizer* richdigi = HRichDigitizer::getDigitizer();
    richdigi->setDeltaElectronUse(doRICHDeltaElectron,kFALSE,109,20.,2.);
    richdigi->setDeltaElectronMinMomCut(0.,0.,0.,0.,0.,0.);

    //----- Filling of MDC de/dx ...
    //HMdcDeDx2Maker::setFillCase(2); // 0 (default)=combined, 1=combined+seg, 2=combined+seg+mod


    //----------------SPLINE and RUNGE TACKING----------------------------------------

    HSplineTaskSet         *splineTaskSet       = new HSplineTaskSet("","");
    HTask *splineTasks     = splineTaskSet      ->make("","spline,runge");

    HParticleCandFiller    *pParticleCandFiller = new HParticleCandFiller   ("particlecandfiller","particlecandfiller","");
    pParticleCandFiller->setFillMdc(kFALSE); // new : testing close pair
    HParticleTrackCleaner  *pParticleCleaner    = new HParticleTrackCleaner ("particlecleaner"   ,"particlecleaner");
    HParticleVertexFind    *pParticleVertexFind = new HParticleVertexFind   ("particlevertexfind","particlevertexfind",kTRUE);
    HParticleEvtInfoFiller *pParticleEvtInfo    = new HParticleEvtInfoFiller("particleevtinfo"   ,"particleevtinfo",beamtime);
    HParticleBt            *pParticleBt         = new HParticleBt("RichBackTracking","RichBackTracking",beamtime);


    // Options for the Kalman filter and DAF.
    // --------------------------------------

    Bool_t      bSim             = kTRUE;

    // Position error in x and y in mm
    // default 0.2mm in x and 0.1 in y direction.
    //Double_t    errX             = 0.2;
    Double_t    errX             = 0.4;

    //Double_t    errY             = 0.1;
    Double_t    errY             = 0.14;

    // Errors for phi = arctan(x/z) and theta = arctan(y/z) in degrees
    //Double_t    errTx            = 0.5;
    //Double_t    errTy            = 0.5;
    Double_t    errTx            = 0.4;
    Double_t    errTy            = 0.3;

    // Momentum estimation error as fraction. For example, 0.05 means
    // that the estimation is within 5% of the true value.
    Double_t    errMom           = 0.07;

    // kIterForward:  Propagate track from MDCI to MDCIV (default).
    // kIterBackward: Propagate track backwards from MDCIV to MDCI.
    Bool_t      dir              = kIterForward;

    // Number of Kalman filter iterations. (default: 1)
    // Raise this number if you wish to wait longer.
    Int_t       nIter            = 1;

    // kVarRot: Rotate coordinate system in the direction of initial
    // track estimation (default).
    // kNoRot:  Do not perform the above transformation.
    Kalman::kalRotateOptions rot = kVarRot;

    // kTRUE: run filter with smoothing (default).
    // kFALSE: run filter without smoothing.
    Bool_t      bSmooth          = kTRUE;

    // kTRUE: work with wire hits.
    // kFALSE: work with segment hits (default).
    Bool_t      bFitWireHits     = kFALSE;

    // Turn on the annealing filter. Only works with wire hits.
    Bool_t      bDoDaf           = kFALSE;

    // Allow for competing wire hits in an MDC layer.
    Bool_t      bCompHits        = kFALSE;

    // Turn on/off energy loss and multiple scattering.
    Bool_t      bDoEnerLoss      = kTRUE;
    Bool_t      bDoMultScat      = kTRUE;

    // kTRUE: Get initial PID from Geant information.
    Bool_t      bGeantPid        = kFALSE;

    // Input for initial state vector.
    // default = Spline, 1 = Geant without smearing,
    // 2 = Geant with smearing, 3 = Runge-Kutta
    Int_t       iniSvMethod      = 0;

    // Reconstruct only particles with certain Geant pid's.
    // Leave this array empty to reconstruct all particles.
    const Int_t nPids            = 0;
    Int_t       pid[nPids];// = {14};
    filtMethod  filtType         = Kalman::kKalConv;

    // Parameters for the annealing filter.
    // Number of DAF iterations.
    const Int_t nDafs            = 7;
    // Annealing factors (temperatures).
    //Double_t    dafT[nDafs]      = { 1. };
    Double_t    dafT[nDafs]      = { 1000., 200., 81., 9., 4., 1., 1. };
    // Cut-off parameter (default is 4).
    Double_t    dafChi2Cut       = 4;

    // Verbose level.
    // 0: no messages, 1: print error messages, >1: print errors and warnings.
    Int_t       verb             = 0;

    TF1 *fTxErr = new TF1("fTxErr", "0.0077 - 0.00021 * x + 4.3e-6 * x^2",
        		   0., 90.);
    TF1 *fTyErr = new TF1("fTyErr", "0.0097 - 0.00048 * x + 8.1e-6 * x^2",
        		   0., 90.);
    TF1 *fMomErr = new TF1("fMomErr", "3.97 - 0.050 * x + 0.0041 * x^2",
        		   0., 90.);

    //----------------------------------------------------------
    // Set up Kalman filter.

    HKalFilterTask* kalFilterTask =
	new HKalFilterTask(refId, bSim, bFitWireHits, bDoDaf, bCompHits);
    // Change default parameters. This must be done after

    kalFilterTask->setDafPars(dafChi2Cut, &dafT[0], nDafs);
    kalFilterTask->setDirection(dir);
    kalFilterTask->setDoPid(&pid[0], nPids);
    kalFilterTask->setDoEnerLoss(bDoEnerLoss);
    kalFilterTask->setDoMultScat(bDoMultScat);
    kalFilterTask->setFilterMethod(filtType);
    kalFilterTask->setNumIterations(nIter);
    kalFilterTask->setRotation(rot);
    kalFilterTask->setSmoothing(bSmooth);
    kalFilterTask->setCounterStep(-1);
    kalFilterTask->setIniStateMethod(iniSvMethod);
    kalFilterTask->setErrors(errX, errY, TMath::Tan(errTx*TMath::DegToRad()),
			     TMath::Tan(errTy*TMath::DegToRad()), errMom);
    kalFilterTask->setFillSites(kTRUE);
    kalFilterTask->setUseGeantPid(bGeantPid);
    kalFilterTask->setVerbose(verb);

    kalFilterTask->setFuncMomErr(fMomErr);
    kalFilterTask->setFuncTxErr(fTxErr);
    kalFilterTask->setFunKyErr(fTyErr);

    //kalFilterTask->setPid(customPid);


    //----------------------- Quality Assessment -------------------------
    HQAMaker *qaMaker =0;
    if (!outDirQA.IsNull())
    {
	qaMaker = new HQAMaker("qamaker","qamaker");
	//qaMaker->setUseSlowPar(kFALSE);
	qaMaker->setOutputDir((Text_t *)outDirQA.Data());
	//qaMaker->setPSFileName((Text_t *)hldFile.Data());
	qaMaker->setSamplingRate(1);
	qaMaker->setIntervalSize(50);
    }



    //------------------------ Master task set ---------------------------
    HTaskSet *masterTaskSet = gHades->getTaskSet("all");
    masterTaskSet->add(startTasks);
    masterTaskSet->add(tofTasks);
    masterTaskSet->add(wallTasks);
    masterTaskSet->add(rpcTasks);
    masterTaskSet->add(richTasks);
    masterTaskSet->add(showerTasks);
    masterTaskSet->add(mdcTasks);
    masterTaskSet->add(splineTasks);
    masterTaskSet->add(kalFilterTask);
    masterTaskSet->add(pParticleCandFiller);
    masterTaskSet->add(pParticleCleaner);
    masterTaskSet->add(pParticleVertexFind); // run after track cleaning
    masterTaskSet->add(pParticleEvtInfo);
    masterTaskSet->add(pParticleBt);
    masterTaskSet->add(new HParticleT0Reco("T0","T0",beamtime));

    //if (qaMaker) masterTaskSet->add(qaMaker);

    HMdcTrackDSet::setTrackParam(beamtime);
    if(!doMetaMatch)HMdcTrackDSet::setMetaMatchFlag(kFALSE,kFALSE);  //do not user meta match in clusterfinder
    if(doMetaMatchScale)HMetaMatchF2::setScaleCut(metaScale,metaScale,metaScale); // (tof,rpc,shower) increase matching window, but do not change normalization of MetaQA

    HStart2HitFSim* starthitf = HStart2HitFSim::getHitFinder() ;
    if(starthitf) starthitf->setResolution(0.06);    // 60 ps start res

    //--------------------------------------------------------------------
    // find best initial params for segment fit (takes long!)
    if(doExtendedFit) {
 	HMdcTrackDSet::setCalcInitialValue(1);  // -  1 : for all clusters 2 : for not fitted clusters
    }
    //--------------------------------------------------------------------

    if (!gHades->init()){
	Error("init()","Hades did not initialize ... once again");
	exit(1);
    }




    //--------------------------------------------------------------------
    //----------------- Set not persistent categories --------------------
    HEvent *event = gHades->getCurrentEvent();

    for(UInt_t i=0;i<sizeof(notPersistentCat)/sizeof(Cat_t);i++){
	HCategory *cat = ((HCategory *)event->getCategory(notPersistentCat[i]));
	if(cat)cat->setPersistency(kFALSE);
    }
    //--------------------------------------------------------------------
    if(doTree){
	// output file
	gHades->setOutputFile((Text_t*)outFile.Data(),"RECREATE","Test",2);
	gHades->makeTree();
    }
    Int_t nProcessed = gHades->eventLoop(nEvents,startEvt);
    printf("Events processed: %i\n",nProcessed);

    cout<<"--Input file      : "<<inFile  <<endl;
    cout<<"--QA directory is : "<<outDirQA<<endl;
    cout<<"--Output file is  : "<<outFile <<endl;

    printf("Real time: %f\n",timer.RealTime());
    printf("Cpu time: %f\n",timer.CpuTime());
    if (nProcessed) printf("Performance: %f s/ev\n",timer.CpuTime()/nProcessed);

    if(kParamFile) rtdb->saveOutput();

    delete gHades;
    timer.Stop();

}