//*-- AUTHOR : Ilse Koenig
//*-- Created : 04/09/2004 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HOraSlowChanMeta
//
// Class for the slowcontrol meta data of a channel
//
//////////////////////////////////////////////////////////////////////////////

#include "horaslowchanmeta.h"

ClassImp(HOraSlowChanMeta)

HOraSlowChanMeta::HOraSlowChanMeta() {
  // Default constructor
  clear();
}

void HOraSlowChanMeta::clear() {
  // Clears the data
  lowGraphLimit=highGraphLimit=lowWarnLimit=highWarnLimit=lowAlarmLimit=highAlarmLimit=0.;
  units="";
  precision=0;
  startTime="";
}

void HOraSlowChanMeta::print() {
  // Prints the data to stdout
  cout<<"Start Time:         "<<startTime<<'\n';
  cout<<"Low Graph Limit:    "<<lowGraphLimit<<'\n';
  cout<<"High Graph Limit:   "<<highGraphLimit<<'\n';
  cout<<"Low Warning Limit:  "<<lowWarnLimit<<'\n';
  cout<<"High Warning Limit: "<<highWarnLimit<<'\n';
  cout<<"Low Alarm Limit:    "<<lowAlarmLimit<<'\n';
  cout<<"High Alarm Limit:   "<<highAlarmLimit<<'\n';
  cout<<"Units:              "<<units<<'\n';
  cout<<"Precision:          "<<precision<<'\n'<<'\n';
}

void HOraSlowChanMeta::write(fstream& fout) {
  // Streams the data
  fout<<"Start Time:         "<<startTime<<'\n';
  fout<<"Low Graph Limit:    "<<lowGraphLimit<<'\n';
  fout<<"High Graph Limit:   "<<highGraphLimit<<'\n';
  fout<<"Low Warning Limit:  "<<lowWarnLimit<<'\n';
  fout<<"High Warning Limit: "<<highWarnLimit<<'\n';
  fout<<"Low Alarm Limit:    "<<lowAlarmLimit<<'\n';
  fout<<"High Alarm Limit:   "<<highAlarmLimit<<'\n';
  fout<<"Units:              "<<units<<'\n';
  fout<<"Precision:          "<<precision<<'\n'<<'\n';
}
