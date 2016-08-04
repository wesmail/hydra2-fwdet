#ifndef HMDCRAWSTRUCT_H
#define HMDCRAWSTRUCT_H

#include "TNamed.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "hparset.h"

class HParIo;

class HMdcRawMothStru: public TNamed {
protected:
  Int_t nTdcs;  // number of TDCs on the motherboard
public:
  HMdcRawMothStru() {clear();}
  ~HMdcRawMothStru() {}
  void setNTdcs(Int_t n) {nTdcs=n;}
  Int_t getNTdcs() {return nTdcs;}
  void clear() {
    nTdcs=0;
  }
  ClassDef(HMdcRawMothStru,1) // Tdc level of MdcRawStruct parameter container
};


class HMdcRawModStru: public TObject {
protected:
  TObjArray* array;
public:
  HMdcRawModStru(Int_t n=16);
  ~HMdcRawModStru();
  HMdcRawMothStru& operator[](Int_t i) {
    return *static_cast<HMdcRawMothStru*>((*array)[i]);
  }
  Int_t getNMotherboards() {return array->GetEntries();}
  Int_t getMboIndex(const Text_t*);
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcRawModStru,1) // Motherboard level of MdcRawStruct parameter container
};


class HMdcRawSecStru: public TObject {
protected:
  TObjArray* array;
public:
  HMdcRawSecStru(Int_t n=4);
  ~HMdcRawSecStru();
  HMdcRawModStru& operator[](Int_t i) {
    return *static_cast<HMdcRawModStru*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcRawSecStru,1) // Sector level of MdcRawStruct parameter container
};


class HMdcRawStruct : public HParSet {
protected:
  TObjArray* array;     // array of pointers
public:
  HMdcRawStruct(const Char_t* name="MdcRawStruct",
                const Char_t* title="Mdc parameters for hardware structure",
                const Char_t* context="",
                Int_t n=6);
  ~HMdcRawStruct();
  HMdcRawSecStru& operator[](Int_t i) {
    return *static_cast<HMdcRawSecStru*>((*array)[i]);
  }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void readline(const Char_t*, Int_t*);
  Bool_t writeline(Char_t *, Int_t, Int_t, Int_t);
  void putAsciiHeader(TString&);
  Int_t getSize() {return array->GetEntries();}
  void clear();
  void printParam();
  void getMaxIndices(TArrayI*);
  ClassDef(HMdcRawStruct,1) // MdcRawStruct parameter container for number of TDCs
};

#endif /* !HMDCRAWSTRUCT_H */







