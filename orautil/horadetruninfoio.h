#ifndef HORADETRUNINFOIO_H
#define HORADETRUNINFOIO_H

using namespace std;
#include "TObject.h"
#include "TString.h"

class HDbConn;

class HOraDetRunInfoIo : public TObject {
private:
  HDbConn* pConn;     // pointer to connection class
  Bool_t hasErrors;   // kTRUE, if current application returned errors
  Int_t numChanges;   // number of changes since last commit/rollback
public:
  HOraDetRunInfoIo();
  ~HOraDetRunInfoIo();
  Bool_t open(const Char_t*,const Char_t* dbName="db-hades");
  void close();
  void print();
  Bool_t isOpen();
  Int_t getNumChanges() {return numChanges;}
  Bool_t setDetector(const Char_t*);
  Int_t createComment(const Char_t*,const Char_t*);
  Bool_t setRunQuality(const Char_t*,Int_t,Int_t);
  void finish();
  void rollback();  
private:
  void commit();
  ClassDef(HOraDetRunInfoIo,0) // Interface to the analysis run info in Oracle
};

#endif  /* !HORADETRUNINFOIO */
