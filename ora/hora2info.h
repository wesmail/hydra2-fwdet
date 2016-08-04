#ifndef HORA2INFO_H
#define HORA2INFO_H

#include "TObject.h"
#include "TString.h"
#include "TList.h"

class HOra2Conn;

class HOra2Info : public TObject {
private:
  HOra2Conn* pConn;   // pointer to Oracle connection class
public:
  HOra2Info(HOra2Conn* p=0) {pConn=p;}
  ~HOra2Info(void) {}
  void showRunStart(Int_t);  
  Int_t getRunId(const Text_t*);
  Int_t getRefRunId(const Text_t*);
  Int_t getLastRun(const Text_t*);
  Bool_t getDaqFilename(Int_t,TString&);
  TList* getListOfRuns(const Char_t* ,const Char_t* startAt="",const Char_t* endAt="");
  TList* getListOfHldFiles(const Char_t* ,const Char_t* startAt="",const Char_t* endAt="",
                           const Char_t* runType="");
private:
  TList* getListOfSimRefRuns(const Char_t*,const Char_t*,const Char_t*);
  Bool_t getRunStart(const Char_t*,TString&);
  Bool_t getSimRefRunStart(const Char_t*,TString&);
  ClassDef(HOra2Info,0) // utility class for the interface to Oracle in Hydra2
};

#endif  /* !HORA2INFO_H */
