// File: anaHGKine.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/08/28 18:22:11
//
// this Hydra macro is used to test the HGeantKineAna task

{

Hades *gHades=new Hades;


HRootSource *source = new HRootSource;
//source->setDirectory("/scratch/jupiter1/SimData/anaout/");
source->setDirectory("/scratch/rhea/data/SimData/anaout/");
// SIM DATA
   source->addFile("50kCC15ana1.root");
   source->addFile("50kCC15ana2.root");
   source->addFile("50kCC15ana3.root");
   source->addFile("50kCC15ana4.root");
   source->addFile("50kCC15ana5.root");
   source->addFile("50kCC15ana6.root");
   source->addFile("50kCC15ana7.root");
   source->addFile("50kCC15ana8.root");
   source->addFile("50kCC15ana9.root");
   source->addFile("50kCC15ana10.root");

gHades->setDataSource(source);


source->disableCategory(catRichHitHdr);
source->disableCategory(catMdcHit);
source->disableCategory(catMdcCal1);
source->disableCategory(catMdcCal2);
source->disableCategory(catSimul);
//source->disableCategory(catGeantHeader);
source->disableCategory(catRichHit);
 source->disableCategory(catMdcSeg);
source->disableCategory(catRichTrack);
source->disableCategory(catMdcGeantRaw);
source->disableCategory(catRichGeantRaw);
source->disableCategory(catShowerGeantRaw);
source->disableCategory(catTofGeantRaw);
source->disableCategory(catRichGeantRaw+1);
source->disableCategory(catRichGeantRaw+1);
//source->disableCategory();
//source->disableCategory();


// the following setup is necessary, otherwise it crashes !
//  Int_t sectors[]     = {1,1,1,1,1,1}; // Bitmap of active sectors
//  Int_t richMods[]    = {1};           // Bitmap of active Rich modules

//  HSpectrometer* spec = gHades->getSetup();

//  HRuntimeDb* rtdb=gHades->getRuntimeDb();

//  HRichDetector *rich = new HRichDetector; 
//  spec->addDetector(rich);
//  for (Int_t is=0; is<6; is++) {
//       rich->setModules(is,richMods);
//  }

//  HMdcDetector *mdc=new HMdcDetector;

//    Int_t mod[4]={1,1,1,1};
//    for(Int_t i=0;i<6;i++) mdc->setModules(i, mod);

//    spec->addDetector(mdc);

//    // RTDB 
//    HRuntimeDb* rtdb=gHades->getRuntimeDb();
//    HParAsciiFileIo *inp1=new HParAsciiFileIo;
//    inp1->open("params/mdcVlad/mdc.par","in");
//    rtdb->setFirstInput(inp1);
//   Int_t runId=0;
//    rtdb->addRun(runId);

  HTaskSet *richTasks = new HTaskSet("Rich","List of Rich tasks");
  richTasks->connect( new HGeantKineAna("HGeantKineAna","HGeantKineAna","/scratch/rhea/data/SimData/anaout/testHGKineHistrheamomthetawindow1.root") );
richTasks->connect((HTask*)NULL,"HGeantKineAna");

gHades->getTaskSet("simulation")->connect(richTasks);
gHades->getTaskSet("simulation")->connect((HTask*)NULL,richTasks);
gHades->getTaskSet("simulation")->print();

if(!gHades->init()) {
    printf("\nError in Hades::init() \n"); 
    rtdb->closeFirstInput();
    return kFALSE;
}

//  gHades->getCurrentEvent()->getCategory(catTofHit)->setPersistency(kFALSE);
//  gHades->getCurrentEvent()->getCategory(catMdcSeg)->setPersistency(kFALSE);
//  gHades->getCurrentEvent()->getCategory(catShowerHitTof)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catSimul)->setPersistency(kFALSE);
TStopwatch timer;
//gHades->setOutputFile("/scratch/jupiter1/SimData/anaout/testHGKineData.root","RECREATE","Test",2);
//gHades->makeTree();
Int_t nEvents = 500000; 
timer.Start();
gHades->eventLoop(nEvents);

  timer.Stop();
 // Print benchmarks
    printf("\nReal time = %f\n",timer.RealTime());
    printf("Cpu time = %f\n",timer.CpuTime());
    printf("Speed = %f evs/cpusec\n",(nEvents)/timer.CpuTime());
    
delete gHades;

}




