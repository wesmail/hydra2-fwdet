#ifndef HTOFLOOKUP_H
#define HTOFLOOKUP_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"

class HTofLookupChan : public TObject {
protected:
    Int_t sector;   // sector number (0..5)
    Int_t module;   // module number (0..21)
    Int_t cell;     // rod number (0..8)
    Char_t side;    // side of rod (L=left, R=right)
public:
    HTofLookupChan() { clear(); }
    ~HTofLookupChan() {;}
    Int_t getSector() { return sector; }
    Int_t getModule() { return module; }
    Int_t getCell()  { return cell; }
    Char_t getSide()  { return side; }
    void getAddress(Int_t& s,Int_t& m,Int_t& c,Char_t& p) {
      s=sector;
      m=module;
      c=cell;
      p=side;
    }
    void fill(const Int_t s,const Int_t m,const Int_t c,const Char_t p) {
      sector=s;
      module=m;
      cell=c;
      side=p;
    }
    void fill(HTofLookupChan& r) {
      sector=r.getSector();
      module=r.getModule();
      cell=r.getCell();
      side=r.getSide();
    }
    void setSector(const Int_t n) { sector=n; }
    void setModule(const Int_t n) { module=n; }
    void setCell(const Int_t n)  { cell=n; }
    void setSide(const Int_t n)  { side=n; }
    void clear() {
      sector=-1;
      module=-1;
      cell=-1;
      side='U';
    }
    ClassDef(HTofLookupChan,1) // Channel level of the lookup table for the TOF unpacker 
};

class HTofLookupSlot: public TObject {
protected:
  TObjArray* array; // pointer array containing HTofLookupChan objects
  Char_t modType;   // type of digital converter (A=ADC, T=TDC, U=undefined)
  Int_t maxChannel; // actual maximum index of channel
  Int_t nChannels;  // maximum number of channels in a slot
public:
  HTofLookupSlot(Int_t numChannels=1);
  ~HTofLookupSlot();
  HTofLookupChan& operator[](Int_t i) {
      return *static_cast<HTofLookupChan*>((*array)[i]);
  }
  HTofLookupChan* getChannel(Int_t c) {
    if (c>=0 && c<=maxChannel) return &((*this)[c]);
    else return 0;
  }
  Int_t getSize()  { return maxChannel+1; }
  Int_t getMaxSize() { return nChannels; }
  Char_t getType() { return modType; }
  void fill(Char_t,Int_t,Int_t,Int_t,Int_t,Char_t);
  void clear();
  ClassDef(HTofLookupSlot,1) // ADC/TDC level of  the lookup table for the TOF unpacker
};

class HTofLookupCrate: public TObject {
protected:
  TObjArray* array; // pointer array containing HTofLookupSlot objects
  Int_t maxSlot;    // actual maximum index of slots
  Int_t nSlots;     // maximum number of slots in a crate
public:
  HTofLookupCrate(Int_t numSlots=1);
  ~HTofLookupCrate();
  HTofLookupSlot& operator[](Int_t i) {
      return *static_cast<HTofLookupSlot*>((*array)[i]);
  }
  HTofLookupSlot* getSlot(Int_t s) {
    if (s>=0 && s<=maxSlot) return &((*this)[s]);
    else return 0;
  }
  Int_t getSize() { return maxSlot+1; }
  Int_t getMaxSize() { return nSlots; }
  void fill(Int_t,Char_t,Int_t,Int_t,Int_t,Int_t,Char_t);
  ClassDef(HTofLookupCrate,1) // Crate level of the lookup table for the TOF unpacker
};

class HTofLookup : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HTofLookupCrate
  Int_t maxCrate;    // actual maximum index of crates
public:
  HTofLookup(const Char_t* name="TofLookup",
             const Char_t* title="Lookup table for Tof unpacker",
             const Char_t* context="",
             Int_t nCrates=1, Int_t nSlots=1);
  ~HTofLookup();
  HTofLookupCrate& operator[](Int_t i) {
    return *static_cast<HTofLookupCrate*>((*array)[i]);
  }
  HTofLookupSlot* getSlot(Int_t c, Int_t s) {
    if (c>=0 && c<=maxCrate) return (*this)[c].getSlot(s);
    else return 0;
  }
  Int_t getSize() { return maxCrate+1; }
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear();
  void printParam();
  void fill(Int_t,Int_t,Char_t,Int_t,Int_t,Int_t,Int_t,Char_t);
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t, Int_t, Int_t);
  ClassDef(HTofLookup,1) // Lookup table for the TOF unpacker
};

#endif  /*!HTOFLOOKUP_H*/









