/**
 * @file   hdstproduction.cc
 * @author Simon Lang
 * @date   Fri May  5 16:16:39 2006
 *
 * @brief  Base class of DST Production Classes
 *
 * $Id: hdstproduction.cc,v 1.39 2009-07-15 11:32:54 halo Exp $
 *
 */

// STL header files
#include <iostream>
#include <cassert>
#include <cstdlib>

extern "C" {
#  ifndef _GNU_SOURCE
#     define _GNU_SOURCE
#  endif
#  include <getopt.h>
}

// Root header files
#include "TGlobal.h"
#include "TObjString.h"
#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TUrl.h"

// Hydra header files
#include "hdstproduction.h"
#include "hdstutilities.h"

#include "hades.h"
#include "hevent.h"
#include "hcategory.h"
#include "htaskset.h"
#include "hrootsource.h"
#include "htool.h"

#include "hspectrometer.h"
#include "hrichdetector.h"
#include "hmdcdetector.h"
#include "htofdetector.h"
#include "hshowerdetector.h"
#include "hstart2detector.h"
#include "hwalldetector.h"
#include "hrpcdetector.h"
#include "htboxdetector.h"

#include "hruntimedb.h"

#ifdef ORACLE_SUPPORT
  #include "hparora2io.h"
  #include "horasimdef.h"
  #include "horadef.h"
#endif

#include "hparrootfileio.h"
#include "hparasciifileio.h"

#include "haddef.h"
#include "hgeantdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "richdef.h"
#include "showerdef.h"
#include "simulationdef.h"
#include "hstartdef.h"
#include "tofdef.h"
#include "walldef.h"
#include "rpcdef.h"

using namespace std;

// we use a large buffer for Root type output trees
const Int_t HDstProduction::TREE_BUFFER_SIZE = 8000;
Bool_t HDstProduction::isBatch = kTRUE;


//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//  Base class of DST Production Classes.
//
//  This is not an abstract base class, however it is not intended to create
//  instances of this class. Instead, create an object of one of the classes
//  derived from here. Therefore, the constructor of this class is protected.
//
////////////////////////////////////////////////////////////////////////////////
ClassImp(HDstProduction)



    HDstProduction::HDstProduction()
{
    // Default Constructor: gHades created, Root set to batch mode.

    dstCfg.richModules[0]    = 0;
    for (UInt_t i = 0; i < 3; i++)  { dstCfg.showerModules[i]  = 0; }
    dstCfg.triggerModules[0] = 0;
    dstCfg.rpcModules[0]     = 0;
    dstCfg.wallModules[0]    = 0;

    for (UInt_t i = 0; i < 22; i++){
	dstCfg.tofModules[i] = 0;
    }

    for (UInt_t i = 0; i < 6; i++) {
	for (UInt_t j = 0; j < 4; j++) {
	    dstCfg.mdcModules[i][j]   = 0;
	}
    }

    for (UInt_t i = 0; i < 10; i++)  { dstCfg.startModules[i] = 0; }

    // these flags are altered by using modules of the related detectors
    dstCfg.triggerIsUsed = kFALSE;
    dstCfg.startIsUsed   = kFALSE;
    dstCfg.richIsUsed    = kFALSE;
    dstCfg.mdcIsUsed     = kFALSE;
    dstCfg.showerIsUsed  = kFALSE;
    dstCfg.tofIsUsed     = kFALSE;
    dstCfg.rpcIsUsed     = kFALSE;
    dstCfg.wallIsUsed    = kFALSE;

    dstCfg.dstId                   = "";
    dstCfg.expId                   = "";
    dstCfg.inputUrl                = "";
    dstCfg.embeddingInput          = "";
    dstCfg.outputDir               = "";
    dstCfg.outputFile              = "";
    dstCfg.qaOutputDir             = "";
    dstCfg.notPersistentCategories = "";
    dstCfg.disabledCategories      = "";
    dstCfg.firstParameterSource    = "none";
    dstCfg.secondParameterSource   = "none";
    dstCfg.historyDate             = "now";
    dstCfg.parameterOutputFile     = "";
    dstCfg.referenceRunId          = 0;
    dstCfg.processNumEvents        = 0;
    dstCfg.startWithEvent          = 0;
    dstCfg.eventCounterInterval    = 10000;
    dstCfg.qaEventInterval         = 50;

    // set up Root - inhibit graphical output in batch mode
    if(isBatch) gROOT->SetBatch();

    // set up singleton analysis instance - gHades is set by the constructor of
    // class Hades
    gHades = NULL;
    new Hades;
}



HDstProduction::~HDstProduction()
{
    if (gHades)
    {
	delete gHades;
    }
}



