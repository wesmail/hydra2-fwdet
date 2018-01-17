#ifndef HORASLOWREADER2016_H
#define HORASLOWREADER2016_H

using namespace std;
#include "horaslowreader.h"

class HOraSlowReader2016 : public HOraSlowReader {
public:
  HOraSlowReader2016() : HOraSlowReader() {;}
  ~HOraSlowReader2016() {;}
  Bool_t readChannelRunSum(HOraSlowChannel*);
  Bool_t readChannelMetaData(HOraSlowChannel*);
  Bool_t readRawData(HOraSlowChannel*,const Char_t*,const Char_t*);
private:
  Int_t readChannelId(HOraSlowChannel*);
  ClassDef(HOraSlowReader2016,0) // Class to read slowcontrol data from Oracle for beam times since 2016
};

#endif  /* !HORASLOWREADER2016 */
