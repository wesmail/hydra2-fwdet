//*-- AUTHOR : Ilse Koenig
//*-- Created : 13/08/2004 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HOraSlowArchRateObj
//
// Class for the slowcontrol archiver rate (entries per minute)
//
//////////////////////////////////////////////////////////////////////////////

#include "horaslowarchrateobj.h"
#include "TDatime.h"

ClassImp(HOraSlowArchRateObj)

HOraSlowArchRateObj::HOraSlowArchRateObj() {
  // Default constructor
  rate=0;
}

HOraSlowArchRateObj::HOraSlowArchRateObj(const Char_t* t,Int_t n) {
  // Constructor with arguments : starttime t and rate n 
  startTime=t;
  rate=n;
}

Int_t HOraSlowArchRateObj::getTimeDiff(const Char_t* t) {
  // Returns the time difference (in seconds) relative to time t
  TDatime t1(t);
  TDatime t2(startTime);
  return (t2.Convert()-t1.Convert());
}

void HOraSlowArchRateObj::print() {
  // Prints the timestamp and rate to stdout
  cout<<startTime<<"  "<<setw(12)<<rate<<'\n';
}

void HOraSlowArchRateObj::write(fstream& fout) {
  // Streams the timestamp and rate 
  fout<<startTime<<"  "<<setw(12)<<rate<<'\n';
}
