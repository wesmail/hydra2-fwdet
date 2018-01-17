#ifndef HORASLOWREADER_H
#define HORASLOWREADER_H

#define NMAX_SCS 2000

using namespace std;
#include "TObject.h"
#include "TString.h"

class HDbConn;
class HOraSlowPartition;
class HOraSlowPeriod;
class HOraSlowChannel;

class HOraSlowReader : public TObject {
protected:
  HDbConn*           pConn;      // pointer to the connection class
  HOraSlowPartition* pPartition; // pointer to partition class
public:
  HOraSlowReader();
  virtual ~HOraSlowReader();
  void setPartition(HOraSlowPartition* p) {pPartition=p;}
  Bool_t open();
  void close();
  void print();
  Bool_t isOpen();
  virtual Bool_t readRunPeriods();
  virtual Bool_t readArchiverRates()                                       {return kFALSE;}
  virtual Bool_t readChannelRunSum(HOraSlowChannel*)                       {return kFALSE;}
  virtual Bool_t readChannelMetaData(HOraSlowChannel*)                     {return kFALSE;}
  virtual Bool_t readRawData(HOraSlowChannel*,const Char_t*,const Char_t*) {return kFALSE;}
protected:
  Bool_t readPartition();
  Bool_t readPeriods(Char_t*,TObjArray*,Int_t,Int_t,Int_t);
  ClassDef(HOraSlowReader,0) // Base class to read slowcontrol data from Oracle
};

#endif  /* !HORASLOWREADER */
