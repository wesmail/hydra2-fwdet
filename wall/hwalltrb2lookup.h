#ifndef HWALLTRB2LOOKUP_H
#define HWALLTRB2LOOKUP_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include "htrbnetdef.h"
#include <fstream>

using namespace Trbnet;
using namespace std;

class HWallTrb2LookupChan : public TObject {
protected:
  Int_t  cell;      // cell number
public:
  HWallTrb2LookupChan()  { clear(); }
  ~HWallTrb2LookupChan() {;}
  Int_t getCell()        { return cell;  }
  void  setCell(Int_t c) { cell=c; }
  void fill(HWallTrb2LookupChan& r) {
    cell=r.getCell();
  }
  void clear() {
    cell=-1;
  }
  ClassDef(HWallTrb2LookupChan,1) // Channel level of the lookup table for the TRB2 unpacker of the Forward Wall
};


class HWallTrb2LookupBoard: public TObject {
  friend class HWallTrb2Lookup;
protected:
  TObjArray* array;     // pointer array containing HWallTrb2LookupChan objects
public:
  HWallTrb2LookupBoard();
  ~HWallTrb2LookupBoard();
  HWallTrb2LookupChan* getChannel(Int_t c) {
    if (c>=0 && c<128) return &((*this)[c]);
    else return 0;
  }
  HWallTrb2LookupChan& operator[](Int_t i) {
      return *static_cast<HWallTrb2LookupChan*>((*array)[i]);
  }
  Int_t getSize()  { return 128; }
  void clear();
  ClassDef(HWallTrb2LookupBoard,1) // Board level of  the lookup table for the TRB2 unpacker of the Forward Wall
};


class HWallTrb2Lookup : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HWallTrb2LookupBoard
  Int_t arrayOffset; // offset to calculate the index
public:
  HWallTrb2Lookup(const Char_t* name="WallTrb2Lookup",
             const Char_t* title="Lookup table for the TRB2 unpacker of the Forward Wall",
             const Char_t* context="WallTrb2LookupProduction",
             Int_t minTrbnetAddress=Trbnet::kWallMinTrbnetAddress,
             Int_t maxTrbnetAddress=Trbnet::kWallMaxTrbnetAddress);
  ~HWallTrb2Lookup();
  HWallTrb2LookupBoard* getBoard(Int_t trbnetAddress) {
    return (HWallTrb2LookupBoard*)(array->At(trbnetAddress-arrayOffset));
  }
  HWallTrb2LookupBoard* operator[](Int_t i) {
    return static_cast<HWallTrb2LookupBoard*>((*array)[i]);
  }
  Int_t getSize() { return array->GetLast()+1; }
  Int_t getArrayOffset() { return arrayOffset; }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParam();
  Bool_t fill(Int_t,Int_t,Int_t);
  Bool_t readline(const Char_t*);
  void putAsciiHeader(TString&);
  void write(fstream&);
  ClassDef(HWallTrb2Lookup,1) // Lookup table for the TRB2 unpacker of the Forward Wall
};

#endif  /*!HWALLTRB2LOOKUP_H*/
