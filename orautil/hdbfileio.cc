//*-- AUTHOR : Ilse Koenig
//*-- Modified : 15/09/2000

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
// HDbFileIo
//
// Oracle I/O class to load data from a client-side file into an Oracle table
// or to write data from an Oracle table to a client-side file
// (uses the Oracle C/C++ precompiler)
//
// Description  of the program:
//      http://www-hades.gsi.de/persons/ilse/orafileio.htm
// 
/////////////////////////////////////////////////////////////////////////////

#include "hdbfileio.h"
#include "hdbfileutil.h"
#include "hdbtable.h"
#include "hdbcolumn.h"
#include "TStopwatch.h"
#include <unistd.h>
#include <time.h>
 
ClassImp(HDbFileIo)

HDbFileIo::HDbFileIo() {
  // Constructor creates the Oracle connection class, but does not open a connection
  dbConn=new HDbConn;
  dbUtil=0;
  fin=0;
  fout=0;
  colDelim=" ";
  textDelim="'";
  nullValue="null";
  maxBufLen=4000;
  maxStmLen=8000;
  autoCommit=kFALSE;
  autoNull=kTRUE;
  tempTables=new TList;
  refTable=0;
  curTable=0;
  needsCommit=kFALSE;
}


HDbFileIo::~HDbFileIo() {
  // Destructor closes the Oracle connection and a file not yet closed
  if (dbConn) {
    closeDbConn();
    delete dbConn;
  }
  if (tempTables) delete tempTables;
  if (dbUtil) delete dbUtil;
  closeFile();
  if (fin) delete fin;
  if (fout) delete fout;
}

Bool_t HDbFileIo::connectDb(TString userName, TString dbName) {
  // Opens an Oracle connection (e.g.with:  "hades","db-hades")
  if (dbConn->isOpen()) dbConn->closeDbConn();
  Bool_t rc=dbConn->connectDb(userName,dbName);
  if (rc) {
    dbUtil=new HDbFileUtil(dbConn);
    setDateMask();
  }
  return rc;
}   
 
void HDbFileIo::closeDbConn() {
  // Closes the Oracle connection
  // Asks for committing all not yet saved transactions
  if (needsCommit) confirmCommit();
  dbConn->closeDbConn(); // makes rollback automatically
  if (curTable) {
    delete curTable;
    curTable=0;
    defTableName="";
  }
  if (refTable) {
    delete refTable;
    refTable=0;
  }
  if (tempTables) tempTables->Delete();
  if (dbUtil) {
    delete dbUtil;
    dbUtil=0;
  }
}

Bool_t HDbFileIo::openFile(const Char_t* fName,const Char_t* ioMode) {
  // Closes an open file and opens the new file 
  // Possible I/O modes are "in", "out", "app"
  closeFile();
  defTableName="";
  TString m(ioMode);
  m.ToLower();
  if (m.CompareTo("in")==0) return openInput(fName);
  if (m.CompareTo("out")==0 || m.CompareTo("app")==0)
    return openOutput(fName,m.Data());
  Error("HDbFileIo::openFile",
        "Wrong I/O mode! Possible modes are \"in\", \"out\", \"app\""); 
  return kFALSE;
}

void HDbFileIo::closeFile() {
  // Closes the file
  if (fin && fin->rdbuf()->is_open()!=0) fin->close();
  if (fout && fout->rdbuf()->is_open()!=0) fout->close();
}

Bool_t HDbFileIo::fileIsOpen() {
  // Returns kTRUE in case the file is open
  if ((fin && fin->rdbuf()->is_open()!=0) ||
      (fout && fout->rdbuf()->is_open()!=0)) return kTRUE;
  return kFALSE;
}

Bool_t HDbFileIo::fileIsWritable() {
  // Returns kTRUE in case the file is writable
  if (fout && fout->rdbuf()->is_open()!=0) return kTRUE;
  return kFALSE;
}

