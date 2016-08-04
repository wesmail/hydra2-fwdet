#ifndef HGEOMFRAMES_H
#define HGEOMFRAMES_H

#include "hgeomset.h"

class  HGeomFrames : public HGeomSet {
protected:
  Char_t modName[2];  // name of module
  Char_t eleName[2];  // substring for elements in module
public:
  HGeomFrames();
  ~HGeomFrames() {}
  const Char_t* getModuleName(Int_t) {return modName;}
  const Char_t* getEleName(Int_t) {return eleName;}
  ClassDef(HGeomFrames,0) // Class for geometry of support structure
};

#endif  /* !HGEOMFRAMES_H */
