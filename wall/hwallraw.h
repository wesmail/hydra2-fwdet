#ifndef HWALLRAW_H
#define HWALLRAW_H

#include "hlocateddataobject.h"

class HWallRaw : public HLocatedDataObject {
protected:
  //1 hit case (old) USED FOR SIMULATION
  Float_t time;   // tdc time
  Float_t charge; // adc charge
  //end 1 hit case (old)

  Int_t nHits;    // number of hits
  Int_t   cell;   // location

  Float_t time1;    // tdc time of 1st hit
  Float_t width1;   // width of 1st hit
  Float_t time2;    // tdc time of 2nd hit
  Float_t width2;   // width of 2nd hit
  Float_t time3;    // tdc time of 3rd hit
  Float_t width3;   // width of 3rd hit
  Float_t time4;    // tdc time of 4th hit
  Float_t width4;   // width of 4th hit

public:
  HWallRaw(void) {clear();}
  ~HWallRaw(void) {}

  void clear(void) {
    nHits=0;
    cell=-1;
    time1=width1=time2=width2=time3=width3=time4=width4=-200;
    width1=-200;
    time=charge=-200;//1 hit (old)
  } 

  //1 hit case (old) USED FOR SIMULATION
  Float_t getTime(void) {return time;}
  Float_t getCharge(void) {return charge;}
  void setTime(Float_t t) {time=t;}
  void setCharge(Float_t c) {charge=c;}
  //end 1 hit case (old)
 
  Int_t getNHits(void) const { return nHits; }
  Int_t getCell(void) {return  cell;}  

  Float_t getTime(const Int_t n) const;
  Float_t getWidth(const Int_t n) const;
  inline Float_t getADC(const Int_t n){return(getWidth(n));}// Alias for getWidth
  void getTimeAndWidth(const Int_t,Float_t&,Float_t&);
  inline void getTimeAndADC(const Int_t n,Float_t &t,Float_t &a){getTimeAndWidth(n,t,a);};// Alias for getTimeAndWidth
  inline Int_t getMaxMult(void){ return(4);}// return number of multiplicity supported (constant)

  void setCell(Int_t c) {cell=c;}
  void setMult(Int_t mu) {nHits=mu;}// use fill and fill_l/t and multiplicity will always be correct

  Bool_t fill(const Float_t,const  Float_t);
  Bool_t fill_lead(const Float_t time);
  Bool_t fill_trail(const Float_t time);

  ClassDef(HWallRaw,2) // Raw data class of Forward Wall
};

#endif /* !HWallRAW_H */
