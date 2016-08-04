#include "unpacker.h"
#include "hptools.h"
#include "ptools.h"

Char_t asciiFile[] = "/misc/mdc/MdcMon/Calibration/rtdb-spt03-mod.txt";
Char_t  rootFile[] = "/misc/mdc/lookupfiles/november2001/mdcNov01_341_080202.root";
Int_t smartRunId = 1;

void unpacker(Char_t indir[200], Char_t inf[200], Char_t outdir[200], Int_t nev, UInt_t startev, Bool_t RemoteSource)
{
#ifdef __CINT__
#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1
#endif

  // do not modify here, but in the input section!

  Int_t refRunId=-1;             // default/noRefRunId: -1;
  Bool_t includeDate=kFALSE;
  Int_t primaryInput  = OFF;     // default OFF        enum {OFF=0,ORAFILE=1,ASCIIFILE=2,ROOTFILE=3}
  Int_t secondaryInput= ORAFILE; // default ORAFILE,   enum {OFF=0,ORAFILE=1,ASCIIFILE=2,ROOTFILE=3}
  Int_t reprate=OFF;             // default: OFF
  Int_t datalevel=CALLEVEL;      // default: CALLEVEL, enum {RAWLEVEL=1,CALLEVEL=2}
                                 //          don't change, w/o no tree will be written out
  TString beamtime="";

  Bool_t FileGrep = kFALSE;
#ifdef FILEGREP
#warning using HldFileGrepSource instead
  FileGrep=kTRUE;
#endif

  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////
  //----------------------  Start of Input Section  ----------------------//
  //////////////////////////////////////////////////////////////////////////
  
  includeDate = kFALSE;     // default kFALSE
  primaryInput  = OFF;      // default OFF      enum {OFF=0,ORAFILE=1,ASCIIFILE=2,ROOTFILE=3}
  secondaryInput = ORAFILE; // default ORAFILE, enum {OFF=0,ORAFILE=1,ASCIIFILE=2,ROOTFILE=3}
  //  refRunId=1;

  reprate=1000;             // default: OFF // 1000
  beamtime = "APR06";
  smartRunId = 1;

  //////////////////////////////////////////////////////////////////////////
  //-----------------------  End of Input Section  -----------------------//
  //////////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  // ------------ starting the timer
  TStopwatch timer;
  timer.Reset();
  timer.Start();

  printf("Processing events...\n");

  // ------------ string operations for input and output file names
  Char_t infile[200],outfile[200],node[200],temp[200];
  PTools::cutEnd(inf,".hld");
  
  sprintf(node,"%s",indir);
  
  TDatime mytime;
  TString *datum = new TString(mytime.AsSQLString());
  datum->ReplaceAll(":","-");   
  datum->ReplaceAll(" ","_");
  nev = nev+1;
  sprintf(infile,"%s%s",inf,".hld");

  switch(datalevel)
    {
    case CALLEVEL:
      if (includeDate) sprintf(temp,"%s_%s_%s%s",inf,"cal1",datum->Data(),".root");
      else sprintf(temp,"%s_%s%s",inf,"cal1",".root");
      break;
    case RAWLEVEL:
      if (includeDate) sprintf(temp,"%s_%s_%s%s",inf,"raw",datum->Data(),".root");
      else sprintf(temp,"%s_%s%s",inf,"raw",".root");
      break;
    default:
      Error("Datalevel"," datalevel %i does not exist! exiting ...",datalevel);
      exit(0);
    }
  printf("%s\n",temp);
  
  if (RemoteSource)
    {
      printf("\n%s %s\n",">>> Remote Input Node:",node);
    }
  else if (FileGrep)
    {
      printf("\n%s %s\n",">>> Input directory:",indir);
    }
  else
    {
      printf("\n%s %s/%s\n",">>> Input:",indir,infile);
    }

  sprintf(outfile,"%s%s%s",outdir,"/",temp);
  printf("%s %s\n\n",">>> Output:",outfile);
    
  // ------------ hades and run time database --------

  Hades *myHades   = new Hades();
  myHades->setTreeBufferSize(8000); //very important. Do not remove it !!!!!!!
  HRuntimeDb *rtdb = myHades->getRuntimeDb();

  // ------------ MDC Detector setup -----------------
  HPTools::mdcDetectorSetup(beamtime.Data());

  // -------------------------------------------------

  HParAsciiFileIo output;
  output.open("rtdb.txt","out");
  rtdb->setOutput(&output);
  
  // ------------ setting the parameter files --------

  HParOraIo *oracle = 0;
  oracle = setParameterSource(rtdb, primaryInput, secondaryInput);  

  // ------------ setting the hld source -------------

  HldSource *source;

  if ( RemoteSource )
    {
      source = new HldSockRemoteSource(node);
    }
  else if (FileGrep)
    {
      source=new HldGrepFileSource(indir,
				   "Grep",
				   5,
				   getExistingRefRunId(infile,
						       beamtime,
						       primaryInput,
						       secondaryInput,
						       refRunId,
						       oracle),
				   10); // directory, grep interval 5s, refId =1 , file should be older than 5s
    }
  else
    {
      source = new HldFileSource;
    }

  myHades->setDataSource(source);
  

  // ------------ unpacker settings ------------------

  //defaults:
  Bool_t debug = kFALSE;       // FALSE/TRUE = do NOT/DO create unpack_error.root
  Bool_t consistency = kFALSE; // FALSE/TRUE = do NOT/DO PRINT out token/hit errors to screen.
  Bool_t standalone = kFALSE;  // Bool_t standalone=kFALSE;
  
  debug = kTRUE;      
  consistency = kTRUE; 
  // standalone = kTRUE;

  Int_t samNr[]={201,202,203,204,205,206,
                 207,208,209,210,211,212,
		 213,214,215,216,217,218,
		 219,220,221,222,223,224};
  Int_t numUnp=(sizeof(samNr)/sizeof(Int_t));
 
  HMdcUnpacker *unpackerList[24];
  
  for (Int_t mysamiter=0;mysamiter<numUnp;mysamiter++)
  {
      unpackerList[mysamiter] = new HMdcUnpacker(samNr[mysamiter], debug, consistency);
      if (standalone) unpackerList[mysamiter]->setStandalone(standalone);
      unpackerList[mysamiter]->setPersistencyRawEventHeader();

#ifdef BLACKLIST
#warning --- BlackList ---
      TString beamTime = beamtime;
      beamTime.ToLower();
      unpackerList[mysamiter]->enableExcludeBlackList(beamTime);
#endif
#ifdef RECOVERBLACKLIST
#warning --- RecoverBlackList ---
      TString beamTime2 = beamtime;
      beamTime2.ToLower();
      unpackerList[mysamiter]->enableRecoverBlackList(beamTime2);
#endif

#ifdef CLEANBAD
#warning --- CleanBadSubEventsAfterInconsistency ---
#warning --- not implemented: unpackerList[mysamiter]->setCleanBadSubEventsAfterInconsistency();
#endif

#ifdef STUBBORN
#warning --- ContinueDecodingAfterInconsistency ---
      unpackerList[mysamiter]->setContinueDecodingAfterInconsistency();
#endif

#ifdef QUIET
#warning --- QuietMode ---
      unpackerList[mysamiter]->setQuietMode();
#endif

#ifdef NASTY
#warning --- NastyDebugMode ---
      unpackerList[mysamiter]->setUnpackerDebug();
#endif
      source->addUnpacker(unpackerList[mysamiter]);
  }


  // ------------ adding files -----------------------

  if ( !RemoteSource && !FileGrep)
    {
      ((HldFileSource *)source)->setDirectory(indir);
      ((HldFileSource *)source)->addFile(infile,
      					 getExistingRefRunId(infile, beamtime, primaryInput, secondaryInput, refRunId, oracle));
    }
  else
  {
    if (RemoteSource) rtdb->addRun(1); // !!!!! still valid ????
  }

  // ------------ tasklist ---------------------------
  HMdcTaskSet *myMdcTaskSet = 0;
  HTask *mdcTask = 0;
  HSUDummyRec *dummyRec=0;

  if(datalevel == CALLEVEL)
    {
      myMdcTaskSet = new HMdcTaskSet();
      //    mdcTask = myMdcTaskSet->make("","cal1,real,notimecuts,nostartandnocal");
      //    mdcTask = myMdcTaskSet->make("","cal1,real,timecuts,nostartandcal");
      mdcTask = myMdcTaskSet->make("","cal1,real,notimecuts,nostartandcal");
      myHades->getTaskSet("real")->add(mdcTask);
      myHades->getTaskSet("calibration")->add(mdcTask);
    }
  if(datalevel == RAWLEVEL)
    {
      dummyRec = new HSUDummyRec(kFALSE);
      dummyRec->setInitOk();
      myHades->getTaskSet("calibration")->add(dummyRec);
      myHades->getTaskSet("real")->add(dummyRec);
    }
  // ------------ init Container for remote source ---

  if (RemoteSource) 
    {
      rtdb->initContainers(1);
      rtdb->setContainersStatic();
    }
  
  // ------------ Counter ----------------------------

  myHades->makeCounter(reprate);

  // ------------ Hades init -------------------------

  HPTools::hadesInit();
  
  // ------------ set output file --------------------

  Int_t compLevel = 2;
  myHades->setOutputFile(outfile,"RECREATE","unpacker",compLevel);
  cout << "root output file allocated!" << endl;
  
  // ------------ Built output tree ------------------
  printf("Make Tree\n");
  myHades->makeTree();
  
  // ------------ Event Loop -------------------------
  printf("Event loop\n");

  HPTools::hadesEventLoop(nev-1,startev);
  
  // ------------ Runtime database output ------------
  rtdb->saveOutput();
  rtdb->closeFirstInput();
  rtdb->closeSecondInput();

  // -------------------------------------------------
  cout << "File written to >>> " << outfile << endl;
  // ------------ delete the hades object ------------
  delete myHades;

  // ------------ stopwatch evaluation ---------------
  timer.Stop();
  PTools::evalStopwatch(&timer,nev-1);
  
  return;
}

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
///// sub routines /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Int_t getExistingRefRunId(TString inputfile, TString beamtime, Int_t primaryFile, Int_t secondaryFile, Int_t refRunId, HParOraIo *oracle)
{
  Int_t runId = -1;
  if (primaryFile != ROOTFILE && secondaryFile != ROOTFILE)
    {
      if (refRunId < 0)
	{
	  if (oracle) 
	    {
	      HOraInfo *oracleInfo=0;
	      oracleInfo = oracle->getOraInfo();
	      if (inputfile.EndsWith(".hld"))
		{
		  runId = oracleInfo->getRunId((Text_t*)inputfile.Data());
		  if (runId==-1)
		    {
		      if (smartRunId)
			{
			  runId = oracleInfo->getLastRun((Text_t*)beamtime.Data());
			  Info("getExistingRefRunId",
			       "via oracleInfo::getLastRun retrieved runId: %i for beamtime %s",
			       runId, beamtime.Data());
			  return runId;
			}
		    }
		}
	    }
	}
    }
  return refRunId;
}

