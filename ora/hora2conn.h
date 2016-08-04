#ifndef HORA2CONN_H
#define HORA2CONN_H

#include <string.h>
#include "TObject.h"

class HOra2Conn : public TObject {
protected:
  Char_t dbName[31];       // database name (may be a remote database)
  Char_t userName[21];     // name of user
  Bool_t isConnected;      // ==kTRUE if connection is open
  Int_t actRunId;          // actual runId (can be -1 if there are no data in Oracle)
  Int_t runStart;          // start time of actual run
  Char_t expLocation[11];  // Beam or simulation run
  Char_t historyDate[21];  // timestamp to retrieve historic data
  Int_t alignmentOutVers;  // current output version for alignment
  Bool_t needsServerCheck; // kTRUE before first call to getRunStart
public:
  HOra2Conn();
  ~HOra2Conn();
  Bool_t open();
  Bool_t open(Char_t*);
  Bool_t open(Char_t*, Char_t*);
  Bool_t isOpen() {return isConnected;}
  Bool_t reconnect();
  void close();
  void disconnect();
  void print();
  void showSqlError(const Char_t*,const Char_t* msg=0);
  Int_t getActRunId(void) { return actRunId; }
  Int_t getRunStart(Int_t);
  Int_t getActRunStart() { return runStart; }
  const Char_t* getExpLocation() { return expLocation; }
  Bool_t setHistoryDate(const Char_t*);
  const Char_t* getHistoryDate() { return historyDate; }
  Bool_t setParamRelease(const Char_t*);
  Bool_t setParamRelease(Int_t);
  Int_t getAlignmentOutputVersion() { return alignmentOutVers; }
  void setAlignmentOutputVersion(Int_t v) { alignmentOutVers=v; }
private:
  Bool_t openConnection(Char_t*);
  Char_t* getPassword();
  Int_t checkServerLoad();

  ClassDef(HOra2Conn,0) // Connection class to Oracle in Hydra2
};

#endif  /* !HORA2CONN_H */
