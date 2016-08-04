#ifndef HGEOMRPCHIT_H
#define HGEOMRPCHIT_H

#include "hgeomhit.h"

class HGeomRpc;

class HGeomRpcHit : public HGeomHit {
public:
  HGeomRpcHit(HGeomRpc* p=0);
  ~HGeomRpcHit() {}
  Int_t getIdType();
  ClassDef(HGeomRpcHit,0) // Class for RPC hit definition in GEANT
};

#endif  /* !HGEOMRPCHIT_H */
