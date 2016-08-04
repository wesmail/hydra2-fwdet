//*-- AUTHOR   : Ilse Koenig
//*-- Created  : 09/09/2004 by Ilse Koenig
//*-- Modified : 21/03/2005 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HOraSlowChanRaw
//
// Class for the raw data for a non-waveform slowcontrol channel
//
//////////////////////////////////////////////////////////////////////////////

#include "horaslowchanraw.h"
#include "TDatime.h"

ClassImp(HOraSlowChanRaw)

HOraSlowChanRaw::HOraSlowChanRaw() {
  // Default constructor
  nanoSeconds=0;
  value=0.;
  status=0;
}

void HOraSlowChanRaw::fill(const Char_t* t,Int_t nano,Double_t val,Int_t sevr) {
  // Fills the timestamp, the nanoseconds, the value and the status
  // (channel data type float)
  timestamp=t;
  nanoSeconds=nano;
  value=val;
  if (sevr==3904||sevr==3872||sevr==3874) status=1;
  else status=0;
}

void HOraSlowChanRaw::fill(const Char_t* t,Int_t nano,Int_t val,Int_t sevr) {
  // Fills the timestamp, the nanoseconds, the value and the status 
  // (channel data type int)
  timestamp=t;
  nanoSeconds=nano;
  value=(Double_t)val;
  if (sevr==3904||sevr==3872||sevr==3874) status=1;
  else status=0;
}

Int_t HOraSlowChanRaw::getTimeDiff(const Char_t* t) {
  // Returns the time difference (in seconds) relative to time t
  TDatime t1(t);
  TDatime t2(timestamp);
  return (t2.Convert()-t1.Convert());  
}

void HOraSlowChanRaw::print(Int_t valPrec) {
  // Prints the data to stdout with data precision valPrec
  cout.setf(ios::fixed,ios::floatfield);
  cout<<timestamp<<setw(11)<<nanoSeconds
      <<cout.precision(valPrec)<<setw(valPrec+10)<<value
      <<setw(3)<<status<<'\n';
}

void HOraSlowChanRaw::write(fstream& fout,Int_t valPrec) {
  // Streams the data with data precision valPrec
  fout.setf(ios::fixed,ios::floatfield);
  fout<<timestamp<<setw(11)<<nanoSeconds
      <<fout.precision(valPrec)<<setw(valPrec+10)<<value
      <<setw(3)<<status<<'\n';
}

