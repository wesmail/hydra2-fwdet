#ifndef HTRBLOOKUP_H
#define HTRBLOOKUP_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"

using namespace std;
#include <fstream>

class HTrbLookupChan : public TObject {
protected:
  Char_t detector;  // identifier for detector
  Int_t  sector;    // sector number
  Int_t  module;    // module number
  Int_t  cell;      // cell number
  Char_t side;      // side of cell
  Int_t  feAddress; // front end address
public:
  HTrbLookupChan()     { clear(); }
  ~HTrbLookupChan()    {;}
  Char_t getDetector() { return detector; }
  Int_t getSector()    { return sector; }
  Int_t getModule()    { return module; }
  Int_t getCell()      { return cell;  }
  Char_t getSide()     { return side;  }
  Int_t getFeAddress() { return feAddress;  }
  void getAddress(Char_t& d,Int_t& s,Int_t& m,Int_t& c,Char_t& t,Int_t& f) {
    d=detector;
    s=sector;
    m=module;
    c=cell;
    t=side;
    f=feAddress;
  }
  void fill(Char_t d,Int_t s,Int_t m,Int_t c,Char_t t,Int_t f) {
    detector=d;
    sector=s;
    module=m;
    cell=c;
    side=t;
    feAddress=f;
  }
  void fill(HTrbLookupChan& r) {
    detector=r.getDetector();
    sector=r.getSector();
    module=r.getModule();
    cell=r.getCell();
    side=r.getSide();
    feAddress=r.getFeAddress();
  }
  void setDetector(Char_t c)       { detector=c; }
  void setSector(const Int_t n)    { sector=n; }
  void setModule(const Int_t n)    { module=n; }
  void setCell(const Int_t n)      { cell=n; }
  void setSide(Char_t c)           { side=c; }
  void setFeAddress(const Int_t n) { feAddress=n; }
  void clear() {
    detector='\0';
    sector=-1;
    module=-1;
    cell=-1;
    side='\0';
    feAddress=-1;
  }
  ClassDef(HTrbLookupChan,2) // Channel level of the lookup table for the TRB unpacker
};


class HTrbLookupBoard: public TObject {
  friend class HTrbLookup;
protected:
  TObjArray* array;     // pointer array containing HTrbLookupChan objects
public:
  HTrbLookupBoard();
  ~HTrbLookupBoard();
  HTrbLookupChan* getChannel(Int_t c) {
    if (c>=0 && c<128) return &((*this)[c]);
    else return 0;
  }
  HTrbLookupChan& operator[](Int_t i) {
      return *static_cast<HTrbLookupChan*>((*array)[i]);
  }
  Int_t getSize()  { return 128; }
  void clear();
  ClassDef(HTrbLookupBoard,1) // Board level of  the lookup table for the TRB unpacker
};


class HTrbLookup : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HTrbLookupBoard
  Int_t arrayOffset; // offset to calculate the index
public:
  HTrbLookup(const Char_t* name="TrbLookup",
             const Char_t* title="Lookup table for TRB unpacker",
             const Char_t* context="TrbLookupProduction",
             Int_t minSubeventId=800, Int_t nBoards=100);
  ~HTrbLookup();
  HTrbLookupBoard* getBoard(Int_t subeventId) {
    return (HTrbLookupBoard*)(array->At(subeventId-arrayOffset));
  }
  HTrbLookupBoard* operator[](Int_t i) {
    return static_cast<HTrbLookupBoard*>((*array)[i]);
  }
  Int_t getSize() { return array->GetLast()+1; }
  Int_t getArrayOffset() { return arrayOffset; }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParam();
  Bool_t fill(Int_t,Int_t,Char_t,Int_t,Int_t,Int_t,Char_t,Int_t);
  Bool_t readline(const Char_t*);
  void putAsciiHeader(TString&);
  void write(fstream&);
  ClassDef(HTrbLookup,1) // Lookup table for the TRB unpacker
};

#endif  /*!HTRBLOOKUP_H*/
