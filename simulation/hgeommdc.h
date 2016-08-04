#ifndef HGEOMMDC_H
#define HGEOMMDC_H

#include "hgeomset.h"
#include "TString.h"

class HGeomBuilder;
class HGeomMedia;
class HGeomMdcWire;

class  HGeomMdc : public HGeomSet {
protected:
  Char_t modName[6];                 // name of module
  Char_t eleName[3];                 // substring for elements in module
  Char_t wnbuf[36];                  // character array to generate the wire names
  Int_t wn0 , wn1 , wn2 , wn3;       // indexes in character array
  vector<HGeomMdcWire*> wireObjects; // temporary working array for creation of cathode wires
public:
  HGeomMdc();
  ~HGeomMdc() {}
  const Char_t* getModuleName(Int_t);
  const Char_t* getEleName(Int_t);
  inline Int_t getSecNumInMod(const TString&);
  inline Int_t getModNumInMod(const TString&);
protected:
  Bool_t createAdditionalGeometry(HGeomBuilder*, const TString&, HGeomMedia*);
  void generateWireName(TString& vName);
  void clearWireObjects();
  Int_t addWireObject(Int_t, TString&, Int_t, Int_t, Float_t, Double_t, Double_t, Double_t,  Double_t);
  ClassDef(HGeomMdc,0) // Class for geometry of Mdc
};

#endif  /* !HGEOMMDC_H */

inline Int_t HGeomMdc::getSecNumInMod(const TString& name) {
  // returns the sector index retrieved from DRxMx
  return (Int_t)(name[4]-'0')-1;
}

inline Int_t HGeomMdc::getModNumInMod(const TString& name) {
  // returns the module index retrieved from DRxMx
  return (Int_t)(name[2]-'0')-1;
}
