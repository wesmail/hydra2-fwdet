//*-- AUTHOR Ilse Koenig
//*-- created : 25/06/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
//
// HTrb2Correction
//
///////////////////////////////////////////////////////////////////////

using namespace std;
#include "htrb2correction.h"
#include "TBuffer.h"
#include <iostream>
#include <iomanip>
#include <ctype.h>
#include <stdlib.h>

ClassImp(HTrb2Correction)

HTrb2Correction::HTrb2Correction(const Char_t* temperatureSensor)
               : nValuesPerChannel(256) {
  // Default constructor with the temperatureSensor identifying the board
  // Sets the name of the board, but does not create the array for the corrections
  SetName(temperatureSensor);
  nChannels=0;
  highResolutionFlag=-1;
  subeventId=-1;
}

Float_t* HTrb2Correction::makeArray() {
  // Creates the array for the corrections if not existing
  if (boardType.CompareTo("TRB")==0) {
    if (nChannels==0) {
      nChannels=128;
      corrData.Set(nChannels*nValuesPerChannel);
      corrData.Reset();
    }
    return corrData.GetArray();
  } else {
    Error("makeArray","Board %s has not type TRB",GetName());
    return 0;
  }
}

void HTrb2Correction::deleteArray() {
  // Deletes the array for the corrections
  nChannels=0;
  corrData.Set(0);
}

Float_t HTrb2Correction::getCorrection(Int_t c, Int_t i) {
  // Returns the correction value for channel c and bin i
  if (nChannels>0 && c>=0 && c<nChannels && i>=0 && i<nValuesPerChannel) {
    return (corrData.GetArray())[c*nValuesPerChannel+i];
  } else {
    Error("getCorrection","No corrections or invalid address");
    return 0;
  }
}

void HTrb2Correction::setCorrection(Int_t c, Int_t i, Float_t v) {
  // Sets the correction value v for channel c and bin i
  if (nChannels>0 && c>=0 && c<nChannels && i>=0 && i<nValuesPerChannel) {
    corrData[c*nValuesPerChannel+i]=v;
  } else Error("getCorrection","No corrections or invalid address");
}

void HTrb2Correction::clearArray() {
  // Sets all values in the array to 0;
  corrData.Reset();
}

Bool_t HTrb2Correction::fillArray(Float_t* data,Int_t nd) {
  // Creates the array for the corrections, if not existing
  // and copies the data
  makeArray();
  Bool_t rc=kFALSE;
  if (data!=0 && nd==getSize()) {
    memcpy(corrData.GetArray(),data,nd*sizeof(Float_t));
    rc=kTRUE;
  } else {
    Error("fill","Invalid number of data: %i",getSize());
  }
  return rc;
}

void HTrb2Correction::print() {
  Int_t l=0;
  cout<<"temperatureSensor: "<<GetName()<<'\n';
  for(Int_t i=0;i<nChannels;i++) {
    l=0;
    cout<<"  channel: "<<i<<'\n';
    for(Int_t k=0;k<nValuesPerChannel;k++) {
      l++;
      cout<<setw(13)<<right<<fixed<<setprecision(6)<<corrData[i*nValuesPerChannel+k];
      if (l==10) {
        cout<<'\n';
        l=0;
      }
    }
    cout<<endl;
  }
}

void HTrb2Correction::write(fstream& fout) {
  if (nChannels>0) {
    Int_t l=0;
    fout<<"temperatureSensor: "<<GetName()<<'\n';
    for(Int_t i=0;i<nChannels;i++) {
      l=0;
      fout<<"  channel: "<<i<<'\n';
      for(Int_t k=0;k<nValuesPerChannel;k++) {
        l++;
        fout<<setw(13)<<right<<fixed<<setprecision(6)<<corrData[i*nValuesPerChannel+k];
        if (l==10) {
          fout<<'\n';
          l=0;
        }
      }
      fout<<endl;
    }
    fout<<"//----------------------------------------------------------------------------"
        <<endl;
  }
}

Float_t HTrb2Correction::compare(HTrb2Correction& b) {
  // compares two data sets (used by Oracle interface)
  Float_t diff=0.F;
  if (b.getHighResolutionFlag()!=highResolutionFlag) {
    diff=999999.F;
  } else {
    Float_t* arr=b.getCorrections();
    for (Int_t i=0;i<getSize();i++) {
      Float_t d=TMath::Abs(corrData[i]-arr[i]);
      if (d>diff) diff=d;
    }
  }
  return diff;
}