HDbTable* HDbFileIo::setTableName(const Char_t* tableName) {
  // Sets the name of the Oracle table.
  // Checks if a table with this name exists already.
  // Returns a pointer to a table.
  // For an existing table this form contains all columns of this table. If the
  // table does not exist the list of columns is empty. This table can be used
  // to define the columns for an insert/update.
  // (You can call the HDbTable::clear() function to remove all columns from the
  // list. Then all or only some columns can be added again in the order needed
  // for the file I/O.) 
  clear();
  if (dbConn->isOpen()) {
    TString t(tableName);
    t=t.Strip(t.kBoth);
    t.ToUpper();
    Int_t n=t.First(".");
    if (n<0) {
      t.Insert(0,".");
      t.Insert(0,dbConn->getUserName());
    }
    refTable=getTempTable(t);
    if (!refTable) {
      refTable=new HDbTable(dbConn,dbUtil,t);
      refTable->describeTable();
    }
    curTable=new HDbTable(dbConn,dbUtil,*refTable);
  }
  else Error("HDbFileIo::setTableName","No connection to Oracle");
  return curTable;
}

void HDbFileIo::setColumnDelimiter(const Char_t* s) {
  // Sets the delimiter beween columns (default value: " ")
  colDelim=s;
  if (colDelim.Length()>1) {
    colDelim.Strip(colDelim.kBoth);
    if (colDelim.Length()==0) colDelim=" ";
  } else {
    if (colDelim.Length()==0) {
      Error("HDbFileIo::setColumnDelimiter",
            "\n  Delimter for columns is empty, replaced by default delimiter whitespace");
      colDelim=" ";
    }
  }
}

Bool_t HDbFileIo::setDateMask(const Char_t* mask) {
  // Sets a date mask in Oracle
  if (dbConn->isOpen()) {
    actDateMask=mask;
    TString stm("begin hades_oper.hdate.set_datemask(");
    if (mask && strlen(mask)>2) stm=stm + "'" + mask + "'); end;";
    else stm=stm + "); end;";
    return dbUtil->executeDirect(stm);
  }
  actDateMask="";
  Error("HDbFileIo::setDateMask","No connection to Oracle");
  return kFALSE;
}

Bool_t HDbFileIo::setSearchDate(const Char_t* time) {
  // Sets the search date in Oracle used to select data from views which
  // require a date specified before retrieving results.
  // The default value is the actual time on the server.
  if (dbConn->isOpen()) {
    TString stm("begin hades_oper.run_query.set_date(");
    stm=stm + "'" + time + "'); end;";
    return dbUtil->executeDirect(stm);
  } else Error("HDbFileIo::setSearchDate","No connection to Oracle");
  return kFALSE;
}

Bool_t HDbFileIo::setRun(const Char_t* daqFile) {
  // Sets the run in Oracle used to select data from views which require
  // a run specified before retrieving results.
  if (dbConn->isOpen()) {
    TString stm("begin hades_oper.run_query.set_run(");
    stm=stm + "'" + daqFile + "'); end;";
    return dbUtil->executeDirect(stm);
  } else Error("HDbFileIo::setRun","No connection to Oracle");
  return kFALSE;
}


HDbTable* HDbFileIo::getTempTable(const Char_t* tableName) {
  // Returns a pointer to a temporary table created inside the current
  // Oracle session
  TString t(tableName);
  t=t.Strip(t.kBoth);
  t.ToUpper();
  HDbTable* table=(HDbTable*)tempTables->FindObject(tableName);
  return table;
}

void HDbFileIo::showTemporaryTables() {
  // Shows all temporary tables created inside the current Oracle session
  if (tempTables->Last()==0) {
    cout<<"The list of temporary tables is empty!"<<endl;
    return;
  }
  TIter next(tempTables);
  HDbTable* t;
  while ((t=(HDbTable*)next())) {
    t->show();
  }
}   

void HDbFileIo::dropTemporaryTable(const Char_t* tableName) {
  // Drops a temporary table created inside the current Oracle session.
  // Each Drop must be confirmed.
  if (needsCommit) confirmCommit();
  Bool_t rc=kFALSE;
  HDbTable* t=getTempTable(tableName);
  if (t) {
    rc=t->dropTable();
    if (rc) removeTempTable(t);
  } else cout<<"Table does not exist or is not a temporary table"<<endl;
}

