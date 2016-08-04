#ifndef HEMCRAW_H
#define HEMCRAW_H

#include "TObject.h"

class HEmcRaw : public TObject {
private:
  Int_t   totMultiplicity;  // number of hits
  Char_t  sector;           // sector number
  UChar_t cell;             // cell number
  Float_t fTime1;           // tdc time of 1st hit
  Float_t fWidth1;          // width of 1st hit
  Float_t fTime2;           // tdc time of 2nd hit
  Float_t fWidth2;          // width of 2nd hit
  Float_t fTime3;           // tdc time of 3rd hit
  Float_t fWidth3;          // width of 3rd hit
  Float_t fTime4;           // tdc time of 4th hit
  Float_t fWidth4;          // width of 4th hit

public:
  HEmcRaw(void)  {clear();}
  ~HEmcRaw(void) {}

  void    clear(void);
  Int_t   getMaxMultiplicity(void)       {return 4;}
  Int_t   getTotMultiplicity(void) const {return totMultiplicity;}
  Char_t  getSector(void) const          {return sector;}
  UChar_t getCell(void) const            {return cell;}
  Float_t getTime(const Int_t n) const;
  Float_t getWidth(const Int_t n) const;
  void    getTimeAndWidth(const Int_t n, Float_t& s, Float_t& c);
  void    getAddress(Int_t& s, Int_t& c);

  void    setSector(Char_t s)            {sector = s;}
  void    setCell(UChar_t c)             {cell = c;}
  void    setAddress(const Int_t s, const Int_t c);
  Bool_t  setTimeAndWidth(const Float_t time, const Float_t width);

  ClassDef(HEmcRaw, 1) // raw data of EMC detector using TRB3 for readout
};

inline void HEmcRaw::clear(void) {
  sector = -1;
  totMultiplicity = cell = 0;
  fTime1 = fWidth1 = fTime2 = fWidth2 = fTime3 = fWidth3 = fTime4 = fWidth4 = -1000000.F;
}

inline void HEmcRaw::getAddress(Int_t& s, Int_t& c) {
  s = sector;
  c = cell;
}

inline void HEmcRaw::setAddress(const Int_t s, const Int_t c) {
  sector = (Char_t)s;
  cell   = (UChar_t)c;
}

#endif /* ! HEMCRAW_H */
