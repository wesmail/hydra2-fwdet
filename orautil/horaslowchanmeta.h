#ifndef HORASLOWCHANMETA_H
#define HORASLOWCHANMETA_H

using namespace std;
#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
#include <fstream>

class HOraSlowChanMeta : public TObject {
private:
  Double_t lowGraphLimit;  // lower limit for the EPICS graph
  Double_t highGraphLimit; // upper limit for the EPICS graph
  Double_t lowWarnLimit;   // lower limit for the EPICS warning
  Double_t highWarnLimit;  // upper limit for the EPICS warning 
  Double_t lowAlarmLimit;  // lower limit for the EPICS alarm
  Double_t highAlarmLimit; // upper limit for the EPICS alarm
  TString units;           // units of the values
  Int_t precision;         // precision of the values
  TString startTime;       // time since these values are valid
public:
  HOraSlowChanMeta();
  ~HOraSlowChanMeta() {}
  void setLowGraphLimit(Double_t v) {lowGraphLimit=v;}
  void setHighGraphLimit(Double_t v) {highGraphLimit=v;}
  void setLowWarnLimit(Double_t v) {lowWarnLimit=v;}
  void setHighWarnLimit(Double_t v) {highWarnLimit=v;}
  void setLowAlarmLimit(Double_t v) {lowAlarmLimit=v;}
  void setHighAlarmLimit(Double_t v) {highAlarmLimit=v;}
  void setUnits(const Char_t* s) {units=s;}
  void setPrecision(Int_t v) {precision=v;}
  void setStartTime(const Char_t* t) {startTime=t;}
  Double_t getLowGraphLimit() {return lowGraphLimit;}
  Double_t getHighGraphLimit() {return highGraphLimit;}
  Double_t getLowWarnLimit() {return lowWarnLimit;}
  Double_t getHighWarnLimit() {return highWarnLimit;}
  Double_t getLowAlarmLimit() {return lowAlarmLimit;}
  Double_t getHighAlarmLimit() {return highAlarmLimit;}
  const Char_t* getUnits() {return units;}
  Int_t getPrecision() {return precision;}
  const Char_t* getStartTime() {return startTime.Data();}
  void clear();
  void print();
  void write(fstream&);
private:
  ClassDef(HOraSlowChanMeta,0) // Slowcontrol meta data of a channel
};

#endif  /* !HORASLOWCHANMETA */
