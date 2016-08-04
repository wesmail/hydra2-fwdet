#ifndef HMDCRAWCOR_H
#define HMDCRAWCOR_H

using namespace std;

#include "TObject.h"
#include <cstdlib>
#include <iostream>
class HMdcRaw;
class HCategory;


class HMdcRawCor : public TObject {
protected:

  Int_t flagraw;
  Int_t rawshift1;
  Int_t rawshift2;
  Int_t rawshift3;
  Int_t rawshift4;
  Int_t rawshift5;
  Int_t rawshift6;
  Int_t sector; // sector number
  Int_t module; // module number
  Int_t mbo;    // motherboard number
  Int_t tdc;    // tdc number * 8 + tdc channel number
 

public:
    HMdcRawCor(void) { clear(); }
    ~HMdcRawCor(void){;}
    void clear(void) {
      flagraw=-1;
      rawshift1=0;
      rawshift2=0;
      rawshift3=0;
      rawshift4=0;
      rawshift5=0;
      rawshift6=0;
      sector=module=mbo=tdc=-1;
    };
    void setAddress(const Int_t s,const Int_t m,const Int_t mb,const Int_t t) {
      sector=s;
      module=m;
      mbo=mb;
      tdc=t;
    };

    void setFlagRaw(const Int_t f){
      flagraw=f;
    };
 
    Int_t getFlagRaw(void){ return flagraw;};
    

    void setTimeShift(const Int_t sh, const Int_t nsh){
      switch (nsh)
	{
	case 1:
	  rawshift1=sh;
	  break;
        case 2:
	  rawshift2=sh;
	  break;
	case 3:
	  rawshift3=sh;
	  break;
        case 4:
	  rawshift4=sh;
	  break;
        case 5:
	  rawshift5=sh;
	  break;
	case 6:
	  rawshift6=sh;
	  break;
	default:
	  cout << " HMdcRawCor::setTimeShift(): Argument not known: " << nsh << endl;
	}
    };

    Int_t getSector(void) const { return sector; };
    Int_t getModule(void) const { return module; };
    Int_t getMbo(void) const    { return mbo; };
    Int_t getTdc(void) const    { return tdc; };
  
    void getAddress(Int_t& s,Int_t& m,Int_t& mb,Int_t& t) {
      s=sector;
      m=module;
      mb=mbo;
      t=tdc;
    };

    ClassDef(HMdcRawCor,1) // RawCor hit on a MDC
};

#endif /* !HMDCRAWCOR_H */