HParOraIo* setParameterSource(HRuntimeDb* rtdb, Int_t primaryInput, Int_t secondaryInput)
{
  HParIo *inp1=0;
  HParIo *inp2=0;
  HParOraIo *ora=0;
  if (primaryInput)
    {
      switch (primaryInput)
	{
	  {
	  case ORAFILE:
	    ora = new HParOraIo ;
	    ora->open();
	    if (!ora->check()) {
	      Error("ora->check","no connection to Oracle\nexiting...!");
	      exit(EXIT_FAILURE);}
	    rtdb->setFirstInput(ora);
	    ora->setHistoryDate("now");
	    if (secondaryInput==ORAFILE) secondaryInput=OFF;
	    break;
	  case ASCIIFILE:
	    // ------------ ascii file info -------------------
	    inp1 = new HParAsciiFileIo;
	    ((HParAsciiFileIo*) inp1)->open(asciiFile);
	    rtdb->setFirstInput(inp1);
	    break;
	  case ROOTFILE:
	    // ------------ root file info --------------------
	    inp1 = new HParRootFileIo;
	    ((HParRootFileIo*) inp1)->open(rootFile);
	    rtdb->setFirstInput(inp1);
	    break;
	  default:
	    Error("unpacker","No valid:%i input chosen\nexiting...",primaryInput);
	    exit(EXIT_FAILURE);
	  }
	}
    }
  if (secondaryInput)
    {
      switch (secondaryInput)
	{
	case ORAFILE:
	  ora = new HParOraIo ;
	  ora->open();
	  if (!ora->check()) {
	    Error("ora->check","no connection to Oracle\nexiting...!");
	    exit(EXIT_FAILURE);}
	  rtdb->setSecondInput(ora);
	  ora->setHistoryDate("now");
	  break;
	case ASCIIFILE:
	  // ------------ ascii file info -------------------
	  inp2 = new HParAsciiFileIo;
	  ((HParAsciiFileIo*) inp2)->open(asciiFile);
	  rtdb->setSecondInput(inp2);
	  break;
	case ROOTFILE:
	  // ------------ root file info --------------------
	  inp2 = new HParRootFileIo;
	  ((HParRootFileIo*) inp2)->open(rootFile);
	  rtdb->setSecondInput(inp2);
	  break;
	default:
	  Error("unpacker","No valid:%i input chosen\nexiting...",secondaryInput);
	  exit(EXIT_FAILURE);
	}
    }
  return ora;
}

