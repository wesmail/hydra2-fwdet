#ifndef HDBTABLE
#define HDBTABLE
using namespace std;
#include <fstream> 
#include <iostream> 
#include <iomanip>
#include "TNamed.h"
#include "TString.h"
#include "TList.h"

class HDbConn;
class HDbFileUtil;
class HDbColumn;

class HDbTable : public TNamed {
private:
  HDbConn* dbConn;             // Pointer to the connection class
  HDbFileUtil* dbUtil;         // Pointer to the utility class
  TString      tabName;        // Name of the table
  TString      tabOwner;       // Owner of the table
  TList*       tabColumns;     // List of columns
  Int_t        nColumns;       // Number of columns
  Bool_t       tempTable;      // Flag (kTRUE for a table created inside the program)
  Bool_t       existing;       // Flag (kTRUE for an existing table)
  TString      colDefaultType; // Default type for columns
  TString      cond;           // Condition for a select statement
  TString      orderClause;    // Order clause for a select statement
public:
  HDbTable();
  HDbTable(HDbConn* p, HDbFileUtil*, const Char_t*);
  HDbTable(HDbConn* p, HDbFileUtil*, HDbTable&);
  ~HDbTable();
  Int_t getNColumns() { return nColumns; }
  HDbColumn* getColumn(Int_t);
  HDbColumn* getColumn(const Char_t*);
  HDbColumn* addColumn(const Char_t*,const Char_t* columnType=0);
  void addColumn(HDbColumn*);
  void removeColumn(const Char_t*);
  void clearColumns();
  void setColumns(const Char_t*, const Char_t* colTypes=0);
  void setColDefaultType(TString& s) { colDefaultType=s; }
  void setCondition(const Char_t* s);
  const Char_t* getCondition() { return cond.Data(); }
  void clearCondition() { cond=""; }
  void setOrderBy(const Char_t* s);
  const Char_t* getOrderBy() { return orderClause.Data(); }
  void clearOrderBy() { orderClause=""; }
  const Char_t* getColDefaultType() { return colDefaultType.Data(); }
  Bool_t describeTable();
  Bool_t createTable();
  Bool_t dropTable();
  Bool_t isTempTable() { return tempTable; }
  Bool_t isExisting() { return existing; }
  void show();
  Int_t writeTable(ostream&, const Char_t*, const Char_t*, const Char_t*);
private:
  void setNames(const Char_t*);
  ClassDef(HDbTable,0) // Class for Oracle table columns
};

#endif /* !HDBTABLE_H */
