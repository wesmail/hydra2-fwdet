#ifndef HSCSCHANNEL_H
#define HSCSCHANNEL_H

using namespace std;
#include "TNamed.h"
#include "TString.h"
#include "TObjArray.h"
#include <iostream>
#include <iomanip>
#include <fstream>

class HScsChannelRaw : public TObject {
private:
  UInt_t  fDate;    // UTC Date of entry
  UInt_t  fTime;    // UTC Time of entry
  Double_t value;   // Channel value
public:
  HScsChannelRaw();
  HScsChannelRaw(UInt_t,UInt_t,Double_t);
  ~HScsChannelRaw() {}
  void setDate(UInt_t d) {fDate=d;}
  void setTime(UInt_t t) {fTime=t;}
  void setValue(Double_t v) {value=v;}
  UInt_t getDate() {return fDate;}
  UInt_t getTime() {return fTime;}
  void getTimestamp(TString&);
  Double_t getValue() {return value;}
  void print(Int_t);
  void write(fstream&,Int_t);
private:
  ClassDef(HScsChannelRaw,1) // Class for the raw data of a slowcontrol channel  
};

class HScsChannel : public TNamed {
private:
  TString    channelType;  // Type of the channel
  Int_t      valPrecision; // Precision of the values
  Double_t   mean;         // Mean value (weighted mean over run)
  Double_t   sigma;        // Sigma of mean value
  Double_t   minVal;       // Minimum value
  Double_t   maxVal;       // Maximum value
  Double_t   previousVal;  // Last value before actual run start
  Int_t      status;       // Status flag
  TObjArray* pRawData;     // Array of raw data
public:
  HScsChannel(const Char_t* name="");
  ~HScsChannel();
  void setChannelType(const Char_t* s) {channelType=s;}
  void setValuePrecision(Int_t n) {valPrecision=n;}
  void setMean(Double_t v) {mean=v;}
  void setSigma(Double_t v) {sigma=v;}
  void setMinValue(Double_t v) {minVal=v;}
  void setMaxValue(Double_t v) {maxVal=v;}
  void setPreviousVal(Double_t v) {previousVal=v;}
  void setStatus(Int_t n) {status=n;}
  void setRawData(TObjArray*);
  const Char_t* getChannelType() {return channelType.Data();}
  Int_t getValuePrecision() {return valPrecision;}
  Double_t getMean() {return mean;}
  Double_t getSigma() {return sigma;}
  Double_t getMinVal() {return minVal;}
  Double_t getMaxVal() {return maxVal;}
  Double_t getPreviousVal() {return previousVal;} 
  Int_t getStatus() {return status;}
  Int_t getNumRawData();
  TObjArray* getRawData() {return pRawData;}
  void clear();
  void print();
  void write(fstream&);
private:
  ClassDef(HScsChannel,1) // Class for the summary data of a slowcontrol channel
};

#endif  /* !HSCSCHANNEL */
