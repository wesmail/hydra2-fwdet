#ifndef HSTART2TRB2LOOKUP_H
#define HSTART2TRB2LOOKUP_H

#include "TObjArray.h"
#include "TObject.h"

#include "hparset.h"
#include "htrbnetdef.h"

using namespace Trbnet;
using namespace std;

class HStart2Trb2LookupChan : public TObject {
protected:
   Int_t  module;    // module number
   Int_t  cell;      // cell number
public:
   HStart2Trb2LookupChan()     {
      clear();
   }
   ~HStart2Trb2LookupChan()    {
      ;
   }
   Int_t getModule()    {
      return module;
   }
   Int_t getCell()      {
      return cell;
   }
   void getAddress(Int_t& m, Int_t& c) {
      m = module;
      c = cell;
   }
   void fill(Int_t m, Int_t c) {
      module = m;
      cell = c;
   }
   void fill(HStart2Trb2LookupChan& r) {
      module = r.getModule();
      cell = r.getCell();
   }
   void setModule(const Int_t n)    {
      module = n;
   }
   void setCell(const Int_t n)      {
      cell = n;
   }
   void clear() {
      module = -1;
      cell = -1;
   }
   ClassDef(HStart2Trb2LookupChan, 1) // Channel level of the lookup table for the START2 TRB2 unpacker
};


class HStart2Trb2LookupBoard: public TObject {
   friend class HStart2Trb2Lookup;
protected:
   TObjArray* array;     // pointer array containing HStart2Trb2LookupChan objects
public:
   HStart2Trb2LookupBoard();
   ~HStart2Trb2LookupBoard();
   HStart2Trb2LookupChan* getChannel(Int_t c) {
      if (c >= 0 && c < 128) return &((*this)[c]);
      else return 0;
   }
   HStart2Trb2LookupChan& operator[](Int_t i) {
      return *static_cast<HStart2Trb2LookupChan*>((*array)[i]);
   }
   Int_t getSize()  {
      return 128;
   }
   void clear();
   ClassDef(HStart2Trb2LookupBoard, 1) // Board level of  the lookup table for the START2 TRB2 unpacker
};


class HStart2Trb2Lookup : public HParSet {
protected:
   TObjArray* array;  // array of pointers of type HStart2Trb2LookupBoard
   Int_t arrayOffset; // offset to calculate the index
public:
   HStart2Trb2Lookup(const Char_t* name = "Start2Trb2Lookup",
                     const Char_t* title = "Lookup table for the TRB2 unpacker of the START2 detector",
                     const Char_t* context = "Start2Trb2LookupProduction",
                     Int_t minTrbnetAddress = Trbnet::kStartMinTrbnetAddress,
                     Int_t maxTrbnetAddress = Trbnet::kStartMaxTrbnetAddress);
   ~HStart2Trb2Lookup();
   HStart2Trb2LookupBoard* getBoard(Int_t trbnetAddress) {
      return (HStart2Trb2LookupBoard*)(array->At(trbnetAddress - arrayOffset));
   }
   HStart2Trb2LookupBoard* operator[](Int_t i) {
      return static_cast<HStart2Trb2LookupBoard*>((*array)[i]);
   }
   Int_t getSize() {
      return array->GetLast() + 1;
   }
   Int_t getArrayOffset() {
      return arrayOffset;
   }
   Bool_t init(HParIo* input, Int_t* set);
   Int_t write(HParIo* output);
   void clear();
   void printParam();
   Bool_t fill(Int_t, Int_t, Int_t, Int_t);
   Bool_t readline(const Char_t*);
   void putAsciiHeader(TString&);
   void write(fstream&);
   ClassDef(HStart2Trb2Lookup, 1) // Lookup table for the TRB2 unpacker of the START2 detector
};

#endif  /*!HSTART2TRB2LOOKUP_H*/
