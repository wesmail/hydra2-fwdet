#ifndef HGEOMEMC_H
#define HGEOMEMC_H

#include "hgeomset.h"

class  HGeomEmc : public HGeomSet {
protected:
  char modName[6];  // name of module
  char eleName[2];  // substring for elements in module
  
public:
  HGeomEmc();
  ~HGeomEmc() {}
  const char* getModuleName(Int_t) {return modName;}
  const char* getEleName(Int_t) {return eleName;}
  Int_t getSecNumInMod(const TString&);
  ClassDef(HGeomEmc,0) // Class for EMC
};

inline Int_t HGeomEmc::getSecNumInMod(const TString& name) {
  // returns the sector index retrieved from GMOMx
  return (Int_t)(name[4]-'0')-1;
}
#endif  /* !HGEOMEMC_H */
