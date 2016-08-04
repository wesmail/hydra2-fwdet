#ifndef HMDCRAW_H
#define HMDCRAW_H

using namespace std;

#include "TObject.h"
#include <cstdlib>
#include <iostream>

class HMdcRaw : public TObject {
protected:
    Int_t nHits;  // number of hits / Tdc mode
    Int_t time1;  // time of 1st hit [binnumber]
    Int_t time2;  // time of 2nd hit [binnumber]
    Int_t time3;  // time of 3rd hit [binnumber]
    Int_t time4;  // time of 4th hit [binnumber]
    Int_t time5;  // time of 5th hit [binnumber]
    Int_t time6;  // time of 6th hit [binnumber]
    Int_t sector; // sector number   [0-5]
    Int_t module; // module number   [0-3]
    Int_t mbo;    // motherboard number [0-15]
    Int_t tdc;    // tdc number * 8 + tdc channel number [0-95]
    Int_t nTrialsToFillHits; // number of trials to fill a time value to this cell / corresponding to nHist
public:
    HMdcRaw(void) { clear(); }
    ~HMdcRaw(void){;}

    /* enum and constants */
    static const Int_t kDefaultValueTime   ; //! default value of time
    static const Int_t kDefaultValueSector ; //! default value of sector
    static const Int_t kDefaultValueModule ; //! default value of module
    static const Int_t kDefaultValueMbo    ; //! default value of motherboard number
    static const Int_t kDefaultValueTdc    ; //! default value of tdc number

    void clear(void)
      {
	// reset data member to default values
	nHits=0;
	sector=kDefaultValueSector;
	module=kDefaultValueModule;
	mbo   =kDefaultValueMbo;
	tdc   =kDefaultValueTdc;
	time1=time2=time3=time4=time5=time6=kDefaultValueTime;
        nTrialsToFillHits=0;
      }

    void setSector(const Int_t n) { sector=n; }
    void setModule(const Int_t n) { module=n; }
    void setMbo(const Int_t n)    { mbo=n; }
    void setTdc(const Int_t n)    { tdc=n; }
    void setAddress(const Int_t s,const Int_t m,const Int_t mb,const Int_t t)
      {
	sector=s;
	module=m;
	mbo=mb;
	tdc=t;
      }

    Bool_t setTime(const Int_t,const Int_t mode=0, const Bool_t noComment = kFALSE);
    Bool_t setTimeNew(const Int_t time, const Int_t nrtime);

    Int_t getSector(void) const { return sector; }
    Int_t getModule(void) const { return module; }
    Int_t getMbo(void) const    { return mbo; }
    Int_t getTdc(void) const    { return tdc; }
    void getAddress(Int_t& s,Int_t& m,Int_t& mb,Int_t& t)
      {
	s=sector;
	m=module;
	mb=mbo;
	t=tdc;
      }
    Int_t getNHits(void) const  { return nHits; }
    Int_t getNTrialsToFillHits(void) const { return nTrialsToFillHits; }
    Int_t getTime(const Int_t n) const;

    ClassDef(HMdcRaw,1) // Raw hit on a MDC
};

#endif /* !HMDCRAW_H */
