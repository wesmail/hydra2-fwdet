#ifndef HMDCCAL2_H
#define HMDCCAL2_H

#include "TObject.h"

class HMdcCal2 : public TObject {
protected:
  Int_t nHits;      // number of hits in this cell
  Float_t dist1;    // distance to the wire of first hit [mm]
  Float_t dist2;    // distance to the wire of second hit [mm]
  Float_t errDist1; // error of distance for first hit  [mm]
  Float_t errDist2; // error of distance for second hit [mm]
  Int_t sector;     // sector number [0-5]
  Int_t module;     // module number [0-3]
  Int_t layer;      // layer number  [0-5]
  Int_t cell;       // cell number in wire plane
public:
  HMdcCal2(void) {clear();}
  ~HMdcCal2(void) {}
  void clear() {
    nHits=0;
    sector=module=layer=cell=-1;
    dist1=dist2=errDist1=errDist2=-99.F;
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
  void setDist1(const Float_t d,const Float_t e) {
    dist1=d;
    errDist1=e;
  }
  void setDist2(const Float_t d,const Float_t e) {
    dist2=d;
    errDist2=e;
  }
  void setNHits(const Int_t n) {nHits=n;}

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
  Float_t getDist1(void) {return dist1;}
  Float_t getDist2(void) {return dist2;}
  Float_t getErrDist1(void) {return errDist1;}
  Float_t getErrDist2(void) {return errDist2;}

  ClassDef(HMdcCal2,1) // cal hit on a MDC
};

#endif  /* HMDCCAL2_H */

