{

  Hades myHades;
  HRuntimeDb* rtdb=gHades->getRuntimeDb();


  HRichDetector rich; 
  Int_t mods[1]={1}; // in RICH there is only 1 module 
  rich.setModules(0,mods);   // 0 sector
  rich.setModules(1,mods);   // 1 sector
  rich.setModules(2,mods);   // 2 sector
  rich.setModules(3,mods);   // 3 sector
  rich.setModules(4,mods);   // 4 sector
  rich.setModules(5,mods);   // 5 sector
  gHades->getSetup()->addDetector(&rich);
//Int_t runID = 990317026;//xx01140020346.hld
Int_t runID = 990385119;//xx01140205839.hld
  rtdb->addRun(runID);

  HParHadAsciiFileIo input1; // 1st ASCII input
  input1.openfilelist("params/rich/may01/rich_key_filelist.txt");
  rtdb->setFirstInput(&input1);
  
  HParHadAsciiFileIo input2("RAW"); // 2nd ASCII input
//  input2.open("input/ri00152141625cal.txt");
  input2.openfilelist("params/rich/may01/rich_raw_filelist2001-05-20_2.txt");
rtdb->setSecondInput(&input2);

//  HParHadAsciiFileIo output;
//  output.open("calanadigigeoout.txt", "RECREATE");
HParRootFileIo output;
output.open("params/rich/test_versmanagement/RICH_ALL_2001-05-20_2.root", "RECREATE");
//output.open("test.root","RECREATE");
  rtdb->setOutput(&output);

  HRichAnalysisPar anapar;
rtdb->addContainer(&anapar);

  HRichMappingPar mappar;
rtdb->addContainer(&mappar);

  

  HRichDigitisationPar digipar;
rtdb->addContainer(&digipar);

  HRichGeometryPar geopar;
  rtdb->addContainer(&geopar);
 
  HRichCalPar calpar;
rtdb->addContainer(&calpar);

  rtdb->initContainers(runID); // explicit containers initialisation
anapar->printParam();
digipar->printParam();
anapar->setChanged();
 mappar->setChanged();
digipar->setChanged();
 geopar->setChanged();
calpar->setChanged();

  rtdb->saveOutput();
  rtdb->print();
  rtdb->closeFirstInput();
  rtdb->closeSecondInput();
  rtdb->closeOutput();
  delete gHades;
}
 