void HDbFileIo::dropTemporaryTables() {
  // Drops all temporary tables created inside the current Oracle session.
  // Each Drop must be confirmed.
  if (needsCommit) confirmCommit();
  if (tempTables->Last()==0) {
    cout<<"List of temporary tables is empty!"<<endl;
    return;
  }
  TIter next(tempTables);
  HDbTable* t;
  Bool_t rc=kTRUE;
  while ((t=(HDbTable*)next())) {
    rc=t->dropTable();
    if (rc) removeTempTable(t);
  }
}   

void HDbFileIo::commit(void) {
  // Commits Oracle transactions
  if (dbUtil) dbUtil->commit();
  needsCommit=kFALSE;
}

void HDbFileIo::rollback(void) {
  // Rolls back Oracle transactions
  if (dbUtil) dbUtil->rollback();
  needsCommit=kFALSE;
}

void HDbFileIo::showExistingTable() {
  // Shows the table definition of the existing table
  if (refTable) {
    if (refTable->isExisting()) refTable->show();
    else cout<<"Table "<<refTable->GetName()<<
               "is not a permanent table and might not exist."<<endl;
  }
  else cout<<"No table defined"<<endl;
}

void HDbFileIo::clear() {
  // Deletes the actual table objects curTable and refTable
  if (curTable) delete curTable;
  curTable=0;
  if (refTable) delete refTable;
  refTable=0;  
}

void HDbFileIo::showSettings() {
  // Shows the actual parameter settings for the I/O
  TString cD(colDelim);  
  if (cD.Contains(" ")) cD.ReplaceAll(" ","whitespace");
  if (cD.Contains("\t")) cD.ReplaceAll("\t","tab");
  cout<<"------------------------------------------------------"<<endl;
  cout<<"------------------  I/O Parameters  ------------------"<<endl;
  cout<<"------------------------------------------------------"<<endl;
  cout<<"Default table name:           "<<defTableName<<endl;
  cout<<"Delimiter for columns:        "<<cD<<endl;
  cout<<"Text enclosed in:             "<<textDelim<<endl;
  cout<<"Null (no value) in i/o:       "<<nullValue<<endl;
  cout<<"Actual date mask:             "<<actDateMask<<endl;
  cout<<"Automatic add of nulls:       ";
  if (autoNull) cout<<"yes"<<endl; 
  else cout<<"no"<<endl; 
  cout<<"Automatic commit for inserts: ";
  if (autoCommit) cout<<"yes"<<endl; 
  else cout<<"no"<<endl; 
  cout<<"Maximum i/o buffer size:      "<<maxBufLen<<endl;
  cout<<"------------------------------------------------------"<<endl;
}

