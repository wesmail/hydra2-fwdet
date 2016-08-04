//*-- AUTHOR : Ilse Koenig
//*-- Created : 03/12/2004

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////
//
//  HScsRawChannel
//
//  Class for the raw data of a slowcontrol channel
//  The timestamp is stored in two UInt_t variables fDate and fTime
//  for direct comparison with the time in the event header
//
//
//  HScsChannel
//
//  Class for the summary data of a slowcontrol channel
//  Contains the raw data in a TObjArray
//
///////////////////////////////////////////////////////////////////

#include "hscschannel.h"

ClassImp(HScsChannelRaw)
ClassImp(HScsChannel)

HScsChannelRaw::HScsChannelRaw() {
  // Default constructor
  fDate=fTime=0;
  value=0.;
}

HScsChannelRaw::HScsChannelRaw(UInt_t d,UInt_t t,Double_t v) {
  // Constructor with date, time and value
  fDate=d;
  fTime=t;
  value=v;
}

void HScsChannelRaw::getTimestamp(TString& s) {
  // Converts date and time to a readable string
  Char_t buf[25];
  sprintf(buf,"%02i-%02i-%4i %02i:%02i:%02i",
         (fDate & 0xFF),1+((fDate>>8) & 0xFF),1900+((fDate>>16) & 0xFF),
         ((fTime>>16) & 0xFF),((fTime>>8) & 0xFF),(fTime & 0xFF));
  s=buf;
}

void HScsChannelRaw::print(Int_t valPrec) {
  // Prints the data to stdout
  TString s;
  getTimestamp(s);
  cout.setf(ios::fixed,ios::floatfield);
  cout<<s<<setw(10)<<fDate<<setw(10)<<fTime<<"    "
      <<cout.precision(valPrec)<<setw(valPrec+10)<<value<<'\n';
}

void HScsChannelRaw::write(fstream& fout,Int_t valPrec) {
  // Writes the data to fstream
  TString s;
  getTimestamp(s);
  fout.setf(ios::fixed,ios::floatfield);
  fout<<s<<setw(10)<<fDate<<setw(10)<<fTime<<"    "
      <<fout.precision(valPrec)<<setw(valPrec+10)<<value<<'\n';
}


HScsChannel::HScsChannel(const Char_t* name) {
  // Constructor with channel name
  SetName(name);
  valPrecision=status=0;
  mean=sigma=minVal=maxVal=previousVal=0.;
  pRawData=0;
}

HScsChannel::~HScsChannel() {
  // Destructor (deletes the array of raw data)
  if (pRawData) {
    pRawData->Delete();
    delete pRawData;
    pRawData=0;
  }
}

void HScsChannel::clear() {
  // Clears the data and deletes the array of raw data
  valPrecision=status=0;
  mean=sigma=minVal=maxVal=previousVal=0.;
  if (pRawData) {
    pRawData->Delete();
    delete pRawData;
    pRawData=0;
  }
}

void HScsChannel::setRawData(TObjArray* pArray) {
  // Sets the array of raw data
  // An existing array is deleted before.
  if (pRawData) {
    pRawData->Delete();
    delete pRawData;
  }
  pRawData=pArray;
}

Int_t HScsChannel::getNumRawData() {
  // Returns the number of raw data
  if (pRawData) return (pRawData->GetLast()+1);
  else return 0;
}

void HScsChannel::print() {
  // Prints the data to stdout
  cout<<"Channel:       "<<GetName()<<'\n';
  cout<<"Type:          "<<channelType<<'\n';  
  cout<<"Precison       "<<valPrecision<<'\n';
  cout<<"Mean:          "<<mean<<'\n';
  cout<<"Sigma:         "<<sigma<<'\n';
  cout<<"MinValue:      "<<minVal<<'\n';
  cout<<"MaxValue:      "<<maxVal<<'\n';
  cout<<"PreviousValue: "<<previousVal<<'\n';
  cout<<"Status:        "<<status<<'\n';
  cout<<"NumRawData:    "<<getNumRawData()<<'\n';
  if (pRawData) {
    for (Int_t i=0;i<getNumRawData();i++) {
      ((HScsChannelRaw*)(pRawData->At(i)))->print(valPrecision);
    }
  }
  cout<<"//------------------------------------------------------------"<<'\n';
}
 
void HScsChannel::write(fstream& fout) {
  // Writes the data to fstream
  fout<<"Channel:       "<<GetName()<<'\n';
  fout<<"Type:          "<<channelType<<'\n';  
  fout<<"Precison       "<<valPrecision<<'\n';
  fout<<"Mean:          "<<mean<<'\n';
  fout<<"Sigma:         "<<sigma<<'\n';
  fout<<"MinValue:      "<<minVal<<'\n';
  fout<<"MaxValue:      "<<maxVal<<'\n';
  fout<<"PreviousValue: "<<previousVal<<'\n';
  fout<<"Status:        "<<status<<'\n';
  fout<<"NumRawData:    "<<getNumRawData()<<'\n';
  if (pRawData) {
    for (Int_t i=0;i<getNumRawData();i++) {
      ((HScsChannelRaw*)(pRawData->At(i)))->write(fout,valPrecision);
    }
  }
  fout<<"//------------------------------------------------------------"<<'\n';
}
