#ifndef HGEOMUSER_H
#define HGEOMUSER_H

#include "hgeomset.h"
#include "TString.h"

class  HGeomUser : public HGeomSet {
protected:
  Char_t modName[6];  // name of module
  Char_t eleName[3];  // substring for elements in module
public:
  HGeomUser();
  ~HGeomUser() {}
  const Char_t* getModuleName(Int_t);
  const Char_t* getEleName(Int_t);
  inline Int_t getSecNumInMod(const TString&);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(HGeomUser,0) // Class for user defined detector
};

#endif  /* !HGEOMUSER_H */

inline Int_t HGeomUser::getSecNumInMod(const TString& name) {
  // returns the sector index retrieved from UxKIx
  if (name.Length()>4) return (Int_t)(name[4]-'0')-1;
  else return -1;
}

inline Int_t HGeomUser::getModNumInMod(const TString& name) {
  // returns the sector index retrieved from UxKIx
  return (Int_t)(name[1]-'0')-1;
}