void HDstProduction::configure(Int_t argc, Char_t** argv)
{
    // First this function checks on the command line, weather a
    // configuration file was specified. Then these information
    // are evaluated, before the command line is search again for
    // other option.
    //
    // This means, that the program parameter are evaluated in the
    // following order of precedence (low to high):
    //   1. Default parameter from constructor
    //   2. Parameter read from configuration file
    //   3. Parameter read from command line

    Int_t    steps_left   = 2;
    Int_t    option       = 0;
    Int_t    option_index = 0;
    Int_t    current_argc = 0;
    Char_t** current_argv = NULL;
    Int_t    file_argc    = 0;
    Char_t** file_argv    = NULL;
    Int_t    print_help   = 0;

    // options allowed on command line and in the config file: their Long_t    // name and possibly the corresponding Short_t (single character) form
    const struct option dst_options[] = {
	{ "cfg-file",                  required_argument, NULL, 'f'   },
	{ "trigger-modules",           required_argument, NULL, 32001 },
	{ "start-detector-modules",    required_argument, NULL, 32002 },
	{ "rich-modules",              required_argument, NULL, 32003 },
	{ "mdc-modules",               required_argument, NULL, 32004 },
	{ "shower-modules",            required_argument, NULL, 32005 },
	{ "tof-modules",               required_argument, NULL, 32006 },
	{ "rpc-modules",               required_argument, NULL, 32007 },
	{ "wall-modules",              required_argument, NULL, 32008 },
	{ "dst-id",                    required_argument, NULL, 32009 },
	{ "exp-id",                    required_argument, NULL, 32010 },
	{ "start-with-event",          required_argument, NULL, 'e'   },
	{ "event-counter-interval",    required_argument, NULL, 'E'   },
	{ "input-url",                 required_argument, NULL, 'i'   },
	{ "embedding-input",           required_argument, NULL, 'I'   },
	{ "output-dir",                required_argument, NULL, 'o'   },
	{ "output-file",               required_argument, NULL, 'O'   },
	{ "qa-output-dir",             required_argument, NULL, 'q'   },
	{ "qa-event-interval",         required_argument, NULL, 'Q'   },
	{ "not-persistent-categories", required_argument, NULL, 'c'   },
	{ "disabled-categories",       required_argument, NULL, 'D'   },
	{ "first-parameter-source",    required_argument, NULL, '1'   },
	{ "second-parameter-source",   required_argument, NULL, '2'   },
	{ "history-date",              required_argument, NULL, 'd'   },
	{ "parameter-output-file",     required_argument, NULL, 'p'   },
	{ "reference-run-id",          required_argument, NULL, 'r'   },
	{ "num-events",                required_argument, NULL, 'n'   },
	{ "help",                      no_argument,       NULL, 'h'   },
	{ 0, 0, 0, 0 }
    };

    // first, try to find and parse an optional configuration file
    for (current_argc = 0; current_argc < argc - 1; current_argc++)
    {
	if (strcmp( argv[current_argc], "-f"         ) == 0  ||
	    strcmp( argv[current_argc], "--cfg-file" ) == 0)
	{
	    if (!HDstUtilities::readCmdLineArgsFromFile(
							argv[current_argc + 1], &file_argc, &file_argv ))
	    {
		terminate( 1 );
	    }
	    break;
	}
    }

    // first, evaluate the parameter from file (from the beginning), then
    // from command line (see code around "goto" statement below, too)
    current_argc = file_argc;
    current_argv = file_argv;

EvalOptions:

    while (1)
    {
	option = getopt_long( current_argc, current_argv,"1:2:c:d:D:e:E:f:hi:I:n:o:O:p:q:Q:r:s",dst_options, &option_index );
	if (option == -1) {
	    break;
	}

	switch (option)
	{
	case 32001:
	    if (sscanf( optarg, "%d", &dstCfg.triggerModules[0] ) != 1) {
		Error( "configure", "Illegal trigger modules definition!" );
		terminate( 1 );
	    }
	    dstCfg.triggerIsUsed = kTRUE;
	    break;
	case 32002:
	    if (sscanf( optarg, "%d %d %d %d %d %d %d %d %d %d",
		       &dstCfg.startModules[0],
		       &dstCfg.startModules[1],
		       &dstCfg.startModules[2],
		       &dstCfg.startModules[3],
		       &dstCfg.startModules[4],
		       &dstCfg.startModules[5],
		       &dstCfg.startModules[6],
		       &dstCfg.startModules[7],
		       &dstCfg.startModules[8],
		       &dstCfg.startModules[9]) != 10) {
		Error( "configure", "Illegal start detectors definition!" );
		terminate( 1 );
	    }
	    dstCfg.startIsUsed = kTRUE;
	    break;
	case 32003:
	    if (sscanf( optarg, "%d", &dstCfg.richModules[0] ) != 1) {
		Error( "configure", "Illegal RICH modules definition!" );
		terminate( 1 );
	    }
	    dstCfg.richIsUsed = kTRUE;
	    break;
	case 32004:
	    if (sscanf( optarg,
		       "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
		       &dstCfg.mdcModules[0][0],
		       &dstCfg.mdcModules[1][0],
		       &dstCfg.mdcModules[2][0],
		       &dstCfg.mdcModules[3][0],
		       &dstCfg.mdcModules[4][0],
		       &dstCfg.mdcModules[5][0],
		       &dstCfg.mdcModules[0][1],
		       &dstCfg.mdcModules[1][1],
		       &dstCfg.mdcModules[2][1],
		       &dstCfg.mdcModules[3][1],
		       &dstCfg.mdcModules[4][1],
		       &dstCfg.mdcModules[5][1],
		       &dstCfg.mdcModules[0][2],
		       &dstCfg.mdcModules[1][2],
		       &dstCfg.mdcModules[2][2],
		       &dstCfg.mdcModules[3][2],
		       &dstCfg.mdcModules[4][2],
		       &dstCfg.mdcModules[5][2],
		       &dstCfg.mdcModules[0][3],
		       &dstCfg.mdcModules[1][3],
		       &dstCfg.mdcModules[2][3],
		       &dstCfg.mdcModules[3][3],
		       &dstCfg.mdcModules[4][3],
		       &dstCfg.mdcModules[5][3] ) != 24) {
		Error( "configure", "Illegal MDC modules definition!" );
		terminate( 1 );
	    }
	    dstCfg.mdcIsUsed = kTRUE;
	    break;
	case 32005:
	    if (sscanf( optarg, "%d %d %d",
		       &dstCfg.showerModules[0],
		       &dstCfg.showerModules[1],
		       &dstCfg.showerModules[2] ) != 3) {
		Error( "configure", "Illegal shower modules definition!");
		terminate( 1 );
	    }
	    dstCfg.showerIsUsed = kTRUE;
	    break;
	case 32006:
	    if (sscanf( optarg,
		       "%d %d %d %d %d %d %d %d %d %d %d "
		       "%d %d %d %d %d %d %d %d %d %d %d",
		       &dstCfg.tofModules[0],
		       &dstCfg.tofModules[1],
		       &dstCfg.tofModules[2],
		       &dstCfg.tofModules[3],
		       &dstCfg.tofModules[4],
		       &dstCfg.tofModules[5],
		       &dstCfg.tofModules[6],
		       &dstCfg.tofModules[7],
		       &dstCfg.tofModules[8],
		       &dstCfg.tofModules[9],
		       &dstCfg.tofModules[10],
		       &dstCfg.tofModules[11],
		       &dstCfg.tofModules[12],
		       &dstCfg.tofModules[13],
		       &dstCfg.tofModules[14],
		       &dstCfg.tofModules[15],
		       &dstCfg.tofModules[16],
		       &dstCfg.tofModules[17],
		       &dstCfg.tofModules[18],
		       &dstCfg.tofModules[19],
		       &dstCfg.tofModules[20],
		       &dstCfg.tofModules[21] ) != 22) {
		Error( "configure", "Illegal TOF modules definition!" );
		terminate( 1 );
	    }
	    dstCfg.tofIsUsed = kTRUE;
	    break;
	case 32007:
	    if (sscanf( optarg, "%d", &dstCfg.rpcModules[0] ) != 1) {
		Error( "configure", "Illegal RPC modules definition!" );
		terminate( 1 );
	    }
	    dstCfg.rpcIsUsed = kTRUE;
	    break;
	case 32008:
	    if (sscanf( optarg, "%d", &dstCfg.wallModules[0] ) != 1) {
		Error( "configure", "Illegal WALL modules definition!" );
		terminate( 1 );
	    }
	    dstCfg.wallIsUsed = kTRUE;
	    break;
	case 32009:
	    dstCfg.dstId = optarg;
	    break;
	case 32010:
	    dstCfg.expId = optarg;
	    dstCfg.expId.ToLower();
	    break;
	case '1':
	    dstCfg.firstParameterSource = optarg;
	    break;
	case '2':
	    dstCfg.secondParameterSource = optarg;
	    break;
	case 'c':
	    dstCfg.notPersistentCategories = optarg;
	    break;
	case 'd':
	    dstCfg.historyDate = optarg;
	    break;
	case 'D':
	    dstCfg.disabledCategories = optarg;
	    break;
	case 'f':
	    // configuration file is already evaluated
	    break;
	case 'e':
	    if (sscanf( optarg, "%d", &dstCfg.startWithEvent ) != 1) {
		Error( "configure", "Illegal start event defined!" );
		terminate( 1 );
	    }
	    break;
	case 'E':
	    if (sscanf( optarg, "%d", &dstCfg.eventCounterInterval ) != 1
		||
		dstCfg.eventCounterInterval < 1) {
		Error( "configure", "Illegal counter interval defined!" );
		terminate( 1 );
	    }
	    break;
	case 'h':
	    print_help = 1;
	    break;
	case 'i':
	    dstCfg.inputUrl += " ";
	    dstCfg.inputUrl += optarg;
	    dstCfg.inputUrl  = dstCfg.inputUrl.Strip( TString::kBoth, ' ' );
	    break;
	case 'I':
	    dstCfg.embeddingInput += " ";
	    dstCfg.embeddingInput += optarg;
	    dstCfg.embeddingInput  =
		dstCfg.embeddingInput.Strip( TString::kBoth, ' ' );
	    break;
	case 'n':
	    if (sscanf( optarg, "%d", &dstCfg.processNumEvents ) != 1
		||
		dstCfg.processNumEvents < 0) {
		Error( "configure", "Illegal number of events defined!" );
		terminate( 1 );
	    }
	    break;
	case 'o':
	    dstCfg.outputDir = optarg;
	    dstCfg.outputDir.Remove( TString::kTrailing, '/' );
	    break;
	case 'O':
	    dstCfg.outputFile = optarg;
	    break;
	case 'p':
	    dstCfg.parameterOutputFile = optarg;
	    break;
	case 'q':
	    dstCfg.qaOutputDir = optarg;
	    dstCfg.outputDir.Remove( TString::kTrailing, '/' );
	    break;
	case 'Q':
	    if (sscanf( optarg, "%d", &dstCfg.qaEventInterval ) != 1
		||
		dstCfg.qaEventInterval < 1) {
		Error( "configure", "Illegal QA event interval defined!" );
		terminate( 1 );
	    }
	    break;
	case 'r':
	    if (sscanf( optarg, "%d", &dstCfg.referenceRunId ) != 1) {
		Error( "configure", "Illegal reference run ID defined!" );
		terminate( 1 );
	    }
	    break;
	case '?':
	    Error( "configure", "Found unkown or ambiguous option!" );
	    terminate( 1 );
	    break;
	default:
	    Error( "configure", "Found illegal option!" );
	    terminate( 1 );
	}
    }

    // first, evaluate the parameter from file, then from command line, while
    // skipping the program name (see code around "goto label" above, too)
    if (--steps_left)
    {
	optind       = 1;
	current_argc = argc;
	current_argv = argv;
	goto EvalOptions;
    }

    while (file_argc > 0) {
	free( file_argv[--file_argc] );
    }
    if (file_argv != NULL) {
	free( file_argv );
    }

    if (print_help) {
	printConfiguration();
	printHelp();
	terminate( 0 );
    }
}



