#ifndef HORASLOWMANAGER_H
#define HORASLOWMANAGER_H

#include "TObject.h"
#include "TList.h"
#include "TGlobal.h"

class HOraSlowManager;
class HOraSlowPartition;
class HOraSlowReader;
class HOraSlowChannel;

R__EXTERN HOraSlowManager* gHOraSlowManager;

class HOraSlowManager : public TObject {
private:
  HOraSlowPartition* partition;     // Data partition 
  TList              channels;      // List of channels
  HOraSlowReader*    pOraReader;    // Interface to Oracle
  TList*             hldfileFilter; // List of hld-files to be applied as filter for output and drawing
public:
  HOraSlowManager();
  ~HOraSlowManager();
  static HOraSlowManager* instance(void) {
    return (gHOraSlowManager)?gHOraSlowManager:new HOraSlowManager;
  }
  Bool_t openOraInput();
  void closeOraInput();
  HOraSlowReader* getOraReader() {return pOraReader;}
  HOraSlowPartition* setPartition(const Char_t*,const Char_t* startTime="",
                                  const Char_t* endTime="");
  HOraSlowPartition* getPartition() {return partition;}
  TList& getChannels() {return channels;}
  HOraSlowChannel* addChannel(const Char_t*);
  HOraSlowChannel* getChannel(const Char_t*);
  void removeChannel(const Char_t*);
  void removeAllChannels();
  Bool_t readSummary();
  Bool_t readArchiverRates();
  void writeSummaryToAscii(const Char_t*,Int_t opt=0);
  void addHldfileFilter(const Char_t*);
  void clearHldfileFilter();
private:
  ClassDef(HOraSlowManager,0) // User interface to slowcontrol data
};

#endif  /* !HORASLOWMANAGER */
