//*-- AUTHOR : Ilse Koenig
//*-- Created : 25/05/2010 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////
// HParOra2Set
//
// Class used in Oracle interfaces to keep the context id and the validity
// time range
//
//////////////////////////////////////////////////////////////////////////

#include "hparora2set.h"

ClassImp(HParOra2Set)

HParOra2Set::HParOra2Set(const Char_t* pName) {
  SetName(pName);
  contextId=-1;
  clearVersDate();
}

void HParOra2Set::clearVersDate() {
  versDate[0]=-1;
  versDate[1]=1.E+12;
}
