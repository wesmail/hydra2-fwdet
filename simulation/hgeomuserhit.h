#ifndef HGEOMUSERHIT_H
#define HGEOMUSERHIT_H

#include "hgeomhit.h"

class HGeomUser;

class HGeomUserHit : public HGeomHit {
public:
  HGeomUserHit(HGeomUser* p=0);
  ~HGeomUserHit() {}
  Int_t getIdType();
  ClassDef(HGeomUserHit,0) // Class for hit definition in GEANT of user defined detector
};

#endif  /* !HGEOMUSERHIT_H */
