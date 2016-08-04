//*-- AUTHOR : Ilse Koenig
//*-- Created : 13/08/2004 by Ilse Koenig
//*-- Modified : 13/04/2005 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HOraSlowPeriod
//
// Class for a slowcontrol time or run period
//
//////////////////////////////////////////////////////////////////////////////

#include "horaslowperiod.h"
#include "TDatime.h"

ClassImp(HOraSlowPeriod)

HOraSlowPeriod::HOraSlowPeriod() {
  // Constructor
  periodId=-1;
  runId=-1;
  filterFlag=1;
}

void HOraSlowPeriod::print() {
  // Prints the data to stdout
  cout<<startTime<<"  "<<endTime;
  if (runId>0) {
    cout<<setw(12)<<runId<<" "<<filename;
  }
  cout<<'\n';
}

void HOraSlowPeriod::write(fstream& fout) {
  // Streams the data
  fout<<startTime<<"  "<<endTime;
  if (runId>0) {
    fout<<setw(12)<<runId<<"  "<<filename;
  }
  fout<<'\n';
}

Int_t HOraSlowPeriod::getDuration() {
  // Returns the period length in seconds
  TDatime t1(startTime);
  TDatime t2(endTime);
  return (t2.Convert()-t1.Convert());
}

