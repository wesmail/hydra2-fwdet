#ifndef HORASLOWPARTITION_H
#define HORASLOWPARTITION_H

using namespace std;
#include "TNamed.h"
#include "TObjArray.h"
#include <iostream>
#include <iomanip>
#include <fstream>

class HOraSlowReader;
class HOraSlowPeriod;
class TGraph;

class HOraSlowPartition : public TNamed {
private:
  TString    partition;       // Name of the data partition
  TString    startTime;       // Start time of the data (format yyyy-mm-dd hh:mi:ss)
  TString    endTime;         // End time of the data (format yyyy-mm-dd hh:mi:ss)
  TObjArray* pRunPeriods;     // Array of time periods or runs
  TIterator* periodIter;      // Iterator on pRunPeriods
  HOraSlowReader* pOraReader; // Interface to Oracle
  TObjArray* pRates;          // Array of archiver rates
  TIterator* ratesIter;       // Iterator on pRates
  TGraph*    pGraph;          // Pointer to the graph showing the archiver rates
public:
  HOraSlowPartition(const Char_t* name="");
  ~HOraSlowPartition();
  HOraSlowReader* getOraReader() {return pOraReader;}
  Bool_t openOraInput();
  void closeOraInput();
  void setTimeRange(const Char_t*,const Char_t*);
  const Char_t* getStartTime() {return startTime.Data();}
  const Char_t* getEndTime() {return endTime.Data();}
  TObjArray* setNumPeriods(Int_t);
  TObjArray* getRunPeriods() {return pRunPeriods;}
  void setRates(TObjArray* p);
  HOraSlowPeriod* getPeriod(Int_t);
  HOraSlowPeriod* getRun(Int_t);
  HOraSlowPeriod* getRun(const Char_t*);
  void print(Int_t opt=0);  
  void write(fstream& fout,Int_t opt=0);
  void printRates();
  void writeRates(fstream& fout);
  TGraph* getRatesGraph(Int_t mStyle=7,Int_t mColor=4);
  void setHldFileFilter(TList* l);
private:
  void clearRunPeriods();
  void clearRates();
  void deleteGraph();
  ClassDef(HOraSlowPartition,0) // Slowcontrol data partition
};

#endif  /* !HORASLOWPARTITION */
