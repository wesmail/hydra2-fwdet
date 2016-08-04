#ifndef HWALLLOOKUP_H
#define HWALLLOOKUP_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"

class HWallLookupChan : public TObject {
protected:
  Int_t cell;     // cell number (0..383)
public:
  HWallLookupChan(void) { clear(); }
  ~HWallLookupChan(void) {;}
  Int_t getCell(void)  { return cell; }
  void fill(HWallLookupChan& r) { cell=r.getCell(); }
  void setCell(const Int_t n)  { cell=n; }
  void clear(void) { cell=-1; }
  ClassDef(HWallLookupChan,1) // Channel level of the lookup table for the Forward Wall unpacker 
};

class HWallLookupSlot: public TObject {
protected:
  TObjArray* array; // pointer array containing HWallLookupChan objects
  Char_t modType;   // type of digital converter (A=ADC, T=TDC, U=undefined)
  Int_t maxChannel; // actual maximum index of channel
  Int_t nChannels;  // maximum number of channels in a slot
public:
  HWallLookupSlot(Int_t numChannels=1);
  ~HWallLookupSlot(void);
  HWallLookupChan& operator[](Int_t i) {
      return *static_cast<HWallLookupChan*>((*array)[i]);
  }
  HWallLookupChan* getChannel(Int_t c) {
    if (c>=0 && c<=maxChannel) return &((*this)[c]);
    else return 0;
  }
  Int_t getSize(void)  { return maxChannel+1; }
  Int_t getMaxSize(void) { return nChannels; }
  Char_t getType(void) { return modType; }
  void fill(Char_t,Int_t,Int_t);
  void clear(void);
  ClassDef(HWallLookupSlot,1) // ADC/TDC level of  the lookup table for the Forward Wall unpacker
};

class HWallLookupCrate: public TObject {
protected:
  TObjArray* array; // pointer array containing HWallLookupSlot objects
  Int_t maxSlot;    // actual maximum index of slots
  Int_t nSlots;     // maximum number of slots in a crate
public:
  HWallLookupCrate(Int_t numSlots=1);
  ~HWallLookupCrate(void);
  HWallLookupSlot& operator[](Int_t i) {
      return *static_cast<HWallLookupSlot*>((*array)[i]);
  }
  HWallLookupSlot* getSlot(Int_t s) {
    if (s>=0 && s<=maxSlot) return &((*this)[s]);
    else return 0;
  }
  Int_t getSize(void) { return maxSlot+1; }
  Int_t getMaxSize(void) { return nSlots; }
  void fill(Int_t,Char_t,Int_t,Int_t);
  ClassDef(HWallLookupCrate,1) // Crate level of the lookup table for the Forward Wall unpacker
};

class HWallLookup : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HWallLookupCrate
  Int_t maxCrate;    // actual maximum index of crates
public:
  HWallLookup(const Char_t* name="WallLookup",
             const Char_t* title="Lookup table for Forward Wall unpacker",
             const Char_t* context="WallLookupProduction",
             Int_t nCrates=1, Int_t nSlots=1);
  ~HWallLookup(void);
  HWallLookupCrate& operator[](Int_t i) {
    return *static_cast<HWallLookupCrate*>((*array)[i]);
  }
  HWallLookupSlot* getSlot(Int_t c, Int_t s) {
    if (c>=0 && c<=maxCrate) return (*this)[c].getSlot(s);
    else return 0;
  }
  Int_t getSize(void) { return maxCrate+1; }
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear(void);
  void printParam(void);
  void fill(Int_t,Int_t,Char_t,Int_t,Int_t);
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t, Int_t, Int_t);
  ClassDef(HWallLookup,1) // Lookup table for the unpacker of the Forward Wall
};

#endif  /*!HWALLLOOKUP_H*/









