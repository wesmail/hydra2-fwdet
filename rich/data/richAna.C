{
  new Hades();
  TStopwatch timer; //For benchmarking
  event = new HRecEvent();
  gHades->setEvent(event);

  Int_t compLevel=2;
  Int_t splitLevel=2;
  gHades->setSplitLevel(splitLevel);

Int_t refrunID =  944433336;//run id in the param ROOT file
Int_t runID = 990317026;//xx01140020346.hld
Int_t runID = 990385119;//xx01140205839.hld
  HSpectrometer* spec=gHades->getSetup(); 

// RICH configuration
  HRichDetector *rich = new HRichDetector;
  Int_t richMods[1]={1}; // in RICH there is only 1 module
  rich->setModules(0,richMods);   // 0 sector
  rich->setModules(1,richMods);   // 1 sector
  rich->setModules(2,richMods);   // 2 sector
  rich->setModules(3,richMods);   // 3 sector
  rich->setModules(4,richMods);   // 4 sector
  rich->setModules(5,richMods);   // 5 sector
  spec->addDetector(rich);


// Set the runtime database;
  HRuntimeDb *rtdb=gHades->getRuntimeDb();

//cout<<" before first input "<<endl;
  HParRootFileIo *inp2=new HParRootFileIo;
    inp2->open("params/rich/may01/RICH_ALL_2001-05-20_2.root", "READ");
    rtdb->setSecondInput(inp2);
//  HParHadAsciiFileIo input1; // 1st ASCII input
//  input1.openfilelist("params/rich/rich_key_filelist.txt");
//  rtdb->setFirstInput(&input1);

//  HParHadAsciiFileIo input2("RAW"); // 2nd ASCII input
//  input2.openfilelist("params/rich/rich_raw_filelist2104.txt");
//  rtdb->setSecondInput(&input2);



rtdb->addContainer(new HRichAnalysisPar);
rtdb->addContainer(new HRichCalPar);
rtdb->addContainer(new HRichGeometryPar);
rtdb->addContainer(new HRichMappingPar);
rtdb->addContainer(new HRichDigitisationPar);

rtdb->initContainers(runID);

//rtdb->getCurrentRun()->print();
rtdb->setContainersStatic(kTRUE); // avoid double init

//pAnalPar->printParam();
// Set the reconstruction tree 

HTaskSet *richTasks = new HTaskSet("Rich","Rich");
  richTasks->connect(new HRichAnalysis("rich.anal","Rich ring finder"));

richTasks->connect((HTask*)NULL, "rich.anal");

  gHades->getTaskSet("real")->connect(richTasks);
gHades->getTaskSet("real")->connect(NULL, richTasks); 
gHades->getTaskSet("real")->connect((HTask*)NULL,richTasks);
gHades->getTaskSet("real")->print();
// Set the split level
gHades->setSplitLevel(2);  

// Set the data source (here from hld file)
  HldFileSource *source=new HldFileSource;



source->addUnpacker(new HRichUnpackerCal99(100));
source->addUnpacker(new HRichUnpackerCal99(101));
source->addUnpacker(new HRichUnpackerCal99(102));
source->addUnpacker(new HRichUnpackerCal99(103));
source->addUnpacker(new HRichUnpackerCal99(104));
source->addUnpacker(new HRichUnpackerCal99(105));
source->addUnpacker(new HRichUnpackerCal99(106));
source->addUnpacker(new HRichUnpackerCal99(107));
source->addUnpacker(new HRichUnpackerCal99(110));
source->addUnpacker(new HRichUnpackerCal99(111));


//source->setDirectory("/scratch/titan/hld"); 

//source->addFile("xx00319054346.hld");
//source->setDirectory("/scratch/mimas/data/hld");
//source->addFile("xx00324191145.hld");
//source->setDirectory("/scratch/halo/hld/apr01");
source->setDirectory("/scratch/halo/hld/may01");
//source->addFile("xx01114034847.hld");
source->addFile("xx01140205839.hld");
gHades->setDataSource(source);

// Initialization
  printf("gHades->init()\n");
  if (!gHades->init()) {
     printf("Error\n");
     return -1;
  }

gHades->setOutputFile("test.root","RECREATE","Test",2);
  gHades->makeTree();


Int_t nEvents = 100;     // number of events to be processed

printf("\n\n ======== Event loop started ========\n");
timer.Start();
gHades->eventLoop(nEvents);

gHades->getRuntimeDb()->closeFirstInput();
//    gHades->getRuntimeDb()->closeSecondInput();


//    // Delete the gHades objects. The data written to file.
delete gHades;

//    // End timer
timer.Stop();


//    // Print benchmarks
//    printf("\nReal time=%f\n",timer.RealTime());
printf("Cpu time=%f\n",timer.CpuTime());
printf("Speed= %f evs/sec\n",nEvents/timer.CpuTime());
}








