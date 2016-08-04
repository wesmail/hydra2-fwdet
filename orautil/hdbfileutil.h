#ifndef HDBFILEUTIL
#define HDBFILEUTIL

#include "TObject.h"
#include "TString.h"

class HDbConn;
 
class HDbFileUtil : public TObject {
  HDbConn* dbConn; 
public:
  HDbFileUtil(HDbConn* db) {dbConn=db;}
  ~HDbFileUtil() {}
  void commit(void);
  void rollback(void);
  void showSqlError(const Char_t*);
  void showDynSqlError(const Char_t*);
  Bool_t executeDirect(TString&);
  Int_t getTokString(TString&, TString&, const Char_t*);
  Bool_t confirm(const Char_t *);
  ClassDef(HDbFileUtil,0) // Utility class for Oracle File I/O
};

#endif /* !HDBFILEUTIL_H */
