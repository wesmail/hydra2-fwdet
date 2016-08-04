#ifndef HGEOMRICH_H
#define HGEOMRICH_H

#include "hgeomset.h"

class  HGeomRich : public HGeomSet {
protected:
  Char_t modName[5];  // name of module
  Char_t eleName[2];  // substring for elements in module
public:
  HGeomRich();
  ~HGeomRich() {}
  const Char_t* getModuleName(Int_t) {return modName;}
  const Char_t* getEleName(Int_t) {return eleName;}
  Bool_t read(fstream&,HGeomMedia*);
  void addRefNodes();
  ClassDef(HGeomRich,0) // Class for Rich
};

#endif  /* !HGEOMRICH_H */
