#ifndef __CINT__
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>
#include <unistd.h> 

#include "TROOT.h"
#include "TError.h"
#include "TFile.h"
#include "TDatime.h"
#include "TStopwatch.h"
#include "TUnixSystem.h"
#include "TObject.h"

#include "TApplication.h"
#include "hades.h"

#include "hmdcdef.h"
#include "haddef.h"

#include "htree.h"
#include "hmatrixcategory.h"

#include "hspectrometer.h"
#include "hmdcdetector.h"

#include "hruntimedb.h"
#include "htaskset.h"

#include "hmdcunpacker.h"
#include "hldfilesource.h"
#include "hldrfiofilesource.h"
//#include "hldsockremotesource.h"

#include "hrecevent.h"
#include "hparset.h"

#include "hparoraio.h"
#include "hmdcparoraio.h"
#include "horainfo.h"

#include "hparasciifileio.h"
#include "hparrootfileio.h"

#include "hmdcgeomstruct.h"
#include "hmdcrawstruct.h"
#include "hmdccalparraw.h"
#include "hmdccalpar.h"
#include "hmdclookupraw.h"
#include "hmdcconnectedwires.h"

#include "hmdccalibrater1.h"
#include "hmdcslope.h"
#include "hptools.h"
#include "ptools.h"
#endif

#include "calibrate.h"

#ifndef __CINT__
Int_t main(Int_t argc, Char_t *argv[])
{
  TROOT Calibrate("calibrate","Programm to calculate offline slopes for the calparraw container");
  if (getopt(argc,argv,"h")>=0) exit(calibrateHelp(gSystem->BaseName(argv[0])));

  switch(argc)
    {
    case 1:
      return calibrateHelp(gSystem->BaseName(argv[0]));
      break;
    case 2:
      return calibrate(ORACLE_DATABASE_INPUT, -1, RFIO_SOURCE, argv[1], "./" , "./", "sep05");
      break;
    case 3:
      return calibrate(ORACLE_DATABASE_INPUT, -1, RFIO_SOURCE, argv[1], "./", argv[2], "sep05");
      break;
    case 4:
      return calibrate(ORACLE_DATABASE_INPUT, -1, RFIO_SOURCE, argv[1], argv[3], argv[2], "sep05");
      break;
    case 5:
      if (atoi(argv[4])!=0)
	{
	  return calibrate(ORACLE_DATABASE_INPUT, atoi(argv[4]) , RFIO_SOURCE, argv[1], argv[3], argv[2], "sep05");
	}
      else
	{
	  return calibrateHelp(gSystem->BaseName(argv[0]));
	}
      break;
    case 6:
      if (atoi(argv[4])!=0 && atoi(argv[5])!=0)
	{
	  return calibrate(atoi(argv[5]), atoi(argv[4]) , RFIO_SOURCE, argv[1], argv[3], argv[2], "sep05");
	}
      else
	{
	  return calibrateHelp(gSystem->BaseName(argv[0]));
	}
      break;
    case 7:
      if (atoi(argv[4])!=0 && atoi(argv[5])!=0 && atoi(argv[6])!=0)
	{
	  return calibrate(atoi(argv[5]), atoi(argv[4]) , atoi(argv[6]), argv[1], argv[3], argv[2], "sep05");
	}
      else
	{
	  return calibrateHelp(gSystem->BaseName(argv[0]));
	}
      break;
    case 8:
      if (atoi(argv[4])!=0 && atoi(argv[5])!=0 && atoi(argv[6])!=0)
	{
	  return calibrate(atoi(argv[5]), atoi(argv[4]) , atoi(argv[6]), argv[1], argv[3], argv[2], argv[7]);
	}
      else
	{
	  return calibrateHelp(gSystem->BaseName(argv[0]));
	}
      break;
    default:
      return calibrateHelp(gSystem->BaseName(argv[0]));
      break;
    }
}
#endif

