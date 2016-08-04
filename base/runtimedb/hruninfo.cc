///*-- AUTHOR : Ilse Koenig
//*-- Created : 10/06/2004 by Ilse Koenig 

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HRunInfo
//
// Class for run information (filled by Oracle interface)
//
//////////////////////////////////////////////////////////////////////////////

#include "hruninfo.h"

ClassImp(HRunInfo)

HRunInfo::HRunInfo(const Char_t* filename) {
  SetName(filename);
  runId=numEvents=-1;
}

void HRunInfo::print() {
  cout.setf(ios::left,ios::adjustfield);
  cout<<setw(50)<<GetName()<<setw(12)<<runId<<"\n    "<<startTime<<"  "
      <<setw(25)<<endTime;
  cout.setf(ios::right,ios::adjustfield);
  cout<<setw(12)<<numEvents<<'\n';
}

void HRunInfo::write(fstream& fout) {
  fout.setf(ios::left,ios::adjustfield);
  fout<<setw(50)<<GetName()<<setw(12)<<runId<<"  "<<startTime<<"  "
      <<setw(25)<<endTime;
  fout.setf(ios::right,ios::adjustfield);
  fout<<setw(12)<<numEvents<<'\n';
}
