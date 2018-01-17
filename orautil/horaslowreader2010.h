#ifndef HORASLOWREADER2010_H
#define HORASLOWREADER2010_H

using namespace std;
#include "horaslowreader.h"

class HOraSlowReader2010 : public HOraSlowReader {
public:
  HOraSlowReader2010() : HOraSlowReader() {;}
  ~HOraSlowReader2010() {;}
  Bool_t readArchiverRates();
  Bool_t readChannelRunSum(HOraSlowChannel*);
  Bool_t readChannelMetaData(HOraSlowChannel*);
  Bool_t readRawData(HOraSlowChannel*,const Char_t*,const Char_t*);
private:
  Int_t readChannelId(HOraSlowChannel*);
  ClassDef(HOraSlowReader2010,0) // Class to read slowcontrol data from Oracle for beam times before 2016
};

#endif  /* !HORASLOWREADER2010 */
