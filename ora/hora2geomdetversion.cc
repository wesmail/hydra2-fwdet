//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/05/2010 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HOra2GeomDetVersion
//
// Utility class for geometry version management in Oracle
//   Hydra2 interface
// (uses the Oracle C/C++ precompiler)
//
/////////////////////////////////////////////////////////////

#include "hora2geomdetversion.h"

ClassImp(HOra2GeomDetVersion)

HOra2GeomDetVersion::HOra2GeomDetVersion(const Char_t* name,Int_t id) {
  SetName(name);
  detectorId=id;
  geomVersion=-1;
  clearVersDate();
}

void HOra2GeomDetVersion::clearVersDate() {
  versDate[0]=-1;
  versDate[1]=-1;
}

void HOra2GeomDetVersion::fill(Int_t id,Int_t v,Double_t s,Double_t u) {
  detectorId=id;
  geomVersion=v;
  versDate[0]=s;
  versDate[1]=u;
}

void HOra2GeomDetVersion::fill(Int_t v,Double_t s,Double_t u) {
  geomVersion=v;
  versDate[0]=s;
  versDate[1]=u;
}