void HDstProduction::setupSpectrometer()
{
    // Setup the spectormeter. This functions creates the setup of the
    // spectrometer using the related module parameters of the dstCfg
    // structure.

    HSpectrometer* spectrometer = NULL;

    // set up the spectrometer
    if (!(spectrometer = gHades->getSetup())) {
	Error( "setupSpectrometer", "Failed to initialize Spectrometer!" );
	terminate( 3 );
    }

    if (dstCfg.richIsUsed) {
	spectrometer->addDetector( new HRichDetector );
	for (Int_t i = 0; i < 6; i++) {
	    spectrometer->getDetector( "Rich" )-> setModules( i, dstCfg.richModules );
	}
    }
    if (dstCfg.mdcIsUsed) {
	spectrometer->addDetector( new HMdcDetector );
	for (Int_t i = 0; i < 6; i++) {
	    spectrometer->getDetector( "Mdc" )->setModules( i, dstCfg.mdcModules[i] );
	}
    }
    if (dstCfg.tofIsUsed) {
	spectrometer->addDetector( new HTofDetector );
	for (Int_t i = 0; i < 6; i++) {
	    spectrometer->getDetector( "Tof" )->setModules( i, dstCfg.tofModules );
	}
    }
    if (dstCfg.rpcIsUsed) {
	spectrometer->addDetector( new HRpcDetector );
	for (Int_t i = 0; i < 6; i++) {
	    spectrometer->getDetector( "Rpc" )->setModules( i, dstCfg.rpcModules );
	}
    }
    if (dstCfg.showerIsUsed) {
	spectrometer->addDetector( new HShowerDetector );
	for (Int_t i = 0; i < 6; i++){
	    spectrometer->getDetector( "Shower" )-> setModules( i, dstCfg.showerModules );
	}
    }


    if (dstCfg.startIsUsed) {
	spectrometer->addDetector( new HStart2Detector );
	spectrometer->getDetector( "Start" )->setModules(-1, dstCfg.startModules);
    }

    if (dstCfg.wallIsUsed) {
	spectrometer->addDetector( new HWallDetector );
	spectrometer->getDetector( "Wall" )->setModules(-1, dstCfg.wallModules);
    }

    if (dstCfg.triggerIsUsed) {
	spectrometer->addDetector( new HTBoxDetector    );
	spectrometer->getDetector( "TBox" )->setModules(-1, dstCfg.triggerModules);
    }
}



