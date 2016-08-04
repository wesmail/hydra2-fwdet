#ifndef HGEOMSTARTHIT_H
#define HGEOMSTARTHIT_H

#include "hgeomhit.h"

class HGeomStart;

class HGeomStartHit : public HGeomHit {
public:
  HGeomStartHit(HGeomStart* p=0);
  ~HGeomStartHit() {}
  Int_t getIdType();
  ClassDef(HGeomStartHit,0) // Class for START hit definition in GEANT
};

#endif  /* !HGEOMSTARTHIT_H */
