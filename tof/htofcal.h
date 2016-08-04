#ifndef  HTOFCAL_H
#define  HTOFCAL_H

#include "TObject.h"

class HTofCal : public TObject {
private:
  Float_t timeR; //TDC: right hit
  Float_t timeL; //TDC: left hit
  Float_t chargeR; //QDC: right hit
  Float_t chargeL; //QDC: left hit
  Short_t nHit; //number of hits
  Short_t address; //sector,module and cell numbers
public:
  HTofCal(void) :  nHit(0) {}
  ~HTofCal(void) {}
  void Clear(void) { nHit=0;}
  Short_t getAddress(void) {return  address;}
  Char_t getSector(void) {return ( address & 0xF000)>>12;}
  Char_t getModule(void) {return ( address & 0x0FC0)>>6;}
  Char_t getCell(void) {return ( address & 0x003F);}
  Short_t getSize(void) {return  nHit;}
  Float_t getTimeLeft(void) {return  timeL;}
  Float_t getTimeRight(void) {return  timeR;}
  Float_t getChargeLeft(void) {return  chargeL;}
  Float_t getCargeRight(void) {return  chargeR;}
  void setTimeLeft(Float_t time) { timeL=time;}
  void setTimeRight(Float_t time) { timeR=time;}
  void setChargeLeft(Float_t charge) { chargeL=charge;}
  void setChargeRight(Float_t charge) { chargeR=charge;}
  void setNHit(Short_t nhit) { nHit=nhit;}
  void setSector(Char_t s) { address=( address & 0x0FFF) | (s<<12);}
  void setModule(Char_t m) { address=( address & 0xF03F) | (m<<6);}
  void setCell(Char_t c) { address=( address & 0xFFC0) | c;}
  void setAddress(Short_t add) { address=add;}
  ClassDef(HTofCal,1) //HTof cal data
};

#endif /* !HTOFCAL_H */
