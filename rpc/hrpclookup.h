#ifndef HRPCLOOKUP_H
#define HRPCLOOKUP_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"

 // FIXME: Codigo solo para Nov05 test

class HRpcLookupChan : public TObject {
protected:
    Int_t cell;     // cell number (0..23)  <-->  Nov05 test
    Char_t side;    // side of cell (L=left, R=right)
public:
    HRpcLookupChan() { clear(); }
    ~HRpcLookupChan() {;}
    Int_t getCell()  { return cell; }
    Char_t getSide()  { return side; }
    void getAddress(Int_t& c,Char_t& p) {
      c=cell;
      p=side;
    }
    void fill(const Int_t c,const Char_t p) {
      cell=c;
      side=p;
    }
    void fill(HRpcLookupChan& r) {
      cell=r.getCell();
      side=r.getSide();
    }
    void setCell(const Int_t n)  { cell=n; }
    void setSide(const Int_t n)  { side=n; }
    void clear() {
      cell=-1;
      side='U';
    }
    ClassDef(HRpcLookupChan,1) // Channel level of the lookup table for the RPC unpacker 
};

/* -----------------------------------------------------------------------------------------

class HRpcLookupSlot: public TObject {
protected:
  TObjArray* array; // pointer array containing HRpcLookupChan objects
  Char_t modType;   // type of digital converter (A=ADC, T=TDC, U=undefined)
  Int_t maxChannel; // actual maximum index of channel
  Int_t nChannels;  // maximum number of channels in a slot
public:
  HRpcLookupSlot(Int_t numChannels=1);
  ~HRpcLookupSlot();
  HRpcLookupChan& operator[](Int_t i) {
      return *static_cast<HRpcLookupChan*>((*array)[i]);
  }
  HRpcLookupChan* getChannel(Int_t c) {
    if (c>=0 && c<=maxChannel) return &((*this)[c]);
    else return 0;
  }
  Int_t getSize()  { return maxChannel+1; }
  Int_t getMaxSize() { return nChannels; }
  Char_t getType() { return modType; }
  void fill(Char_t,Int_t,Int_t,Char_t);   // dcType, channel, cell, side
  void clear();
  ClassDef(HRpcLookupSlot,1) // ADC/TDC level of  the lookup table for the RPC unpacker
};

class HRpcLookupCrate: public TObject {
protected:
  TObjArray* array; // pointer array containing HRpcLookupSlot objects
  Int_t maxSlot;    // actual maximum index of slots
  Int_t nSlots;     // maximum number of slots in a crate
public:
  HRpcLookupCrate(Int_t numSlots=1);
  ~HRpcLookupCrate();
  HRpcLookupSlot& operator[](Int_t i) {
      return *static_cast<HRpcLookupSlot*>((*array)[i]);
  }
  HRpcLookupSlot* getSlot(Int_t s) {
    if (s>=0 && s<=maxSlot) return &((*this)[s]);
    else return 0;
  }
  Int_t getSize() { return maxSlot+1; }
  Int_t getMaxSize() { return nSlots; }
  void fill(Int_t,Char_t,Int_t,Int_t,Char_t);  // slot, cdType, channel, cell, side
  ClassDef(HRpcLookupCrate,1) // Crate level of the lookup table for the RPC unpacker
};

---------------------------------------------------------------------------------------- */

class HRpcLookup : public HParSet {
protected:
  TObjArray* array;   // array of pointers of type HRpcLookupChan
  Int_t maxChannel;   // actual maximum index of channels
  Int_t nChannels;    // maximum number of channels in a board
public:
  HRpcLookup(const Char_t* name="RpcLookup",
             const Char_t* title="Lookup table for Rpc unpacker",
             const Char_t* context="Nov05 Test run",
             Int_t numChannels=1);
  ~HRpcLookup();
  HRpcLookupChan& operator[](Int_t i) {
    return *static_cast<HRpcLookupChan*>((*array)[i]);
  }
  HRpcLookupChan* getChannel(Int_t c) {
    //if (c>=0 && c<=maxChannel) return static_cast<HLookupChan*>array->Get(c);  //return &((*this)[c]);
    if (c>=0 && c<=maxChannel) return &((*this)[c]);
    else return 0;
  }
  Int_t getSize() { return maxChannel+1; }
  Bool_t init(HParIo*,Int_t*);
  Int_t write(HParIo*);
  void clear();
  void printParam();
  void fill(Int_t,Int_t,Char_t);  // channel, cell, side
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t*, Int_t);   // buffer, channel
  ClassDef(HRpcLookup,1) // Lookup table for the RPC unpacker
};

#endif  /*!HRPCLOOKUP_H*/


