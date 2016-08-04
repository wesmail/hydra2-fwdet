#include "hlinkeddataobject.h"

//*-- Author : R. Holzmann
//*-- Modified : 18/11/1999

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HLinkedDataObject
//
// This is a data object which also stores the cat index of the next hit,
// e.g. with same track number. This allows to construct linked lists of hits.
//
// Int_t getNextHitIndex() returns the index of next hit.
// void setNextHitIndex(Int_t) sets it.
// 
//////////////////////////////////////////////////////////////////////////////

HLinkedDataObject::HLinkedDataObject(void) {
  nextHit = -1;  // index of next hit in category (-1 if none)
  sortVariable = 0.0;  // sort on this one
}

HLinkedDataObject::~HLinkedDataObject(void) {
}

ClassImp(HLinkedDataObject)
