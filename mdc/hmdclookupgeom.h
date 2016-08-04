#ifndef HMDCLOOKUPGEOM_H
#define HMDCLOOKUPGEOM_H

#include "TNamed.h"
#include "TObjArray.h"
#include "hparset.h"

class HParIo;

class HMdcLookupChan: public TObject {
protected:
  Int_t  nLayer;      // number of the layer
  Int_t  nCell;       // number of the cell
  Char_t readoutSide; // side, where the signal is read out
public:
  HMdcLookupChan(Int_t nl=-1,Int_t nc=-1, Char_t s='0') {
    nLayer=nl;
    nCell=nc;
    readoutSide=s;
  }
  ~HMdcLookupChan() {}
  void setNLayer(Int_t l) {nLayer=l;}
  void setNCell(Int_t c) {nCell=c;}
  void setReadoutSide(Char_t s) {
    readoutSide=(s=='L'||s=='R'||s=='U') ? s : '0';
  }
  void fill(Int_t l,Int_t c,Char_t s) {
    nLayer=l;
    nCell=c;
    setReadoutSide(s);
  }
  void fill(HMdcLookupChan& r) {
    nLayer=r.getNLayer();
    nCell=r.getNCell();
    readoutSide=r.getReadoutSide();
  }
  Int_t getNLayer() {return nLayer;}
  Int_t getNCell() {return nCell;}
  Char_t getReadoutSide() {return readoutSide;}
  void clear() {
    nLayer=-1;
    nCell=-1;
    readoutSide='0';
  }
  ClassDef(HMdcLookupChan,2) // Channel level of MDC lookup table (channel->wire)
};


class HMdcLookupMoth: public TNamed {
protected:
  TObjArray* array;
public:
  HMdcLookupMoth(Int_t n=0,const Text_t* name="");
  ~HMdcLookupMoth();
  HMdcLookupChan& operator[](Int_t i) {
    return *static_cast<HMdcLookupChan*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcLookupMoth,1) // Motherboard level of MDC lookup table (channel->wire)
};


class HMdcLookupGMod: public TObject {
protected:
  TObjArray* array;
public:
  HMdcLookupGMod(Int_t n=16);
  ~HMdcLookupGMod();
  HMdcLookupMoth& operator[](Int_t i) {
    return *static_cast<HMdcLookupMoth*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  void createMoth(Int_t, Int_t, const Char_t*);
  ClassDef(HMdcLookupGMod,1) // Module level of MDC lookup table (channel->wire)
};


class HMdcLookupGSec: public TObject {
protected:
  TObjArray* array;
  Int_t sector;
public:
  HMdcLookupGSec(Int_t n=4);
  ~HMdcLookupGSec();
  HMdcLookupGMod& operator[](Int_t i) {
    return *static_cast<HMdcLookupGMod*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcLookupGSec,1) // Sector level of MDC lookup table (channel->wire)
};


class HMdcLookupGeom : public HParSet {
protected:
  TObjArray* array;     // array of pointers
public:
  HMdcLookupGeom(const Char_t* name="MdcLookupGeom",
                 const Char_t* title="Mdc lookup table from raw to cal1",
                 const Char_t* context="",
                 Int_t n=6);
  ~HMdcLookupGeom();
  HMdcLookupGSec& operator[](Int_t i) {
    return *static_cast<HMdcLookupGSec*>((*array)[i]);
  }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t write(HParIo* output);
  void readline(const Char_t*, Int_t*);
  void putAsciiHeader(TString&);
  Bool_t writeline(Char_t *, Int_t, Int_t, Int_t, Int_t);
  Int_t getSize() {return array->GetEntries();}
  void clear();
  void printParam();
  ClassDef(HMdcLookupGeom,1) //  MDC lookup table (channel->wire)
};

#endif /* !HMDCLOOKUPGEOM_H */














