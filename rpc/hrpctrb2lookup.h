#ifndef HRPCTRB2LOOKUP_H
#define HRPCTRB2LOOKUP_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include "htrbnetdef.h"
#include <fstream>

using namespace Trbnet;
using namespace std;

class HRpcTrb2LookupChan : public TObject {
protected:
  Int_t  sector;    // sector number
  Int_t  column;    // column number
  Int_t  cell;      // cell number
  Char_t side;      // side of cell
  Int_t  feAddress; // front end address
public:
  HRpcTrb2LookupChan()     { clear(); }
  ~HRpcTrb2LookupChan()    {;}
  Int_t getSector()    { return sector; }
  Int_t getColumn()    { return column; }
  Int_t getCell()      { return cell;  }
  Char_t getSide()     { return side;  }
  Int_t getFeAddress() { return feAddress;  }
  void getAddress(Int_t& se,Int_t& co,Int_t& ce,Char_t& si,Int_t& fa) {
    se=sector;
    co=column;
    ce=cell;
    si=side;
    fa=feAddress;
  }
  void fill(Int_t se,Int_t co,Int_t ce,Char_t si,Int_t fa) {
    sector=se;
    column=co;
    cell=ce;
    side=si;
    feAddress=fa;
  }
  void fill(HRpcTrb2LookupChan& r) {
    sector=r.getSector();
    column=r.getColumn();
    cell=r.getCell();
    side=r.getSide();
    feAddress=r.getFeAddress();
  }
  void setSector(const Int_t n)    { sector=n; }
  void setColumn(const Int_t n)    { column=n; }
  void setCell(const Int_t n)      { cell=n; }
  void setSide(Char_t c)           { side=c; }
  void setFeAddress(const Int_t n) { feAddress=n; }
  void clear() {
    sector=column=cell=feAddress=-1;
    side='\0';
  }
  ClassDef(HRpcTrb2LookupChan,1) // Channel level of the lookup table for the RPC TRB2 unpacker
};


class HRpcTrb2LookupBoard: public TObject {
  friend class HRpcTrb2Lookup;
protected:
  TObjArray* array;     // pointer array containing HRpcTrb2LookupChan objects
public:
  HRpcTrb2LookupBoard();
  ~HRpcTrb2LookupBoard();
  HRpcTrb2LookupChan* getChannel(Int_t c) {
    if (c>=0 && c<128) return &((*this)[c]);
    else return 0;
  }
  HRpcTrb2LookupChan& operator[](Int_t i) {
      return *static_cast<HRpcTrb2LookupChan*>((*array)[i]);
  }
  Int_t getSize()  { return 128; }
  void clear();
  ClassDef(HRpcTrb2LookupBoard,1) // Board level of  the lookup table for the RPC TRB2 unpacker
};


class HRpcTrb2Lookup : public HParSet {
protected:
  TObjArray* array;  // array of pointers of type HRpcTrb2LookupBoard
  Int_t arrayOffset; // offset to calculate the index
public:
  HRpcTrb2Lookup(const Char_t* name="RpcTrb2Lookup",
             const Char_t* title="Lookup table for the TRB2 unpacker of the RPC detector",
             const Char_t* context="RpcTrb2LookupProduction",
             Int_t minTrbnetAddress=Trbnet::kRpcMinTrbnetAddress,
             Int_t maxTrbnetAddress=Trbnet::kRpcMaxTrbnetAddress);
  ~HRpcTrb2Lookup();
  HRpcTrb2LookupBoard* getBoard(Int_t trbnetAddress) {
    return (HRpcTrb2LookupBoard*)(array->At(trbnetAddress-arrayOffset));
  }
  HRpcTrb2LookupBoard* operator[](Int_t i) {
    return static_cast<HRpcTrb2LookupBoard*>((*array)[i]);
  }
  Int_t getSize() { return array->GetLast()+1; }
  Int_t getArrayOffset() { return arrayOffset; }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void clear();
  void printParam();
  Bool_t fill(Int_t,Int_t,Int_t,Int_t,Int_t,Char_t,Int_t);
  Bool_t readline(const Char_t*);
  void putAsciiHeader(TString&);
  void write(fstream&);
  ClassDef(HRpcTrb2Lookup,1) // Lookup table for the TRB2 unpacker of the RPC detector
};

#endif  /*!HRPCTRB2LOOKUP_H*/
