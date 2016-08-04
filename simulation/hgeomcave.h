#ifndef HGEOMCAVE_H
#define HGEOMCAVE_H

#include "hgeomset.h"
#include "TString.h"

class HGeomMedia;

class  HGeomCave : public HGeomSet {
protected:
  TString name;
public:
  HGeomCave();
  ~HGeomCave() {}
  const Char_t* getModuleName(Int_t) {return name.Data();}  
  Bool_t read(fstream&,HGeomMedia*);
  void addRefNodes();
  void write(fstream&);
  void print();
  ClassDef(HGeomCave,0) // Class for the geometry of CAVE
};

#endif  /* !HGEOMCAVE_H */
