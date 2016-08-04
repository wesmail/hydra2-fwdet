////////////////////////////////////////////////////////////////////
//Using:
//  root [0] .x mdcTrackD.C();
//  or
//  root [0] .x mdcTrackD.C("file_name"); (inDir will used !!!)
////////////////////////////////////////////////////////////////////
gROOT->Reset();
//=Input and Output files and directory=============================

Char_t * default_indir =  "/lustre/hades/aug11";

Char_t inDir[200];

Int_t refrunID = -1000; //1; //-1000; //1945425940; //1; // run number used to initialize rtdb

Char_t* inFiles[]={
//   "222/be1122215262201.hld"
  "231/be1123107545001.hld",
  "231/be1123107545002.hld",
  "231/be1123107545003.hld",
  "231/be1123107545004.hld",
  "231/be1123107545005.hld",
  "231/be1123107545006.hld",
  "231/be1123107545007.hld",
  "231/be1123107545008.hld"
};

Bool_t  makeTree  = kFALSE; //kTRUE;
TString outFile   = "./dst01.root";

// Parameters:
Char_t  typeFile1 = 'a'; //'a';  // ='a'-ASCII or 'r'-root file
Char_t* parFile1  = "./param_24oct11.txt";


// List of modules:
  Int_t mdcMods[6][4] = {
    {0,0,0,0},
    {0,0,0,0},
    {1,1,1,1},
    {0,0,0,0},
    {1,1,1,1},
    {1,1,1,1}
  };
  Int_t nStartMods[10] = {1,1,1,1,0,0,0,0,0,0};
  Int_t nTrigMods[]    = {1};
                                                           
// ----------------  Unpacker configuration --------------------------
  Int_t mdcUnpackers[12]  = { 0x1000 ,0x1100,0x1010 ,0x1110, 0x1020 ,0x1120, 0x1030 ,0x1130, 0x1040 ,0x1140, 0x1050 ,0x1150 };
  Int_t startUnpackers[1] = {0x8800};

//===Mdc time cuts======================================================= 
  // cutStatus      = 0 - No drift time cuts.
  // cutStatus      = 1 - HMdcTrackFinder and HMdcEfficiency will do drift time cuts.
  // cutStatus      = 2 - HMdcCalibrater1 will do drift time cuts.
  // sourceCutPar   = 0 - using param. of time cuts from parameters files or ORACLE
  // sourceCutPar   = 1 - seting of cut parameters in this macro.
  // calibrater1Ver = 1 - No Start and Cal
  //                = 2 - Start and Cal
  //                = 3 - No Start and no Cal

  Int_t calibrater1Ver = 2; //1; // see class HMdcCalibrater1
  Int_t cutStatus      = 1; //2; //1; //0;
  Int_t sourceCutPar   = 0; //1; //0;

  Float_t time1Low[4]  = {  260.,  240.,  200.,  200.};  // t1 cut
  Float_t time1Up[4]   = {  800.,  800., 1000., 1100.};  // t1 cut
  Float_t totLow [4]   = {   45.,   45.,   45.,   45.};  // t2-t1 cut
  Float_t totUp[4]     = {  400.,  400.,  600.,  600.};  // t2-t1 cut
  Float_t cutBumpTime1L[4]={-2000., -2000., -2000., -2000.};
  Float_t cutBumpTime1R[4]={-2000., -2000., -2000., -2000.};
  Float_t cutBumpTotL  [4]={-2000., -2000., -2000., -2000.};
  Float_t cutBumpTotR  [4]={-2000., -2000., -2000., -2000.};
  
//=Parameters of track finder and efficiency tasks:=====================
  // noFieldData     = kTRUE(kFULSE) - The current in coil is  switched off(on).
  // typeClustFinder = 1-independent, 0-combined
  // noFieldData     = kTRUE,  typeClustFinder = 1: Independent track finder for each MDC
  // noFieldData     = kTRUE,  typeClustFinder = 0: Combined track finder for all MDCs in sector
  Bool_t noFieldData     = kTRUE;
  Int_t  typeClustFinder = 1;       // =1,0 1-independent, 0-combined

  // HMdcClus persistency:
  Bool_t persistency     = kFALSE;  // write or not the HMdcClus data in output file

  // Parameters for cluster finder for estimatin of efficiency:
  Int_t level4s1         = 4;
  Int_t level5s1         = 100000;
  Int_t level4s2         = 4;
  Int_t level5s2         = 100000;

  // List of num. of working layers in modules: lLayers[sec][mod]
  Int_t nLayers[6][4]    = { {6,6,6,6}, 
                             {6,6,6,6}, 
                             {6,6,6,6},
                             {6,6,6,6}, 
                             {6,6,6,6}, 
                             {6,6,6,6} };
//======================================================================
  HMdcEfficiency* mdcEff;
  HMdcEffMenu* effMenu;
//======================================================================

