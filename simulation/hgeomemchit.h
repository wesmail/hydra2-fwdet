#ifndef HGEOMEMCHIT_H
#define HGEOMEMCHIT_H

#include "hgeomhit.h"

class HGeomEmc;

class HGeomEmcHit : public HGeomHit {
public:
  HGeomEmcHit(HGeomEmc* p=0);
  ~HGeomEmcHit() {}
  Int_t getIdType();
  ClassDef(HGeomEmcHit,0) // Class for EMC hit definition in GEANT
};

#endif  /* !HGEOMEMCHIT_H */