Bool_t HDbFileIo::readFile(Int_t nLineStart, Int_t nLineEnd) {
  // Reads a file starting at line nLineStart( default 1) and stopping after
  // line nLineEnd (default 999999) or EOF and inserts the data into an Oracle
  // table. Inserts into an existing table must be confirmed.
  // When the name of the table was not set before, a default table name is
  // generated from the filename. The creation of a temporary table must be
  // confirmed.
  // The file is not closed automatically.
  if (!dbConn->isOpen()) {
    Error("HDbFileIo::readFile","No connection to Oracle");
    return kFALSE;
  }
  if (!fin || fin->rdbuf()->is_open()==0) {
    Error("HDbFileIo::readFile","No file open");
    return kFALSE;
  }
  fin->seekg(0,ios::beg);
  fin->clear();
  TString stm;
  TString insert("insert into ");
  Bool_t all=kFALSE;
  Bool_t tabChecked=kFALSE;
  Int_t nInserts=0;
  Int_t i, nData, nColumns=0, l1=0, l2=0;
  TString line(maxBufLen);
  //  TStopwatch timer;
  //  timer.Reset();
  //  timer.Start();
  do {
    l1=l2;
    i=0;
    stm="begin ";
    while (stm.Length()<maxStmLen && !all) {
      line.ReadLine(*fin);
      l2++;
      if (fin->eof() || l2>nLineEnd) {
        all=kTRUE;
        break;
      }
      if (l2<nLineStart) continue;
      line=line.Strip(line.kBoth);
      if (line[0]!='\0' && line[0]!='/') {
        if (!tabChecked) {
          nData=checkTable(line); 
          if (nData==0) return kFALSE;
          tabChecked=kTRUE;
          insert=insert + curTable->GetName() + "(";
          nColumns=curTable->getNColumns();
          for(Int_t j=0;j<nColumns;j++) {
            insert=insert + curTable->getColumn(j)->GetName();
            if (j<(nColumns-1)) insert=insert + ",";
          }
          insert=insert + ")";
          if (!curTable->isTempTable()) {
            cout<<"-----------------------------------------"<<endl;
            cout<<"-------  Insert in existing table  ------"<<endl;
            cout<<"-----------------------------------------"<<endl;
            cout<<insert<<endl;
            if (!dbUtil->confirm("insert")) return kFALSE;
          }
          insert=insert +" values (";
        }
        else nData=getValues(line,nColumns);
        if (nData==nColumns) stm = stm + insert + line + ");";
        else {
          if (autoNull) {
            stm = stm + insert + line + ",";
            for(Int_t k=++nData;k<nColumns;k++) stm = stm + "null" + ",";
            stm = stm + "null" + ");";
          } else {
            Error("HDbFileIo::readFile",
                  "Number of values too small in line %i",l2);
            return kFALSE;
          }
        }
        i++;
      }
    }
    stm=stm + "end;";
    // cout<<endl<<"SQL: "<<stm<<endl;
    if (i>0) {
      if (dbUtil->executeDirect(stm)==kFALSE) {
        Error("HDbFileInput::readData","Error in lines %i - %i",(l1+1),l2);
        rollback();
        return kFALSE;
      }
      nInserts+=i;
    }
  } while (all==kFALSE);
  cout<<"-----------------------------------------"<<endl;
  cout<<"  "<<nInserts<<" rows inserted in table "<<curTable->GetName()<<endl;
  cout<<"-----------------------------------------"<<endl;
  //  timer.Stop();
  //  cout<<endl<<"Real time:   "<<timer.RealTime()<<endl;
  //  cout<<"Cpu time:    "<<timer.CpuTime()<<endl;
  //  cout<<"Inserts/sec: "<<(nInserts/timer.CpuTime())<<endl;
  checkCommit();
  return kTRUE;
} 

Bool_t HDbFileIo::makeInsert(const Char_t* colNames, const Char_t* colValues) {
  // Inserts one row in the actual table
  // colNames:  comma separated list of columns;
  //            use "*" for an insert with values for all columns
  // colValues: comma separated list of values for the columns 
  TString cols(colNames);
  cols=cols.Strip(cols.kBoth);
  cols.ToUpper();
  TString  stm("insert into ");
  stm=stm + curTable->GetName();
  if (cols[0]=='*') stm=stm + " values (" + colValues + ")";
  else stm=stm + "(" + cols + ") values (" + colValues + ")";
  if (!curTable->isTempTable()) {
    cout<<"-----------------------------------------"<<endl;
    cout<<"-------  Insert in existing table  ------"<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<stm<<endl;
    if (!dbUtil->confirm("insert")) return kFALSE;
  }
  if (dbUtil->executeDirect(stm)==kFALSE) {
    rollback();
    return kFALSE;
  }
  checkCommit();
  return kTRUE;
}

Bool_t HDbFileIo::makeUpdate(const Char_t* setVal, const Char_t* condition) {
  // Updates one or more rows in a temporary table
  // setVal   : SET part of an update statement (col1=val1, col2=val2, ...) 
  // condition: Condition for the columns to be updated
  //            WARNING: An empty condition will update all columns!
  if (!curTable->isTempTable()) {
    cout<<"The update of an existing table is forbidden in this program"<<endl;
    return kFALSE;
  }
  TString con(condition);
  con=con.Strip(con.kBoth);
  if (con.Length()==0) cout<<"*****  WARNING: No condition set  *****"<<endl;
  TString  stm("update ");
  stm=stm + curTable->GetName() + " set " + setVal ;
  if (con.Length()>0) stm=stm + " where " + con;
  if (dbUtil->executeDirect(stm)==kFALSE) {
    rollback();
    return kFALSE;
  }
  checkCommit();
  return kTRUE;
}

