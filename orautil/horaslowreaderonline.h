#ifndef HORASLOWREADERONLINE_H
#define HORASLOWREADERONLINE_H

using namespace std;
#include "horaslowreader.h"

class HOraSlowPartition;
class HOraSlowPeriod;
class HOraSlowChannel;

class HOraSlowReaderOnline : public HOraSlowReader {
public:
  HOraSlowReaderOnline() : HOraSlowReader() {;}
  ~HOraSlowReaderOnline() {;}
  Bool_t readRunPeriods();
  Bool_t readChannelRunSum(HOraSlowChannel*);
  Bool_t readChannelMetaData(HOraSlowChannel*);
  Bool_t readRawData(HOraSlowChannel*,const Char_t*,const Char_t*);
private:
  Int_t readChannelId(HOraSlowChannel*);
  Bool_t readPeriods(TObjArray*,Int_t,Int_t,Int_t);
  ClassDef(HOraSlowReaderOnline,0) // Class to read slowcontrol data from Oracle during beam time
};

#endif  /* !HORASLOWREADERONLINE */
