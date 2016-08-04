#ifndef HORAMAGNETIMPORT_H
#define HORAMAGNETIMPORT_H

using namespace std;
#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
#include <fstream>

class HDbConn;

class HOraMagnetImport : public TObject {
private:
  HDbConn* pConn;        // pointer to the connection class
  TString fileName;      // file name (= date in format ddmmyyyy)
  TString fileDirectory; // file directory
  ifstream* fin;         // input stream
public:
  HOraMagnetImport();
  ~HOraMagnetImport();
  Bool_t open(TString, TString dbName="db-hades");
  void close();
  Bool_t check();
  void print();
  void setFileDirectory(const Char_t* s) {fileDirectory =s; }
  const Char_t* getFileDirectory() { return fileDirectory.Data(); }
  Bool_t storeData(const Char_t*);
private:
  Bool_t openFile(const Char_t* filename);
  void closeFile();
  void commit();
  void rollback();
  ClassDef(HOraMagnetImport,0) // Class to store the Magnet data in Oracle
};

#endif  /* !HORAMAGNETIMPORT_H */