void HDstProduction::setupParameterSources()
{
    // Setup and open parameter sources. The function uses the related variables
    // of the dstCfg structure to decide which source is taken.

    HRuntimeDb* runtime_db = gHades->getRuntimeDb();

    // assign first parameter source
    if (dstCfg.firstParameterSource.CompareTo( "oracle",
					      TString::kIgnoreCase ) == 0)
    {

#ifdef ORACLE_SUPPORT

	HParOra2Io* input = new HParOra2Io();
	if (!input->open())
	{
	    Error( "setupParameterSources", "Could not open DB connection!" );
	    terminate( 3 );
	}
	if (dstCfg.historyDate.CompareTo( "none", TString::kIgnoreCase ) != 0)
	{
	    if(dstCfg.historyDate.Contains("-") != 0 || dstCfg.historyDate.CompareTo("now") == 0)
	    {
		// looks like a date format
		if(!input->setHistoryDate( dstCfg.historyDate.Data() ))
		{
		    Error( "setupParameterSources","historyDate contains \"-\", but is no date format : %s !",dstCfg.historyDate.Data() );
		    terminate( 3 );
		}
	    } else {
		// looks like a param release format
		if(!input->setParamRelease( dstCfg.historyDate.Data() ))
		{
		    Error( "setupParameterSources","historyDate contains no \"-\", but is no valid param release : %s !",dstCfg.historyDate.Data() );
		    terminate( 3 );
		}
	    }
	}
	runtime_db->setFirstInput( input );
#endif
    }
    else if (dstCfg.firstParameterSource.EndsWith( ".root",
						  TString::kIgnoreCase ))
    {
	HParRootFileIo* input = new HParRootFileIo();
	if (!input->open(const_cast<Text_t*>(dstCfg.firstParameterSource.Data()),
			 "READ" ))
	{
	    Error( "setupParameterSources","Could not open Root parameter input file!" );
	    terminate( 3 );
	}
	runtime_db->setFirstInput( input );
    }
    else if (dstCfg.firstParameterSource.CompareTo( "none",TString::kIgnoreCase ) == 0)
    {
	Error( "setupParameterSources", "First parameter source not defined!" );
	terminate( 1 );
    }
    else
    {
	HParAsciiFileIo* input = new HParAsciiFileIo();
	if (!input->open(const_cast<Text_t*>(dstCfg.firstParameterSource.Data()), "in" )){
	    Error( "setupParameterSources","Could not open ASCII parameter input file!" );
	    terminate( 3 );
	}
	runtime_db->setFirstInput( input );
    }

    // assign second parameter source
    if (dstCfg.secondParameterSource.CompareTo( "oracle",TString::kIgnoreCase ) == 0)
    {
#ifdef ORACLE_SUPPORT
	HParOra2Io* input = new HParOra2Io();
	if (!input->open())
	{
	    Error( "setupParameterSources", "Could not open DB connection!" );
	    terminate( 3 );
	}
	if (dstCfg.historyDate.CompareTo( "none", TString::kIgnoreCase ) != 0)
	{
	    if(dstCfg.historyDate.Contains("-") != 0 || dstCfg.historyDate.CompareTo("now") == 0)
	    {
		// looks like a date format
		if(!input->setHistoryDate( dstCfg.historyDate.Data() )) {
		    Error( "setupParameterSources","historyDate contains \"-\", but is no date format : %s !",dstCfg.historyDate.Data() );
		    terminate( 3 );
		}
	    } else {
		// looks like a param release format
		if(!input->setParamRelease( dstCfg.historyDate.Data() )) {
		    Error( "setupParameterSources","historyDate contains no \"-\", but is no valid param release : %s !",dstCfg.historyDate.Data() );
		    terminate( 3 );
		}
	    }
	}
	runtime_db->setSecondInput( input );
#endif
    }
    else if (dstCfg.secondParameterSource.EndsWith( ".root",TString::kIgnoreCase ))
    {
	HParRootFileIo* input = new HParRootFileIo();
	if (!input->open(const_cast<Text_t*>(dstCfg.secondParameterSource.Data()),"READ")){
	    Error( "setupParameterSources","Could not open Root parameter input file!" );
	    terminate( 3 );
	}
	runtime_db->setSecondInput( input );
    }
    else if (dstCfg.secondParameterSource.CompareTo("none", TString::kIgnoreCase ) == 0)
    {
	// in this case, do nothing
    }
    else
    {
	HParAsciiFileIo* input = new HParAsciiFileIo();
	if (!input->open(const_cast<Text_t*>(dstCfg.secondParameterSource.Data()), "in" )){
	    Error( "setupParameterSources", "Could not open ASCII parameter input file!" );
	    terminate( 3 );
	}
	runtime_db->setSecondInput( input );
    }
}



