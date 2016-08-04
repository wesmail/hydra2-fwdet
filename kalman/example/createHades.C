#ifndef __CREATEHADES__
#define __CREATEHADES__

// from ROOT
#include "TSystem.h"

// from hydra
#include "hades.h"

#include "hmdcsizescells.h"
#include "hmdcidealtracking.h"

#include "hparora2io.h"
#include "hparrootfileio.h"
#include "hparasciifileio.h"

#include "hrootsource.h"
#include "hruntimedb.h"

#include "hmagnetpar.h"
#include "hmdccal2par.h"
#include "hmdccal2parsim.h"

#include "hmdcsetup.h"

#include "hspectrometer.h"
#include "hrichdetector.h"
#include "hmdcdetector.h"
#include "htofdetector.h"
#include "hshowerdetector.h"
#include "hrpcdetector.h"

#include "hmdctrackdset.h"
#include "htaskset.h"
#include "htask.h"
#include "hmdctrackfinder.h"

#include "hmdcdigitizer.h"

#include "hmdclayercorrpar.h"
#include "hrichtaskset.h"
#include "hmdctaskset.h"
#include "htoftaskset.h"
#include "hshowertaskset.h"
#include "hrpctaskset.h"
#include "hsplinetaskset.h"
#include "hkalfiltertask.h"

#include "hparticlecandfiller.h"
#include "hparticletrackcleaner.h"
#include "hparticleevtinfofiller.h"

#include "hrktrackBF2.h"

#include "hedhelpers.h"

#include <iostream>
using namespace std;


Bool_t initDetectorSetup() {
    Int_t richMods[]    = {1};

    Int_t mdcMods[6][4] =
    { {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1},
    {1,1,1,1} };

    Int_t rpcMods[]     = {1};
    Int_t tofMods[22]   = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    Int_t showerMods[3] = {1,2,3};
    HSpectrometer* spec = gHades->getSetup();


    spec->addDetector(new HRichDetector);
    spec->addDetector(new HMdcDetector);
    spec->addDetector(new HRpcDetector);
    spec->addDetector(new HTofDetector);
    spec->addDetector(new HShowerDetector);

    // ----- Set active modules for each detector: NO NEED TO CHANGE -----

    for (Int_t is = 0; is < 6; is ++) {
        spec->getDetector("Rpc")->setModules(is,rpcMods);
        spec->getDetector("Shower")->setModules(is,showerMods);
        spec->getDetector("Tof")->setModules(is,tofMods);
        spec->getDetector("Mdc")->setModules(is,mdcMods[is]);
        spec->getDetector("Rich")->setModules(is,richMods);
    }
    return kTRUE;
}



Bool_t setParamSource(TString paramSource, TString asciiParFile, TString rootParFile,
                     TString oraDate) {

    HRuntimeDb *rtdb = gHades->getRuntimeDb();

    if(paramSource.Contains("ROOT")) {
        HParRootFileIo *input1=new HParRootFileIo;
        input1->open(((Char_t *)rootParFile.Data()),"READ");

        if (paramSource.Contains("ASCII")) {
            rtdb->setSecondInput(input1);
        } else {
            rtdb->setFirstInput(input1);
        }
    }


    if(paramSource.Contains("ASCII")) {
        if(gSystem->AccessPathName(asciiParFile.Data())) {
            cout<<"ASCII file "<<asciiParFile.Data()<<" not found."<<endl;
        } else {
            HParAsciiFileIo *input2 = new HParAsciiFileIo();
            input2->open((Text_t*)asciiParFile.Data(),"in");
            rtdb->setFirstInput(input2);
        }
    }

    // ----- SECOND PARAM INPUT FOR PID

    if (paramSource.Contains("ORACLE")) {
        HParOra2Io *ora = new HParOra2Io;
        ora->open();
        ora->setHistoryDate(oraDate.Data());
        rtdb->setSecondInput(ora);
    }

    return kTRUE;
}