void mdcEfficiency(Char_t* fileI=0, Char_t* inDir2=0) {
  if (inDir2 == 0) sprintf( inDir, "%s", default_indir );
  else sprintf(inDir, "%s", inDir2 );

  Hades* myHades = new Hades();
  myHades->setQuietMode();
  HSpectrometer* spec=myHades->getSetup();
     
  spec->addDetector(new HTBoxDetector);
  spec->addDetector(new HStart2Detector);
  spec->getDetector("TBox")->setModules(-1,nTrigMods);
  spec->getDetector("Start")->setModules(-1,nStartMods);
  

  // MDC configuration
  HMdcDetector *mdc=new HMdcDetector;
  for(UInt_t i=0;i<6;i++) mdc->setModules(i, mdcMods[i]);
  spec->addDetector(mdc);

  HRuntimeDb* rtdb=myHades->getRuntimeDb();

  // The data source:
  HldFileSource *source = new HldFileSource;
  myHades->setDataSource(source);
  source->setDirectory(inDir);
//      rtdb->addRun(refrunID);
  if(fileI) {
    if(refrunID<-1) source->addFile(fileI);
    else            source->addFile(fileI,refrunID);
  } else {
    Int_t numInFiles=sizeof(inFiles)/sizeof(*inFiles);
    for(Int_t n=0; n<numInFiles; n++) {
      if(refrunID<-1) source->addFile(inFiles[n]);
      else            source->addFile(inFiles[n],refrunID);
    }
  }

  source->addUnpacker( new HLatchUnpacker(startUnpackers[0]) );
  source->addUnpacker( new HTBoxUnpacker(startUnpackers[0]) );
  
  for(UInt_t i=0; i<(sizeof(mdcUnpackers)/sizeof(Int_t)); i++) {
    HMdcUnpacker* mdc_unpacker = new HMdcUnpacker( mdcUnpackers[i], kTRUE );
    mdc_unpacker->setQuietMode(kTRUE);

    mdc_unpacker->setFillDataWord(kFALSE);
    mdc_unpacker->setFillOepStatus(kTRUE);
    mdc_unpacker->setPersistencyDataword(kTRUE);
    mdc_unpacker->setPersistencyOepStatusData(kFALSE);

    //!    mdc_unpacker->setStandaloneMode(kTRUE,1);
    //mdc_unpacker->setFillDataWord(kFALSE);         // kann man rausnehmen
    source->addUnpacker( (HMdcUnpacker*)mdc_unpacker );
  }
  
  for(UInt_t i=0; i<(sizeof(startUnpackers)/sizeof(Int_t)); i++){
    HStart2Trb2Unpacker *startUnp = new HStart2Trb2Unpacker(startUnpackers[i]);
    startUnp->setDebugFlag(0);
    startUnp->setcorrectINL();
    startUnp->setQuietMode();
    //    //startUnp->disableTimeRef();
    source->addUnpacker( startUnp );
  }

  // Oracle:
  HParOra2Io *ora = new HParOra2Io;
  ora->open();
  if (!ora->check()) {
    Error("ora->check","no connection to Oracle\nexiting...!");
    exit(1);
  }
  rtdb->setSecondInput(ora);
  ora->setHistoryDate("now");
  
  //   HMdcLayerCorrPar* fMdcLayerCorrPar = (HMdcLayerCorrPar*)rtdb->getContainer("MdcLayerCorrPar");
  // //  fMdcLayerCorrPar->setDefaultPar();
  //   fMdcLayerCorrPar->setStatic();
 
  if(typeFile1 == 'a') {
    HParAsciiFileIo *inp1=new HParAsciiFileIo;
    inp1->open(parFile1,"in");
    rtdb->setFirstInput(inp1);
  } else if(typeFile1 == 'r') {
    HParRootFileIo *inp1=new HParRootFileIo;
    inp1->open(parFile1,"READ");
    rtdb->setFirstInput(inp1);
  }
  //-------------------------------------------------------

  if(cutStatus==1 || (cutStatus==2 && sourceCutPar==1)) {
    HMdcTimeCut* fcut=new HMdcTimeCut();
    rtdb->addContainer(fcut);
    if(sourceCutPar==1) {
      fcut->setCut(time1Low, time1Up, -2000.,2000., totLow,totUp,cutBumpTime1L, cutBumpTime1R,cutBumpTotL, cutBumpTotR);
      fcut->setStatic(kTRUE);
    }
    if(cutStatus==1) fcut->setIsUsed(kFALSE);
  }
  
  // Tasks:-------------------------------------------------
  HMdcTrackFinder* trackFinder;
  HStartTaskSet *startTaskSet = new HStartTaskSet();
  HTask         *startTasks   = startTaskSet->make("","");
  
  HTaskSet *masterTaskSet = gHades->getTaskSet("real");
  masterTaskSet->add(startTasks);
  
  //  HFlexFiller* flexfiller = new HFlexFiller("FlexFiller","FlexFiller");
  //  flexfiller->setUserFill(selectVeto,0,0,kFALSE); // function,HHIstMap*,TObjArray*,CreateFlexCat

  // HMdcBitFlipCor* mdcbitflipcor = new HMdcBitFlipCor("mdcbitflipcor","mdcbitflipcor");
  // masterTaskSet->add(mdcbitflipcor);
  
  masterTaskSet->add(new HMdcCalibrater1("MdcCali1","MdcCali1",calibrater1Ver,(cutStatus<=1)?0:1));
  masterTaskSet->add(trackFinder = new HMdcTrackFinder("MdcTrackD","MdcTrackD",
                                                       noFieldData,typeClustFinder));
  masterTaskSet->add(mdcEff = new HMdcEfficiency("MdcEff","MdcEff"));
  if(makeTree) masterTaskSet->add(((new HMdcClusterToHit("ClHit","ClHit")),"MdcEff"));

  //====Seting of parameters of mdcTrackFinder===============
  trackFinder->setPersistency(persistency);
  trackFinder->setLevel(level4s1, level5s1, level4s2, level5s2);
  trackFinder->setNLayers(&nLayers[0][0]);
  //=========================================================

  if (!myHades->init()) {
    rtdb->closeFirstInput();
    rtdb->closeSecondInput();
    delete myHades;
    return;
  }
  

  // Set output file
  if(makeTree) {
    Int_t compLevel = 2;
    myHades->setOutputFile(outFile.Data(),"RECREATE","runs",compLevel);
    myHades->makeTree();
  }

  cout << "init finished" << endl;
  gHades->eventLoop(1);

  effMenu=HMdcEffMenu::makeMenu(mdcEff);
// delete myHades;
}
