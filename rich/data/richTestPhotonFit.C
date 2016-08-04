// File: richTestFit.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 01/07/16 16:11:35
//
// this Hydra macro is used to test the HRichChernovRingFitter task

{

Hades *gHades=new Hades;


HRootSource *source = new HRootSource;
source->setDirectory("./");
source->addFile("xx01112020509_DST.root");
source->addFile("xx01112021206_DST.root");
source->addFile("xx01112025319_DST.root");
source->addFile("xx01112040044_DST.root");
source->addFile("xx01114023224_DST.root");
source->addFile("xx01114024826_DST.root");
source->addFile("xx01114031107_DST.root");
source->addFile("xx01114033419_DST.root");
source->addFile("xx01114034847_DST.root");
source->addFile("xx01114041928_DST.root");
source->addFile("xx01114043658_DST.root");
source->addFile("xx01114045906_DST.root");
source->addFile("xx01114052201_DST.root");
source->addFile("xx01114053832_DST.root");
source->addFile("xx01114055246_DST.root");

gHades->setDataSource(source);
//source->disableCategory(catRichCal);
source->disableCategory(catRichPID);
source->disableCategory(catRichHitHdr);
source->disableCategory(catShowerHit);
source->disableCategory(catShowerCal);
source->disableCategory(catShowerHitHdr);
source->disableCategory(catShowerPID);
source->disableCategory(catTofHit);
source->disableCategory(catTofinoCal);
source->disableCategory(catMdcHit);
source->disableCategory(catMdcCal1);
source->disableCategory(catMdcCal2);
source->disableCategory(catMdcSeg);//same problem as catRichPID
source->disableCategory(catShowerHitTof);
source->disableCategory(catStartCal);
source->disableCategory(catStartHit);
source->disableCategory(catKickTrack);



Int_t sectors[]     = {1,1,1,1,1,1}; // Bitmap of active sectors
Int_t richMods[]    = {1};           // Bitmap of active Rich modules
HSpectrometer* spec = gHades->getSetup();
HRuntimeDb* rtdb=gHades->getRuntimeDb();
HRichDetector *rich = new HRichDetector; 
spec->addDetector(rich);
for (Int_t is=0; is<6; is++) {
     rich->setModules(is,richMods);
}
HParRootFileIo *input=new HParRootFileIo;
 input->open("/home/teberl/hades/hydra/v530muc/macros/All/params_day324.root","READ");
 rtdb->setFirstInput(input);


  HTaskSet *richTasks = new HTaskSet("Rich","List of Rich tasks");
  HRichPhotonFinder *photonfinder = new HRichPhotonFinder("Rich Photon Finder","Rich Photon Finder");
  richTasks->connect(photonfinder);
HRichChernovRingFitter *fitter = new HRichChernovRingFitter("Rich Ring Fitter","Rich Ring Fitter",kTRUE);
  richTasks->connect(fitter,"Rich Photon Finder");
  richTasks->connect((HTask*)NULL,"Rich Ring Fitter");
  gHades->getTaskSet("real")->connect(richTasks);


if(!gHades->init()) {
    printf("\nError in Hades::init() \n"); 
    rtdb->closeFirstInput();
    return kFALSE;
}
gHades->getCurrentEvent()->getCategory(catRichCal)->setPersistency(kFALSE);
TStopwatch timer;
gHades->setOutputFile("photon.root","RECREATE","Test",2);
gHades->makeTree();
Int_t nEvents = 1000000; 
timer.Start();
gHades->eventLoop(nEvents);

  timer.Stop();
 // Print benchmarks
    printf("\nReal time = %f\n",timer.RealTime());
    printf("Cpu time = %f\n",timer.CpuTime());
    printf("Speed = %f evs/sec\n",(nEvents)/timer.CpuTime());
    
delete gHades;

}



