#ifndef HMDCCAL1_H
#define HMDCCAL1_H

#include "TObject.h"

class HMdcCal1 : public TObject {
protected:
  Int_t nHits;    // number of hits in this cell
  Float_t time1;  // drift time of first hit  [ns]
  Float_t time2;  // drift time of second hit (2 leading edge mode) or trailing edge of signal [ns]
  Int_t sector;   // sector number [0-5]
  Int_t module;   // module number [0-3]
  Int_t layer;    // layer number  [0-5]
  Int_t cell;     // cell number in wire plane
public:
  HMdcCal1(void) {clear();}
  ~HMdcCal1(void) {}
  void clear() {
      // reset data members to default value
      nHits=0;
      sector=module=layer=cell=-1;
      time1=time2=-999.F;
  }
  void setSector(const Int_t s) { sector = s; }
  void setModule(const Int_t m) { module = m; }
  void setLayer(const Int_t l)  { layer = l; }
  void setCell(const Int_t c)   { cell = c; }
  void setAddress(const Int_t s,const Int_t m,const Int_t l,const Int_t c) {
    sector=s;
    module=m;
    layer=l;
    cell=c;
  }
  void setNHits(const Int_t n) {nHits=n;}
  void setTime1(const Float_t t) {time1=t;}
  void setTime2(const Float_t t) {time2=t;}

  Int_t getSector(void) const { return sector; }
  Int_t getModule(void) const { return module; }
  Int_t getLayer(void) const  { return layer; }
  Int_t getCell(void) const   { return cell; }
  void getAddress(Int_t& s,Int_t& m,Int_t& l,Int_t& c) {
    s=sector;
    m=module;
    l=layer;
    c=cell;
  }
  Int_t getNHits(void) const  { return nHits; }
  Float_t getTime1(void) const {return time1;}
  Float_t getTime2(void) const {return time2;}

  ClassDef(HMdcCal1,1) // cal hit on a MDC
};

#endif  /* HMDCCAL1_H */

