#ifndef HEMCRAW_H
#define HEMCRAW_H


/////////////////////////////////////////////////////////////
//
//  HEmcRaw
//
//  Event data structure for HADES Electromagnetic Calorimeter (EMC).
//  first sketch bei I.Koenig 2017,
//  continued by JAM (j.adamczewski@gsi.de) 23-apr-2018
//
/////////////////////////////////////////////////////////////


#include "TObject.h"

#define NTIMESEMCRAW 10

class HEmcHit_t : public TObject
{
private:

    Float_t fLeadingEdgeTime;
    Float_t fTrailingEdgeTime;

public:
    HEmcHit_t(Float_t leading=0., Float_t trailing=0.):
	fLeadingEdgeTime(leading), fTrailingEdgeTime(trailing){}


    Float_t getLeadingTime() const { return fLeadingEdgeTime; }
    Float_t getTrailingTime() const { return fTrailingEdgeTime; }

    void  setTimes(Float_t leading, Float_t trailing)  { fLeadingEdgeTime = leading; fTrailingEdgeTime = trailing; }
    void  setTimes(HEmcHit_t& hit)  { fLeadingEdgeTime = hit.getLeadingTime(); fTrailingEdgeTime = hit.getTrailingTime(); }

    Float_t getToT() const
    {
	if(fLeadingEdgeTime==0 || fTrailingEdgeTime==0)
	    return -1; // detect incomplete edges
	return (fTrailingEdgeTime - fLeadingEdgeTime);
    }
    void clear() { fLeadingEdgeTime = 0; fTrailingEdgeTime = 0;}

    ClassDef(HEmcHit_t, 1) // object keeping leading and trailing edge times
};



class HEmcRaw : public TObject {
private:
    Char_t  fSector;           // sector number
    UChar_t fCell;             // cell number
    Char_t  fRow;               // row number (0..14)
    Char_t  fColumn;            // column number (0..16)

    HEmcHit_t fFastHits[NTIMESEMCRAW];
    HEmcHit_t fSlowHits[NTIMESEMCRAW];
    UInt_t fnFast;
    UInt_t fnSlow;

public:
    HEmcRaw() :fSector(-1),fCell(0),fRow(0),fColumn(0) {clearHits();}
    virtual ~HEmcRaw() {}

    Char_t   getSector()           const { return fSector; }
    UChar_t  getCell()             const { return fCell; }
    Char_t   getRow(void)          const {return fRow;}
    Char_t   getColumn(void)       const {return fColumn;}
    UInt_t   getFastMultiplicity() const { return fnFast; }
    UInt_t   getSlowMultiplicity() const { return fnSlow; }
    Float_t  getFastTimeLeading(UInt_t n) const;
    Float_t  getFastTimeTrailing(UInt_t n) const;
    Float_t  getSlowTimeLeading(UInt_t n) const;
    Float_t  getSlowTimeTrailing(UInt_t n) const;
    Float_t  getFastWidth(UInt_t n) const;
    Float_t  getSlowWidth(UInt_t n) const;
    void     getFastTimeAndWidth(UInt_t n, Float_t& s, Float_t& c) const;
    void     getSlowTimeAndWidth(UInt_t n, Float_t& s, Float_t& c) const;
    void     getAddress(Int_t& s, Int_t& c, Char_t& ro, Char_t& co) const;
    void     setSector(Char_t s)        { fSector = s; }
    void     setCell(UChar_t c)         { fCell   = c; }
    void     setRow(Char_t r)           { fRow    = r;}
    void     setColumn(Char_t c)        { fColumn = c;}
    void     setAddress(Int_t s, Int_t c, Char_t ro, Char_t co);
    void     clear() ;
    void     clearHits();
    void     addFastHit(HEmcHit_t theHit)                  { if(fnFast<NTIMESEMCRAW) { fFastHits[fnFast].setTimes(theHit); fnFast++;} }
    void     addFastHit(Float_t leading, Float_t trailing) { if(fnFast<NTIMESEMCRAW) { fFastHits[fnFast].setTimes(leading,trailing); fnFast++;}}
    void     addSlowHit(HEmcHit_t theHit)                  { if(fnSlow<NTIMESEMCRAW) { fSlowHits[fnSlow].setTimes(theHit); fnSlow++;} }
    void     addSlowHit(Float_t leading, Float_t trailing) { if(fnSlow<NTIMESEMCRAW) { fSlowHits[fnSlow].setTimes(leading,trailing); fnSlow++;}  }


    ClassDef(HEmcRaw, 1) // raw data of EMC detector using TRB3 for readout
};

inline void HEmcRaw::clear(void) {
    fSector = -1;
    fCell = 0;
    fRow = 0;
    fColumn = 0;
    for(Int_t i=0;i<NTIMESEMCRAW; i++){
         fFastHits[i].clear();
         fSlowHits[i].clear();
    }
    fnFast = 0;
    fnSlow = 0;
}

inline void HEmcRaw::clearHits(void) {
    for(Int_t i=0;i<NTIMESEMCRAW; i++){
         fFastHits[i].clear();
         fSlowHits[i].clear();
    }
    fnFast = 0;
    fnSlow = 0;
}

inline void HEmcRaw::getAddress(Int_t& s, Int_t& c, Char_t& ro, Char_t& co) const {
    s = fSector;
    c = fCell;
    ro= fRow;
    co= fColumn;
}

inline void HEmcRaw::setAddress(Int_t s, Int_t c,Char_t ro, Char_t co) {
    fSector = static_cast<Char_t>(s);
    fCell   = static_cast<UChar_t>(c);
    fRow    = ro;
    fColumn = co;
}

#endif /* ! HEMCRAW_H */