Int_t calibrateHelp(const Char_t *command)
{
      printf("\nUsage:");
      printf("\t%s inputFileName.hld     [outputDirName=\"./\"] [inputDirName=\"./\"]     [number of events=-1] [mode=%i] [datasource=%i] [beamtime=\"sep05\"]\n\n",command,ORACLE_DATABASE_INPUT,RFIO_SOURCE);
      printf("\t%s inputFileListName.txt [outputDirName=\"./\"] [inputListDirName=\"./\"] [number of events=-1] [mode=%i] [datasource=%i] [beamtime=\"sep05\"]\n\n",command,ORACLE_DATABASE_INPUT,RFIO_SOURCE);
      printf("\t%s remoteEventServerName [outputDirName=\"./\"] [dummy=\"./\"]            [number of events=-1] [mode=%i] [datasource=%i] [beamtime=\"sep05\"]\n\n",command,ORACLE_DATABASE_INPUT,RFIO_SOURCE);
      printf("\t\tmode = %i ->> input from ORACLE (default)\n",ORACLE_DATABASE_INPUT);
      printf("\t\tmode = %i ->> input from ASCII file\n"      ,ASCII_FILE_INPUT);
      printf("\t\tmode = %i ->> input from ROOT file\n\n"     ,ROOT_FILE_INPUT);
      printf("\t\tdatasource = %i --> hld file source\n",HLD_FILE_SOURCE);
      printf("\t\tdatasource = %i --> RFIO hld file source\n",RFIO_SOURCE);
      printf("\t\tdatasource = %i --> Remote event source (not implemented)\n\n",REMOTE_SOURCE);
      printf("\tcalibrate -h \t: for this help\n\n");
      printf("\t\t -- if a input list txt-file is used the file should contain line by line a filename\n");
      printf("\t\t -- in case of hld file source input the full path has to be included\n\n");
      printf("\n\t The task of this program is to produce the slope calibration for the MDCs of HADES.\n");
      printf("\t Input: raw listmode data in form of hld-files from disk or robot or from a remote event server.\n");
      printf("\t Analysis: The raw data unpacked and the calibration events inside are analyzed.\n");
      printf("\t           Based on this analysis for each calibrated channel a slope value is deduced. \n");
      printf("\t           Uncalibrated channels get a default value of 0.5 ns/ch. \n");
      printf("\t Output: As output a textfile containing the database formatted list of slopes and \n");
      printf("\t         in addition an ntuple file containing each calculated slope value\n\n");
      return EXIT_SUCCESS;
}

