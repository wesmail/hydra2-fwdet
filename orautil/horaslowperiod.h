#ifndef HORASLOWPERIOD_H
#define HORASLOWPERIOD_H

using namespace std;
#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
#include <fstream>

class HOraSlowPeriod : public TObject {
private:
  Int_t      periodId;   // Oracle id of the period
  TString    startTime;  // Start time of the period 
  TString    endTime;    // End time of the period 
  Int_t      runId;      // Id of the corresponding run
  TString    filename;   // Hld-filename of the corresponding run
  Int_t      filterFlag; // Flag for the Hld-file filter
public:
  HOraSlowPeriod();
  ~HOraSlowPeriod() {}
  void setPeriodId(Int_t n) {periodId=n;}
  void setStartTime(const Char_t* s) {startTime=s;}
  void setEndTime(const Char_t* s) {endTime=s;}
  void setRunId(Int_t n) {runId=n;}
  void setFilename(const Char_t* s) {filename=s;}
  void setFilterFlag(Int_t f) {filterFlag=f;}
  Int_t getPeriodId() {return periodId;}
  const Char_t* getStartTime() {return startTime.Data();}
  const Char_t* getEndTime() {return endTime.Data();}
  Int_t getDuration();
  Int_t getRunId() {return runId;}
  const Char_t* getFilename() {return filename.Data();}
  Int_t getFilterFlag() {return filterFlag;}
  void print();
  void write(fstream&);
private:
  ClassDef(HOraSlowPeriod,0) // Slowcontrol run period
};

#endif  /* !HORASLOWPERIOD */