#ifndef __CINT__

Int_t main(Int_t argc, Char_t **argv)
{
  TROOT Unpacker("Unpacker","compiled unpacker macros for mdc");
  switch (argc)
    {
    case 1:
      unpacker();
      return EXIT_SUCCESS;
      break;
    case 2:
      unpacker(argv[2]);
      return EXIT_SUCCESS;
      break;
    case 4:
      if (atoi(argv[3])!=0)
	{
	  unpacker(argv[1],argv[2],atoi(argv[3]));
	  return EXIT_SUCCESS;
	}
      else
	{
	  unpacker(argv[1],argv[2],argv[3]);
	  return EXIT_SUCCESS;
	}
    case 5:
      unpacker(argv[1],argv[2],argv[3],atoi(argv[4]));
      return EXIT_SUCCESS;
    case 6:
      unpacker(argv[1],argv[2],argv[3],atoi(argv[4]),atoi(argv[5]));
      return EXIT_SUCCESS;
      break;
    default:
      printf("Usage:\n");
      printf("\t%s %s %s\n","hldFile:",gSystem->BaseName(argv[0]),"indirectory inputfile outdirectory [number of events [start evt]]");
      printf("\t%s %s %s\n","remote :",gSystem->BaseName(argv[0]),"node outdirectory number_of_events");
      printf("\t%s %s %s\n","command:",gSystem->BaseName(argv[0]),"commandfilename");
      printf("\t%s %s %s\n","fileGrep:",gSystem->BaseName(argv[0]),"indirectory dummyfilename outdirectory [number of events [start evt]]");
      return EXIT_SUCCESS;
    }
}
#endif

