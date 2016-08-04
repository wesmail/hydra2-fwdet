#ifndef HMDCLOOKUPRAW_H
#define HMDCLOOKUPRAW_H

#include "TNamed.h"
#include "TObjArray.h"
#include "hparset.h"

class HParIo;

class HMdcLookupCell: public TObject {
protected:
  Int_t nMoth;        // number of the Motherboard
  Int_t nChan;        // number of the TDC channel
  Char_t readoutSide; // side, where thes signal is read out
public:
  HMdcLookupCell(Int_t m=-1,Int_t c=-1, Char_t s='0') {
    nMoth=m;
    nChan=c;
    readoutSide=s;
  }
  ~HMdcLookupCell() {}
  void setNMoth(Int_t m) {nMoth=m;}
  void setNChan(Int_t c) {nChan=c;}
  void setReadoutSide(Char_t s) {
    readoutSide=(s=='L'||s=='R'||s=='U') ? s : '0';
  }
  void fill(Int_t m,Int_t c,Char_t s) {
    nMoth=m;
    nChan=c;
    setReadoutSide(s);
  }
  Int_t getNMoth() {return nMoth;}
  Int_t getNChan() {return nChan;}
  Char_t getReadoutSide() {return readoutSide;}
  void clear() {
    nMoth=-1;
    nChan=-1;
    readoutSide='0';
  }
  ClassDef(HMdcLookupCell,2) // Cell level of MDC lookup table (cell->channel)
};


class HMdcLookupLayer: public TObject {
protected:
  TObjArray* array;
public:
  HMdcLookupLayer(Int_t n=0);
  ~HMdcLookupLayer();
  HMdcLookupCell& operator[](Int_t i) {
    return *static_cast<HMdcLookupCell*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcLookupLayer,1) // Layer level of MDC lookup table (cell->channel)
};


class HMdcLookupRMod: public TObject {
protected:
  TObjArray* array;
public:
  HMdcLookupRMod(Int_t n=6);
  ~HMdcLookupRMod();
  HMdcLookupLayer& operator[](Int_t i) {
    return *static_cast<HMdcLookupLayer*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  void createLayer(Int_t, Int_t);
  ClassDef(HMdcLookupRMod,1) // Module level of MDC lookup table (cell->channel)
};


class HMdcLookupRSec: public TObject {
protected:
  TObjArray* array;
  Int_t sector;
public:
  HMdcLookupRSec(Int_t n=4);
  ~HMdcLookupRSec();
  HMdcLookupRMod& operator[](Int_t i) {
    return *static_cast<HMdcLookupRMod*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcLookupRSec,1) // Sector level of MDC lookup table (cell->channel)
};


class HMdcLookupRaw : public HParSet {
protected:
  TObjArray* array;     // array of pointers
public:
  HMdcLookupRaw(const Char_t* name="MdcLookupRaw",
                const Char_t* title="Mdc lookup table from cal1 to raw",
                const Char_t* context="",
                Int_t n=6);
  ~HMdcLookupRaw();
  HMdcLookupRSec& operator[](Int_t i) {
    return *static_cast<HMdcLookupRSec*>((*array)[i]);
  }
  Bool_t init(HParIo* input);
  Int_t  write(HParIo* output);
  Bool_t writeline(Char_t *, Int_t, Int_t, Int_t, Int_t);
  void putAsciiHeader(TString&);
  Int_t  getSize() {return array->GetEntries();}
  void clear();
  ClassDef(HMdcLookupRaw,1) //  MDC lookup table (cell->channel)
};

#endif /* !HMDCLOOKUPRAW_H */