Bool_t HDbFileIo::writeFile() {
  // Writes the data from an Oracle table into a file
  Bool_t rc=kFALSE;
  if (fout && fout->rdbuf()->is_open()!=0) {
    rc=write(*fout);
    closeFile();
  } else Error("HDbFileIo::writeFile()","No output file open");
  return rc;
}

Bool_t HDbFileIo::printResult() {
  // Prints the data from an Oracle table on the screen
  return write(cout);
}

////////////////////////////  private functions  //////////////////////

Bool_t HDbFileIo::openInput(const Char_t* fileName) {
  // Opens a file for input and generates the default table name
  if (needsCommit) confirmCommit();
  clear();
  defTableName="";
  if (!fin) fin=new ifstream;
  fin->clear();
  fin->open(fileName,ios::in);
  if (fin && fin->rdbuf()->is_open()!=0) {
    defTableName=fileName;
    defTableName.Remove(0,(defTableName.Last('/')+1));
    Int_t n=defTableName.First('.');
    if (n>0) defTableName.Remove(n,defTableName.Length()-n);
    defTableName.ToUpper();
    defTableName.Insert(0,"TMP_");
    return kTRUE;
  }
  Error("HDbFileIo::openInput","File not found");
  return kFALSE;
}

Bool_t HDbFileIo::openOutput(const Char_t* fileName, const Char_t* oMode) {
  // Opens a file for output
  if (!fout) fout=new ofstream;
  if (strcmp(oMode,"out")==0) {
    if (access(fileName,F_OK)!=0) fout->open(fileName,ios::out);
    else {
      cout<<"File "<<fileName<<" exists already"<<endl;
      if (dbUtil->confirm("Recreate file")==kFALSE) return kFALSE;
      fout->open(fileName,ios::out);
    }
  } else fout->open(fileName,ios::app);
  if (fout && fout->rdbuf()->is_open()!=0) return kTRUE;
  Error("HDbFileIo::openOutput","File not found or not writable");
  return kFALSE;
}

Int_t HDbFileIo::checkTable(TString& line) {
  // Sets the default table name and table definition if necessary. Checks if
  // the table exists and creates it otherwise. A newly created table is added
  // in the list of temporary tables.
  Int_t nData=0;
  Int_t nColumns=0;
  Char_t buf[4];
  if (!curTable) {
    curTable=setTableName(defTableName.Data());
    nColumns=curTable->getNColumns();
    if (nColumns==0) {
      nData=getValues(line,254);        
      TString s1="Col";
      for(Int_t i=1;i<=nData;i++) {
        sprintf(buf,"%i",i);
        TString s=s1 + buf;
        curTable->addColumn(s.Data());
      }
      if (!curTable->createTable()) return 0;
      addTempTable(curTable);
    } else nData=getValues(line,nColumns);        
  } else {
    nColumns=curTable->getNColumns();
    nData=getValues(line,nColumns);
    if (!curTable->isExisting()) {
      if (!curTable->createTable()) return 0;
      addTempTable(curTable); 
    }    
  }  
  return nData;
}
  
void HDbFileIo::addTempTable(HDbTable* table) {
  // Adds a temporary table to the list of temporary tables
  if (!tempTables) tempTables=new TList;
  HDbTable* t=new HDbTable(dbConn,dbUtil,*table);
  tempTables->Add(t);
}

void HDbFileIo::removeTempTable(HDbTable* table) {
  // Removes a table from the list of temporary tables and deletes it.
  // Deletes also the actual tables when they have identical names.
  tempTables->Remove(table);
  if (curTable && strcmp(curTable->GetName(),table->GetName())==0) {
    delete curTable;
    curTable=0;
    delete refTable;
    refTable=0;
  }
  delete table;
  table=0;
}

