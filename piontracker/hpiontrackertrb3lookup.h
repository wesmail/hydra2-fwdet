#ifndef HPIONTRACKERTRB3LOOKUP_H
#define HPIONTRACKERTRB3LOOKUP_H

#include "TObjArray.h"
#include "hparset.h"
#include "htrbnetdef.h"

using namespace Trbnet;
using namespace std;

class HPionTrackerTrb3LookupChan : public TObject {
protected:
  Int_t  module;    // module number
  Int_t  cell;      // cell number
public:
  HPionTrackerTrb3LookupChan()  {clear();}
  ~HPionTrackerTrb3LookupChan() {}

  Int_t getModule() {return module;}
  Int_t getCell()   {return cell;}

  void getAddress(Int_t& m, Int_t& c) {
    m = module;
    c = cell;
  }

  void clear() {
    module = -1;
    cell = -1;
  }

  void setModule(const Int_t n) {module = n;}
  void setCell(const Int_t n)   {cell = n;}

  void fill(Int_t m, Int_t c) {
    module = m;
    cell = c;
  }

  void fill(HPionTrackerTrb3LookupChan& r) {
    module = r.getModule();
    cell = r.getCell();
  }

  ClassDef(HPionTrackerTrb3LookupChan, 1) // Channel level of the lookup table for the PionTracker TRB3 unpacker
};


class HPionTrackerTrb3LookupBoard: public TObject {
  friend class HPionTrackerTrb3Lookup;
protected:
  TObjArray* array;     // pointer array containing HPionTrackerTrb3LookupChan objects
public:
  HPionTrackerTrb3LookupBoard();
  ~HPionTrackerTrb3LookupBoard();

  HPionTrackerTrb3LookupChan* getChannel(Int_t c) {
    if (c >= 0 && c < 256) return &((*this)[c]);
    else return 0;
  }

  HPionTrackerTrb3LookupChan& operator[](Int_t i) {
    return *static_cast<HPionTrackerTrb3LookupChan*>((*array)[i]);
  }

  Int_t getSize()  {return 256;}
  void  clear();
  ClassDef(HPionTrackerTrb3LookupBoard, 1) // Board level of  the lookup table for the PionTracker TRB3 unpacker
};


class HPionTrackerTrb3Lookup : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HPionTrackerTrb3LookupBoard
  Int_t arrayOffset; // offset to calculate the index
public:
  HPionTrackerTrb3Lookup(const Char_t* name = "PionTrackerTrb3Lookup",
                         const Char_t* title = "Lookup table for the TRB3 unpacker of the PionTracker",
                         const Char_t* context = "PionTrackerTrb3LookupProduction",
                         Int_t minTrbnetAddress = Trbnet::kPionTrackerTrb3MinTrbnetAddress,
                         Int_t maxTrbnetAddress = Trbnet::kPionTrackerTrb3MaxTrbnetAddress);
  ~HPionTrackerTrb3Lookup();

  HPionTrackerTrb3LookupBoard* getBoard(Int_t trbnetAddress) {
    return (HPionTrackerTrb3LookupBoard*)(array->At(trbnetAddress - arrayOffset));
  }
  HPionTrackerTrb3LookupBoard* operator[](Int_t i) {
     return static_cast<HPionTrackerTrb3LookupBoard*>((*array)[i]);
  }

  Int_t getSize() {return array->GetLast() + 1;}
  Int_t getArrayOffset() {return arrayOffset;}

  Bool_t init(HParIo* input, Int_t* set);
  Int_t  write(HParIo* output);
  void   clear();
  void   printParams();
  Bool_t fill(Int_t, Int_t, Int_t, Int_t);
  Bool_t readline(const Char_t*);
  void   putAsciiHeader(TString&);
  void   write(fstream&);
  ClassDef(HPionTrackerTrb3Lookup, 1) // Lookup table for the TRB3 unpacker of the PionTracker
};

#endif  /*!HPIONTRACKERTRB3LOOKUP_H*/