Bool_t buildTaskSets(Int_t refId, Double_t errX=0.2, Double_t errY=0.1,
		     Bool_t bIdealTracking=kFALSE, Bool_t bFitWireHits=kFALSE,
		     Bool_t bDoDaf=kFALSE, Bool_t bCompHits=kFALSE) {

    HRuntimeDb *rtdb = gHades->getRuntimeDb();

    HRichTaskSet         *richTaskSet         = new HRichTaskSet();
    HRpcTaskSet          *rpcTaskSet          = new HRpcTaskSet();
    HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
    HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
    HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet();
    mdcTaskSet->setVersionDeDx(1); // 0 = no dEdx, 1 = HMdcDeDx2


    HTask *richTasks          = richTaskSet        ->make("simulation","noiseon");
    HTask *tofTasks           = tofTaskSet         ->make("simulation");
    HTask *rpcTasks           = rpcTaskSet         ->make("simulation");
    HTask *showerTasks        = showerTaskSet      ->make("","simulation,magneton,hit");


    //  ----------- MDC ------------------------------------------------------
    //HMdcCal2ParSim* cal2 = (HMdcCal2ParSim*)rtdb->getContainer("MdcCal2ParSim");
    //cal2->setConstantVD(40, 40 , 40 , 40, 2, 2, 2, 2);

    HMdcSetup* setup = (HMdcSetup*)rtdb->getContainer("MdcSetup");
    rtdb->initContainers(refId);
    setup->setStatic();
    if(bIdealTracking) setup->getMdcCommonSet()->setAnalysisLevel(1);


    HTask *mdcTasks      = mdcTaskSet         ->make("rtdb","");
    //mdcTaskSet->getDigitizer()->setWireOffsetUse(kFALSE);
    //mdcTaskSet->getDigitizer()->setTofUse(kFALSE);
    //mdcTaskSet->getDigitizer()->setErrorUse(kFALSE);

    HMdcIdealTracking *mdcidealtrackTasks = new HMdcIdealTracking("idealtrack","idealtrack");
    mdcidealtrackTasks->setResolutionX(errX, errX, errX, errX);
    mdcidealtrackTasks->setResolutionY(errY, errY, errY, errY);
    //mdcidealtrackTasks->fillParallelCategories();

    //----------------SPLINE and RUNGE TRACKING----------------------------------------
    HSplineTaskSet         *splineTaskSet       = new HSplineTaskSet("","");
    HTask *splineTasks=splineTaskSet->make("","spline,runge");

    //-----------------------------------------------------------------------
    //HParticleCandFiller  *pParticleCandFiller = new HParticleCandFiller("particlecandfiller","particlecandfiller","debug,goodseg0,goodseg1,goodmeta,goodrk");
    HParticleCandFiller    *pParticleCandFiller = new HParticleCandFiller  ("particlecandfiller","particlecandfiller","");
    HParticleTrackCleaner  *pParticleCleaner    = new HParticleTrackCleaner("particlecleaner"   ,"particlecleaner");
    HParticleEvtInfoFiller *pParticleEvtInfo    = new HParticleEvtInfoFiller("particleevtinfo"  ,"particleevtinfo");

    //------------------------ Master task set --------------------------

    HTaskSet *masterTaskSet = gHades->getTaskSet("simulation");

    masterTaskSet->add(tofTasks);

    masterTaskSet->add(rpcTasks);

    masterTaskSet->add(richTasks);
    masterTaskSet->add(showerTasks);

    masterTaskSet->add(mdcTasks);

    /*
    masterTaskSet->connect(new HMdcTrackFinder("MdcTrackD","MdcTrackD"), mdcTasks);

    if(HMdcTrackDSet::getMdcTrackFinder()) {
        HMdcTrackDSet::getMdcTrackFinder()->setMetaMatchFlag(kFALSE);
    } else {
        Warning("buildTaskSets()", "No MdcTrackFinder object found.");
        exit(1);
    }
    */

    if(bIdealTracking) masterTaskSet->add(mdcidealtrackTasks);

    masterTaskSet->add(splineTasks);

    masterTaskSet->add(pParticleCandFiller);
    masterTaskSet->add(pParticleCleaner);
    masterTaskSet->add(pParticleEvtInfo);

    HKalFilterTask *kalFilter = new HKalFilterTask(kTRUE, refId, bFitWireHits, bDoDaf, bCompHits);
    masterTaskSet->add(kalFilter);

    masterTaskSet->isTimed(kTRUE);

    return kTRUE;
}



