#ifndef HGEOMSTART_H
#define HGEOMSTART_H

#include "hgeomset.h"

class  HGeomStart : public HGeomSet {
protected:
  Char_t modName[2];  // name of module
  Char_t eleName[2];  // substring for elements in module
public:
  HGeomStart();
  ~HGeomStart() {}
  const Char_t* getModuleName(Int_t) {return modName;}
  const Char_t* getEleName(Int_t) {return eleName;}
  ClassDef(HGeomStart,0) // Class for geometry of Start/Veto detector
};

#endif  /* !HGEOMSTART_H */
