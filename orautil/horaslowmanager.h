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
  TList*             hldfileFilter; // List of hld-files to be applied as filter for output and drawing
  TString            oraUser;       // Oracle user name
  TString            dbName;        // Database name
public:
  HOraSlowManager();
  ~HOraSlowManager();
  static HOraSlowManager* instance(void) {
    return (gHOraSlowManager)?gHOraSlowManager:new HOraSlowManager;
  }
  void setOraUser(const Char_t* name) {oraUser=name;}
  void setDbName(const Char_t* name)  {dbName=name;}
  const Char_t* getOraUser() {return oraUser.Data();}
  const Char_t* getDbName() {return dbName.Data();}
  HOraSlowPartition* setPartition(const Char_t*,const Char_t* startTime="",
                                  const Char_t* endTime="");
  HOraSlowPartition* getPartition() {return partition;}
  TList& getChannels() {return channels;}
  HOraSlowChannel* addChannel(const Char_t*);
  HOraSlowChannel* getChannel(const Char_t*);
  void removeChannel(const Char_t*);
  void removeAllChannels();
  Bool_t readArchiverRates();
  Bool_t readSummary();
  void writeSummaryToAscii(const Char_t*,Int_t opt=0);
  void addHldfileFilter(const Char_t*);
  void clearHldfileFilter();
private:
  ClassDef(HOraSlowManager,0) // User interface to slowcontrol data
};

#endif  /* !HORASLOWMANAGER */
