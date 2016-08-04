#ifndef MDC_CALIBRATE
#define MDC_CALIBRATE

#ifndef __CINT__
#ifndef TRUE
const Int_t TRUE=1;
#endif

#ifndef FALSE
const Int_t FALSE=0;
#endif
#endif

Char_t setupPath[160];
Char_t referenceRunIdTag[80];
Char_t CalibrationFileNamesTag[80];
Char_t InputDirectoriesTag[80];
Char_t OutputDirectoriesTag[80];
Char_t DefaultReferenceRunIdTag[80];
Char_t OracleReferenceRunIdTag[80];
Char_t RootReferenceRunIdTag[80];
Char_t DetectorSetupTag[80];
Char_t unpackerArgumentsTag[80];
Char_t specUnpackerArgumentsTag[80];
Char_t calcOptsTag[80];
Char_t fitOptsTag[80];
Char_t selectOptsTag[80];
Char_t startEventTag[80];                               

Char_t rtdbOutput[160];
Char_t outputFile[160];
Char_t outputTitle[160];
TString suffix;
Char_t mdcSlopeOutputFile[160];
Char_t ZeroAsciiInputFile[160];
Char_t asciiInputFile[300];
Char_t rootInputFile[160];
Char_t calParRawOutputFile[160];
Char_t ntupleOutputFile[160];

Int_t numberOfEvents;

enum mode   {ORACLE_DATABASE_INPUT = 3, ASCII_FILE_INPUT = 1, ROOT_FILE_INPUT = 2};
enum resources {HLD_FILE_SOURCE = 3, REMOTE_SOURCE = 2, RFIO_SOURCE=1};

Bool_t reset(void);
Bool_t access(void);
TList* parseSetupForFilename();
Int_t calibrate(Int_t inputMode =ORACLE_DATABASE_INPUT, Int_t numberOfEvents = -1,
	      Int_t datasource=RFIO_SOURCE, TString inputFileName="", TString inputDirName="",
	      TString outputDir = "./", TString beamtime = "sep05");
Int_t calibrateHelp(const Char_t *);
void evalStopwatch(TStopwatch *, Int_t);
#endif
