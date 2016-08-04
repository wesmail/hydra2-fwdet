#ifndef HTOFRAW_H
#define HTOFRAW_H
#pragma interface

#include "hlocateddataobject.h"

class HTofRaw : public HLocatedDataObject {
protected:
  Float_t  rightT; //TDC: right hit
  Float_t  leftT; //TDC: left hit
  Float_t  rightQ; //QDC: right hit
  Float_t  leftQ; //QDC: left hit
  Short_t  nHit; //number of hit
  Char_t  sector, module, cell; //Location
public:
  HTofRaw(void);
  ~HTofRaw(void);
  inline Float_t getRightTime(void);
  inline Float_t getLeftTime(void);
  inline Float_t getRightCharge(void);
  inline Float_t getLeftCharge(void);
  inline Short_t getNHit(void);
  Short_t getAddress(void) {return  sector;}
  Char_t getSector(void) {return  sector;}
  Char_t getModule(void) {return  module;}
  Char_t getCell(void) {return  cell;}  
  inline void setRightTime(Float_t arightT);
  inline void setLeftTime(Float_t aleftT);
  inline void setRightCharge(Float_t arightQ);
  inline void setLeftCharge(Float_t aleftQ);
  inline void setNHit(Short_t anHit); 
  void incNHit(void) {nHit++;} 
  inline Int_t getNLocationIndex(void);
  inline Int_t getLocationIndex(Int_t i);
  void setSector(Char_t s) { sector=s;}
  void setModule(Char_t m) { module=m;}
  void setCell(Char_t c) { cell=c;}
  void setAddress(Short_t add) { sector=add;}
  void doit(void);
  virtual void clear();
  ClassDef(HTofRaw,1) //TOF raw data class
};


inline Float_t HTofRaw::getRightTime(void) {
  return  rightT;
}

inline Float_t HTofRaw::getLeftTime(void) {
  return  leftT;
}

inline Float_t HTofRaw::getRightCharge(void) {
  return  rightQ;
}

inline Float_t HTofRaw::getLeftCharge(void) {
  return  leftQ;
}

inline Short_t HTofRaw::getNHit(void) {
  return  nHit;
}

inline void HTofRaw::setRightTime(Float_t arightT) {
  rightT=arightT;
}
  
inline void HTofRaw::setLeftTime(Float_t aleftT) {
   leftT=aleftT;
}

inline void HTofRaw::setRightCharge(Float_t arightQ){
   rightQ=arightQ;
}

inline void HTofRaw::setLeftCharge(Float_t aleftQ) {
   leftQ=aleftQ;
}

inline void HTofRaw::setNHit(Short_t anHit){
   nHit=anHit;
}

inline Int_t HTofRaw::getNLocationIndex(void) {
  return 3;
}

inline Int_t HTofRaw::getLocationIndex(Int_t i) {
  switch (i) {
  case 0 : return getSector(); break;
  case 1 : return getModule(); break;
  case 2 : return getCell(); break;
  }
  return -1;
}

#endif /* !HTOFRAW_H */
