#ifndef HSHOWERWIRETAB_H
#define HSHOWERWIRETAB_H

#include "TObject.h"
#include "TArrayF.h"

class HParamList;

class HShowerWireTab : public TObject{
private:
  Int_t m_nModuleID; // module identifiction
  Float_t fDistWire; // distance between wires
  Int_t nWires;      // number of wires
  TArrayF wiresArr;  // wire positions
public:
  HShowerWireTab();
  virtual ~HShowerWireTab() {}

  void   reset();
  void   putParams(HParamList*);
  Bool_t getParams(HParamList*);

  void setDistWire(Float_t fDist) {fDistWire = fDist;}
  Float_t getDistWire() {return fDistWire;}

  Float_t getWirePos(Int_t n) {return wiresArr.At(n);}
  void setWirePos(Int_t n, Float_t y) {wiresArr.AddAt(y,n);}

  Int_t getNWires() {return nWires;}

  Int_t lookupWire(Float_t y);

  void setModuleID(Int_t id) {m_nModuleID = id;}
  Int_t getModuleID() {return m_nModuleID;}

  ClassDef(HShowerWireTab, 1)
};


#endif /* !HSHOWERWIRETAB_H */
