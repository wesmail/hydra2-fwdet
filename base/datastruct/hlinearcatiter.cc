#include "hlinearcatiter.h"
#include "hlinearcategory.h"
#include "TClass.h"

#include "TClonesArray.h"
#include "TIterator.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 9/11/1998 by Manuel Sanchez

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////
//HLinearCatIter
//
// This is a iterator for a HLinearCategory
///////////////////////////////////////

HLinearCatIter::HLinearCatIter(void) {
  //Private constructor
}

HLinearCatIter::HLinearCatIter(HLinearCategory *cat,Bool_t dir) {
  //Constructor. "cat" is the category being iterated and "dir" is the
  //iteration direction.
  fArray=cat->fData;
  fDir=dir;
  fCurrentLoc.set(1,0);
  fIterator=fArray->MakeIterator(dir);
  fLocated=fLast=kFALSE;
}

HLinearCatIter::~HLinearCatIter(void) {
  //Destructor
  if (fIterator) delete fIterator;
}

TCollection *HLinearCatIter::GetCollection(void) const {
  return fArray;
}

TObject *HLinearCatIter::Next(void) {
  //Advances to next object in the category and returns the current one.
  //warning "HLinearCatIter::Next needs optimization..."
  if (fLocated && fLast) {
    fCurrent=NULL;
  } else {
    fCurrent=fIterator->Next();
    fLast=kTRUE;
  }
  return fCurrent;
}

void HLinearCatIter::Reset(void) {
  //Resets the iterator
  fLocated=fLast=kFALSE;
  fIterator->Reset();
}

Bool_t HLinearCatIter::gotoLocation(HLocation &loc) {
  if (loc.getNIndex()==0) {
    Reset();
  } else {
    fLocated=kTRUE; fLast=kFALSE;
    fCurrent=fArray->At(loc[0]);
    if (!fCurrent) return kFALSE;
  }
  return kTRUE;
}

HLocation &HLinearCatIter::getLocation(void) {
  fCurrentLoc[0]=fArray->IndexOf(fCurrent);
  return fCurrentLoc;
}


ClassImp(HLinearCatIter)


