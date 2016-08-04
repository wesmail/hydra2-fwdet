#ifndef HGEOMSHOWER_H
#define HGEOMSHOWER_H

#include "hgeomset.h"
#include "TString.h"

class  HGeomShower : public HGeomSet {
protected:
  Char_t keepinName[5]; // name of Shower keepin volume
  Char_t modName[6];    // name of module
  Char_t eleName[3];    // substring for elements in module
public:
  HGeomShower();
  ~HGeomShower() {}
  const Char_t* getKeepinName(Int_t,Int_t);
  const Char_t* getModuleName(Int_t);
  const Char_t* getEleName(Int_t);
  inline Int_t getSecNumInMod(const TString&);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(HGeomShower,0) // Class for geometry of Shower
};

#endif  /* !HGEOMSHOWER_H */

inline Int_t HGeomShower::getSecNumInMod(const TString& name) {
  // returns the sector index retrieved from SHxMx
  return (Int_t)(name[4]-'0')-1;
}

inline Int_t HGeomShower::getModNumInMod(const TString& name) {
  // returns the sector index retrieved from SHxMx
  return (Int_t)(name[2]-'0')-1;
}
