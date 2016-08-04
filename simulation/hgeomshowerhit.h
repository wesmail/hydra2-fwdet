#ifndef HGEOMSHOWERHIT_H
#define HGEOMSHOWERHIT_H

#include "hgeomhit.h"

class HGeomShower;

class HGeomShowerHit : public HGeomHit {
public:
  HGeomShowerHit(HGeomShower* p=0);
  ~HGeomShowerHit() {}
  Int_t getIdType();
  ClassDef(HGeomShowerHit,0) // Class for SHOWER hit definition in GEANT
};

#endif  /* !HGEOMSHOWERHIT_H */