Int_t calibrate(Int_t inputMode , Int_t numberOfEvents ,
	      Int_t datasource, TString inputFileName, TString inputDirName, TString outputDirName,
	      TString beamtime)
{
  /*
   cout << "inputMode      " << inputMode << endl;
   cout << "numberOfEvents " << numberOfEvents << endl;
   cout << "datasource     " << datasource << endl;
   cout << "inputFileName  " << inputFileName << endl;
   cout << "inputDirName   " << inputDirName << endl;
   cout << "outputDirName  " << outputDirName << endl;
   */

  // check valid datasource HLD_FILE_SOURCE, REMOTE_SOURCE, RFIO_SOURCE
    if (
	!(HLD_FILE_SOURCE == datasource) &&
	!(RFIO_SOURCE     == datasource) &&
	!(REMOTE_SOURCE   == datasource)
       )
    {
	Error("check datasource mode","mode %i not valid as datasource mode ... exiting!",datasource);
        return EXIT_FAILURE;
    }

    // check inputFileName ending
    if (! inputFileName.IsNull())
    {
	if (! (inputFileName.EndsWith(".txt") || inputFileName.EndsWith(".hld")))
	{
	    Error("inputFileName",
		  "input file name: %s does not end with \".hld\" or \".txt\" (list mode) ... exiting!",
		  inputFileName.Data());
                  return EXIT_FAILURE;
	}
    }

  //  enum mode   {ORACLE_DATABASE_INPUT = 0, ASCII_FILE_INPUT = 1, ROOT_FILE_INPUT = 2};

  // ----------------------------------------------
  // ----- operational field ----------------------
  // ----------------------------------------------

  Bool_t trendCalculationActive = kTRUE; // offline
  Bool_t createNtupleActive = kTRUE; //
  Bool_t rootOutput  = kFALSE; //
  Bool_t writeTree = kFALSE;

  Bool_t linRegActive   = kTRUE;
  Bool_t histFitActive  = kFALSE;
  Bool_t graphFitActive = kFALSE;

  Int_t  binError       = 04;
  Bool_t selectorActive = kFALSE;
  Int_t  mbNumber       = -1;   // -1 takes all
  Int_t  tdcNumber      = -1;   // -1 takes all

  Int_t startEvent=0; // due to have an original file for initalisation...

  TString tapeDirectory = beamtime + "/hld"; // used for RFIO_SOURCE
  TString robotName="gsitsma";               // used for RFIO_SOURCE
  TString archiveName="hadesraw";            // used for RFIO_SOURCE

  Int_t refRunId=-1;
  //refRunId=482031944; //sep05 runID

  // ----------------------------------------------
  // ----------------------------------------------

  reset();

  suffix="";
  if (!inputFileName.IsNull())
  {
      suffix=gSystem->BaseName(inputFileName.Data());
      if (suffix.EndsWith(".hld") || suffix.EndsWith(".txt"))
      { suffix = suffix(0,suffix.Last('.')-1);}
  }
  else
  {
      suffix=beamtime;
  }
  outputDirName+=Form("/%s",suffix.Data());
  outputDirName.ReplaceAll("//","/");
  outputDirName.ReplaceAll("//","/");
#ifdef __CINT__
  suffix+="_cint";
#else
  suffix+="";
#endif

  // -------------------------------------------------

  if (selectorActive) {suffix+=Form("_MB%i-TDC%i",mbNumber,tdcNumber);}

  // -------------------------------------------------

  sprintf(rtdbOutput,         "%s/%s%s%s",outputDirName.Data(),"mdc_"        ,suffix.Data(),".root");
  sprintf(calParRawOutputFile,"%s/%s%s%s",outputDirName.Data(),"calpar_"     ,suffix.Data(),".txt" );
  sprintf(outputFile         ,"%s/%s%s%s",outputDirName.Data(),"cal_"        ,suffix.Data(),".root");
  sprintf(outputTitle        ,"%s/%s%s"  ,outputDirName.Data(),"Calibration ",suffix.Data()        );
  sprintf(mdcSlopeOutputFile ,"%s/%s%s%s",outputDirName.Data(),"slope_"      ,suffix.Data(),".root");
  sprintf(ntupleOutputFile   ,"%s/%s%s%s",outputDirName.Data(),"ntuple_"     ,suffix.Data(),".root");

  //create outputDir
  gSystem->mkdir(outputDirName.Data(),kTRUE);
  Info("mkdir","created outputDirectory %s",outputDirName.Data());

  // ------------ Stopwatch --------------------------
  TStopwatch *stopwatch = new TStopwatch();
  stopwatch->Reset();
  stopwatch->Start();

  // ------------ hades and run time database --------

  Hades *myHades = new Hades;
  HRuntimeDb    *rtdb = myHades->getRuntimeDb();
  myHades->setTreeBufferSize(8000); //very important. Do not remove it !!!!!!!

  // ------------ MDC Detector setup -----------------
  HPTools::mdcDetectorSetup(beamtime.Data());

  // ------------ Paramaters Sources ------------------
  
  sprintf(ZeroAsciiInputFile,"%s",
	  "defaultcalparraw.txt");
  //	  "calparrawNoSlopesNoOffsetsNov02.txt");
  sprintf(asciiInputFile,"%s",
	  "/u/mdc/lookupfiles/november2001/lookup_mdc_nov01_061201_valid_from_041201-12pm.txt");
  sprintf(rootInputFile, "%s",
	  "/u/mdc/lookupfiles/november2001/mdcNov01_341_080202.root");

  // ------------------------ INPUT SECTION #

  Bool_t ASCII_INPUT_ACTIVE = kFALSE;
  Bool_t ROOT_INPUT_ACTIVE  = kFALSE;
  Bool_t ORA_INPUT_ACTIVE   = kFALSE;

  switch(inputMode)
    {
    case ORACLE_DATABASE_INPUT: 
      ORA_INPUT_ACTIVE = kTRUE;
      break;
      
    case ASCII_FILE_INPUT: 
      ASCII_INPUT_ACTIVE = kTRUE; 
      break;
      
    case ROOT_FILE_INPUT:  
      ROOT_INPUT_ACTIVE = kTRUE;
      break;
      
    default:
      Error("parameter input","No valid input chosen\nexiting...");
      exit(EXIT_FAILURE);
      break;
    }

  // --------- RTDB INPUT --------------
  // ------------------- ASCII INPUT ---

  HParAsciiFileIo *asciiInp1= new HParAsciiFileIo;
  asciiInp1->open(ZeroAsciiInputFile,"in");

  rtdb->setFirstInput(asciiInp1);

  if (ASCII_INPUT_ACTIVE)
    {
      HParAsciiFileIo *asciiInp2= new HParAsciiFileIo;
      asciiInp2->open(asciiInputFile,"in");
      rtdb->setSecondInput(asciiInp2);
    }

  // ------------------- ROOT INPUT -----
 
  if (ROOT_INPUT_ACTIVE)
    {
      HParRootFileIo  *RootInp  = new HParRootFileIo;
      RootInp->open(rootInputFile,"in");
      rtdb->setSecondInput(RootInp);
    }

  // ------------------- ORACLE INPUT ---

  if (ORA_INPUT_ACTIVE)
    {
      HParOraIo *oraInp = new HParOraIo ;
      oraInp->open();
      if (!oraInp->check()) 
	{
	  printf("no connection to Oracle\n");
	  return EXIT_FAILURE;
	}
      rtdb->setSecondInput(oraInp);
    }

  // --------------------------------------------------
  // input files >> file sources
  // --------------------------------------------------
  HldSource *source = 0;
  
  switch(datasource)
    {
    case HLD_FILE_SOURCE:
      {
	  source  = new HldFileSource;

	  {
	      if (ORA_INPUT_ACTIVE || ROOT_INPUT_ACTIVE || ASCII_INPUT_ACTIVE)
	      {
		  HPTools::addSourceList(source, inputFileName, inputDirName, refRunId);
	      }
	      else
	      {
		  Error("file sources","no input files defined ... exiting");
                  return EXIT_FAILURE;
	      }
	  }
      }
      break;
     case REMOTE_SOURCE:
       {
	   Error("data sources","REMOTE source mode not available ... exiting");
           return EXIT_FAILURE;
	   //source =  new HldSockRemoteSource(inputFileName.Data());
       }
       break;
    case RFIO_SOURCE:
	{
	    source = new HldRFIOFileSource();

	    ((HldRFIOFileSource *)source)->setArchive(archiveName.Data());
	    ((HldRFIOFileSource *)source)->setRobot(robotName.Data());
	    ((HldRFIOFileSource *)source)->setDirectory((Text_t*)tapeDirectory.Data());

	    HPTools::addSourceList(source, inputFileName, inputDirName);
	}
      break;
    default:
      Warning("calibrate","no input source defined,\n exiting...");
      return EXIT_FAILURE;
    }

  myHades->setDataSource(source);
 
 //defaults:
  Bool_t debug = kFALSE;       // FALSE/TRUE = do NOT/DO create unpack_error.root
  Bool_t consistency = kFALSE; // FALSE/TRUE = do NOT/DO PRINT out token/hit errors to screen.
  Bool_t standalone=kFALSE;    // Bool_t standalone=kFALSE;
  
  debug = kTRUE;      
  consistency = kTRUE; 
  //standalone = kTRUE;    

  Int_t samNr[]={ 201,202,203,204,205,206,
		    207,208,209,210,211,212,
		    213,214,215,216,217,218,
		    219,220,221,222,223,224
		    };
  
  Int_t numUnp=(Int_t) sizeof(samNr)/sizeof(Int_t);

  HMdcUnpacker *unpackerList[numUnp];

  for (Int_t mysamiter=0;mysamiter<numUnp;mysamiter++)
    {
      unpackerList[mysamiter] = new HMdcUnpacker(samNr[mysamiter], debug, consistency);	
      if (standalone) unpackerList[mysamiter]->setStandalone(standalone);
      unpackerList[mysamiter]->setPersistencyRawEventHeader();
      unpackerList[mysamiter]->setPersistencyDataWord();
      unpackerList[mysamiter]->setDecodeVersion(1);
      unpackerList[mysamiter]->setQuietMode();

      source->addUnpacker(unpackerList[mysamiter]);
   }
  
  // ----------------------------------------------------------------
  // --------- Set Slope Class Properties -----------------------
  // ----------------------------------------------------------------

  HMdcSlope *mdcSlope = new HMdcSlope("MdcSlope", "MdcSlope");

  mdcSlope->setTrendCalculationActive(trendCalculationActive);

  if (rootOutput)     mdcSlope->setOutputRoot(mdcSlopeOutputFile);
  mdcSlope->setBinError(binError);

  // selector
  if (selectorActive)
    { 
      mdcSlope->setMbSelector(mbNumber);
      mdcSlope->setTdcSelector(tdcNumber);
      mdcSlope->setSelectorActive(selectorActive);
    }

  mdcSlope->setLinRegActive(linRegActive);
  mdcSlope->setHistFitActive(histFitActive);
  mdcSlope->setGraphFitActive(graphFitActive);

  mdcSlope->setCreateNtupleActive(createNtupleActive);
  if (createNtupleActive) 
    {
      mdcSlope->setOutputNtuple(ntupleOutputFile);
    }
  
  // ----------------------------------------------------------------
  // ------ Tasks ---------------------------------------------------
  // ----------------------------------------------------------------

  HTaskSet *mdctasks = new HTaskSet("MDC","MDC Slope");
  
  mdctasks->connect(mdcSlope);
  mdctasks->connect((HTask*)NULL,(Text_t*)"MdcSlope");
  
  HTaskSet *calibrationTaskSet = NULL;
  calibrationTaskSet = myHades->getTaskSet("calibration");
  
  if (calibrationTaskSet == NULL) 
    { 
      Error("TaskSet","TaskSet not existing ... exiting...");
      exit(EXIT_FAILURE);
    }

  calibrationTaskSet->connect(mdctasks);
  calibrationTaskSet->print();

  // ----------------------------------------------------------------  
  // Init
  // ----------------------------------------------------------------

  myHades->makeCounter(1000);

   // ------------ Hades init -------------------------

  HPTools::hadesInit();
 
  // ----------------------------------------------------------------
  if (writeTree)
    {
      Int_t compLevel = 2;
      myHades->setOutputFile(outputFile,"RECREATE",outputTitle,compLevel);
      
      // ----- Built output tree
      printf("Make Tree\n");
      myHades->makeTree();
    }

  // ------------ Event Loop -------------------------
  printf("Event loop\n");
  HPTools::hadesEventLoop(numberOfEvents,startEvent);
  // ----- end of Event Loop

  printf("Preparing Ascii File Output\n");
  
  // ----- Ascii File output
  HParAsciiFileIo *asciiOutputFile = new HParAsciiFileIo;
  asciiOutputFile->open(calParRawOutputFile, "out");

  HMdcCalParRaw* calparraw = (HMdcCalParRaw*) rtdb->getContainer("MdcCalParRaw");
  
  cout << "writing HMdcCalParRaw to >> " << calParRawOutputFile << endl;
  calparraw->write(asciiOutputFile);
  
  asciiOutputFile->close();
  delete asciiOutputFile;
  
  //  rtdb->print(); 
  
  rtdb->closeOutput();
  
  stopwatch->Stop();
  PTools::evalStopwatch(stopwatch, numberOfEvents);

  //  myHades->Dump();

  //delete myHades;
  return EXIT_SUCCESS;
}
 
