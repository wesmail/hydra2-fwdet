#ifndef HTRBNETADDRESSMAPPING_H
#define HTRBNETADDRESSMAPPING_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include "htrb2correction.h"
#include "htrbnetdef.h"
#include <fstream>

using namespace Trbnet;
using namespace std;

class HTrbnetAddressMapping : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HTrb2Correction
  Int_t arrayOffset; // offset to calculate the index
public:
  HTrbnetAddressMapping(const Char_t* name="TrbnetAddressMapping",
             const Char_t* title="Mapping of trbnet addresses to boards",
             const Char_t* context="Trb2Production",
             Int_t minTrbnetAddress=Trbnet::kTrb2MinTrbnetAddress,
	     Int_t maxTrbnetAddress=Trbnet::kTrb2MaxTrbnetAddress);
  ~HTrbnetAddressMapping();
  HTrb2Correction* getBoard(Int_t trbnetAddress) {
    return (HTrb2Correction*)(array->At(trbnetAddress-arrayOffset));
  }
  HTrb2Correction* getBoard(const Char_t* temperaturSensor) {
    return (HTrb2Correction*)(array->FindObject(temperaturSensor));
  }
  HTrb2Correction* operator[](Int_t i) {
    return static_cast<HTrb2Correction*>((*array)[i]);
  }
  Int_t getSize() { return array->GetSize(); }
  Int_t getArrayOffset() { return arrayOffset; }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParam();
  HTrb2Correction* addBoard(Int_t,const Char_t*,const Char_t*,Int_t,Int_t);
  Bool_t readline(const Char_t*);
  void putAsciiHeader(TString&);
  void write(fstream&);
  ClassDef(HTrbnetAddressMapping,1) // Mapping of trbnet addresses to boards
};

#endif  /*!HTRBNETADDRESSMAPPING_H*/
