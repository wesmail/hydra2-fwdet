void anaSimRM(char *filenamein, char *filenameout)
{
  Hades* myHades = new Hades();
  gHades->setQuietMode();
  Int_t splitLevel=2;
  gHades->setSplitLevel(splitLevel);

  HSpectrometer* spec=gHades->getSetup();

  // RICH configuration
  HRichDetector rich; 
  Int_t mods[1]={1}; // in RICH there is only 1 module 
  rich.setModules(0,mods);   // 0 sector
  rich.setModules(1,mods);   // 1 sector
  rich.setModules(2,mods);   // 2 sector
  rich.setModules(3,mods);   // 3 sector
  rich.setModules(4,mods);   // 4 sector
  rich.setModules(5,mods);   // 5 sector
  spec->addDetector(&rich);

  // MDC configuration
  HMdcDetector *mdc=new HMdcDetector;

  Int_t mod[4]={1,1,1,1};
  for(Int_t i=0;i<6;i++) mdc->setModules(i, mod);

  spec->addDetector(mdc);

  // RTDB 
  HRuntimeDb* rtdb=gHades->getRuntimeDb();
  HParAsciiFileIo *inp1=new HParAsciiFileIo;
  inp1->open("params/mdcVlad/mdc.par","in");
  rtdb->setFirstInput(inp1);
//    HParAsciiFileIo *inp2=new HParAsciiFileIo;
//    inp2->open("params/mdc/kick_pl_new.par","in");
//    rtdb->setSecondInput(inp2);
  HParRootFileIo *inp2=new HParRootFileIo;
  inp2->open("params/rich/SIMnoise.root", "READ");
  rtdb->setSecondInput(inp2);
  Int_t runId=0;
  rtdb->addRun(runId);
HRichAnalysisPar* pAnalPar = new HRichAnalysisPar;
HRichGeometryPar *pGeomPar =  new HRichGeometryPar;
HRichDigitisationPar *pDigiPar =  new HRichDigitisationPar;

rtdb->addContainer(pAnalPar);
rtdb->addContainer(pGeomPar);
rtdb->addContainer(pDigiPar);

rtdb->initContainers(0); // explicit containers initialisation
rtdb->setContainersStatic(kTRUE);
pAnalPar->printParam();
pDigiPar->printParam();

  gHades->getRuntimeDb()->print();

// Tasks:-------------------------------------------------
  HTaskSet* tasks = new HTaskSet("MDC","Mdc Track Finding Display");

  HRichDigitizer *digr = new HRichDigitizer("richDigit","richDigit");
  HRichAnalysisSim *hitr = new HRichAnalysisSim("richAna","richAna");

  tasks->connect(new HMdcDigitizer("mdcDigit","mdcDigit"));
  tasks->connect((trackFinder=new HMdcTrackFinder("MdcTrackD","MdcTrackD",
                                     kFALSE,0)),"mdcDigit");
  tasks->connect((new HMdcClusterToHit("fhit","fhit")),"MdcTrackD");
//tasks->connect((HTask*)NULL,"fhit");
  tasks->connect(digr,"fhit");
  tasks->connect(hitr,"richDigit");
  tasks->connect((HTask*)NULL,"richAna");
  gHades->getTaskSet("simulation")->connect(tasks);
  gHades->getTaskSet("simulation")->connect((HTask*)NULL,tasks);
  gHades->getTaskSet("simulation")->print();

  
  HRootSource *data=new HRootSource();
  data->addFile("/scratch/jupiter1/SimData/hgeantout/50kCC15All1.root");
  gHades->setDataSource(data);

  if (!gHades->init()) {
    rtdb->closeFirstInput();
    rtdb->closeSecondInput();
    delete gHades;
    return;
  }

// Set output file
  Int_t compLevel = 2;
  gHades->setOutputFile("/scratch/jupiter1/SimData/anaout/50kCC15ana1.root","RECREATE","runs",compLevel);

  gHades->getCurrentEvent()->getCategory(catRichCal)->setPersistency(kFALSE); 

  gHades->makeTree();

  gHades->eventLoop(50000);

  delete gHades;
}
