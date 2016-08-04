#ifndef HRUNINFO_H
#define HRUNINFO_H

using namespace std;
#include "TNamed.h"
#include <iostream>
#include <iomanip>
#include <fstream>

class HRunInfo : public TNamed {
protected:
  Int_t   runId;      // run identifier
  TString startTime;  // start time
  TString endTime;    // stop time
  Int_t   numEvents;  // num of events
public:
  HRunInfo(const Char_t* filename="");
  virtual ~HRunInfo() {}
  void setRunId(Int_t n) {runId=n;}
  void setStartTime(const Char_t* s) {startTime=s;}
  void setEndTime(const Char_t* s) {endTime=s;}
  void setNumEvents(Int_t n) {numEvents=n;}
  Int_t getRunId() {return runId;}
  const Char_t* getStartTime() {return startTime.Data();}
  const Char_t* getEndTime() {return endTime.Data();}
  Int_t getNumEvents() {return numEvents;}
  virtual void print();
  virtual void write(fstream&);
private:
  ClassDef(HRunInfo,0) // Class for run information
};

#endif  /* !HRUNINFO */
