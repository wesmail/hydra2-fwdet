#ifndef HGEOMFWDETHIT_H
#define HGEOMFWDETHIT_H

#include "hgeomhit.h"

class HGeomFwDet;

class HGeomFwDetHit : public HGeomHit {
public:
    HGeomFwDetHit(HGeomFwDet* p = 0);
    ~HGeomFwDetHit() {}
    Int_t getIdType();
    ClassDef(HGeomFwDetHit, 0) // Class for the GEANT hit definition of the new forward detector
};

#endif  /* !HGEOMFWDETHIT_H */
