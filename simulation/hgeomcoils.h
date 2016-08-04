#ifndef HGEOMCOILS_H
#define HGEOMCOILS_H

#include "hgeomset.h"
#include "TString.h"

class  HGeomCoils : public HGeomSet {
protected:
  Char_t modName[6];  // name of module
  Char_t eleName[2];  // substring for elements in module
public:
  HGeomCoils();
  ~HGeomCoils() {}
  const Char_t* getModuleName(Int_t) {return modName;}
  const Char_t* getEleName(Int_t) {return eleName;}
  Int_t getSecNumInMod(const TString& name) {return (Int_t)(name[4]-'0')-1;}
  ClassDef(HGeomCoils,0) // Class for the geometry of Magnet
};

#endif  /* !HGEOMCOILS_H */
