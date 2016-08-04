#ifndef HTOFTRB2LOOKUP_H
#define HTOFTRB2LOOKUP_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include "htrbnetdef.h"
#include <fstream>

using namespace Trbnet;
using namespace std;

class HTofTrb2LookupChan : public TObject {
protected:
  Int_t  sector;    // sector number
  Int_t  module;    // module number
  Int_t  cell;      // cell number
  Char_t side;      // side of cell
public:
  HTofTrb2LookupChan()     { clear(); }
  ~HTofTrb2LookupChan()    {;}
  Int_t getSector()    { return sector; }
  Int_t getModule()    { return module; }
  Int_t getCell()      { return cell;  }
  Char_t getSide()     { return side;  }
  void getAddress(Int_t& s,Int_t& m,Int_t& c,Char_t& t) {
    s=sector;
    m=module;
    c=cell;
    t=side;
  }
  void fill(Int_t s,Int_t m,Int_t c,Char_t t) {
    sector=s;
    module=m;
    cell=c;
    side=t;
  }
  void fill(HTofTrb2LookupChan& r) {
    sector=r.getSector();
    module=r.getModule();
    cell=r.getCell();
    side=r.getSide();
  }
  void setSector(const Int_t n)    { sector=n; }
  void setModule(const Int_t n)    { module=n; }
  void setCell(const Int_t n)      { cell=n; }
  void setSide(Char_t c)           { side=c; }
  void clear() {
    sector=-1;
    module=-1;
    cell=-1;
    side='\0';
  }
  ClassDef(HTofTrb2LookupChan,1) // Channel level of the lookup table for the TOF TRB2 unpacker
};


class HTofTrb2LookupBoard: public TObject {
  friend class HTofTrb2Lookup;
protected:
  TObjArray* array;     // pointer array containing HTofTrb2LookupChan objects
public:
  HTofTrb2LookupBoard();
  ~HTofTrb2LookupBoard();
  HTofTrb2LookupChan* getChannel(Int_t c) {
    if (c>=0 && c<128) return &((*this)[c]);
    else return 0;
  }
  HTofTrb2LookupChan& operator[](Int_t i) {
      return *static_cast<HTofTrb2LookupChan*>((*array)[i]);
  }
  Int_t getSize()  { return 128; }
  void clear();
  ClassDef(HTofTrb2LookupBoard,1) // Board level of  the lookup table for the TOF TRB2 unpacker
};


class HTofTrb2Lookup : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HTofTrb2LookupBoard
  Int_t arrayOffset; // offset to calculate the index
public:
  HTofTrb2Lookup(const Char_t* name="TofTrb2Lookup",
             const Char_t* title="Lookup table for the TRB2 unpacker of the TOF detector",
             const Char_t* context="TofTrb2LookupProduction",
             Int_t minTrbnetAddress=Trbnet::kTofMinTrbnetAddress,
             Int_t maxTrbnetAddress=Trbnet::kTofMaxTrbnetAddress);
  ~HTofTrb2Lookup();
  HTofTrb2LookupBoard* getBoard(Int_t trbnetAddress) {
    return (HTofTrb2LookupBoard*)(array->At(trbnetAddress-arrayOffset));
  }
  HTofTrb2LookupBoard* operator[](Int_t i) {
    return static_cast<HTofTrb2LookupBoard*>((*array)[i]);
  }
  Int_t getSize() { return array->GetLast()+1; }
  Int_t getArrayOffset() { return arrayOffset; }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParam();
  Bool_t fill(Int_t,Int_t,Int_t,Int_t,Int_t,Char_t);
  Bool_t readline(const Char_t*);
  void putAsciiHeader(TString&);
  void write(fstream&);
  ClassDef(HTofTrb2Lookup,1) // Lookup table for the TRB2 unpacker of the TOF detector
};

#endif  /*!HTOFTRB2LOOKUP_H*/
