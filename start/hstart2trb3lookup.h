#ifndef HSTART2TRB3LOOKUP_H
#define HSTART2TRB3LOOKUP_H

#include "TObjArray.h"
#include "TObject.h"

#include "hparset.h"
#include "htrbnetdef.h"

using namespace Trbnet;
using namespace std;

class HStart2Trb3LookupChan : public TObject {
protected:
   Int_t  module;    // module number
   Int_t  cell;      // cell number
public:
   HStart2Trb3LookupChan()     {
      clear();
   }
   ~HStart2Trb3LookupChan()    {
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
   void fill(HStart2Trb3LookupChan& r) {
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
   ClassDef(HStart2Trb3LookupChan, 1) // Channel level of the lookup table for the START2 TRB3 unpacker
};


class HStart2Trb3LookupTdc: public TObject {
   friend class HStart2Trb3Lookup;
protected:
   TObjArray* array;     // pointer array containing HStart2Trb3LookupChan objects
public:
   HStart2Trb3LookupTdc();
   ~HStart2Trb3LookupTdc();
   Int_t getSize()  {
      return array ? array->GetLast()+1 : 0;
   }
   HStart2Trb3LookupChan* getChannel(Int_t c) {
      if (c >= 0 && c < getSize()) return &((*this)[c]);
      else return 0;
   }
   HStart2Trb3LookupChan& operator[](Int_t i) {
      return *static_cast<HStart2Trb3LookupChan*>((*array)[i]);
   }
   void clear();
   ClassDef(HStart2Trb3LookupTdc, 1) // Tdc level of  the lookup table for the START2 TRB3 unpacker
};


class HStart2Trb3Lookup : public HParSet {
protected:
   TObjArray* array;  // array of pointers of type HStart2Trb2LookupTdc
   Int_t arrayOffset; // offset to calculate the index
public:
   HStart2Trb3Lookup(const Char_t* name = "Start2Trb3Lookup",
                     const Char_t* title = "Lookup table for the TRB3 unpacker of the START2 detector",
                     const Char_t* context = "Start2Trb3LookupProduction",
                     Int_t minTrbnetAddress = Trbnet::kStartTrb3MinTrbnetAddress,
                     Int_t maxTrbnetAddress = Trbnet::kStartTrb3MaxTrbnetAddress);
   ~HStart2Trb3Lookup();
   HStart2Trb3LookupTdc* getTdc(Int_t trbnetAddress) {
      return (HStart2Trb3LookupTdc*)(array->At(trbnetAddress - arrayOffset));
   }
   HStart2Trb3LookupTdc* operator[](Int_t i) {
      return static_cast<HStart2Trb3LookupTdc*>((*array)[i]);
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
   ClassDef(HStart2Trb3Lookup, 1) // Lookup table for the TRB2 unpacker of the START2 detector
};

#endif  /*!HSTART2TRB3LOOKUP_H*/
