#ifndef HPARORA2IO_H
#define HPARORA2IO_H

#include "hpario.h"
#include "hora2conn.h"

class HOra2Info;

class HParOra2Io : public HParIo {
private:
  HOra2Conn* pConn;      // pointer to the connection class
  HOra2Info* pInfo;      // pointer to the utility class
  Bool_t isConnected;    // ==kTRUE if connection is open
public:
  HParOra2Io();
  ~HParOra2Io();
  Bool_t open();
  Bool_t open(Char_t *userName);
  Bool_t open(Char_t *dbName, Char_t *userName);
  Bool_t reconnect();
  void close();
  void disconnect();
  Bool_t check() {return isConnected;}  // kTRUE if connection is open
  void print();
  void setDetParIo(const Text_t*);
  HOra2Info* getOra2Info() {return pInfo;}
  Bool_t setHistoryDate(const Char_t*);
  Bool_t setParamRelease(const Char_t*);
  TList* getListOfRuns(const Char_t*,const Char_t* startAt="",const Char_t* endAt="");
private:
  Bool_t activateDetIo();
  ClassDef(HParOra2Io,0) // Class for parameter I/O from Oracle
};

#endif  /* !HPARORA2IO_H */