Bool_t createHades(const TString &inputDir, const TObjArray &inputFiles, Int_t refId,
                   TString paramSource, TString oraDate,
		   Double_t errX=0.2, Double_t errY=0.1, Bool_t bUseAuAuPars=kFALSE,
                   Bool_t bIdealTracking=kFALSE, Bool_t bFitWireHits=kFALSE,
                   Bool_t bDoDaf=kFALSE, Bool_t bCompHits=kFALSE) {

    Hades *myHades = new Hades();
    myHades->setTreeBufferSize(8000);
    myHades->setQuietMode(2);
    myHades->makeCounter(1);

    HRuntimeDb *rtdb = myHades->getRuntimeDb();



    // ------------ Set input data file: NO NEED TO CHANGE  --------------
    HRootSource *source = new HRootSource();
    source->replaceHeaderVersion(0,kTRUE);
    source->setDirectory(((Text_t *)inputDir.Data()));
    for(Int_t i = 0; i < inputFiles.GetEntries(); i++) {
        TString inputFile = ((TObjString*)inputFiles.At(i))->GetString();
        if(gSystem->AccessPathName(gSystem->ConcatFileName(inputDir.Data(), inputFile.Data()))) {
            Error("createHades()", "Kalman Filter : Input file not found ! #######################");
            exit(1);
        }
        source->addFile((Text_t *)inputFile.Data());
    }
    source->setGlobalRefId(refId);
    myHades->setDataSource(source);
    //--------------------------------------------------------------------



    // ----------Add detectors to the setup: NO NEED TO CHANGE -----------
    //----------------- Detector setup configuration ---------------------
    initDetectorSetup();
    //--------------------------------------------------------------------



    //--------------------------------------------------------------------
    //  PARAMETER SOURCES

    //TString asciiParFile1 = "/misc/kempter/projects/eventdisplay/tatyana/missing_params_sim_14042011.txt";
    TString asciiParFile1 = "/u/ekrebs/workspace/kalman/mdcLayerGeomPar_10000.txt";
    TString rootParFile1  = "";
    setParamSource(paramSource,asciiParFile1, rootParFile1, oraDate);

    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // ALIGNMENT OF WIRE PLANES
    HMdcLayerCorrPar* fMdcLayerCorrPar =  (HMdcLayerCorrPar*)rtdb->getContainer("MdcLayerCorrPar");
    //  fMdcLayerCorrPar->setDefaultPar();
    fMdcLayerCorrPar->setStatic();
    //--------------------------------------------------------------------



    // ----------- Build TASK SETS (using H***TaskSet::make) -------------
    buildTaskSets(refId, errX, errY, bIdealTracking, bFitWireHits, bDoDaf, bCompHits);
    //--------------------------------------------------------------------


    Bool_t hasTracking = kFALSE;

    HTaskSet* set = (HTaskSet*) gHades->getTaskSet("simulation")->getSetOfTask()->FindObject("Mdc");
    if(set && set->getSetOfTask()->FindObject("mdc.hitf")) hasTracking = kTRUE;

    if(!hasTracking){
        set = (HTaskSet*) gHades->getTaskSet("real")->getSetOfTask()->FindObject("Mdc");
        if(set && set->getSetOfTask()->FindObject("mdc.hitf")) hasTracking = kTRUE;
    }

    if(hasTracking){
        cout<<"Kalman Filter : TRACKING IS RUNNING ########################"<<endl;
    } else {
        cout<<"Kalman Filter : NO TRACKING IS RUNNING #####################"<<endl;
    }

    //HMdcTrackGFieldPar* magnetpar=(HMdcTrackGFieldPar*)rtdb->getContainer("MdcTrackGFieldPar");
    // 1=old,
    // 2=tricubic exact,
    // 3=tricubic smooth,
    // 4=trilinear (should be identical to old)
    //magnetpar->getPointer()->setInterpolation(1);


    if(!myHades->init()) {
        Error("initHades()", "Hades Init() failed!");
        delete myHades;
        return kFALSE;
    }

    //--------------------------------------------------------------------
    // if tracking is running set parameters
    if(hasTracking && bUseAuAuPars) {                      // is Tracking running ?
        cout<<"KALMAN FILTER : SETTING TRACKING PARAMETERS FOR AU+AU @ 1.5 AGEV ############"<<endl;
        HMdcTrackDSet::setDefParAu15Au(); // For Au+Au tracking
    }
    //--------------------------------------------------------------------

    return kTRUE;
}

#endif
