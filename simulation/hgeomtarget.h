#ifndef HGEOMTARGET_H
#define HGEOMTARGET_H

#include "hgeomset.h"

class  HGeomTarget : public HGeomSet {
protected:
  Char_t modName[2];  // name of module
  Char_t eleName[2];  // substring for elements in module
public:
  HGeomTarget();
  ~HGeomTarget() {}
  const Char_t* getModuleName(Int_t) {return modName;}
  const Char_t* getEleName(Int_t) {return eleName;}
  ClassDef(HGeomTarget,0) // Class for geometry of Target
};

#endif  /* !HGEOMTARGET_H */
