#ifndef HGEOMMDCHIT_H
#define HGEOMMDCHIT_H

#include "hgeomhit.h"

class HGeomMdc;

class HGeomMdcHit : public HGeomHit {
public:
  HGeomMdcHit(HGeomMdc* p=0);
  ~HGeomMdcHit() {}
  Int_t getIdType();
  ClassDef(HGeomMdcHit,0) // Class for MDC hit definition in GEANT
};

#endif  /* !HGEOMMDCHIT_H */
