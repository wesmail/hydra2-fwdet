// File: proto.C
//
// Author: Thomas Eberl <Thomas.Eberl@physik.tu-muenchen.de>
// Last update by Thomas Eberl: 03/02/22 16:54:55
//
// This macro exemplifies the initialization of a HYDRA analysis
// 
// FIXME: how to handle an eventlist ????
Bool_t proto(
		      TString inputDir="./",           // -- trailing slash 
		      TString inputFile="default.root",//
		      TString outputDir="./",          // -- trailing slash
		      TString paramFile,               // -- path/file
		      TString paramSource="ORACLE",    // ORACLE, ROOT, ASCII
		      TString evtType="real",          // real or simulation
		      Int_t   nEvtsToProcess=1000000   // nb of evts to process
	     ) 

{

//  NOTE:
//  Several variables influencing the behaviour of the code are set in the following,
//  not in the argument list of the function, as it would become too long.
//  
 
//  reset CINT environment
    gROOT->Reset();

//  create HADES object
    Hades *myHades=new Hades;

//  create stop watch that measures processing rates in the event loop
    TStopwatch timer;

//  number of events actually processed by event loop, here only init
    Int_t evN=0;

//  number of events stored in the tree that is read in for processing,init
    Int_t evtsInTree=0;

//  init of reference run id for parameter initialization
//  this value must be set later if you want to init a parameter set different than the
//  default returned by the version management. This is done using the filename later in
//  this code 
    Int_t refId = 0;

//  init of variable that will contain the final number of events to be processed
//  either the user requested number with which it is initialized or the maximum 
//  number of events in the opened tree 
    Int_t maxEvts=nEvtsToProcess;


    /////////////////////////////////////////////////////////////////////////////////////

//  HERE several variables are initialized that determine the program flow
//
//
    Bool_t kNoRTDB=kFALSE;
    if (!paramSource.Contains("ORACLE") && !paramSource.Contains("ROOT") && !paramSource.Contains("ASCII"))
	kNoRTDB=kTRUE;
//  switch for unpacker initialization in case of real list mode data 
    Bool_t isUnpacking=kFALSE;
//
//  set this to kTRUE if your input data ROOT file contains more than one runid
//  this happens e.g. if it is a merged file, implies that you use one param version
//  for multiple runids 
    Bool_t isMergedFile=kFALSE;//merg. high lvl file w/ mult runid, no vers man
//
//  set this if you want to use different param contexts than the defaults
//  defined in HRichContFac
    Bool_t useOnlyDefaultParamContext=kFALSE;//allow special context
//
//  When initializing from ORACLE you get by default the params valid for the latest
//  parameter release (thought to be approved), not the latest changes you have made !
//  set this flag if you want to initialize with the latest changes active !
    Bool_t isLatestParams=kTRUE;//latest params or from the last valid release
//
//  switch data ROOT file with categories in tree on or off
    Bool_t  createTreeFile=kTRUE;
//
//  switch between limited nb of cats in output and full copy of input
    Bool_t writeOnlySelectedCats=kFALSE;
//
//  substitute categories in output from a list
    Bool_t substituteCategories=kTRUE;
//
//  set event number to start from
    Int_t   startEvt=0;
//
//  switch to create a parameter ROOT file with all containers init by tasks
//  from ORACLE. This allows easily to make a param snapshot for offline processing 
    Bool_t  kDumpParam=kFALSE;
    if (kDumpParam) nEvtsToProcess=3;
//
//  set suffix for data output file creation  
    TString outputFileSuffix="ana1";
//
//  select 1st or 2nd lvl trigger evts in data
    TString triggerType="level1";

    /////////////////////////////////////////////////////////////////////////////////////

//  analyze input filename
    if (inputFile.Contains("hld") && evtType.Contains("real")     
	) isUnpacking=kTRUE;

//  ALGORITHMS: HERE you can switch task sets on and off
    Bool_t start          = 0;
    Bool_t tbox           = 0;
    Bool_t mdc            = 0;
    Bool_t tof            = 0;
    Bool_t tofino         = 0;
    Bool_t shower         = 0;
    Bool_t showertofino   = 0;
    Bool_t rich           = 0;
    Bool_t richipu        = 0; 
    Bool_t trig           = 0; 
    Bool_t kick           = 0;
    Bool_t corr           = 0;
    Bool_t qa             = 0;
    Bool_t ringfilter     = 1;
    Bool_t multfilter     = 0;
    Bool_t objfilter      = 0;
    Bool_t paircreation   = 0;
    Bool_t hgeantana      = 0;
    Bool_t splittrigger   = 0;
    Bool_t ringanalysis   = 0;
    Bool_t pca            = 0;
    Bool_t histfiller     = 1;
    Bool_t pid            = 0;
//  SETUP: HERE you can switch on and off active detectors
//  you must activate a detector here if you want to use its unpackers or tasks
    Bool_t startsetup        = 0;
    Bool_t tboxsetup         = 0;
    Bool_t mdcsetup          = 1; // enforces initialization of HMdcGeomStruct !
    Bool_t tofsetup          = 1;
    Bool_t tofinosetup       = 1;
    Bool_t showersetup       = 1;
    Bool_t showertofinosetup = 1;
    Bool_t richsetup         = 1;
    Bool_t richipusetup      = 0; 
    Bool_t trigsetup         = 0; 

//  switch off graphics
//  in case of X-Window and/or PostScript creation suppress output to screen
//  NOTE: this analysis still needs a valid X-Server connection even if it does not
//  open any window ! If it has no access to a DISPLAY it bombs 
    gROOT->SetBatch();


//  create output filename that contains data tree
    TString outputFile = inputFile;
    outputFile.Remove(outputFile.Length()-5,outputFile.Length());
    outputFile = outputDir+"/"+outputFile+"_"+outputFileSuffix+".root";

//  create filenames and directories for task specific output
    TString outDirH = outputDir+"/histograms";
    if (! gSystem->OpenDirectory(outDirH.Data()) ) 
    {
	if(gSystem->MakeDirectory(outDirH.Data())==-1)
	{
	    Error("histogram default directory","directory could not be created");
	}
    }

    TString outputHistFile = inputFile;
    outputHistFile.Remove(outputHistFile.Length()-5,
			  outputHistFile.Length());
    outputHistFile = outDirH+"/"+outputHistFile+"_"+
	outputFileSuffix+".root";

    TString richcorrelatorhistos(outputHistFile.Data());
    richcorrelatorhistos.Remove(richcorrelatorhistos.Length()-5, 
				richcorrelatorhistos.Length() );
    richcorrelatorhistos.Append("_corrhist.root");
    TString richpaircreatorhistos(outputHistFile.Data());
    richpaircreatorhistos.Remove(richpaircreatorhistos.Length()-5, 
				 richpaircreatorhistos.Length() );
    richpaircreatorhistos.Append("_pairhist.root");
    TString geantkineanahistos(outputHistFile.Data());
    geantkineanahistos.Remove(geantkineanahistos.Length()-5, 
			      geantkineanahistos.Length() );
    geantkineanahistos.Append("_kinehist.root");
    TString pcahistos(outputHistFile.Data());
    pcahistos.Remove(pcahistos.Length()-5, 
		     pcahistos.Length() );
    pcahistos.Append("_pcahist.root");
    TString histofiller(outputHistFile.Data());
    histofiller.Remove(histofiller.Length()-5, 
		     histofiller.Length() );
    histofiller.Append("_histofiller.root");

//  quality assessment default directory
    TString outDirQA = outputDir+"/QA";
    if (qa)
    {
	if (! gSystem->OpenDirectory(outDirQA.Data()) ) 
	{
	    if(gSystem->MakeDirectory(outDirQA.Data())==-1)
	    {
		Error("QA default directory","directory could not be created");
	    }
	}
    
    }
    
//  --- CHANGE HERE: tree input / output data categories  
    
    
//  write these categories to the tree (all others switched off)
//  you find the names in the respective <DET>def.h headers
//  in the resp. library src subdirectories
    if (createTreeFile && writeOnlySelectedCats)
    {
	Cat_t PersistentCat[] = 
	    {
	    
		catMatchHit,catDiMatchHit
	    
	    };
    }
//  skip these categories when reading input tree
//  e.g. when recreating a data category in a tree
//  !!!!!
//  When reading large files specify all except those that 
//  are really needed by your algorithms, it is decisive for the 
//  processing speed of your analysis (orders of magnitude !)
//
//  If a category is already in the tree and should be recreated you must
//  switch off the reading of this category, otherwise it is not overwritten, 
//  but simply new objects are mixed with the already existing ones in the tree
    if (substituteCategories)
    {
	Cat_t SubstituteCat[] = 
	    {
		
		catDiMatchHit
		
	    };
	
    }

//  CHANGE HERE: unpacker configuration

    if (isUnpacking)
    {
	// status nov02
	Int_t richUnpackers[] = { 100,101,102,103,104,105,
				  106,107,108,109,110,111 };

	Int_t mdcUnpackers[6][4] = {{201,207,213,217},
				    {202,208,  0,  0},
				    {203,209,214,  0},
				    {204,210,215,218},
				    {205,211,  0,  0},
				    {206,212,216,  0} };
    
	Int_t showerUnpackers[] = { 300,301,302,303,304,305,
				    306,307,308,309,310,311 };
    
	Int_t tofUnpackers[]    = { 411,412,413 };
	Int_t tofinoUnpackers[] = { 412 };
	Int_t startUnpackers[]  = { 412 };
    }
    
//  --- CHANGE HERE: detector setup configuration
//  detailed module setup of the various detectors
//  changes from beamtime to beamtime and in simulation
//  only for "setup" modules are initialized and algorithms applied
    Int_t richMods[]   = {1};
    
    Int_t mdcMods[6][4] = { {0,1,1,1},
			    {1,1,0,0},
			    {1,1,1,0},
			    {1,1,1,0},
			    {1,1,0,0},
			    {1,1,1,0} };      
    // nov02 MDC SETUP
//     Int_t mdcMods[6][4] =  { {1,1,1,1},
// 			     {1,1,0,0},
// 			     {1,1,1,0},
// 			     {1,1,1,1},
// 			     {1,1,0,0},
// 			     {1,1,1,0} }; 


    Int_t tofMods[22] = {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    
    Int_t showerMods[3]  = {1,2,3};
    Int_t tofinoMods[][] = { {1},{1},{1},{1},{1},{1} };
    Int_t startMods[3]   = {1,1,1};
    Int_t trigMods[]     = {1};
    

//  data source: can be ROOT or HLD file(s) (or a socket connection)
    if (inputFile.Contains(".hld"))
    {
	HldFileSource *source=new HldFileSource;
    }
    else if(inputFile.Contains(".root"))
    {
	HRootSource *source=new HRootSource;
    }
    if (gFile) // global ROOT pointer to file list in local CINT context
    {
	// look in CINT session environment for files and concatenate them
	// assumes that files are attached with correct path
	source->setDirectory(".");
	TIter next(gROOT->GetListOfFiles());
	Int_t nFileCnt=0;
	TFile *fileobj=0;
	while ( fileobj = (TFile*) next() )
	{
	    cout<<"adding file: "<<fileobj->GetName()<<endl;
	    source->addFile(fileobj->GetName());
	    //  --- CHANGE HERE: Reference run id
	    TString inputFile(fileobj->GetName());
	    if (inputFile.Contains("c2c")) refId = 3;
	    if (inputFile.Contains("c1c")) refId = 4;
	    if (inputFile.Contains("lf")) refId = 4;
	    if (inputFile.Contains("be02")) refId = 3;
	    if (pid)                        refId = 1;
	    if (refId==0) Warning("reference run for parameters not set properly");
	    if (refId) source->setRefId(source->getCurrentRunId(),refId);
	    nFileCnt++;
	}
	cout<<"[info: ] "<<nFileCnt<<" files concatenated"<<endl;
    }
    else
    {
	// source is only one file provided in argument list
	source->setDirectory(inputDir.Data());
	source->addFile(inputFile.Data());
	//  --- CHANGE HERE: Reference run id
	if (inputFile.Contains("c2c")) refId = 3;
	if (inputFile.Contains("c1c")) refId = 4;
	if (inputFile.Contains("lf")) refId = 4;
	if (inputFile.Contains("be02")) refId = 3;
	if (pid)                        refId = 1;
	if (refId==0) Warning("reference run for parameters not set properly");
	if (refId) source->setRefId(source->getCurrentRunId(),refId);
    }
    
    gHades->setDataSource(source);
    
    // ico a ROOT file (or many concatenated) determine the total nb of events available in the tree
    // and set this to the number of events that will be processed if smaller than the requested number
    if(inputFile.Contains(".root"))
    {
	evtsInTree=(((HRootSource*)gHades->getDataSource())->getTree())->GetEntries();
	if (evtsInTree<nEvtsToProcess) maxEvts=evtsInTree;
    } 

 
   
//  do not read this category from the tree
    if (substituteCategories)
    {
	for(UInt_t j=0;j<sizeof(SubstituteCat)/sizeof(Cat_t);j++)
	    source->disableCategory(SubstituteCat[j]);
    }

//  add unpackers
    if (isUnpacking)
    {
	if (rich)
	{
	    for(UInt_t i=0; i<(sizeof(richUnpackers)/sizeof(Int_t)); i++)
	    {
		source->addUnpacker( new HRichUnpackerCal99(richUnpackers[i]) );
		if (richipu) source->addUnpacker( new HRichUnpackerRaw99(richUnpackers[i]) );
	    }
	}
	if (mdc)
	{
	    for(Int_t s=0; s<6; s++) for (Int_t m=0; m<4; m++) 
	    {
		if(mdcMods[s][m] && mdcUnpackers[s][m])
		{
		    HMdcUnpacker *mdcunpacker=
			new HMdcUnpacker(mdcUnpackers[s][m],kFALSE); 
		    mdcunpacker->setDecodeVersion(1);
		    mdcunpacker->setQuietMode();
		    source->addUnpacker(mdcunpacker);
		}
	    }
	}
    
	//   for(UInt_t i=0; i<(sizeof(mdcUnpackers)/sizeof(Int_t)); i++)
	//       source->addUnpacker( new HMdcUnpacker(mdcUnpackers[i],kFALSE) );
    
	if (shower)
	{
	    for(UInt_t i=0; i<(sizeof(showerUnpackers)/sizeof(Int_t)); i++)
		source->addUnpacker( new HShowerUnpacker(showerUnpackers[i]) );
	}
	if (tof)
	{
	    for(UInt_t i=0; i<(sizeof(tofUnpackers)/sizeof(Int_t)); i++)
		source->addUnpacker( new HTofUnpacker(tofUnpackers[i]) );
	}
	if (tofino)
	{
	    for(UInt_t i=0; i<(sizeof(tofinoUnpackers)/sizeof(Int_t)); i++)
		source->addUnpacker( new HTofinoUnpacker(tofinoUnpackers[i]) );
	}
	if (start)
	{
	    for(UInt_t i=0; i<(sizeof(startUnpackers)/sizeof(Int_t)); i++)
		source->addUnpacker( new HStartUnpacker(startUnpackers[i]) );
	}
	if (trig) source->addUnpacker( new HMatchUUnpacker(1,kTRUE,0));
    
    }

    
//  add detectors to the setup
    HSpectrometer* spec = gHades->getSetup();
  
    if (trigsetup)    spec->addDetector(new HTriggerDetector);
    if (richsetup)    spec->addDetector(new HRichDetector);
    if (mdcsetup)     spec->addDetector(new HMdcDetector);
    if (tofsetup)     spec->addDetector(new HTofDetector);
    if (tofinosetup)  spec->addDetector(new HTofinoDetector);
    if (showersetup)  spec->addDetector(new HShowerDetector);
    if (startsetup)   spec->addDetector(new HStartDetector);
    if (tboxsetup)    spec->addDetector(new HTBoxDetector);

//  set active modules for each detector
    if (startsetup)   spec->getDetector("Start")->setModules(-1,startMods);
    if (tboxsetup)    spec->getDetector("TBox")->setModules(0,trigMods);

    for (Int_t is=0; is<6; is++) 
    {
	if (showersetup) spec->getDetector("Shower")->setModules(is,showerMods);
	if (tofsetup)    spec->getDetector("Tof")   ->setModules(is,tofMods);
	if (tofinosetup) spec->getDetector("Tofino")->setModules(is,tofinoMods[is]);
	if (mdcsetup)    spec->getDetector("Mdc")   ->setModules(is,mdcMods[is]);
	if (richsetup)   spec->getDetector("Rich")  ->setModules(is,richMods);
    }

//  RTDB: RunTime DataBase

    HRuntimeDb* rtdb=gHades->getRuntimeDb();

//  --- CHANGE HERE: contexts to be used for parameter initialization

    // non-default param contexts
    if (!kNoRTDB && !useOnlyDefaultParamContext)
    {
	HRichCorrelatorPar *hrichcorrpar =
	    (HRichCorrelatorPar*)rtdb->getContainer("RichCorrelatorParameters");
	hrichcorrpar->setParamContext("Rich_Kick_Mode");
	// switch off version management ico merged file w/ mult runids
	if (isMergedFile) hrichcorrpar->setStatic();
    }

//  set parameter data source
  if (paramSource.Contains("ORACLE"))
  {
      HParOraIo* input=new HParOraIo;
      input->open();
      if (!input->check()) 
      {
          Error("input->check","no connection to Oracle\nexiting...!");
          exit(1);
      }
      rtdb->setFirstInput(input);
      if (isLatestParams) input->setHistoryDate("now");
  }
  else if (paramSource.Contains("ROOT"))
  {
      HParRootFileIo *input=new HParRootFileIo;
      input->open(paramFile.Data(),"READ");
      if (!rtdb->getFirstInput()) rtdb->setFirstInput(input);
      else rtdb->setSecondInput(input);
  }
  else if (paramSource.Contains("ASCII"))
  {
      // assumes RICH param data format
      HParHadAsciiFileIo *input1 = new HParHadAsciiFileIo; // 1st ASCII input
      input1->open(paramFile.Data());
      rtdb->setFirstInput(input1);
      // take rest from ORACLE
      HParOraIo* input2=new HParOraIo;
      input2->open();
      if (!input2->check()) 
      {
          Error("input2->check","no connection to Oracle\nexiting...!");
          exit(1);
      }
      if (isLatestParams) input2->setHistoryDate("now");
      rtdb->setSecondInput(input2);
      // if second input is ROOT, uncomment the following

//       HParRootFileIo *input2=new HParRootFileIo;
//       TString paramFile2("yourpath/yourfile");
//       input->open(paramFile2.Data,"READ");
//       rtdb->setSecondInput(input);
  }


//  save containers init in this run to ROOT file
//  allows ORACLE snapshot, filename has date
  if (paramSource.Contains("ORACLE") && kDumpParam)
  {
      TString outputName("PAR");
      TDatime dt;
      dt.Set();
      outputName=outputName+dt.GetDate()+"_"+dt.GetTime()+".root";
      HParRootFileIo* output=new HParRootFileIo;
      output->open((outputDir+outputName).Data(),"RECREATE");
      rtdb->setOutput(output);
  }


//  define sets of tasks (algorithms) for event-wise application to data
//  - all tasks get param data from RTDB (design)


//  split level1 and level2 triggered data for analysis
  if (evtType.Contains("real") && splittrigger)
  {
      HTaskSet *deco = new HTaskSet("event Decoding","event Decoding");
      if (triggerType.Contains("level1"))  
	  deco->add( new  HDeco("deco","deco","downsc"));
      if (triggerType.Contains("level2"))  
	  deco->add( new  HDeco("deco","deco","trigger"));
  }



//  R E A L

  if (evtType.Contains("real"))
  {
      // DST setting like in nov02
      if (trig)         HTriggerTaskSet      *triggerTaskSet      = new HTriggerTaskSet();
      if (start)        HStartTaskSet        *startTaskSet        = new HStartTaskSet();
      if (rich)         HRichTaskSet         *richTaskSet         = new HRichTaskSet();
      if (richipu)      HRichIPUTaskSet      *richIPUTaskSet      = new HRichIPUTaskSet();
      if (shower)       HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
      if (tof)          HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
      if (tofino)       HTofinoTaskSet       *tofinoTaskSet       = new HTofinoTaskSet();
      if (showertofino) HShowerTofinoTaskSet *showerTofinoTaskSet = new HShowerTofinoTaskSet();
      if (mdc)          HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet(); 
      if (kick)         HKickTaskSet         *kickTaskSet         = new HKickTaskSet();

      if (trig)         HTask *triggerTasks      = triggerTaskSet      -> make("real");
      if (start)        HTask *startTasks        = startTaskSet        -> make("","controlhists");
      if (rich)         HTask *richTasks         = richTaskSet         -> make("real","");
      if (richipu)      HTask *richIPUTasks      = richIPUTaskSet      -> make("real");
      if (shower)       HTask *showerTasks       = showerTaskSet       -> make();
      if (tof)          HTask *tofTasks          = tofTaskSet          -> make("real");
      if (tofino)       HTask *tofinoTasks       = tofinoTaskSet       -> make();
      if (showertofino) HTask *showerTofinoTasks = showerTofinoTaskSet -> make();
      if (mdc)          HTask *mdcTasks          = mdcTaskSet          -> make("rtdb","");
      //if (kick)         HTask *kickTasks         = kickTaskSet         -> make("","lowres,debug,tofclust");
      if (kick)         HTask *kickTasks         = kickTaskSet         -> make("","lowres,debug");
      if (mdc)
      {
	  HMdcVertexFind *mdcVertexFind 
	      = new HMdcVertexFind("vertexfinder","vertexfinder",0,kFALSE);
	  mdcTaskSet->add(mdcVertexFind); 
      }

      //    event filters
      if (ringfilter)
      {
	  HTaskSet *evtFilterTasks = new HTaskSet("EvtFilter",
						  "List of Evt Filter Tasks");
	  evtFilterTasks->connect( new HRichEvtFilter("Rich_Hit",
						      "Rich_Hit","ringmult",1));
	  cout<<"Ring filter switched on"<<endl;
      }
      //    ring analysis
      if (ringanalysis)
      {
	  HTaskSet *ringAnalysisTasks = new HTaskSet("Ring analysis","Ring analysis");
	  ringAnalysisTasks->add( new HRichChernovRingFitter("HRichChernovRingFitter",
							     "HRichChernovRingFitter",
							     kTRUE));
	  cout<<"Ring fitter switched on"<<endl;
      }
      //    detector correlation tasks
      if (corr)
      {
	  HTaskSet *detCorrTasks = new HTaskSet("Detector correlation","Detector correlation");
	  detCorrTasks->add( new HRichCorrelator("HRichCorrelator",
						 "HRichCorrelator",
						 richcorrelatorhistos.Data(),
						 kFALSE));
	  cout<<"Rich correlator switched on"<<endl;
      }
//    object filters
      if (objfilter)
      {
	  HTaskSet *objFilterTasks = new HTaskSet("Object Filters","Object Filters");
	  objFilterTasks->add( new HRichObjFilter("HRichObjFilter",
						  "HRichObjFilter","MATCH_HIT"));
	  cout<<"Object Filter switched on"<<endl;
      }
//    pair analysis tasks
      if (paircreation)
      {
	  // the pair creator creates pairs (HDiHitMatch) from tracklets (HHitMatch)
	  // it should not apply initial cuts on tracklets, do this in the object filtering
	  HTaskSet *pairAnalysisTasks = new HTaskSet("LeptonPair Analysis","LeptonPair Analysis");
	  pairAnalysisTasks->add( new HRichPairCreator("HRichPairCreator",
						       "HRichPairCreator",
						       richpaircreatorhistos.Data())); 
	  cout<<"Rich pair creator switched on"<<endl;
      }
//    principle component analysis for rich rings tasks
      if (pca)
      {
	  HTaskSet *pcaTasks = new HTaskSet("Principle Component Analysis","Principle Component Analysis");
	  pcaTasks->add( new HRichPCA("PCA",
				      "PCA",
				      pcahistos.Data()));
	  cout<<"Rich PCA  switched on"<<endl;
      }
//   task that fills histograms from tracklets (HHitMatch) and pairs (HDiHitMatch)
      if (histfiller)
      {

	  HTaskSet *histoTasks = new HTaskSet("Histogram filler","Histogram filler");
	  histoTasks->add( new HRichHistoFiller("HistoFiller",
						"HistoFiller",
						histofiller.Data(),""));
	  cout<<"Rich Histo Filler switched on"<<endl;
      }
//   task in pid lib (from Jacek Otwinowski)
      if (pid)
      {
	  
	  HTaskSet *pidTasks = new HTaskSet("PID","PID");

	  HPidTrackFiller *pPidTrackFiller = new HPidTrackFiller();
	  pPidTrackFiller->setParameters("NOMDC,SKIPNOLEPTON");

	  HPidTofinoTrackFilter *pPidTofinoTrackFilter = new HPidTofinoTrackFilterSim();
	  //pPidTofinoTrackFilter->setParameters("SKIPBAD,TOFIMULT1");
	  //pPidTofinoTrackFilter->setParameters("SKIPBAD");

	  HPidReconstructor *pPidRec = new HPidReconstructor("pdf,debug0");
	  Short_t partIds[] = {2,3};
	  Short_t size = 2;
	  //pPidRec->setDefaultParticleIds(); // all pids
	  pPidRec->setParticleIds(partIds,size); // only e+,e-

	  //standard cuts algorithm (by Jacek)
	  HPidAlgStandCuts *pPidAlgStandCuts = new HPidAlgStandCuts();
	  pPidRec->addAlgorithm(pPidAlgStandCuts);
	  pPidAlgStandCuts->setRichParams(370,28,3.0);
	  pPidAlgStandCuts->setTofParams(6.2,8.6); 

	  HPidParticleFiller *pPartFiller = new HPidParticleFiller();
	  pPartFiller->setCondition(7, 0.1, ">=", kTRUE,kFALSE);
	  pPartFiller->print();
 
	  HPidDileptonFiller *pDilepton = new HPidDileptonFiller("debug");

	  pidTasks->add( pPidTrackFiller );
	  pidTasks->add( pPidTofinoTrackFilter );
	  pidTasks->add( pPidRec );
	  pidTasks->add( pPartFiller );
	  pidTasks->add( pDilepton );


      }
      
  }


//   S I M U L A T I O N


  if (evtType.Contains("simulation"))
  {

      // no START det simulation
      //if (trig)         HTriggerTaskSet      *triggerTaskSet      = new HTriggerTaskSet();
      if (rich)         HRichTaskSet         *richTaskSet         = new HRichTaskSet();
      if (richipu)      HRichIPUTaskSet      *richIPUTaskSet      = new HRichIPUTaskSet();
      if (shower)       HShowerTaskSet       *showerTaskSet       = new HShowerTaskSet();
      if (tof)          HTofTaskSet          *tofTaskSet          = new HTofTaskSet();
      if (tofino)       HTofinoTaskSet       *tofinoTaskSet       = new HTofinoTaskSet();
      if (showertofino) HShowerTofinoTaskSet *showerTofinoTaskSet = new HShowerTofinoTaskSet();
      if (mdc)          HMdcTaskSet          *mdcTaskSet          = new HMdcTaskSet(); 
      if (kick)         HKickTaskSet         *kickTaskSet         = new HKickTaskSet();
      
      //if (trig)         HTask *triggerTasks      = triggerTaskSet      -> make("simulation");
      if (rich)         HTask *richTasks         = richTaskSet         -> make("simulation","noiseon");
      if (richipu)      HTask *richIPUTasks      = richIPUTaskSet      -> make("simulation");
      if (shower)       HTask *showerTasks       = showerTaskSet       -> make("","simulation,leprecognition");
      if (tof)          HTask *tofTasks          = tofTaskSet          -> make("simulation");
      if (tofino)       HTask *tofinoTasks       = tofinoTaskSet       -> make("simulation","simulation");
      if (showertofino) HTask *showerTofinoTasks = showerTofinoTaskSet -> make("","simulation,leprecognition");
      if (mdc)          HTask *mdcTasks          = mdcTaskSet          -> make("rtdb","");
      if (kick)         HTask *kickTasks         = kickTaskSet         -> make("","simulation,lowres,tofclust");
      if (mdc)
      {
	  HMdcVertexFind *mdcVertexFind 
	      = new HMdcVertexFind("vertexfinder","vertexfinder",0,kFALSE);
	  mdcTaskSet->add(mdcVertexFind); 
      }
      //    simulate 1st lvl trigger, mult of tof + tofino hits, filter evts
      if (multfilter)
      {
	  HTaskSet *evtFilterTasks1 = new HTaskSet("EvtFilter1",
						   "List 1 of Evt Filter Tasks");
	  evtFilterTasks1->connect( new HRichEvtFilter("Tof+Tofino_Mult",
						       "Tof+Tofino_Mult","tofmult",4));
	  cout<<"tof mult filter switched on"<<endl;
      }
      //    event filters
      if (ringfilter)
      {
	  HTaskSet *evtFilterTasks2 = new HTaskSet("EvtFilter2",
						   "List 2 of Evt Filter Tasks");
	  evtFilterTasks2->connect( new HRichEvtFilter("Rich_Hit",
						       "Rich_Hit","ringmult",1));
	  cout<<"ring filter switched on"<<endl;
      }
      //    ring analysis
      if (ringanalysis)
      {
	  HTaskSet *ringAnalysisTasks = new HTaskSet("Ring analysis","Ring analysis");
	  ringAnalysisTasks->add( new HRichChernovRingFitter("HRichChernovRingFitter",
							     "HRichChernovRingFitter",
							     kTRUE));
	  cout<<"ring fitter swt on"<<endl;
      }
      //    detector correlation tasks
      if (corr)
      {
	  HTaskSet *detCorrTasks = new HTaskSet("Detector correlation","Detector correlation");
	  detCorrTasks->add( new HRichCorrelatorSim("HRichCorrelatorSim",
						    "HRichCorrelatorSim",
						    richcorrelatorhistos.Data(),
						    kFALSE));
	  cout<<"rich correlator switched on"<<endl;
      }
      //    object filters
      if (objfilter)
      {
	  HTaskSet *objFilterTasks = new HTaskSet("Object Filters","Object Filters");
	  objFilterTasks->add( new HRichObjFilter("HRichObjFilter",
						  "HRichObjFilter","MATCH_HIT"));
	  cout<<"rich object filter swt on"<<endl;
      }
      //    pair analysis tasks
      if (paircreation)
      {
	  HTaskSet *pairAnalysisTasks = new HTaskSet("LeptonPair Analysis","LeptonPair Analysis");
	  pairAnalysisTasks->add( new HRichPairCreatorSim("HRichPairCreatorSim",
							  "HRichPairCreatorSim",
							  richpaircreatorhistos.Data())); 
	  cout<<"rich pair creator swt on"<<endl;
      }
      //    HGeant analysis tasks
      if (hgeantana)
      {
	  HTaskSet *HGeantAnalysisTasks = new HTaskSet("HGeant Analysis","HGeant Analysis");
	  HGeantAnalysisTasks->add( new HGeantKineAna("HGeantKineAna",
						      "HGeantKineAna",
						      geantkineanahistos.Data(),
						      "urqmd")); 
	  cout<<"rich hgeant ana swt on"<<endl;
      }
//    principle component analysis for rich rings tasks
      if (pca)
      {
	  HTaskSet *pcaTasks = new HTaskSet("Principle Component Analysis","Principle Component Analysis");
	  pcaTasks->add( new HRichPCA("PCA",
				      "PCA",
				      pcahistos.Data()));
	  cout<<"Rich PCA  switched on"<<endl;
      }
//   task that fills histograms from tracklets (HHitMatch) and pairs (HDiHitMatch)
      if (histfiller)
      {

	  HTaskSet *histoTasks = new HTaskSet("Histogram filler","Histogram filler");
	  histoTasks->add( new HRichHistoFiller("HistoFiller",
						"HistoFiller",
						histofiller.Data(),""));
	  cout<<"Rich Histo Filler switched on"<<endl;
      }

      //   task in pid lib (from Jacek Otwinowski)
      if (pid)
      {
	  
	  HTaskSet *pidTasks = new HTaskSet("PID","PID");

	   // 4 - mult. TOF and TOFINO
	  HPidSimTrigger *pPidSimTrigger = new HPidSimTrigger(4);

	  HPidTrackFillerSim *pPidTrackFiller = new HPidTrackFillerSim();
	  pPidTrackFiller->setParameters("NOMDC,SKIPNOLEPTON");

	  HPidTofinoTrackFilter *pPidTofinoTrackFilter = new HPidTofinoTrackFilterSim();
	  //pPidTofinoTrackFilter->setParameters("SKIPBAD,TOFIMULT1");
	  //pPidTofinoTrackFilter->setParameters("SKIPBAD");

	  HPidReconstructor *pPidRec = new HPidReconstructor("pdf,debug0");
	  Short_t partIds[] = {2,3};
	  Short_t size = 2;
	  //pPidRec->setDefaultParticleIds(); // all pids
	  pPidRec->setParticleIds(partIds,size); // only e+,e-

	  //standard cuts algorithm (by Jacek)
	  HPidAlgStandCuts *pPidAlgStandCuts = new HPidAlgStandCuts();
	  pPidRec->addAlgorithm(pPidAlgStandCuts);
	  pPidAlgStandCuts->setRichParams(370,28,3.0);
	  pPidAlgStandCuts->setTofParams(6.2,8.6); 

	  HPidParticleFillerSim *pPartFiller = new HPidParticleFillerSim();
	  pPartFiller->setCondition(7, 0.1, ">=", kTRUE,kFALSE);
	  pPartFiller->print();
 
	  HPidDileptonFiller *pDilepton = new HPidDileptonFiller("debug");

	  pidTasks->add( pPidSimTrigger  );
	  pidTasks->add( pPidTrackFiller );
	  pidTasks->add( pPidTofinoTrackFilter );
	  pidTasks->add( pPidRec );
	  pidTasks->add( pPartFiller );
	  pidTasks->add( pDilepton );

      }


  }

 
  
  //    switch on quality assessment
  HQAMaker *qaMaker =0;
  if (!outDirQA.IsNull()&&qa)
  {
      qaMaker = new HQAMaker("qa.maker","qa.maker");
      qaMaker->setOutputDir((Char_t *)outDirQA.Data());
      qaMaker->setSamplingRate(5);
      qaMaker->setIntervalSize(1000);
      cout<<"QA swt on"<<endl;
  }
  
  
  //   master task set
  
  HTaskSet *masterTaskSet = gHades->getTaskSet(evtType.Data());
  
  if (evtType.Contains("real"))
  {
      if (tbox) 
      {
	  HTaskSet *masterTaskSetScaler = gHades->getTaskSet("calibration");
	  HTask *scalerTask = new HTBoxSum("scaler.add","Scalers");
	  masterTaskSetScaler->add(scalerTask);
	  masterTaskSetScaler->isTimed(kTRUE);
      }

      if (splittrigger) masterTaskSet->add(deco);
      if (trig)         masterTaskSet->add(triggerTasks);
      if (start)        masterTaskSet->add(startTasks);
      if (tof)          masterTaskSet->add(tofTasks);
      if (tofino)       masterTaskSet->add(tofinoTasks);
      if (rich)         masterTaskSet->add(richTasks);
      if (richipu)      masterTaskSet->add(richIPUTasks);
      if (shower)       masterTaskSet->add(showerTasks);
      if (mdc)          masterTaskSet->add(mdcTasks);
      if (kick)         masterTaskSet->add(kickTasks);
      if (showertofino) masterTaskSet->add(showerTofinoTasks); 

      if (ringanalysis) masterTaskSet->add(ringAnalysisTasks);  
      if (ringfilter)   masterTaskSet->add(evtFilterTasks);
      if (corr)         masterTaskSet->add(detCorrTasks);
      if (objfilter)    masterTaskSet->add(objFilterTasks);
      if (paircreation) masterTaskSet->add(pairAnalysisTasks);
      if (pca)          masterTaskSet->add(pcaTasks);
      if (histfiller)   masterTaskSet->add(histoTasks);
      if (pid)          masterTaskSet->add(pidTasks);
      if (qaMaker)      masterTaskSet->add(qaMaker);
      
  }
  
  if (evtType.Contains("simulation"))
  {
      if (tof)          masterTaskSet->add(tofTasks);
      if (tofino)       masterTaskSet->add(tofinoTasks);
      if (multfilter)   masterTaskSet->add(evtFilterTasks1);
      if (rich)         masterTaskSet->add(richTasks);
      if (richipu)      masterTaskSet->add(richIPUTasks);
      if (shower)       masterTaskSet->add(showerTasks);
      if (mdc)          masterTaskSet->add(mdcTasks);
      if (kick)         masterTaskSet->add(kickTasks);
      if (showertofino) masterTaskSet->add(showerTofinoTasks); 

      if (ringanalysis) masterTaskSet->add(ringAnalysisTasks);  
      if (ringfilter)   masterTaskSet->add(evtFilterTasks2);
      if (corr)         masterTaskSet->add(detCorrTasks);
      if (objfilter)    masterTaskSet->add(objFilterTasks);
      if (paircreation) masterTaskSet->add(pairAnalysisTasks);
      if (pca)          masterTaskSet->add(pcaTasks);
      if (hgeantana)    masterTaskSet->add(HGeantAnalysisTasks);
      if (histfiller)   masterTaskSet->add(histoTasks);
      if (pid)          masterTaskSet->add(pidTasks);
      if (qaMaker)      masterTaskSet->add(qaMaker);
      
  }
  
  
  gHades->getTaskSet(evtType.Data())->isTimed(kTRUE);
  gHades->makeCounter(5000);
  
  
  //   initialization 
  if (!gHades->init()) 
  {
      printf("Error in initialization, exiting\n");
      delete myHades;
      return kFALSE;
  }
  
  
  //  set persistent categories
if (createTreeFile && writeOnlySelectedCats)
  {
      HEvent *event = gHades->getCurrentEvent();
      for(Int_t i=0;i<500;i++)
      {
	  if (event->getCategory(i)) 
	      event->getCategory(i)->setPersistency(kFALSE);
      }
  
      for(UInt_t j=0;j<sizeof(PersistentCat)/sizeof(Cat_t);j++)
      {
	  if (event->getCategory(PersistentCat[j]))
	      ((HCategory *)event->getCategory(PersistentCat[j]))->setPersistency(kTRUE);
      }
  }
  if (createTreeFile)
  {
      //   set output file
      gHades->setOutputFile(outputFile.Data(),"RECREATE","Test",2);
      gHades->makeTree();
  }
  
  
  
  //   event loop  
  printf("Processing %i events...\n",maxEvts);
  timer.Reset();
  timer.Start();
  if (nEvtsToProcess<1) 
  {
      evN=gHades->eventLoop();
  } 
  else 
  {
      evN=gHades->eventLoop(nEvtsToProcess,startEvt);
  }
  timer.Stop();
  
  
  
  //   write RTDB containers init by tasks to file
  if (paramSource.Contains("ORACLE") && kDumpParam)
  {
      if (!rtdb->initContainers(refId))
	  Error("dump params","RTDB not initialized for ref run");
      
      rtdb->writeContainers();
      rtdb->setContainersStatic();
      rtdb->saveOutput();
      rtdb->print();
  }  
  

  //------------------------------------------------------------------
  if (evN) gHades->getTaskSet(evtType.Data())->printTimer();
  
  //------------------------------------------------------------------
  
  delete myHades;
  
  //------------------------------------------------------------------
  printf("------------------------------------------------------\n");
  if (evN)
  {
      printf("Events processed\t: %i\n",evN);
      printf("Real time\t\t: %f\n",timer.RealTime());
      printf("Cpu time\t\t: %f\n",timer.CpuTime());
      printf("Performance\t\t: %f cpu-s/ev\n",timer.CpuTime()/evN);
      printf("Performance (real)\t: %f ev/s\n",evN/timer.RealTime());
      
      return kTRUE;
  }
  else
  {
      cout<<"ERROR: no events processed"<<endl;
      return kFALSE;
      
  }
}
