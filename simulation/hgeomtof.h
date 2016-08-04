#ifndef HGEOMTOF_H
#define HGEOMTOF_H

#include "hgeomset.h"
#include "TString.h"

class  HGeomTof : public HGeomSet {
protected:
  Char_t modName[6];    // name of module
  Char_t eleName[4];    // substring for elements in module
public:
  HGeomTof();
  ~HGeomTof() {}
  const Char_t* getModuleName(Int_t);
  const Char_t* getEleName(Int_t);
  inline Int_t getSecNumInMod(const TString&);
  inline Int_t getModNumInMod(const TString&);
  ClassDef(HGeomTof,0) // Class for geometry of TOF
};

#endif  /* !HGEOMTOF_H */

inline Int_t HGeomTof::getSecNumInMod(const TString& name) {
  // returns the sector index retrieved from TxxFx
  return (Int_t)(name[4]-'0')-1;
}

inline Int_t HGeomTof::getModNumInMod(const TString& name) {
  // returns the module index retrieved from TxxFx
  return (Int_t)(name[1]-'0')*10+(Int_t)(name[2]-'0')-1;
}
