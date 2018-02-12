
// base
#include "hades.h"
#include "hsrckeeper.h"
#include "hruntimedb.h"
#include "htask.h"
#include "hevent.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "hrootsource.h"
#include "hgeantmergesource.h"
#include "hdst.h"
#include "htime.h"

// tasksets
#include "hstarttaskset.h"
#include "hrichtaskset.h"
#include "hmdctaskset.h"
#include "htoftaskset.h"
#include "hrpctaskset.h"
#include "hshowertaskset.h"
#include "hwalltaskset.h"
#include "hsplinetaskset.h"


//tasks
#include "hmdcdedx2maker.h"
#include "hmdccalibrater1.h"
#include "hmdcdigitizer.h"
#include "hrichdigitizer.h"
#include "hmdctrackdset.h"
#include "hmdcvertexfind.h"
#include "hmdc12fit.h"
#include "hmetamatchF2.h"
#include "hparticlevertexfind.h"
#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"
#include "hparticlestart2hitf.h"
#include "hparticlet0reco.h"
#include "hparticlebt.h"
#include "hstart2calibrater.h"
#include "hstart2hitfsim.h"
#include "hqamaker.h"
#include "htofhitf2.h"


// defs
#include "haddef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "showerdef.h"
#include "rpcdef.h"
#include "tofdef.h"
#include "walldef.h"
#include "hstartdef.h"

// containers
#include "hmdcsetup.h"
#include "hmdclayercorrpar.h"
#include "htrbnetaddressmapping.h"


// ROOT
#include "TSystem.h"
#include "TROOT.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TStopwatch.h"

// standard
#include <iostream>
#include <cstdio>



#include "HSelectEmbedding.h"

using namespace std;



