#ifndef HDBCONN
#define HDBCONN

#include "TObject.h"
#include "TString.h"
 
class HDbConn : public TObject {
  Bool_t dbIsOpen;  // Flag (kTRUE, if the connection is open)
  Bool_t noDDL;     // Flag (kTRUE, if Readonly connection)
  TString uName;    // Name of the user
public:
  HDbConn();
  ~HDbConn();
  Bool_t connectDb(TString, TString dbName="db-hades");
  void closeDbConn();
  Bool_t isOpen() { return dbIsOpen; }
  Bool_t isReadonly() { return noDDL; }
  const Char_t* getUserName() { return uName.Data(); } 
  void showSqlError(const Char_t*);
private:
  ClassDef(HDbConn,0) // Oracle connection class
};

#endif /* !HDBCONN_H */
