#ifndef HGEOMFWALL_H
#define HGEOMFWALL_H

#include "hgeomset.h"

class  HGeomFWall : public HGeomSet {
protected:
  Char_t modName[5];  // name of module
  Char_t eleName[2];  // substring for elements in module
public:
  HGeomFWall();
  ~HGeomFWall() {}
  const Char_t* getModuleName(Int_t) {return modName;}
  const Char_t* getEleName(Int_t) {return eleName;}
  ClassDef(HGeomFWall,0) // Class for Forward Wall
};

#endif  /* !HGEOMFWALL_H */
