//*-- AUTHOR : Ilse Koenig

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
// HFileInfo
//
// Class for HLD file and run information (filled by Oracle interface)
//
//////////////////////////////////////////////////////////////////////////////

#include "hfileinfo.h"

ClassImp(HFileInfo)

HFileInfo::HFileInfo(const Char_t* filename) : HRunInfo(filename) {
  magnetCurrent=-1;
}

void HFileInfo::print() {
  cout.setf(ios::left,ios::adjustfield);
  cout<<setw(25)<<GetName()<<setw(12)<<runId<<"\n    "<<startTime<<"  "
      <<setw(25)<<endTime;
  cout.setf(ios::right,ios::adjustfield);
  cout<<setw(12)<<numEvents<<setw(8)<<magnetCurrent<<'\n';
}

void HFileInfo::write(fstream& fout) {
  fout.setf(ios::left,ios::adjustfield);
  fout<<setw(25)<<GetName()<<setw(12)<<runId<<"  "<<startTime<<"  "
      <<setw(25)<<endTime;
  fout.setf(ios::right,ios::adjustfield);
  fout<<setw(12)<<numEvents<<setw(7)<<magnetCurrent<<'\n';
}
