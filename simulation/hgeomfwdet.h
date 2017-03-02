#ifndef HFWDET_H
#define HFWDET_H

#include "hgeomset.h"

class  HGeomFwDet : public HGeomSet {
protected:
  Char_t modName[5];  // name of module
  Char_t eleName[3];  // substring for elements in module
public:
  HGeomFwDet();
  ~HGeomFwDet() {}
  inline const Char_t* getModuleName(Int_t);
  inline const Char_t* getEleName(Int_t);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(HGeomFwDet,0) // Class for geometry of the new forward detector
};

#endif  /* !HFWDET_H */

inline const Char_t* HGeomFwDet::getModuleName(Int_t m) {
  // Returns the name of module m
  sprintf(modName,"AD%iM",m+1);
  return modName;
}

inline const Char_t* HGeomFwDet::getEleName(Int_t m) {
  // Returns the element name in module m
  sprintf(eleName,"A%i",m+1);
  return eleName;
}

inline Int_t HGeomFwDet::getModNumInMod(const TString& name) {
  // returns the module index retrieved from ADxM
  return (Int_t)(name[2]-'0')-1;
}
