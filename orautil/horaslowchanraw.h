#ifndef HORASLOWCHANRAW_H
#define HORASLOWCHANRAW_H

using namespace std;
#include "TObject.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
#include <fstream>

class HOraSlowChannel;

class HOraSlowChanRaw : public TObject {
private:
  TString  timestamp;   // Timestamp (date + time)
  Int_t    nanoSeconds; // Nanoseconds
  Double_t value;       // Data value
  Int_t    status;      // Status flag (0=connected, 1=disconnected)
public:
  HOraSlowChanRaw();
  ~HOraSlowChanRaw() {}
  void setTimestamp(const Char_t* s) {timestamp=s;}
  void setNanoSeconds(Int_t v) {nanoSeconds=v;}
  void setValue(Double_t v) {value=v;}
  void setValue(Int_t v) {value=(Double_t)v;}
  void setStatus(Int_t v) {status=v;}
  void fill(const Char_t*,Int_t,Double_t,Int_t);
  void fill(const Char_t*,Int_t,Int_t,Int_t);
  const Char_t* getTimestamp() {return timestamp.Data();}
  Int_t getNanoSeconds() {return nanoSeconds;}
  Double_t getValue() {return value;}
  Int_t getStatus() {return status;}
  Int_t getTimeDiff(const Char_t*);
  void print(Int_t valPrec=3);
  void write(fstream& fout,Int_t valPrec=3);
private:
  ClassDef(HOraSlowChanRaw,0) // Raw data of a slowcontrol channel
};

#endif  /* !HORASLOWCHANRAW */
