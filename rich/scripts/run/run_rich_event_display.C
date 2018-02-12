Int_t run_rich_event_display(Int_t nEvents=5, Int_t startEvt=0)
{
    TString inFile = "/u/slebedev/hades/data/hgeant.e1.root";
    TString outFile = "/u/slebedev/hades/data/hgeant.eventdisplay.e1.root";

    string outputDir = "results_wls_scan/results_test/";
    TString asciiParFile = "../rich_params_wls.txt";

    TString rootParFile = "/cvmfs/hades.gsi.de/param/sim/apr12/allParam_APR12_sim_run_12001_gen8_27012016.root";
    TString paramSource = "ascii,root"; // root, ascii, oracle
    TString paramRelease = "APR12SIM_dst_gen8a"; // 27012016

    new Hades;
    TStopwatch timer;
    gHades->setTreeBufferSize(8000);

    HRuntimeDb *rtdb = gHades -> getRuntimeDb();
    Int_t refId = 12001; // apr12sim_mediumfieldalign_auau;

     Bool_t doTree = kTRUE;
     Bool_t doRICHDeltaElectron= kFALSE;

    Int_t sourcetype = 3; // root source
    TString dirname   = gSystem->DirName(inFile.Data());
    TString filename  = gSystem->BaseName(inFile.Data());
    TString beamtime="apr12";

    HDst::setupSpectrometer(beamtime,NULL,"start,rich,mdc,tof,rpc,shower,wall");
    HDst::setupParameterSources(paramSource,asciiParFile,rootParFile,paramRelease);
    HDst::setDataSource(sourcetype,dirname,filename,refId);


    HTaskSet *masterTaskSet = gHades->getTaskSet("all");

    HRich700Digitizer *digitizer = new HRich700Digitizer();
    digitizer->setStoreOnlyConvertedPhotonTrackIds(true); // true for ideal, fasle for HT
    // crosstalk and nof noise hits should be set in parameter file
    //digitizer->setCrossTalkProbability(0.04);
    //digitizer->setNofNoiseHits(0);
    masterTaskSet->add(digitizer);

    //HRich700RingFinderHough *ringFinder = new HRich700RingFinderHough();
    HRich700RingFinderIdeal *ringFinder = new HRich700RingFinderIdeal();
    masterTaskSet->add(ringFinder);

    HRich700EventDisplay *eventDisplay = new HRich700EventDisplay();
    eventDisplay->setOutputDir(outputDir);
    eventDisplay->setNofEventsToDraw(nEvents);
    masterTaskSet->add(eventDisplay);


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





// {
//
//     Hades *gHades=new Hades;
//   HSpectrometer* spec = gHades->getSetup();
//   spec->addDetector(new HRichDetector);
//   Int_t richMods  [1]  = {1};
//   for (Int_t is=0; is<6; is++) {
//     spec->getDetector("Rich")  ->setModules(is,richMods);
//   }
//
//   HRootSource *source = new HRootSource;
//   source->setDirectory("/u/slebedev/hades/data/");
//   source->addFile("hgeant.e1.root");
//
//   gHades->setDataSource(source);
//
//   HRich700Digitizer *digitizer = new HRich700Digitizer();
//   digitizer->setStoreOnlyConvertedPhotonTrackIds(false); // true for ideal, fasle for HT
//   digitizer->setCrossTalkProbability(0.02);
//   digitizer->setNofNoiseHits(0);
//   gHades->getTaskSet("simulation")->add(digitizer);
//
//   //HRich700RingFinderIdeal *ringFinder = new HRich700RingFinderIdeal();
//   HRich700RingFinderHough *ringFinder = new HRich700RingFinderHough();
//   gHades->getTaskSet("simulation")->add(ringFinder);
//
//   HRich700EventDisplay *eventDisplay = new HRich700EventDisplay();
//   eventDisplay->setOutputDir("results_eventdisplay_2000");
//   eventDisplay->setNofEventsToDraw(5);
//   gHades->getTaskSet("simulation")->add(eventDisplay);
//
//   gHades->getTaskSet("simulation")->print();
//
//   if(!gHades->init()) {
//       printf("\nError in Hades::init() \n");
//       rtdb->closeFirstInput();
//       return kFALSE;
//   }
//
// //  gHades->getCurrentEvent()->getCategory(catTofHit)->setPersistency(kFALSE);
// //  gHades->getCurrentEvent()->getCategory(catMdcSeg)->setPersistency(kFALSE);
// //  gHades->getCurrentEvent()->getCategory(catShowerHitTof)->setPersistency(kFALSE);
// //gHades->getCurrentEvent()->getCategory(catSimul)->setPersistency(kFALSE);
// TStopwatch timer;
// //gHades->setOutputFile("/scratch/jupiter1/SimData/anaout/testHGKineData.root","RECREATE","Test",2);
// //gHades->makeTree();
// Int_t nEvents = 100;
// timer.Start();
// gHades->eventLoop(nEvents);
//
//   timer.Stop();
//  // Print benchmarks
//     printf("\nReal time = %f\n",timer.RealTime());
//     printf("Cpu time = %f\n",timer.CpuTime());
//     printf("Speed = %f evs/cpusec\n",(nEvents)/timer.CpuTime());
//
// delete gHades;
//
// }
