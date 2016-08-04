// @(#)$Id: ana.C,v 1.1 2002-12-01 02:12:44 jaskula Exp $
// How to use HDummyRec defined in dummyRec.cc file

Bool_t ana(Int_t nEvents = 0,Int_t startEvt = 0)
{
//-----------------------------------------------------------------

TString inDir   = "/net/disk104/hydra_v606/files/";
char *aFiles[] = {  "./geant_C2C_field_600kfull_dst.root",
                    "./geant_C2C_field_600kfull_dst_1.root",
                    "./geant_C2C_field_600kfull_dst_2.root"
                 };


//TString outFile = "./results.root";   // uncoment to use the output Tree
TString outFile = "";

TString sTaskListName = "simulation";

//-----------------------------------------------------------------

    gSystem->SetIncludePath(" $HYDRA_INCLUDES ");
    gROOT->LoadMacro("dummyRec.cc+");

//-----------------------------------------------------------------

  gROOT->Reset();
  TStopwatch timer;
  Int_t evN=0;

  Hades::instance();
  HSUErrorHandler::setHandler();

  printf("Setting configuration...\n");

//---------------  Set HDummyRec (connected later to taks list)

HReconstructor *pRec = new HDummyRec("./resNtup.root");

//---------------  Set batch (needed for TCanvas's) ------------------

  gROOT->SetBatch();

// ------------ Set input data file: ------------------ --------------

  HRootSource *source = new HRootSource(kFALSE);
  source->setDirectory(inDir.Data());
  for(Int_t i = 0; i < sizeof(aFiles) / sizeof(char *); i++)
      source->addFile(aFiles[i]);

  gHades->setDataSource(source);

// ----------Add detectors to the setup: NO NEED TO CHANGE -----------

// no detectors needed on this level

//------------------------ Add PID Tasks -----------------------------

  gHades->getTaskSet(sTaskListName.Data())->add(pRec);

//------------------------ Initialization ----------------------------

  printf("gHades->init()\n");

  gHades->makeCounter(1000);
  if(!gHades->init())
     printf("Error\n");

//-------------------------- Set output ------------------------------

  if(outFile.Length() > 0)
  {
      gHades->setOutputFile(outFile.Data(),"RECREATE","Test",2);
      gHades->makeTree();
  }

//--------------------------------------------------------------------

  printf("Processing events...\n");
  timer.Reset();
  timer.Start();
  if (nEvents<1) {
    evN=gHades->eventLoop();
  } else {
    evN=gHades->eventLoop(nEvents,startEvt);
  }

  gHades->getTaskSet("simulation")->printTimer();

  delete gHades;
  timer.Stop();

  printf("------------------------------------------------------\n");
  printf("Events processed: %i\n",evN);
  printf("Real time: %f\n",timer.RealTime());
  printf("Cpu time: %f\n",timer.CpuTime());
  if (evN) printf("Performance: %f s/ev\n",timer.CpuTime()/evN);

  return kTRUE;
}

