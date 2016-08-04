//*-- Author : Manuel Sanchez
//*-- Modified : 2/04/98
//*-- Modified : 29/10/99 R. Holzmann
//*-- Copyright : GENP

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////
//HMatrixCatIter
//
//   This is a HIterator for HMatrixCategory.
//
////////////////////////////////////////////
using namespace std;
#include "hmatrixcatiter.h"
#include "hdebug.h"
#include <iostream> 
#include <iomanip>

ClassImp(HMatrixCatIter) //;

  HMatrixCatIter::HMatrixCatIter(void) {
    //Default constructor. Never call
    fCategory=NULL;
    fSizes=NULL;
    fCursor=0;
    fDir=kIterForward;
    //    Reset();
}

HMatrixCatIter::HMatrixCatIter(HMatrixCategory *cat,Bool_t dir) {
  //Allocates a new HIterator pointing to the category cat and iterating in the direction
  //dir
  fCategory=cat;
  fSizes=cat->fIndexTable->getDimensions();
  fCursor=0;
  fDir=dir;
  //  Reset();
}

HMatrixCatIter::~HMatrixCatIter(void) {
  //Destructor
}

TCollection *HMatrixCatIter::GetCollection(void) const {
  //Returns the internal collection where the category stores data
  return fCategory->fData;
}

TObject *HMatrixCatIter::Next(void) {
  //Advances to next object and returns a pointer to it.
  //Returns NULL when reaches the end
  TObject *r=NULL;
  Int_t i;
  if (fDir==kIterForward) {
    if (fCursor < fEnd ) {
      r=fCategory->fData->At(fCategory->fIndexTable->getIndex(fCursor));
      fAntCursor=fCursor;
      do {
	fCursor++;
	//gDebuger->message("cursor=%i end=%i",fCursor,fEnd);
      } while ( (fCursor<fEnd) && 
		(i=fCategory->fIndexTable->getIndex(fCursor))<0);
    } else r=NULL;
  } else {
    if (fCursor > fBegin ) {
      r=fCategory->fData->At(fCategory->fIndexTable->getIndex(fCursor));
      fAntCursor=fCursor;
      do {
	fCursor--;
      } while ( (fCursor>fBegin) && 
		(i=fCategory->fIndexTable->getIndex(fCursor))<0);
    } else r=NULL;
  }
  return r;
}

void HMatrixCatIter::Reset(void) {
  //Points the iterator to the first object in the category
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HMatrixCatIter::Reset");
#endif
  Int_t i=0;
  fSizes=fCategory->fIndexTable->getDimensions(); // ptr fSizes changes on read
  if (fDir==kIterForward) {
    fCursor=0;
    fBegin=0;
    fEnd=fCategory->fIndexTable->getEntries();
    fCurrentLoc.setNIndex(fSizes->fN);
    for (i=0;i<fSizes->fN;i++) fCurrentLoc[i]=0;

    while ( (fCursor<fEnd) && 
	    (i=fCategory->fIndexTable->getIndex(fCursor))<0) {
      fCursor++;
    }
    fBegin=fAntCursor=fCursor;
  } else {
    Int_t i;
    fCursor=fCategory->fIndexTable->getEntries()-1;
    fBegin=fCursor;
    fEnd=-1;
    fCurrentLoc.setNIndex(fSizes->GetSize());
    for (i=0;i<fSizes->GetSize();i++) {
      fCurrentLoc.setIndex(i,fSizes->At(i)-1);
    }
    while ( (fCursor>fBegin) && 
	      (i=fCategory->fIndexTable->getIndex(fCursor))<0) {
      fCursor--;
    }
    fBegin=fAntCursor=fCursor;
  }
#if DEBUG_LEVEL>2
  gDebuger->leaveFunc("HMatrixCatIter::Reset");
#endif
}

Bool_t HMatrixCatIter::gotoLocation(HLocation &loc) {
  //Points the iterator to the object given in the location loc
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HMatrixCatIter::gotoLocation");
#endif
  Int_t i=0;
  if (loc.getNIndex()==0) {
    Reset();
  } else {
    fCurrentLoc.readIndexes(loc);
    fCursor=fCurrentLoc.getLinearIndex(fSizes);
    fBegin=fCursor;
    ++loc;
    fEnd=loc.getLinearIndex(fSizes);
    if (fEnd>fCategory->fIndexTable->getEntries()) {
	fEnd=fCategory->fIndexTable->getEntries();
    }
    --loc;
    if (fCursor>=fCategory->fIndexTable->getEntries()) return kFALSE;
    if (fDir==kIterForward) {
      fCursor=fBegin;
      while ( (fCursor<fEnd) && 
	      (i=fCategory->fIndexTable->getIndex(fCursor))<0) {
	fCursor++;
      } 
      fBegin=fAntCursor=fCursor;
    } else {
      fCursor=fEnd;
      while ( (fCursor>fBegin) && 
	      (i=fCategory->fIndexTable->getIndex(fCursor))<0) {
	fCursor--;
      }
      fEnd=fAntCursor=fCursor;
    }
  }
  return kTRUE;
}

HLocation &HMatrixCatIter::getLocation(void) {
  //Gets the location of the currently pointed object.
  Int_t prod=1;
  Int_t i=0;
  for (i=fCurrentLoc.getNIndex()-1;i>-1;i--) {
    fCurrentLoc[i]=(fAntCursor/prod) % fSizes->fArray[i];
    prod*=fSizes->fArray[i];
  }
  return fCurrentLoc;
}
  