void HDstProduction::setupRootInput()
{
    // Setup data input file: This function determines and opens
    // one or more .root input files of analysed data. All necessary
    // information are provided by dstCfg structure.

    HRootSource* source      = NULL;
    TObjArray*   files       = NULL;
    TIterator*   file        = NULL;
    TObjString*  filename    = NULL;
    TObjArray*   local_files = NULL;
    TIterator*   local_file  = NULL;
    TFile*       root_file   = NULL;
    TGlobal*     g           = NULL;
    TString      directory;

    // to disable categories from ROOT input
    TObjString* category_name = NULL;
    TObjArray*  categories    = NULL;
    TIterator*  element       = NULL;
    TString     name;

    files = dstCfg.inputUrl.Tokenize( " \t" );
    if (!files) {
	Error( "setupRootInput", "No input file(s) defined!" );
	terminate( 2 );
    }
    file  = files->MakeIterator();
    file->Reset();
    while ((filename = (TObjString*)file->Next()))
    {
	TUrl url( filename->GetString().Data(), kTRUE );

	if (!source)
	{
	    source = new HRootSource;
	    source->setDirectory(const_cast<Text_t*>(gSystem->DirName( url.GetFile() )) );
	    directory = gSystem->DirName( url.GetFile() );
	}
	if (strcmp( url.GetProtocol(), "file" ) == 0)
	{
	    local_files = HTool::glob( url.GetFile() );
	    if (!local_files) {
		Error( "setupRootInput", "Input file(s) do not exist!" );
		terminate( 2 );
	    }
	    local_file = local_files->MakeIterator();
	    while ((filename = (TObjString*)local_file->Next()))
	    {
		if (directory.CompareTo(gSystem->DirName( filename->GetString().Data() ) ) != 0)
		{
		    Error( "setupRootInput", "Only one input directory allowed!" );
		    terminate( 2 );
		}

		root_file = TFile::Open( filename->GetString().Data(), "READ" );
		if (!root_file->IsOpen()  ||  root_file->IsZombie()  ||
		    root_file->GetSize() < 8000) {
		    Error( "setupRootInput", "Broken/invalid .root input file '%s'!",filename->GetString().Data()  );
		    terminate( 2 );
		}
		delete root_file;

		if (!source->addFile(const_cast<Text_t*>(gSystem->BaseName(filename->GetString().Data() )) )) {
		    Error( "setupRootInput", "Adding input file '%s' failed!",filename->GetString().Data()  );
		    terminate( 2 );
		}
	    }
	    delete local_file;
	    delete local_files;
	}
	else
	{
	    Error( "setupRootInput", "Input URL must refer to a local file!" );
	    terminate( 2 );
	}
    }
    delete file;


    // disable categories from the input here...
    categories = dstCfg.disabledCategories.Tokenize( " \t" );
    element    = categories->MakeIterator();
    element->Reset();
    while ((category_name = dynamic_cast<TObjString*>(element->Next())))
    {
	name = category_name->GetString();
	g = gROOT->GetGlobal( name.Data() );
	if( NULL != g ) {
	    source->disableCategory( *static_cast<Cat_t*>(g->GetAddress()) );
	}
	else {
	    Error( "setupRootInput", "Unknown input category '%s'!",name.Data() );
	    terminate( 2 );
	}

    }
    delete categories;
    delete element;

    gHades->setDataSource( source );
}



