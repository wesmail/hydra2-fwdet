#ifndef HEMCCAL_H
#define HEMCCAL_H

using namespace std;

#include "TObject.h"

class HEmcCal : public TObject {
protected:
  UChar_t nHits;   // number of hits in this cell
  Float_t time1;   // time of first hit
  Float_t time2;   // time of second hit
  Float_t energy;  // energy deposited in one crystal
  Char_t  sector;  // sector number (0..5)
  UChar_t cell;    // cell number (0..231)
  Char_t  row;     // row number (0..13)
  Char_t  column;  // column number (0..16)
  
public:
  HEmcCal() {clear();}
  ~HEmcCal() {}
  virtual void clear(void);

  void setNHits(const UChar_t n)  {nHits=n;}
  void setTime1(const Float_t t)  {time1=t;}
  void setTime2(const Float_t t)  {time2=t;}
  void setEnergy(const Float_t e) {energy=e;}
  void setSector(const Char_t s)  {sector=s;}
  void setCell(const UChar_t c)   {cell=c;}
  void setRow(const Char_t r)     {row=r;}
  void setColumn(const Char_t c)  {column=c;}
  void setAddress(const Char_t se, const UChar_t ce, const Char_t ro, const Char_t co);

  UChar_t getNHits(void)  const {return nHits;}
  Float_t getTime1(void)  const {return time1;}
  Float_t getTime2(void)  const {return time2;}
  Float_t getEnergy(void) const {return energy;}
  Char_t  getSector(void) const {return sector;}
  UChar_t getCell(void)   const {return cell;}
  Char_t  getRow(void)    const {return row;}
  Char_t  getColumn(void) const {return column;}
  
  ClassDef(HEmcCal,1) //EMC cal data class
};

inline void HEmcCal::setAddress(const Char_t se, const UChar_t ce, const Char_t ro, const Char_t co) {
  sector = se;
  cell   = ce;
  row    = ro;
  column = co;
}

#endif /* !HEMCCAL_H */