Bool_t reset(void)
{
  sprintf(setupPath,"%s","calibrate-setup.txt");

  sprintf(referenceRunIdTag,       "%s","referenceRunId");
  sprintf(CalibrationFileNamesTag, "%s","CalibrationFileNames");
  sprintf(InputDirectoriesTag,     "%s","InputDirectories");
  sprintf(OutputDirectoriesTag,    "%s","OutputDirectories");
  sprintf(DefaultReferenceRunIdTag,"%s","DefaultReferenceRunId");
  sprintf(OracleReferenceRunIdTag, "%s","OracleReferenceRunId");
  sprintf(RootReferenceRunIdTag,   "%s","RootReferenceRunId");
  sprintf(DetectorSetupTag,        "%s","DetectorSetup");
  sprintf(unpackerArgumentsTag,    "%s","unpackerArguments");
  sprintf(specUnpackerArgumentsTag,"%s","specUnpackerArguments");
  sprintf(calcOptsTag,             "%s","calcOpts");
  sprintf(fitOptsTag,              "%s","fitOpts");
  sprintf(selectOptsTag,           "%s","selectOpts");
  sprintf(startEventTag,           "%s","startEvent");       

  //  clear(void);
  return EXIT_SUCCESS;
}

Bool_t access(void)
{    
  return (access(setupPath, F_OK)==0); // file exists
}

TList* parseSetupForFilename()
{
  FILE *file;
  file = fopen(setupPath,"r");
  fclose(file);
  return NULL;
}

