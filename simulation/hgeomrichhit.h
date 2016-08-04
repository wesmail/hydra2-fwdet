#ifndef HGEOMRICHHIT_H
#define HGEOMRICHHIT_H

#include "hgeomhit.h"

class HGeomRich;

class HGeomRichHit : public HGeomHit {
public:
  HGeomRichHit(HGeomRich* p=0);
  ~HGeomRichHit() {}
  Int_t getIdType();
  ClassDef(HGeomRichHit,0) // Class for RICH hit definition in GEANT
};

#endif  /* !HGEOMRICHHIT_H */
