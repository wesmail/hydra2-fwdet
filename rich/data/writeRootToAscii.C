{

  // Macro to test runtime-database

  gROOT->Reset();

  Hades* myHades = new Hades;
  event = new HRecEvent();
  gHades->setEvent(event);

  TStopwatch timer; // for benchmarking


  // Create detector and its setup

  HRichDetector rich; 
  Int_t mods[1]={1}; // in RICH there is only 1 module 
  rich.setModules(0,mods);   // 0 sector
  rich.setModules(1,mods);   // 1 sector
  rich.setModules(2,mods);   // 2 sector
  rich.setModules(3,mods);   // 3 sector
  rich.setModules(4,mods);   // 4 sector
  rich.setModules(5,mods);   // 5 sector
  gHades->getSetup()->addDetector(&rich);


  // Activate runtime database

  HRuntimeDb* rtdb = gHades->getRuntimeDb();
  rtdb->addRun(944433336);

//  HParHadAsciiFileIo input("RAW"); // 1st ASCII input
//  input.open("calpar.txt", "READ");
//  rtdb->setFirstInput(&input);

  HParRootFileIo input;
  input.open("params/rich/may01/richAll_2005_cal2.root");
  rtdb->setFirstInput(&input);

//  HParHadAsciiFileIo input2; // 2nd ASCII input
//  input2.openfilelist("richfilelist.txt");
//  rtdb->setSecondInput(&input2);

  HParHadAsciiFileIo output;
  output.open("cal_2001-05-20_2.txt", "RECREATE");
  rtdb->setOutput(&output);

//  HParRootFileIo output;
//  output.open("out.root", "RECREATE");
//  rtdb->setOutput(&output);

 HRichCalPar* pCalPar = new HRichCalPar;
//HRichAnalysisPar* pAnalPar = new HRichAnalysisPar;
//HRichGeometryPar *pGeomPar =  new HRichGeometryPar;
//HRichDigitisationPar *pDigiPar =  new HRichDigitisationPar;
//HRichMappingPar *pMapPar = new HRichMappingPar;
//rtdb->addContainer(pGeomPar);
//rtdb->addContainer(pAnalPar);
//rtdb->addContainer(pDigiPar);
  rtdb->addContainer(pCalPar);
//rtdb->addContainer(pMapPar);
//rtdb->initContainers(0); // explicit containers initialisation
rtdb->initContainers(944433336);
//  rtdb->readAll();

//  rtdb->setContainersStatic(kTRUE);
  // this is necessary if you do not want to reinitialise parameters
  // for next event file

  gHades->getRuntimeDb()->print();


// pAnalPar->setChanged();
//pGeomPar->setChanged();
//pDigiPar->setChanged();
  pCalPar->setChanged();
//pMapPar->setChanged();
  rtdb->saveOutput();

  rtdb->closeFirstInput();
  rtdb->closeSecondInput();
  rtdb->closeOutput();

  // Delete the gHades objects. The data written to file.
  delete gHades;

  // End timer
  timer.Stop();


  // Print benchmarks
  printf("\nReal time=%f\n",timer.RealTime());
  printf("Cpu time=%f\n",timer.CpuTime());

}

