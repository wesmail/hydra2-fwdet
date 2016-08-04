#ifndef HORA2GEOMOBJ_H
#define HORA2GEOMOBJ_H

#include "TNamed.h"

class HOra2GeomObj : public TNamed {
public:
  TObject* pObj;            // pointer to object of type HModGeomPar or HGeomVolume
  Char_t   volType;         // M: Module, R: Reference module, C: inner Component
  TObject* refObj;          // pointer to the mother volume of a component
  Int_t    objId;           // id of the geometry volume in Oracle
  Int_t    volId;           // id of the volume points
  Int_t    transId;         // id of the volume transformation
  HOra2GeomObj(const Char_t*, TObject*, Char_t, TObject*);
  ~HOra2GeomObj() {}
  ClassDef(HOra2GeomObj,0) // Oracle interface storage class for geometry objects
};

#endif  /* !HORA2GEOMOBJ_H */
