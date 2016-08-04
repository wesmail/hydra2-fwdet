//*-- AUTHOR   : Ilse Koenig
//*-- Created  : 13/08/2004 by Ilse Koenig
//*-- Modified : 13/04/2005 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HOraSlowChanRunSum
//
// Class for the run based summary data of a slowcontrol channel
//
//////////////////////////////////////////////////////////////////////////////

#include "horaslowchanrunsum.h"
#include "horaslowperiod.h"

ClassImp(HOraSlowChanRunSum)

HOraSlowChanRunSum::HOraSlowChanRunSum() {
  // Default constructor
  period=0;
  periodIndex=-1;
  nData=0;
  nMonData=-1;
  status=0;
  mean=sigma=minVal=maxVal=0.;
}

void HOraSlowChanRunSum::fill(Int_t pInd,Double_t mea,Double_t sig,
                 Double_t min,Double_t max,Int_t nD,Int_t st,Int_t nMon) {
  // Fills the data elements
  periodIndex=pInd;
  mean=mea;
  sigma=sig;
  minVal=min;
  maxVal=max;
  nData=nD;
  status=st;
  nMonData=nMon;
}

void HOraSlowChanRunSum::print(Int_t valPrec) {
  // Prints the data to stdout
  // Takes as arguments the precision of the data
  Int_t n=valPrec+10;
  cout.setf(ios::fixed,ios::floatfield);
  cout.setf(ios::showpoint);
  cout<<setprecision(valPrec)<<setw(n)<<mean
      <<setw(n)<<sigma<<setw(n)<<minVal<<setw(n)<<maxVal<<setw(8)<<nData
      <<setw(3)<<status<<setw(5)<<getMonRate()<<setprecision(6);
  if (period!=0&&period->getRunId()>0) {
    cout<<setw(12)<<period->getRunId()<<" "<<period->getFilename();
  }
  cout<<'\n';
}

void HOraSlowChanRunSum::write(fstream& fout,Int_t valPrec) {
  // Streams the data
  // Takes as arguments the precision of the data
  Int_t n=valPrec+10;
  fout.setf(ios::fixed,ios::floatfield);
  fout.setf(ios::showpoint);
  fout<<setprecision(valPrec)<<setw(n)<<mean
      <<setw(n)<<sigma<<setw(n)<<minVal<<setw(n)<<maxVal<<setw(8)<<nData
      <<setw(3)<<status<<setw(5)<<getMonRate()<<setprecision(6);
  if (period!=0&&period->getRunId()>0) {
    fout<<setw(12)<<period->getRunId()<<"  "<<period->getFilename();
  }
  fout<<'\n';
}


Int_t HOraSlowChanRunSum::getMonRate() {
  // Returns the rate (number of entries/minute) of the corresponding
  // monitor channel.
  // The rate should be about 60 (one entry/s).
  // A much lower rate indicated, that the archiver was not running for
  // the whole period.
  Int_t rate=nMonData;
  if (period!=0&&nMonData!=-1) {
    Int_t dt=period->getDuration();
    if (dt>0) {
      rate=(Int_t)(60.*nMonData/dt+0.5);
    }
  }
  return rate;
}

