#ifndef HGEOMTOFHIT_H
#define HGEOMTOFHIT_H

#include "hgeomhit.h"

class HGeomTof;

class HGeomTofHit : public HGeomHit {
public:
  HGeomTofHit(HGeomTof* p=0);
  ~HGeomTofHit() {}
  Int_t getIdType();
  ClassDef(HGeomTofHit,0) // Class for TOF/TOFINO hit definition in GEANT
};

#endif  /* !HGEOMTOFHIT_H */
