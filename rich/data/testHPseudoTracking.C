{
  Int_t compLevel=2;
  Int_t splitLevel=2;
  gHades->setSplitLevel(splitLevel);
// Int_t nEvents=50;

  //HSpectrometer* spec=gHades->getSetup(); 
TString tracking = "dubna";
// SHOWER configuration
  Int_t runID = 0;
  Int_t refrunID = 0;
// Set the runtime database;
 
Int_t sectors[]     = {1,1,1,1,1,1}; // Bitmap of active sectors
Int_t showerMods[3] = {1,2,3};
Int_t tofMods[22]   = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//Int_t tofinoMods[][]  = { {1},{1},{1},{0},{0},{1} };
 Int_t tofinoMod[] = {1};
 
  Int_t mdcMods[6][4]=
//         { {0,1,0,0},   // moduledef for sector 1
//           {0,1,0,0},   // moduledef for sector 2
//           {1,1,0,0},   // ...
//           {1,1,0,0},
//           {0,0,0,0},
//           {1,1,0,0} };
        { {1,1,1,1},   // moduledef for sector 1
	  {1,1,1,1},   // moduledef for sector 2
	  {1,1,1,1},   // ...
	  {1,1,1,1},
	  {1,1,1,1},
	  {1,1,1,1} };


Int_t richMods[]    = {1};           // Bitmap of active Rich modules


/////////////////////////////////////////////////////////////////////////
// String normalization
tracking.ToLower();

/////////////////////////////////////////////////////////////////////////
// Detectors setup configuration

HSpectrometer* spec = gHades->getSetup();
HRuntimeDb* rtdb=gHades->getRuntimeDb();
rtdb->addRun(refrunID);

HShowerDetector *shower=new HShowerDetector;
spec->addDetector(shower);

HTofDetector *tof=new HTofDetector;
spec->addDetector(tof);

HTofinoDetector *tofino=new HTofinoDetector;
spec->addDetector(tofino);

HMdcDetector *mdc=new HMdcDetector;
 for (Int_t i=0;i<6;i++) {mdc->setModules(i,mdcMods[i]);}
spec->addDetector(mdc);

HRichDetector *rich = new HRichDetector; 
spec->addDetector(rich);


for (Int_t is=0; is<6; is++) {
  shower->setModules(is,showerMods);
  tof->setModules(is,tofMods);
  rich->setModules(is,richMods);
  tofino->setModules(is,tofinoMod);
  //  mdc->setModules(is, mdcMods[is]);
}
// ***************** paramter files *******************
HParRootFileIo *inp1=new HParRootFileIo;
//inp1->open("params/mar2001NEW_Simparams.root", "READ");
inp1->open("/home/lfabbi/hydra_v531/params/mar2001NEW_Simparams.root", "READ");

HParAsciiFileIo *inp2=new HParAsciiFileIo;
//inp2->open("mdc_kick_old.txt","in");
inp2->open("/home/lfabbi/hydra_v531/params/kick+mdcNew.txt","in");

rtdb->setFirstInput(inp2);
rtdb->setSecondInput(inp1);

//*****************************************************

//******************* container definition ************
rtdb->addRun(runID);
rtdb->addRun(0,runID);
//rtdb->addContainer(new HMdcRawStruct);

rtdb->addContainer(new HMdcGeomStruct);
rtdb->addContainer(new HMdcRawStruct);
 rtdb->initContainers(runID);       // mandatory here to initialize properly!
cout << "eco\n";

rtdb->addContainer(new HMdcLookupGeom);
rtdb->addContainer(new HMdcLayerGeomPar);
rtdb->addContainer(new HMdcDigitPar);
cout << "eco\n";
HMdcGeomPar* geom=(HMdcGeomPar*)(gHades->getSetup()
				 ->createDetGeomPar("Mdc"));
cout << "ecoin\n";
//geom->HParSet::init();
cout << "eco\n";
HSpecGeomPar* specGeom=(HSpecGeomPar*)(gHades->getSetup()
                                               ->createSpecGeomPar());

HKickTask::setKickParIo(rtdb);
rtdb->addContainer(new HKickPlane);
// specGeom->HParSet::init(); 
cout << "eco\n";
 HTofCalPar* tofcal = new HTofCalPar;

  rtdb->addContainer(tofcal);
  rtdb->initContainers(runID);

  rtdb->addContainer(new HShowerDigiDetPar);
  rtdb->addContainer(new HShowerCalPar);
  rtdb->addContainer(new HShowerHitFPar);
  rtdb->addContainer(new HShowerGeometry);

  rtdb->addContainer(new HRichDigitisationPar);
  rtdb->addContainer(new HRichAnalysisPar);
  rtdb->addContainer(new HRichGeometryPar);


  rtdb->addContainer(new HTofinoCalPar);
  rtdb->addContainer(new HTofinoShowerMap);
cout << "eco\n";
  rtdb->initContainers(runID);
cout << "eco\n";

( (HRichAnalysisPar *)rtdb->getContainer("RichAnalysisParameters"))->printParam();  

//HTofGeomPar* geomtof=(HTofGeomPar*)gHades->getSetup()->createDetGeomPar("Tof");

// HKickTask::setKickParIo(rtdb);
// rtdb->addContainer(new HKickPlane);
rtdb->initContainers(refrunID);

// ************** tasks definition ******************
 
//**********************  RICH ********************
 HTaskSet *richTasks = new HTaskSet("Rich","Rich");
  richTasks->connect(new HRichDigitizer("rich.digi","Rich digitizer"));
               richTasks->connect(new HRichAnalysisSim("rich.anal","Rich ring finder"),"rich.digi");
cout<<"after rich "<<endl;
//************************ MDC *******************
// do I need these cut on drift time for simulations???

 //   // Calibreted data nov2000 (  HMdcCalibrater1("MdcCali1","MdcCali1")  )
 Float_t cutTime1L[4]={  -7.,  -12.,  -15., 0.};                                    
 Float_t cutTime1R[4]={ 160.,  220.,  540., 0.};                                    
 Float_t cutTime2L[4]={   0.,    0.,    0., 0.};                                    
 Float_t cutTime2R[4]={ 220.,  270.,  570., 0.};                                    
 Float_t cutLeft[4]  ={   9.,   10.,   20., 0.};  // cutLeft < driftTime1-driftTime2
 Float_t cutRight[4] ={ 120.,  170.,  190., 0.};

HMdcTimeCut* fcut=new HMdcTimeCut(cutTime1L, cutTime1R,
             cutTime2L, cutTime2R,   cutLeft, cutRight);

////////////////////
  HTaskSet *mdcTasks = new HTaskSet("Mdc","Mdc");
mdcTasks->connect(new HMdcDigitizer("mdc.digi","Mdc digitizer"));
mdcTasks->connect(new HMdcCalibrater2Sim("mdc.cal2","Mdc calibrater 2"),"mdc.digi");
 HMdcTrackFinder *mdchit=new HMdcTrackFinder("mdc.hitf","mdc dubna finder",kFALSE,0);
mdcTasks->connect(mdchit,"mdc.digi");
mdcTasks->connect(new HMdcTrackFinder("mdc.hitf","Dubna hit finder",kFALSE,1),"mdc.cal2");
 mdcTasks->connect(new HMdcClusterToHit("mdc.c2h","mdc.c2h"),"mdc.hitf");
 mdcTasks->connect((HTask *)NULL,"mdc.c2h");
//cout<<" after mdc "<<endl;
//******************* TOF ***********************
  HTaskSet *tofTasks = new HTaskSet("Tof","Tof");
  tofTasks->connect(new HTofDigitizer("tof.digi","Tof digitizer"));
  tofTasks->connect(new HTofHitFSim("tof.hitf","Tof hit finder"),"tof.digi");

//tofTasks->connect((HTask *)NULL, "tof.hitf");
  
cout<<"after tof"<<endl;
// ******************* TOFINO ***********************
HTaskSet *tofinoTasks = new HTaskSet("Tofino","Tofino");
  tofinoTasks->connect(new HTofinoDigitizer("tofino.digi","Tofino digitizer"));  
cout<<"after tofino"<<endl; 

//***************** SHOWER *********************
  
  HTaskSet *shoTasks = new HTaskSet("Shower","Shower");
  shoTasks->connect(new HShowerHitDigitizer("hit.digi","Shower hit digitizer"));
  shoTasks->connect(new HShowerPadDigitizer("pad.digi","Shower pad digitizer"),"hit.digi");
  shoTasks->connect(new HShowerCopy("copy","Shower copier"),"pad.digi"); 
  shoTasks->connect(new HShowerHitFinder("shower.hitf","Shower hit finder"),
		    "copy");
//shoTasks->connect(new HShowerHitTrackMatcher("hit.match","HitTrackMatcher"),
//                   "shower.hitf"); 
//shoTasks->connect(new HShowerPIDTrackMatcher("pid.match","PIDTrackMatcher"),
//                  "hit.match");

  shoTasks->connect(new HShowerTofinoCorrelator("shower.tofino","Correlater"),"shower.hitf");


//----------------------- Kick Plane task ---------------------
 HTaskSet *kickTasks = new HTaskSet("KickPlane","KickPlane");

kickTasks->connect(new HKickTrackF("kick.trackf","kick.trackf"));



//*****************************************************
cout<<"before task conn"<<endl;
gHades->getTaskSet("simulation")->connect(richTasks); 
gHades->getTaskSet("simulation")->connect(tofTasks, richTasks);
gHades->getTaskSet("simulation")->connect(tofinoTasks,tofTasks);
gHades->getTaskSet("simulation")->connect(mdcTasks,tofinoTasks);
gHades->getTaskSet("simulation")->connect(shoTasks, mdcTasks); 
gHades->getTaskSet("simulation")->connect(kickTasks,shoTasks);
gHades->getTaskSet("simulation")->connect((HTask *)NULL,kickTasks);
gHades->getTaskSet("simulation")->isTimed(kTRUE);      // activate timers
gHades->getTaskSet("simulation")->print();
  

 
  gHades->setSplitLevel(2);  

// Set the data source (here from hld file)
//HRootSource *data=new HRootSource();


//HGeantSource *data=new HGeantSource;
HRootSource *data=new HRootSource();
//data->addFile("/s/bielcik/A15Aana/geant/geant311_AA_15AGeV_omega_pi0ee_N1_50k1.root");
//data->addFile("/scratch/titan1/hgeantoutput/C15CThemFeb01/50k11.root");
//data->addFile("/home/lfabbi/hgeant_v309/test1.root");
//data->addFile("/scratch/titan1/hgeantoutput/Omega/omega10k1.root");
data->addFile("/home/lfabbi/hgeant_v309/1ele1.root");
gHades->setDataSource(data);

// Initialization
  printf("gHades->init()\n");
  if (!gHades->init()) {
     printf("Error\n");
     return -1;
  }


// Set output 
cout<<" before output"<<endl;


gHades->setOutputFile("60ele.root","RECREATE","Test",2);


//gHades->getCurrentEvent()->getCategory(catGeantKine)->setPersistency(kFALSE);
gHades->getCurrentEvent()->getCategory(catMdcGeantRaw)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catRichGeantRaw)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catRichGeantRaw+1)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catRichGeantRaw+2)->setPersistency(kFALSE);
gHades->getCurrentEvent()->getCategory(catShowerGeantRaw)->setPersistency(kFALSE);
gHades->getCurrentEvent()->getCategory(catTofGeantRaw)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catRichCal)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catRichHit)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catMdcCal1)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catMdcCal2)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catMdcHit)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catMdcGeantCell)->setPersistency(kFALSE);

gHades->getCurrentEvent()->getCategory(catShowerCal)->setPersistency(kFALSE);
gHades->getCurrentEvent()->getCategory(catShowerGeantWire)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catShowerHit)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catShowerHitHdr)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catShowerHitTrack)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catShowerPID)->setPersistency(kFALSE);
gHades->getCurrentEvent()->getCategory(catShowerTrack)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catShowerPIDTrack)->setPersistency(kFALSE);
gHades->getCurrentEvent()->getCategory(catShowerRawMatr)->setPersistency(kFALSE);

gHades->getCurrentEvent()->getCategory(catTofRaw)->setPersistency(kFALSE);
//gHades->getCurrentEvent()->getCategory(catTofHit)->setPersistency(kFALSE);



  gHades->makeTree();

}








