
Int_t run_rich_reco(Int_t nEvents=10000000, Int_t startEvt=0)
{
    TString inFile = "/u/slebedev/hades/data/2018.wls.hgeant.e.11.root";
    TString outFile = "/u/slebedev/hades/data/2018.wls.reco.e.11.root";

    string outputDir = "results_wls_scan/single_e/mixwls_recoqa/";
    TString asciiParFile = "../rich_params_mixwls.txt";

    TString rootParFile = "/cvmfs/hades.gsi.de/param/sim/apr12/allParam_APR12_sim_run_12001_gen8_27012016.root";
    TString paramSource = "ascii,root"; // root, ascii, oracle
    TString paramRelease = "APR12SIM_dst_gen8a"; // 27012016

    new Hades;
    TStopwatch timer;
    gHades->setTreeBufferSize(8000);
    gHades->makeCounter(100);

    HRuntimeDb *rtdb = gHades -> getRuntimeDb();
    Int_t refId = 12001; // apr12sim_mediumfieldalign_auau;

     Bool_t doTree = kTRUE;
     Bool_t doRICHDeltaElectron= kFALSE;

    Int_t sourcetype = 3; // root source
    TString dirname = gSystem->DirName(inFile.Data());
    TString filename = gSystem->BaseName(inFile.Data());
    TString beamtime = "apr12";

    HDst::setupSpectrometer(beamtime,NULL,"start,rich,mdc,tof,rpc,shower,wall");
    HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,paramRelease);
    HDst::setDataSource(sourcetype,dirname,filename,refId);

    HTaskSet *masterTaskSet = gHades->getTaskSet("all");

    HRich700Digitizer *digitizer = new HRich700Digitizer();
    digitizer->setStoreOnlyConvertedPhotonTrackIds(false); // true for ideal, fasle for HT
    masterTaskSet->add(digitizer);

    HRich700RingFinderHough *ringFinder = new HRich700RingFinderHough();
     //HRich700RingFinderIdeal *ringFinder = new HRich700RingFinderIdeal();
     masterTaskSet->add(ringFinder);

     HRich700RecoQa *recoQa = new HRich700RecoQa();
     recoQa->setOutputDir(outputDir);
     masterTaskSet->add(recoQa);

    if (!gHades->init()){
    	Error("init()","Hades did not initialize ... once again");
    	exit(1);
    }

    if(doTree){
    	// output file
    	gHades->setOutputFile((Text_t*)outFile.Data(),"RECREATE","Test",2);
    	gHades->makeTree();
    }

    Int_t nProcessed = gHades->eventLoop(nEvents, startEvt);
    cout << "Events processed:" << nProcessed << endl ;
    cout <<"--Input file      : "<<inFile  << endl;
    cout <<"--Output file is  : "<<outFile << endl;
    cout << "Real time:" << timer.RealTime() << endl;;
    cout << "Cpu time:" << timer.CpuTime() << endl;

    delete gHades;
    timer.Stop();

    return 0;
}