Int_t HDbFileIo::getValues(TString& str, Int_t nColumns) {
  // Gets the column values from a line read from file
  TString s1(str.Strip(str.kBoth));
  Int_t slen=s1.Length();
  if (slen==0) return 0;
  TString s2, s3;
  Int_t n=0;
  str="";
  while (slen>0 && n<nColumns) {
    if (s1[0]==textDelim[0]) {
      s1=s1.Remove(0,1);
      slen=dbUtil->getTokString(s1,s2,textDelim);
      str=str + "'" + s2 + "'";
      if (slen>0 && colDelim[0]!=' ') {
        s1.Remove(0,colDelim.Length()); // remove delimiter
        s1=s1.Strip(s1.kLeading,' ');
        slen=s1.Length();
      }
    } else {
      slen=dbUtil->getTokString(s1,s2,colDelim);
      if (s2.Length()==0 || s2.CompareTo(nullValue)==0) str=str + "null";
      else str=str + "'" + s2 + "'";
    }
    n++;
    if (slen>0 && n<nColumns) str=str + ",";
  }
  return n;
}

Bool_t HDbFileIo::write(ostream& pout) {
  // Writes the table content to a file or to stdout
  if (!curTable) {
    Error("HDbFileIo::write","No table defined!");
    return kFALSE;
  }
  if (!curTable->isExisting()) {
    Error("HDbFileIo::write","Table does not exist!");
    return kFALSE;
  }
  Int_t n=curTable->getNColumns();
  Int_t r=refTable->getNColumns();
  if (r>0) {
    if (n>0) {
      for(Int_t i=0;i<n;i++) {
        HDbColumn* col=curTable->getColumn(i);
        HDbColumn* refCol=refTable->getColumn(col->GetName());
        if (refCol) col->copyColumn(refCol);
        else {
          Error("HDbFileIo::write","Column %s does not exist!",col->GetName());
          return kFALSE;
        }
      }
    } else {
      for(Int_t i=0;i<r;i++) {
        HDbColumn* refCol=refTable->getColumn(i);
        HDbColumn* col=curTable->addColumn(refCol->GetName());
        col->copyColumn(refCol);
      }
    }
  }
  pout<<"// -----------------------------------------"<<endl;
  pout<<"// Result of Oracle table "<<curTable->GetName()<<endl;
  pout<<"// Date: "<<getActDate()<<"// Columns:"<<endl<<"//    ";
  for(Int_t i=0;i<n-1;i++) pout<<curTable->getColumn(i)->GetName()<<"  ";
  pout<<curTable->getColumn(n-1)->GetName()<<endl;
  pout<<"// -----------------------------------------"<<endl;
  Int_t l=curTable->writeTable(pout,colDelim.Data(),textDelim.Data(),
                               nullValue.Data());
  if (l<0) return kFALSE;
  cout<<"-----------------------------------------"<<endl;
  cout<<"  "<<l<<" rows selected from table "<<curTable->GetName()<<endl;
  cout<<"-----------------------------------------"<<endl;
  return kTRUE; 
}

TString HDbFileIo::getActDate() {
  // Returns the actual date and time
  struct tm *newtime;
  time_t t;
  time(&t);
  newtime=localtime(&t);
  TString s(asctime(newtime));
  return s;
}

void HDbFileIo::checkCommit() {
  // Calls commit() when flag autoCommit is kTRUE
  if (autoCommit) commit();
  else {
    needsCommit=kTRUE;
    cout<<"***************************************"<<endl;
    cout<<"*****  Transaction needs commit.  *****"<<endl;
    cout<<"***************************************"<<endl;
  }
}

void HDbFileIo::confirmCommit() {
  // Ask to confirm uncommitted transactions
  cout<<"************************************************"<<endl;
  cout<<"*****  There are uncommitted transactions  *****"<<endl;
  if (dbUtil && dbUtil->confirm("commit")) commit();
  else rollback();
}
