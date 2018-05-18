#ifndef HEMCCALQA_H
#define HEMCCALQA_H

#include "hemcraw.h"

using namespace std;

#include "TObject.h"

struct HEmcHitQA_t {
  Float_t time;
  Float_t width;
  HEmcHitQA_t() : time(0), width(0) {}
  virtual ~HEmcHitQA_t(){};
  ClassDef(HEmcHitQA_t, 1);
};

struct HEmcHitMatchQA_t {
  Float_t time;
  Float_t energy;
  HEmcHitMatchQA_t() : time(0), energy(0) {}
  virtual ~HEmcHitMatchQA_t(){};
  ClassDef(HEmcHitMatchQA_t, 1);
};

class HEmcCalQA : public TObject {
protected:
  Char_t  sector;       // sector number (0..5)
  UChar_t cell;         // cell number (0..254)
  Char_t  row;          // row number (0..14)
  Char_t  column;       // column number (0..16)
  UInt_t  nFast;
  UInt_t  nSlow;
  UInt_t  nMatched;
  HEmcHitQA_t fast[NTIMESEMCRAW];  //[nFast]
  HEmcHitQA_t slow[NTIMESEMCRAW];  //[nSlow]
  HEmcHitMatchQA_t matched[NTIMESEMCRAW]; //[nMatched]

public:
  HEmcCalQA() :
    sector(-1),
    cell(0),
    row(-1),
    column(-1),
    nFast(0),
    nSlow(0),
    nMatched(0)
  {
  }
  ~HEmcCalQA() {}
  virtual void clear(void);

  void setAddress(Char_t se, UChar_t ce, Char_t ro, Char_t co);
  void setSector(Char_t s)        {sector = s;}
  void setCell(UChar_t c)         {cell   = c;}
  void setRow(Char_t r)           {row    = r;}
  void setColumn(Char_t c)        {column = c;}

  UChar_t getNfast(void)        const {return nFast;}
  UChar_t getNslow(void)        const {return nSlow;}
  UChar_t getNmatched(void)     const {return nMatched;}
  Char_t  getSector(void)       const {return sector;}
  UChar_t getCell(void)         const {return cell;}
  Char_t  getRow(void)          const {return row;}
  Char_t  getColumn(void)       const {return column;}

  void addFastHit(Float_t time, Float_t width);
  void addFastHit(const HEmcHitQA_t & h);
  HEmcHitQA_t getFastHit(UInt_t n) const;
  void addSlowHit(Float_t time, Float_t width);
  void addSlowHit(const HEmcHitQA_t & h);
  HEmcHitQA_t getSlowHit(UInt_t n) const;
  void addMatchedHit(Float_t time, Float_t energy);
  void addMatchedHit(const HEmcHitMatchQA_t & h);
  HEmcHitMatchQA_t getMatchedHit(UInt_t n) const;

  ClassDef(HEmcCalQA,1) //EMC cal data class
};

inline void HEmcCalQA::setAddress(Char_t se, UChar_t ce, Char_t ro, Char_t co) {
  sector = se;
  cell   = ce;
  row    = ro;
  column = co;
}

#endif /* !HEMCCALQA_H */
