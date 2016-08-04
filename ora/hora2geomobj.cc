//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/05/2010 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HOra2GeomObj
//
// Class used in Oracle interfaces for geometry parameter containers to keep
// internal identifiers for geometry objects
//
// The variable volType defines the type of volume:
//   M: Module 
//   R: Reference module
//   C: inner Component
//   I: inner copy Component
// The volume points are only read for types R and C
//////////////////////////////////////////////////////////////////////////////

#include "hora2geomobj.h"

ClassImp(HOra2GeomObj)

HOra2GeomObj::HOra2GeomObj(const Char_t* pName="", TObject* p=0,
                           Char_t vType='M', TObject* r=0) {
  SetName(pName);
  pObj=p;
  volType=vType;
  refObj=r;
  objId=-1;
  volId=-1;
  transId=-1;
}
