#ifndef HDBFILEIO
#define HDBFILEIO
using namespace std;
#include <fstream> 
#include <iostream> 
#include "TObject.h"
#include "TString.h"
#include "hdbconn.h"
 
class HDbFileUtil;
class HDbTable;
 
class HDbFileIo : public TObject {
private:
  HDbConn* dbConn;      // Pointer to the Oracle connection class
  HDbFileUtil* dbUtil;  // Pointer to the Oracle utility class
  ifstream* fin;        // Pointer to an input filestream
  ofstream* fout;       // Pointer to an output filestream
  TString defTableName; // Default table name (generated from filename)
  TList* tempTables;    // List of temporary tables created in the current session
  HDbTable* refTable;   // Pointer to an existing table
  HDbTable* curTable;   // Pointer to a table form for I/O
  TString colDelim;     // Delimiter between columns in the file
  TString textDelim;    // Enclosing delimiter for text in a single column
  TString nullValue;    // Identifier for a null value in a column
  Int_t maxBufLen;      // Maximum length of the I/O buffer (4000 bytes)
  Int_t maxStmLen;      // Maximum length of the insert statement (about 8000 bytes)
  Bool_t autoCommit;    // Flag (if kTRUE, a transaction is committed automatically)
  Bool_t autoNull;      // Flag (if kTRUE, null values are added automatically)
  TString actDateMask;  // Actual date format
  Bool_t needsCommit;   // Flag (is kTRUE, when inserts/updates where not yet committed)
public:
  HDbFileIo();
  ~HDbFileIo();
  Bool_t connectDb(TString, TString dbName="db-hades");
  Bool_t dbIsOpen() {return dbConn->isOpen(); }
  void closeDbConn();  
  HDbTable* setTableName(const Char_t*);
  void setColumnDelimiter(const Char_t* s=" ");
  void setTextDelimiter(const Char_t s) { textDelim[0]=s; }
  void setNullValue(const Char_t* s="null") { nullValue=s; }
  void setAutoCommit(Bool_t b) { autoCommit=b; }
  void setAutoNull(Bool_t b=kTRUE) { autoNull=b; }
  Bool_t setDateMask(const Char_t* mask="DD-MON-YYYY HH24:MI:SS");
  Bool_t setSearchDate(const Char_t* time="sysdate");
  Bool_t setRun(const Char_t* daqFile);
  HDbTable* getCurTable() { return curTable; }
  const Char_t* getColumnDelimiter() { return colDelim.Data(); }
  const Char_t getTextDelimiter() { return textDelim[0]; }
  const Char_t* getNullValue() const { return nullValue.Data(); }
  Bool_t getAutoCommit() { return autoCommit; }
  Bool_t getAutoNull() { return autoNull; }
  const Char_t* getDateMask() { return actDateMask.Data(); }
  Bool_t openFile(const Char_t*, const Char_t*);
  void closeFile();
  Bool_t fileIsOpen();
  Bool_t fileIsWritable();
  Bool_t readFile(Int_t nLineStart=1,Int_t nLineEnd=999999);
  Bool_t makeInsert(const Char_t*, const Char_t*);
  Bool_t makeUpdate(const Char_t*, const Char_t*);
  Bool_t writeFile();
  Bool_t printResult();
  void showTemporaryTables();
  HDbTable* getTempTable(const Char_t*);
  void dropTemporaryTable(const Char_t*);
  void dropTemporaryTables();
  void commit(void);
  void rollback(void);
  void clear();
  void showExistingTable();
  void showSettings();
private:
  Bool_t openInput(const Char_t*);
  Bool_t openOutput(const Char_t*, const Char_t*);
  Int_t checkTable(TString&);
  Int_t getValues(TString&, Int_t);
  void addTempTable(HDbTable* table);
  void removeTempTable(HDbTable* table);
  Bool_t write(ostream&);
  TString getActDate();
  void checkCommit();
  void confirmCommit();
  ClassDef(HDbFileIo,0) // Oracle I/O class for client-side files
};

#endif /* !HDBFILEIO_H */