void HDstProduction::setupOutput()
{
    // Setup data output: This function opens a .root output file, those
    // name is derived from the input filename and initializes the output
    // tree 'T' in this file.
    // CAUTION: This function must be called AFTER initialize(), otherwise
    // the output is corrupted or the program might crash.

    TObjString* category_name = NULL;
    TObjArray*  categories    = NULL;
    TIterator*  element       = NULL;
    HCategory*  category      = NULL;
    TGlobal*    g             = NULL;
    TString     name;

    // check given output locations
    if (gSystem->AccessPathName( dstCfg.outputDir.Data(), kWritePermission )) {
	Error( "setupOutput", "Ouput directory '%s' not writable!",dstCfg.outputDir.Data() );
	terminate( 2 );
    }
    if (dstCfg.qaOutputDir.IsNull()) {
	dstCfg.qaOutputDir = dstCfg.outputDir;
    }
    else if (gSystem->AccessPathName( dstCfg.qaOutputDir.Data(),kWritePermission ))
    {
	Error( "setupOutput", "QA output directory '%s' not writable!",dstCfg.qaOutputDir.Data() );
	terminate( 2 );
    }

    categories = dstCfg.notPersistentCategories.Tokenize( " \t" );
    element    = categories->MakeIterator();
    element->Reset();
    while ((category_name = dynamic_cast<TObjString*>(element->Next())))
    {
	name = category_name->GetString();
	g = gROOT->GetGlobal( name.Data() );
	if( NULL != g ) {
	    category = gHades->getCurrentEvent()->getCategory( *static_cast<Cat_t*>(g->GetAddress()) );
	} else {
	    Error( "setupOutput", "Unknown output category '%s' defined!",name.Data() );
	    terminate( 2 );
	}

	if( category ) {
	    category->setPersistency( kFALSE );
	} else {
	    Warning( "setupOutput", "Output category '%s' does not exist!",name.Data() );
	}
    }
    delete categories;
    delete element;

    // we use a large output tree buffer
    gHades->setTreeBufferSize( TREE_BUFFER_SIZE );

    // derive output file name from input file name (which might be a list,
    // separated by blanks), if it was not set e.g. by the task list before
    if (dstCfg.outputFile.IsNull())
    {
	dstCfg.outputFile = dstCfg.inputUrl.Data();
	Ssiz_t filename = dstCfg.outputFile.Index( ' ', 1, 1, TString::kExact );
	if (filename != kNPOS) {
	    dstCfg.outputFile.Resize( filename );
	}
	if (dstCfg.outputFile.EndsWith( ".root" )) {
	    dstCfg.outputFile.ReplaceAll( ".root", 5, "_dst.root", 9 );
	}
	else if (dstCfg.outputFile.EndsWith( ".hld" )) {
	    dstCfg.outputFile.ReplaceAll( ".hld",  4, "_dst.root", 9 );
	}
	else {
	    dstCfg.outputFile += "_dst.root";
	}
    }
    dstCfg.outputFile = gSystem->BaseName( dstCfg.outputFile.Data() );
    dstCfg.outputFile.Prepend( "/" );
    dstCfg.outputFile.Prepend( dstCfg.outputDir );

    // open and inititialize a compressed data output file (level 2)
    gHades->setOutputFile( const_cast<Text_t*>(dstCfg.outputFile.Data()),"RECREATE",const_cast<Text_t*>(dstCfg.dstId.Data()),2 );

    // create output tree of categories
    gHades->makeTree();
}



void HDstProduction::setupAllParameterOutput()
{
    // Dump all parameter of an experiment to an .root output file.
    // This function expects a filename ending on '.root' in dstCfg parameter
    // structure, if the output should be written to a Root file.
    // Actually to dump the parameter to file one has to destroy the object
    // gHades after the event loop was executed, thus do:
    //    setupAllParameterOutput();
    //    initialize();
    //    runEventLoop();
    //    finalize();

    if (dstCfg.expId.IsNull()) {
	Error( "setupAllParameterOutput", "Experiment ID not set!" );
	terminate( 2 );
    }

    if (dstCfg.parameterOutputFile.IsNull()) {
	Error( "setupAllParameterOutput","Parameter output file not defined!" );
	terminate( 2 );
    }

    if (!dstCfg.parameterOutputFile.EndsWith( ".root", TString::kIgnoreCase )) {
	Error( "setupAllParameterOutput","Since the output is a Root file, it must end on .root!" );
	terminate( 2 );
    }

    if (!gHades->getRuntimeDb()->makeParamFile(const_cast<Text_t*>(dstCfg.parameterOutputFile.Data()), const_cast<Text_t*>(dstCfg.expId.Data()) )) {
	Error( "setupAllParameterOutput","Could not open .root parameter output file!" );
	terminate( 3 );
    }
}



void HDstProduction::setupParameterOutput()
{
    // Dump the used parameter to an .root output file.
    // This function expects a filename ending on '.root' in dstCfg parameter
    // structure, if the output should be written to a Root file.
    // Actually to dump the parameter to file one has to destroy the object
    // gHades after the event loop was executed, thus do:
    //    setupParameterOutput();
    //    initialize();
    //    runEventLoop();
    //    finalize();

    if (dstCfg.parameterOutputFile.IsNull()) {
	Error( "setupParameterOutput", "Parameter output file not defined!" );
	terminate( 2 );
    }

    if (!dstCfg.parameterOutputFile.EndsWith( ".root", TString::kIgnoreCase )) {
	Error( "setupParameterOutput","Since the output is a Root file, it must end on .root!" );
	terminate( 2 );
    }

    HParRootFileIo* output = new HParRootFileIo();
    if (!output->open(const_cast<Text_t*>(dstCfg.parameterOutputFile.Data()),"RECREATE" )) {
	Error( "setupParameterOutput","Could not open .root parameter output file!" );
	terminate( 3 );
    }
    gHades->getRuntimeDb()->setOutput( output );
}



