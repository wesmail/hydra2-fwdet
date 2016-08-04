//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/06/2004 by Ilse Koenig 

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////
//
//  HParamFileGenerator
//
//  Class to create a parameter file from Oracle
//  (Only used by the runtime database)
//
//////////////////////////////////////////////////////////////////////////

#include "hparamfilegenerator.h"
#include "hparrootfileio.h"
#include "hruninfo.h"

ClassImp(HParamFileGenerator)

HParamFileGenerator::HParamFileGenerator(const Char_t* beamtime,
		                         const Char_t* startAt,const Char_t* endAt ) {
  experiment=beamtime;
  rangeStart=startAt;
  rangeEnd=endAt;
  runs=0;
  logFile=0;
}

HParamFileGenerator::~HParamFileGenerator() {
  if (runs) {
    runs->Delete();
    delete runs;
    runs=0;
  }
  closeLogFile();
}

HParRootFileIo* HParamFileGenerator::openParameterFile(const Char_t* filename) {
  paramFilename=filename;
  if (!paramFilename.IsNull()) {
    HParRootFileIo* f=new HParRootFileIo;
    Bool_t rc=f->open(filename,"CREATE");
    if (rc) {
      Ssiz_t n=paramFilename.Last('.');
      logFilename=paramFilename(0,n+1);
      logFilename.Append("log");
      closeLogFile();
      logFile=new fstream();
      logFile->open((Char_t*)logFilename.Data(),ios::out);
      if (logFile->rdbuf()->is_open()==1) return f;
      else { 
        Error("openParameterFile","No log file open");
        closeLogFile();
      }
    } else {
      Error("openParameterFile","File %s already exits",filename);
      delete f;
    }
  }
  return 0; 
}

void HParamFileGenerator::closeLogFile() {
  if (logFile) {
    logFile->close();
    delete logFile;
    logFile=0;
  }
}

void HParamFileGenerator::setListOfRuns(TList* r) {
  if (runs) {
    runs->Delete();
    delete runs;
    runs=0;
  }
  runs=r;
}

void HParamFileGenerator::writeRuns() {
  if (logFile&&runs) {
    fstream& fout=*logFile;
    fout<<"Experiment: "<<experiment<<'\n'
        <<"Range:      "<<rangeStart<<" - "<<rangeEnd<<'\n';
    TIter next(runs);
    HRunInfo* run;
    while ((run=(HRunInfo*)next())) {
      run->write(fout);
    }
  }
}