void unpacker()
{
  printf("\n%s\n","Usage:");
#ifdef __CINT__ 
  printf("\t%s\n","hldFile: unpacker.cc(indirectory, inputfile, outdirectory, [number of events [start evt]])");
  printf("\t%s\n","fileGrep: unpacker.cc(indirectory, dummyfilename, outdirectory, [number of events [start evt]])");
  printf("\t%s\n","remote : unpacker.cc(node, outdirectory, number of events,kTRUE)");
  //  printf("\t%s\n","command: unpacker.cc(commandfilename)");
#else
  printf("\t%s\n","hldFile: unpacker indirectory inputfile outdirectory [number of events [start evt]]");
  printf("\t%s\n","fileGrep: unpacker indirectory dummyfilename outdirectory [number of events [start evt]]");
  printf("\t%s\n","remote : unpacker node outdirectory number_of_events");
  //   printf("\t%s\n","command: unpacker commandfilename ");
#endif
  printf("\n");
}

void unpacker(Char_t node[200],Char_t *outdirectory, Int_t nevts)
{
  unpacker(node, node, outdirectory, nevts, kTRUE);
}

void unpacker(Char_t commandfile[200])
{
  cout << "\f\n 'unpacker with commandfile' not yet implemented" << endl;
  unpacker();
}
