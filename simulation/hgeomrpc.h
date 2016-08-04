#ifndef HGEOMRPC_H
#define HGEOMRPC_H

#include "hgeomset.h"

class  HGeomRpc : public HGeomSet {
protected:
  Char_t modName[6];  // name of module
  Char_t eleName[2];  // substring for elements in module
public:
  HGeomRpc();
  ~HGeomRpc() {}
  const Char_t* getModuleName(Int_t) {return modName;}
  const Char_t* getEleName(Int_t) {return eleName;}
  Int_t getSecNumInMod(const TString&);
  ClassDef(HGeomRpc,0) // Class for RPC
};

inline Int_t HGeomRpc::getSecNumInMod(const TString& name) {
  // returns the sector index retrieved from EBOXx
  return (Int_t)(name[4]-'0')-1;
}
#endif  /* !HGEOMRPC_H */
