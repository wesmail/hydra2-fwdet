#ifndef HGEOMFWALLHIT_H
#define HGEOMFWALLHIT_H

#include "hgeomhit.h"

class HGeomFWall;

class HGeomFWallHit : public HGeomHit {
public:
  HGeomFWallHit(HGeomFWall* p=0);
  ~HGeomFWallHit() {}
  Int_t getIdType();
  ClassDef(HGeomFWallHit,0) // Class for hit definition of Forward Wall in GEANT
};

#endif  /* !HGEOMFWALLHIT_H */