Int_t analysisDST(TString inFile,TString inFileGeant, TString outdir,Int_t nEvents=1, Int_t startEvt=0)
{
    new Hades;
    TStopwatch timer;
    gHades->setEmbeddingMode(1);    // 1 : do embedding , 0 no embedding (default) , 2 keep geant
    //gHades->setEmbeddingDebug(1);   // 1 : do embedding with empty real data , 0  (default) normal embedding
    gHades->setTreeBufferSize(8000);
    gHades->makeCounter(100);
    HRuntimeDb *rtdb = gHades -> getRuntimeDb();

    gHades->getSrcKeeper()->addSourceFile("analysis_embedding.cc");
    gHades->getSrcKeeper()->addSourceFile("analysis_pluto.cc");
    gHades->getSrcKeeper()->addSourceFile("analysis_vertex.cc");
    gHades->getSrcKeeper()->addSourceFile("HSelectEmbedding.h");
    gHades->getSrcKeeper()->addSourceFile("pluto_embedded.C");
    gHades->getSrcKeeper()->addSourceFile("writeVertex.C");
    gHades->getSrcKeeper()->addSourceFile("Makefile_dst");
    gHades->getSrcKeeper()->addSourceFile("Makefile_vertex");
    gHades->getSrcKeeper()->addSourceFile("Makefile_pluto");
    gHades->getSrcKeeper()->addSourceFile("sendScript.sh");

    //HTofHitF2::setForceKeepGeant(kTRUE) ;   // do not make realistic embedding

    cout<<"infile hld   :"<<inFile<<endl;
    cout<<"infile geant :"<<inFileGeant<<endl;
    cout<<"outdir       :"<<outdir<<endl;
    cout<<"n events     :"<<nEvents<<endl;



    //####################################################################
    //######################## CONFIGURATION #############################
    printf("Setting configuration...+++\n");

    TString asciiParFile     = "";
    TString rootParFile      = "/cvmfs/hades.gsi.de/param/embedding/apr12/allParam_APR12_gen9_embedding_gen1_17112017.root";
    TString paramSource      = "root"; // root, ascii, oracle

    TString outFileSuffix    = "_embedding_dst_apr12.root";

    Int_t  refId             = -1; //
    Bool_t kParamFile        = kFALSE;
    Bool_t doExtendedFit     = kTRUE; // switch on/off fit for initial params of segment fitter (10 x slower!)
    Bool_t doStartCorrection = kTRUE;  // kTRUE (default)=  use run by run start corrections
    Bool_t doMetaMatch       = kFALSE;  // default : kTRUE, kFALSE switch off metamatch in clusterfinder
    Bool_t useOffVertex      = kTRUE;  // default : kTRUE,  kTRUE=use off vertex procedure  (apr12 gen8:kFALSE, gen9:kTRUE)
    Bool_t doMetaMatchScale  = kTRUE;
    Bool_t useWireStat       = kTRUE;
    Float_t metaScale        = 1.5;
    Bool_t doTree            = kTRUE;
    Bool_t doMDCDeltaElectron = kTRUE;
    Bool_t doRICHDeltaElectron= kFALSE; // kFALSE : suppress inserted delta electrons

    TString beamtime         = "apr12";
    TString paramRelease     = "APR12SIM_dst_gen9";// "07-FEB-2016 00:00:00";  // 07-FEB-2016 00:00:00  (before OLGA added new LayerCorrPar!)
    //####################################################################
    //####################################################################




    //-------------- Default Settings for the File names -----------------
    TString baseDir = outdir;
    if(!baseDir.EndsWith("/")) baseDir+="/";

    TString fileWoPath = gSystem->BaseName(inFile.Data());
    fileWoPath.ReplaceAll(".hld","");
    Int_t day      = HTime::getDayFileName(fileWoPath,kFALSE);
    Int_t evtBuild = HTime::getEvtBuilderFileName(fileWoPath,kFALSE);

    //TString outDir   = Form("%s%i/",baseDir.Data(),day);
    TString outDir   = Form("%s",baseDir.Data());
    TString outDirQA = outDir+"qa/";

    if(gSystem->AccessPathName(outDir.Data()) != 0){
	cout<<"Creating output dir :"<<outDir.Data()<<endl;
	gSystem->Exec(Form("mkdir -p %s",outDir.Data()));
    }
    if(gSystem->AccessPathName(outDirQA.Data()) != 0){
	cout<<"Creating QA output dir :"<<outDirQA.Data()<<endl;
	gSystem->Exec(Form("mkdir -p %s",outDirQA.Data()));
    }

    TString hldSuffix =".hld";
    TString hldFile   = gSystem->BaseName(inFile.Data());
    if (hldFile.EndsWith(hldSuffix)) hldFile.ReplaceAll(hldSuffix,"");

    TString hld      = hldFile+hldSuffix;
    TString outFile  = outDir+hld+outFileSuffix;
    outFile.ReplaceAll("//", "/");
    //--------------------------------------------------------------------
    /*

    //------ extended output for eventbuilder 1------------------------------------//
    //----------------------------------------------------------------------------//
    Cat_t notPersistentCatAll[] =
    {
	//catRich, catMdc, catShower, catTof, catTracks,
	catRichRaw,
	//catRichDirClus, catRichHitHdr, //catRichHit,
	catRichCal,
	catMdcRaw,
	//catMdcCal1,
	catMdcCal2,catMdcClusInf,catMdcHit,
	//catMdcSeg,
	catMdcTrkCand,catMdcRawEventHeader,
	catShowerCal, catShowerPID, catShowerHitHdr, catShowerRaw, catShowerGeantWire,catShowerRawMatr,catShowerTrack,
	//catShowerHit,
	catTofRaw,
	//catTofHit, catTofCluster,
	catRpcRaw,
	//catRpcCal,
	//catRpcHit, catRpcCluster,
	catRKTrackB, catSplineTrack,
	//catMetaMatch,
	//catParticleCandidate, catParticleEvtInfo,
	catWallRaw, catWallOneHit, catWallCal,
	catStart2Raw //catStart2Cal, catStart2Hit,
	// catTBoxChan
    };
    //--------------------------------------------------------------------

    //------standard output for dst production------------------------------------//
    //----------------------------------------------------------------------------//
    Cat_t notPersistentCat[] =
    {
	//catRich, catMdc, catShower, catTof, catTracks,
	catRichRaw,
	//catRichDirClus, catRichHitHdr, catRichHit,
	catRichCal,
	catMdcRaw,
	//catMdcCal1,
	catMdcCal2,catMdcClusInf, catMdcHit,
	//catMdcSeg,
	catMdcTrkCand, catMdcRawEventHeader,
	catShowerCal, catShowerPID, catShowerHitHdr, catShowerRaw, catShowerGeantWire,catShowerRawMatr,catShowerTrack,
	//catShowerHit,
	catTofRaw,
	//catTofHit, catTofCluster,
	catRpcRaw,
	//catRpcCal,
	//catRpcHit, catRpcCluster,
	catRKTrackB, catSplineTrack,
	//catMetaMatch,
	//catParticleCandidate, catParticleEvtInfo,
	catWallRaw, catWallOneHit, catWallCal,
	catStart2Raw //catStart2Cal, catStart2Hit,
	// catTBoxChan
    };
    //--------------------------------------------------------------------
    */

    //------ extended output for eventbuilder 1------------------------------------//
    //----------------------------------------------------------------------------//
    Cat_t notPersistentCatAll[] =
    {
	//catRich, catMdc, catShower, catTof, catTracks,
	catRichRaw,
	//catRichDirClus, catRichHitHdr, //catRichHit,
	catRichCal,
	catMdcRaw,
	catMdcCal1,
	catMdcCal2,catMdcClusInf,catMdcHit,
	catMdcSeg, catMdcTrkCand,
	catMdcRawEventHeader,
	catShowerCal, catShowerPID, catShowerHitHdr, catShowerRaw, catShowerGeantWire,
	catShowerRawMatr,
	catShowerTrack,
	catShowerHit,
	catTofRaw,
	catTofHit, catTofCluster,
	catRpcRaw, catRpcCal,
	catRpcHit,
	catRpcCluster,
	catRKTrackB, catSplineTrack,
	catMetaMatch,
	//catParticleCandidate, catParticleEvtInfo,
	catWallRaw, catWallOneHit, catWallCal,
	catStart2Raw //catStart2Cal, catStart2Hit,
	// catTBoxChan
    };
    //--------------------------------------------------------------------

    //------standard output for dst production------------------------------------//
    //----------------------------------------------------------------------------//
    Cat_t notPersistentCat[] =
    {
	//catRich, catMdc, catShower, catTof, catTracks,
	catRichRaw,
	//catRichDirClus, catRichHitHdr, catRichHit,
	catRichCal,
	catMdcRaw,
	catMdcCal1,
	catMdcCal2,catMdcClusInf, catMdcHit,
	catMdcSeg, catMdcTrkCand,
	catMdcRawEventHeader,
	catShowerCal, catShowerPID, catShowerHitHdr, catShowerRaw, catShowerGeantWire,
	catShowerRawMatr,
	catShowerTrack,
	catShowerHit,
	catTofRaw,
	catTofHit, catTofCluster,
	catRpcRaw, catRpcCal,
	catRpcHit,
	catRpcCluster,
	catRKTrackB, catSplineTrack,
	catMetaMatch,
	//catParticleCandidate, catParticleEvtInfo,
	catWallRaw, catWallOneHit, catWallCal,
	catStart2Raw //catStart2Cal, catStart2Hit,
	// catTBoxChan
    };
    //--------------------------------------------------------------------

    //####################################################################
    //####################################################################



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

    HDst::setupSpectrometer(beamtime,mdcMods,"rich,mdc,tof,rpc,shower,wall,start,tbox");
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

    HDst::setDataSource(0,"",inFile,refId); // Int_t sourceType,TString inDir,TString inFile,Int_t refId, TString eventbuilder"
    // datasource 0 = hld, 1 = hldgrep 2 = hldremote, 3 root
    // like "lxhadeb02.gsi.de"  needed by dataosoure = 2
    // inputDir needed by dataosoure = 1,2
    // inputFile needed by dataosoure = 1,3



    //------------- Operations on the filenames --------------------------
    HGeantMergeSource* source = new HGeantMergeSource(kTRUE,kTRUE);
    source->addMultFiles(inFileGeant);
    source->setGlobalRefId(refId);
    gHades->setSecondDataSource(source);
    //--------------------------------------------------------------------

    HDst::setupUnpackers(beamtime,"rich,mdc,tof,rpc,shower,tbox,wall,latch,start");
    // beamtime apr12
    // detectors (default)= rich,mdc,tof,rpc,shower,wall,tbox,latch,start

    if(kParamFile) {

	TDatime time;
        TString paramfilename= Form("allParam_APR12_gen9_embedding_gen1_%02i%02i%i",time.GetDay(),time.GetMonth(),time.GetYear());  // without .root


	if(gSystem->AccessPathName(Form("%s.root",paramfilename.Data())) == 0){
	    gSystem->Exec(Form("rm -f %s.root",paramfilename.Data()));
	}
	if(gSystem->AccessPathName(Form("%s.log",paramfilename.Data())) == 0){
	    gSystem->Exec(Form("rm -f %s.log" ,paramfilename.Data()));
	}

	//if (!rtdb->makeParamFile(Form("%s.root",paramfilename.Data()),"apr12","10-APR-2012","10-MAY-2012")) {
	if (!rtdb->makeParamFile(Form("%s.root",paramfilename.Data()),"apr12","04-APR-2012","15-MAY-2012")) {
	    delete gHades;
	    exit(1);
	}
    }

    refId = gHades->getDataSource()->getCurrentRunId();

    //--------------------------------------------------------------------
    // ----------- Build TASK SETS (using H***TaskSet::make) -------------
    HStartTaskSet        *startTaskSet        = new HStartTaskSet();
    HRichTaskSet         *richTaskSet         = new HRichTaskSet();
    HRpcTaskSet          *rpcTaskSet          = new HRpcTaskSet();
    HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
    HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
    HWallTaskSet         *wallTaskSet         = new HWallTaskSet();
    HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet();


    HMdcSetup* mysetup = (HMdcSetup*)rtdb->getContainer("MdcSetup");
    HTrbnetAddressMapping* trbnetmap = (HTrbnetAddressMapping*)rtdb->getContainer("TrbnetAddressMapping");
    rtdb->initContainers(refId);
    mysetup->setStatic();
    trbnetmap->setStatic();

    mysetup->getMdcCommonSet()->setIsSimulation(0);                 // sim=1, real =0
    mysetup->getMdcCommonSet()->setAnalysisLevel(4);                // fit=4
    mysetup->getMdcCalibrater1Set()->setMdcCalibrater1Set(2, 0);    // 1 = NoStartandCal, 2 = StartandCal, 3 = NoStartandNoCal ::  0 = noTimeCut, 1 = TimeCut
    mysetup->getMdcTrackFinderSet()->setIsCoilOff(kFALSE);          // field is on
    mysetup->getMdcTrackFinderSet()->setNLayers(&nLayers[0][0]);
    mysetup->getMdcTrackFinderSet()->setNLevel(nLevel);
    mysetup->getMdc12FitSet()->setMdc12FitSet(2,1,0,kFALSE,kFALSE); // tuned fitter, seg


    HTask *startTasks         = startTaskSet       ->make("real","");
    HTask *richTasks          = richTaskSet        ->make("real","");
    HTask *tofTasks           = tofTaskSet         ->make("real");
    HTask *wallTasks          = wallTaskSet        ->make("real");
    HTask *rpcTasks           = rpcTaskSet         ->make("real");
    HTask *showerTasks        = showerTaskSet      ->make("real","lowshowerefficiency");
    HTask *mdcTasks           = mdcTaskSet         ->make("rtdb","");

    HMdcDigitizer* digi = mdcTaskSet->getDigitizer();

    digi->setDeltaElectronUse(doMDCDeltaElectron,kFALSE,109,-750.,600.,20.);
    digi->setDeltaElectronMinMomCut(2.,2.,4.5,2.,2.,4.5);  // take care of glass mirrors in sec 2+5
    digi->setTimeCutUse(kTRUE);

    HRichDigitizer* richdigi = HRichDigitizer::getDigitizer();
    richdigi->setDeltaElectronUse(doRICHDeltaElectron,kFALSE,109,20.,2.);
    richdigi->setDeltaElectronMinMomCut(0.,0.,0.,0.,0.,0.);

    //----------------SPLINE and RUNGE TACKING----------------------------------------
    HSplineTaskSet         *splineTaskSet       = new HSplineTaskSet("","");
    HTask *splineTasks     = splineTaskSet      ->make("","spline,runge");

    HParticleStart2HitF    *pParticleStart2HitF = new HParticleStart2HitF   ("particlehitf"      ,"particlehitf","");
    HParticleCandFiller    *pParticleCandFiller = new HParticleCandFiller   ("particlecandfiller","particlecandfiller","");
    pParticleCandFiller->setFillMdc(kFALSE); // new : testing close pair

    HParticleTrackCleaner  *pParticleCleaner    = new HParticleTrackCleaner ("particlecleaner"   ,"particlecleaner");
    HParticleVertexFind    *pParticleVertexFind = new HParticleVertexFind   ("particlevertexfind","particlevertexfind",kTRUE);
    HParticleEvtInfoFiller *pParticleEvtInfo    = new HParticleEvtInfoFiller("particleevtinfo"   ,"particleevtinfo",beamtime);
    HParticleBt            *pParticleBt         =  new HParticleBt("RichBackTracking","RichBackTracking",beamtime);


    //----------------------- Quality Assessment -------------------------
    HQAMaker *qaMaker =0;
    if (!outDirQA.IsNull())
    {
	qaMaker = new HQAMaker("qamaker","qamaker");
	qaMaker->setOutputDir((Text_t *)outDirQA.Data());
	//qaMaker->setPSFileName((Text_t *)hldFile.Data());
	qaMaker->setSamplingRate(1);
	qaMaker->setIntervalSize(50);
    }



    //------------------------ Master task set ---------------------------
    HTaskSet *masterTaskSet = gHades->getTaskSet("all");
    masterTaskSet->add(new HSelectEmbedding("select_emb","select_emb"));

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
    masterTaskSet->add(pParticleBt);
    masterTaskSet->add(new HParticleT0Reco("T0","T0",beamtime));

    //if (qaMaker) masterTaskSet->add(qaMaker);

    //--------------------------------------------------------------------
    //  special settings
    HMdcTrackDSet::setTrackParam(beamtime);
    HMdcTrackDSet::setFindOffVertTrkFlag(useOffVertex);
    if(!doMetaMatch)HMdcTrackDSet::setMetaMatchFlag(kFALSE,kFALSE);  //do not user meta match in clusterfinder
    if(doMetaMatchScale)HMetaMatchF2::setScaleCut(metaScale,metaScale,metaScale); // (tof,rpc,shower) increase matching window, but do not change normalization of MetaQA
    if(useWireStat) HMdcCalibrater1::setUseWireStat(kTRUE);

    HStart2HitFSim* starthitf = HStart2HitFSim::getHitFinder() ;
    if(starthitf) starthitf->setResolution(0.06);    // 60 ps start res

    //--------------------------------------------------------------------
    // find best initial params for segment fit (takes long!)
    if(doExtendedFit) {
	HMdcTrackDSet::setCalcInitialValue(1);  // -  1 : for all clusters 2 : for not fitted clusters
    }
    //--------------------------------------------------------------------

    // setup the vertexfinder for embedding
    // standard for using dsts vertex files:
    HMdcVertexFind::setRejectEmbeddedTracks(kTRUE);   // (default: kTRUE)  reject embedded tracks from vertex calculation (needed if no event seq is used)
    HMdcVertexFind::setUseEventSeqNumber   (kTRUE);   // (default: kTRUE)  use the event seq number stored in HGeantKine
    HMdcVertexFind::setSkipNoVertex        (kFALSE);  // (default: kFALSE) kTRUE: skip events where no vertex could be calculated

    HMdcDeDx2Maker::setFillCase(2);                      // 0 (default)=combined, 1=combined+seg, 2=combined+seg+mod
    HStart2Calibrater::setCorrection(doStartCorrection); // kTRUE (default)=  use
    //--------------------------------------------------------------------

    if (!gHades->init()){
	Error("init()","Hades did not initialize ... once again");
	exit(1);
    }

    //--------------------------------------------------------------------
    //----------------- Set not persistent categories --------------------
    HEvent *event = gHades->getCurrentEvent();

    HCategory *cat = ((HCategory *)event->getCategory(catRichCal));
    if(cat) cat->setPersistency(kTRUE);


    if(evtBuild == 1) {
	for(UInt_t i=0;i<sizeof(notPersistentCatAll)/sizeof(Cat_t);i++){
	    cat = ((HCategory *)event->getCategory(notPersistentCatAll[i]));
	    if(cat)cat->setPersistency(kFALSE);
	}
    } else {
	for(UInt_t i=0;i<sizeof(notPersistentCat)/sizeof(Cat_t);i++){
	    cat = ((HCategory *)event->getCategory(notPersistentCat[i]));
	    if(cat)cat->setPersistency(kFALSE);
	}
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

    delete gHades; // comment for the moment
    timer.Stop();

    return 0;

}

#ifndef __CINT__
int main(int argc, char **argv)
{
    TROOT AnalysisDST("AnalysisDST","compiled analysisDST macros");


    TString nevents,startevent;
    switch (argc)
    {
    case 4:
	return analysisDST(TString(argv[1]),TString(argv[2]),TString(argv[3])); // inputfile + inputfile2 + outdir
	break;
    case 5:  // inputfile + inputfile2 + outdir + nevents
	nevents=argv[4];

	return analysisDST(TString(argv[1]),TString(argv[2]),TString(argv[3]),nevents.Atoi());
	break;
	// inputfile + inputfile2 + nevents + startevent
    case 6: // inputfile + outdir + nevents + startevent
	nevents   =argv[4];
	startevent=argv[5];
	return analysisDST(TString(argv[1]),TString(argv[2]),TString(argv[3]),nevents.Atoi(),startevent.Atoi());
	break;
    default:
	cout<<"usage : "<<argv[0]<<" inputfile inputfile2 outputdir [nevents] [startevent]"<<endl;
	return 0;
    }
}
#endif
