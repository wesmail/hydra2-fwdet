#ifndef HPARAMFILEGENERATOR_H
#define HPARAMFILEGENERATOR_H

using namespace std;
#include "TObject.h"
#include "TList.h"
#include <iostream>
#include <iomanip>

class HParRootFileIo;

class HParamFileGenerator : public TObject {
private:
  TString paramFilename;
  TString logFilename;
  TString experiment;  
  TString rangeStart;  
  TString rangeEnd;  
  TList* runs;
  fstream* logFile;     
public:
  HParamFileGenerator(const Char_t* beamtime,
                      const Char_t* startAt="",const Char_t* endAt="" );
  ~HParamFileGenerator();
  HParRootFileIo* openParameterFile(const Char_t* filename);
  const Char_t* getParamFilename() {return paramFilename.Data();} 
  const Char_t* getLogFilename() {return logFilename.Data();} 
  const Char_t* getExperiment() {return experiment.Data();} 
  const Char_t* getRangeStart() {return rangeStart.Data();} 
  const Char_t* getRangeEnd() {return rangeEnd.Data();} 
  void setListOfRuns(TList* r);
  void writeRuns();
  void closeLogFile();
  fstream* getLogFile() {return logFile;}
private:
  ClassDef(HParamFileGenerator,0) // Class to create a parameter file
};

#endif  /* !HPARAMFILEGENERATOR_H */