void HDstProduction::printConfiguration()
{
    // Just print the content of the dstCfg structure.

    TObjArray*  inputs    = NULL;
    TIterator*  input     = NULL;
    TObjString* inputname = NULL;
    TObjArray*  files     = NULL;
    TIterator*  file      = NULL;
    TObjString* filename  = NULL;

    cout << endl;

    cout << "==> DST Production ID ...............: "
	<< (dstCfg.dstId.IsNull()  ?  "-unset-"  :  dstCfg.dstId) << endl;

    cout << "==> Experiment ID ...................: "
	<< (dstCfg.expId.IsNull()  ?  "-unset-"  :  dstCfg.expId) << endl;

    cout << "==> Reference Run ID ................: ";
    if (dstCfg.referenceRunId) {
	cout << dstCfg.referenceRunId << endl;
    } else {
	cout << "-unset-" << endl;
    }

    cout << "==> First Parameter Source ..........: "
	<< dstCfg.firstParameterSource << endl;

    cout << "==> Second Parameter Source .........: "
	<< dstCfg.secondParameterSource << endl;

    cout << "==> Database History Date ...........: "
	<< dstCfg.historyDate << endl;

    cout << "==> Parameter Output File ...........: "
	<< (dstCfg.parameterOutputFile.IsNull()  ?  "-unset-"  : dstCfg.parameterOutputFile) << endl;

    cout << "==> Number of Events to be processed : ";
    if (dstCfg.processNumEvents) {
	cout << dstCfg.processNumEvents << endl;
    } else {
	cout << "all" << endl;
    }

    cout << "==> First Event to be processed .....: "
	<< dstCfg.startWithEvent << endl;

    if (dstCfg.inputUrl.IsNull()) {
	cout << "==> Input URL .......................: -unset-" << endl;
    } else {
	inputs = dstCfg.inputUrl.Tokenize( " \t" );
	input  = inputs->MakeIterator();
	while ((inputname = (TObjString*)input->Next()))
	{
	    files = HTool::glob( inputname->GetString() );
	    if (files)
	    {
		file  = files->MakeIterator();
		while ((filename = (TObjString*)file->Next()))
		{
		    cout << "==> Input URL .......................: " << filename->GetString() << endl;
		}
		delete files;
		delete file;
	    }
	}
	delete inputs;
	delete input;
    }

    if (!dstCfg.embeddingInput.IsNull())
    {
	inputs = dstCfg.embeddingInput.Tokenize( " \t" );
	input  = inputs->MakeIterator();
	while ((inputname = (TObjString*)input->Next()))
	{
	    files = HTool::glob( inputname->GetString() );
	    if (files)
	    {
		file  = files->MakeIterator();
		while ((filename = (TObjString*)file->Next()))
		{
		    cout << "==> Embedding Input .................: " << filename->GetString() << endl;
		}
		delete files;
		delete file;
	    }
	}
	delete inputs;
	delete input;
    }

    cout << "==> Output Directory ................: " << (dstCfg.outputDir.IsNull()  ?  "-unset-"  :  dstCfg.outputDir) << endl;

    cout << "==> Output Filename .................: " << (dstCfg.outputFile.IsNull() ?  "-unset: derived from input filename-"  :  dstCfg.outputFile) << endl;

    cout << "==> Output Directory of QA Reports ..: " << (dstCfg.qaOutputDir.IsNull()?  "-unset: using data output directory-"  :  dstCfg.qaOutputDir) << endl;

    cout << "==> Trigger Modules .................: ";
    if (dstCfg.triggerIsUsed) {
	cout << dstCfg.triggerModules[0] << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> Start Modules ...................: ";
    if (dstCfg.startIsUsed) {
	cout << dstCfg.startModules[0] << " "
	    << dstCfg.startModules[1] << " "
	    << dstCfg.startModules[2] << " "
	    << dstCfg.startModules[3] << " "
	    << dstCfg.startModules[4] << " "
	    << dstCfg.startModules[5] << " "
	    << dstCfg.startModules[6] << " "
	    << dstCfg.startModules[7] << " "
	    << dstCfg.startModules[8] << " "
	    << dstCfg.startModules[9] << " "
	    << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> RICH Modules ....................: ";
    if (dstCfg.richIsUsed) {
	cout << dstCfg.richModules[0] << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> MDC Modules .....................: ";
    if (dstCfg.mdcIsUsed) {
	for (UInt_t i = 0; i < 4; i++) {
	    for (UInt_t j = 0;  j < 6; j++) {
		cout << dstCfg.mdcModules[j][i] << " ";
	    }
	    cout << " ";
	}
	cout << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> Shower Modules ..................: ";
    if (dstCfg.showerIsUsed) {
	cout << dstCfg.showerModules[0] << " "
	    << dstCfg.showerModules[1] << " "
	    << dstCfg.showerModules[2] << " "
	    << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> TOF Modules .....................: ";
    if (dstCfg.tofIsUsed) {
	for (UInt_t i = 0; i < sizeof(dstCfg.tofModules) / sizeof(Int_t); i++) {
	    cout << dstCfg.tofModules[i] << " ";
	}
	cout << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> RPC Modules .....................: ";
    if (dstCfg.rpcIsUsed) {
	cout << dstCfg.rpcModules[0] << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> Wall Modules ....................: ";
    if (dstCfg.wallIsUsed) {
	cout << dstCfg.wallModules[0] << endl;
    } else {
	cout << "-unused-" << endl;
    }

    cout << "==> Not persistent Categories .......: " << (dstCfg.notPersistentCategories.IsNull()  ?  "none"  :  dstCfg.notPersistentCategories) << endl;
    cout << endl;

    cout << "==> Disabled Categories .............: " << (dstCfg.disabledCategories.IsNull()  ?  "none"  : dstCfg.disabledCategories) << endl;
    cout << endl;

}



void HDstProduction::initialize()
{
    // Just initialize the analysis run.

    // initialize the anlysis run
    if (!gHades->init()) {
	Error( "initialize", "Initialization failed!" );
	terminate( 3 );
    }
}



void HDstProduction::runEventLoop()
{
    // Run the event loop an print the timing information of the
    // executed tasks, if available.
    // The number of events to be processed and other parameter
    // are provided by the dstCfg structure.

    Int_t      num_events = 0;
    TStopwatch timer;

    // produce DST by looping over input events
    timer.Reset();
    timer.Start();
    gHades->makeCounter( dstCfg.eventCounterInterval );
    if (dstCfg.processNumEvents < 1) {
	num_events = gHades->eventLoop();
    } else {
	num_events = gHades->eventLoop( dstCfg.processNumEvents,dstCfg.startWithEvent );
    }
    timer.Stop();

    // print some statistics on what was done
    cout << endl;
    gHades->getTaskSet( "real" )->printTimer();
    cout << endl;
    gHades->getTaskSet( "simulation" )->printTimer();
    cout << endl;
    cout << "\n"
	<< "==> Events processed : " << num_events       << "\n"
	<< "==> Needed Time .....: " << timer.RealTime() << " seconds\n"
	<< "==> Needed CPU Time .: " << timer.CpuTime()  << " seconds\n";
    if (num_events) {
	cout << "==> Performance .....: "
	    << num_events / timer.RealTime() << " events per second\n"
	    << "==> CPU Performance .: " << timer.CpuTime() / num_events
	    << " CPU seconds per event\n";
    }
    cout << endl;
}



void HDstProduction::finalize()
{
    // Close all IO sources and exit the program successfully.

    terminate( 0 );
}



void HDstProduction::terminate(Int_t exit_code)
{
    // Write parameters to the output file. If setupAllParameterOutput()
    // was used to set the name of the output parameter file,
    // parameters for the whole beamtime period are written.
    // In the case of using setupParameterOutput() function,
    // parameters only for a single runID are written.
    //
    // At the end close all IO sources and exit the program with an error code.

    gHades->getRuntimeDb()->saveOutput();

    if (gHades) {
	delete gHades;
    }

    // this quits Root, too
    exit( exit_code );
}



void HDstProduction::printHelp()
{
    // Just print the help on the command line options.

    cout << "\n"
	<< " Use the following Long_t option flags on cammand line, in your configuration\n"
	<< " file or as aguments while calling the macro within CINT. The short options\n"
	<< " might be used on command line, only!\n"
	<< "\n"
	<< " Only long options:\n"
	<< "   --trigger-modules        Array of used trigger modules\n"
	<< "   --start-detector-modules Array of used start modules\n"
	<< "   --rich-modules           Array of used RICH modules\n"
	<< "   --mdc-modules            Array of used MDC modules\n"
	<< "   --shower-modules         Array of used shower modules\n"
	<< "   --tof-modules            Array of used TOF modules\n"
	<< "   --rpc-modules            Array of used RPC modules\n"
	<< "   --wall-modules           Array of used WALL modules\n"
	<< "   --dst-id                 DST type description\n"
	<< "   --exp-id                 Experiment ID (like used in Oracle)\n"
	<< "\n"
	<< " Short and long options:\n"
	<< "   -1, --first-parameter-source    First parameter source\n"
	<< "   -2, --second-parameter-source   Second parameter source\n"
	<< "   -c, --not-persistent-categories List of not persistent categories (will\n"
	<< "                                   not be written to the output ROOT file)\n"
	<< "   -d, --history-date              Oracle Database History Date:\n"
	<< "                                    - \"none\" : Use lastest parameter release\n"
	<< "                                    - \"now\"  : Use lastest parameter version\n"
	<< "                                    - Or explicitly the name of a release\n"
	<< "                                    - \"now\" is the default\n"
	<< "                                   not be written to the output ROOT file)\n"
	<< "   -D  --disabled-categories       List of disabled categories (will not be\n"
	<< "                                   opened from the input ROOT file)\n"
	<< "   -e, --start-with-event          Event to start the analysis from\n"
	<< "   -E, --event-counter-interval    Event counter interval\n"
	<< "   -f, --cfg-file                  Configuration file (All the configuration\n"
	<< "                                   can be stored in this file, command line\n"
	<< "                                   options overwrite the options in this file)\n"
	<< "   -i, --input-url                 Input for the analysis (file on the disk or\n"
	<< "                                   robot, access via NFS or rootd = future GSI\n"
	<< "                                   access scheme). One can define several input\n"
	<< "                                   files. In this case use the option for each\n"
	<< "                                   file in the order they should be used.\n"
	<< "   -I, --embedding-input           Second input source of simulated data for\n"
	<< "                                   \"embedded\" mode. Here, the same rules are\n"
	<< "                                   valid as for normal input files.\n"
	<< "   -n, --num-events                Number of events to analyse.\n"
	<< "   -o, --output-dir                Directory, where the output is stored.\n"
	<< "   -O, --output-file               Name of the output file.\n"
	<< "   -p, --parameter-output-file     Name of the parameter output root file for\n"
	<< "                                   the case if some/all parameters will be\n"
	<< "                                   written from parameter sources to a single\n"
	<< "                                   root file\n"
	<< "   -q, --qa-output-dir             Directory, where the output of QA task is\n"
	<< "                                   stored\n"
	<< "   -Q, --qa-event-interval         QA event interval\n"
	<< "   -r, --reference-run-id          Reference runID (needed mainly for\n"
	<< "                                   simulation)\n"
	<< endl;
}
