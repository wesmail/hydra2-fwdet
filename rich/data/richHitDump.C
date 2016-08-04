// File: richHistoFiller.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/08/20 20:17:28
//
// this Hydra macro shows how to use HRootSource for analysis

{

Hades *gHades=new Hades;

HRootSource *source = new HRootSource;
source->setDirectory("/scratch/jupiter1/SimData/anaout/");

// SIM DATA
 source->addFile("50kCC15ana1.root");

gHades->setDataSource(source);


//source->disableCategory(catRichPID);
//this category could not be found ! why ?
/*
Error: No symbol catRichPID in current scope  FILE:foo.C LINE:35
*/

Int_t sectors[]     = {1,1,1,1,1,1}; // Bitmap of active sectors
Int_t richMods[]    = {1};           // Bitmap of active Rich modules
HSpectrometer* spec = gHades->getSetup();

HRichDetector *rich = new HRichDetector; 
spec->addDetector(rich);
for (Int_t is=0; is<6; is++) {
     rich->setModules(is,richMods);
}



  HTaskSet *richTasks = new HTaskSet("Rich","List of Rich tasks");
  richTasks->connect(new HRichDumpHitSim("rich.hit.dump","Rich Hit Dumper"));
  gHades->getTaskSet("simulation")->connect(richTasks);


if(!gHades->init()) {
    printf("\nError in Hades::init() \n"); 
    rtdb->closeFirstInput();
    return kFALSE;
}

TStopwatch timer;

Int_t nEvents = 100; 
timer.Start();
gHades->eventLoop(nEvents);

  timer.Stop();
 // Print benchmarks
    printf("\nReal time = %f\n",timer.RealTime());
    printf("Cpu time = %f\n",timer.CpuTime());
    printf("Speed = %f evs/sec\n",(nEvents)/timer.CpuTime());
    
delete gHades;

/*
Number of events processed:  10000 

Real time = 16.800000
Cpu time = 15.470000
Speed = 646.412411 evs/sec
*/
}



