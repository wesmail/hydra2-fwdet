#ifndef HTRB2CORRECTION_H
#define HTRB2CORRECTION_H

#include "TNamed.h"
#include "TArrayF.h"

using namespace std;
#include <fstream>

class HTrb2Correction: public TNamed {
protected:
  const Int_t nValuesPerChannel; // number of values per channel
  TString boardType;             // type of board
  Int_t subeventId;              // related subevent id
  Int_t nChannels;               // number of channels with corrections
  Int_t highResolutionFlag;      // 0 = low resolution, 1 = high resolution 
  TArrayF corrData;              // correction data for the tdc channels
public:
  HTrb2Correction(const Char_t* temperatureSensor="");
  ~HTrb2Correction() {;}
  Int_t getNChannels() { return nChannels; }
  Int_t getHighResolutionFlag() { return highResolutionFlag; }
  Bool_t isHighResolution() {return (0==highResolutionFlag) ? kFALSE : kTRUE;}
  const Char_t* getBoardType() { return boardType.Data(); }
  Int_t getSubeventId() { return subeventId; }
  Int_t getNValuesPerChannel() { return nValuesPerChannel; }
  Int_t getSize() { return corrData.GetSize(); }
  Float_t getCorrection(Int_t, Int_t);
  void setBoardType(const Char_t* t) { boardType=t; }
  void setHighResolutionFlag(Int_t f) {highResolutionFlag=f; }
  void setSubeventId(Int_t i) {subeventId=i; }
  Float_t* makeArray();
  void deleteArray();
  void setCorrection(Int_t c, Int_t i, Float_t v);
  Bool_t fillArray(Float_t*,Int_t);
  void clearArray();
  void print();
  void write(fstream&);
  Float_t compare(HTrb2Correction&);
  Float_t* getCorrections() {
    return corrData.GetArray();
  }
  ClassDef(HTrb2Correction,1) // Correction table for the tdc channels of one TRB board
};

#endif  /*!HTRB2CORRECTION_H*/
