#include "hratreeext.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hlocateddataobject.h"
#include "hiterator.h"
#include "hdebug.h"
#include "TIterator.h"
#include "TArrayI.h"
#include "TClass.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 6/10/98 by Manuel Sanchez

HRaTreeExt::HRaTreeExt(void) {
   m_pNullObject = NULL;
}

HRaTreeExt::HRaTreeExt(HCategory *cat,TArrayI *sizes) :
    HRaTree(cat, sizes) {
 
    makeNullObject();
}


HRaTreeExt::HRaTreeExt(HCategory *cat,HLocation &loc,TArrayI *sizes) :
    HRaTree(cat, loc, sizes) {
 
    makeNullObject();
}

HRaTreeExt::~HRaTreeExt(void) {
  if (m_pNullObject) {
      delete m_pNullObject;
      m_pNullObject = NULL;
  }
}

TObject *HRaTreeExt::getObject(HLocation &aLoc) {
  //Warning: for the sake of speed no index checking is done here.
  TObject *r;

  r = HRaTree::getObject(aLoc);

  return (r) ? r : m_pNullObject;
}

TObject *HRaTreeExt::getObject(Int_t i1,Int_t i2,Int_t i3,Int_t i4,
				  Int_t i5,Int_t i6,Int_t i7,Int_t i8,
					  Int_t i9) {

  TObject *r;

  r = HRaTree::getObject(i1, i2, i3, i4, i5, i6, i7, i8, i9);

  return (r) ? r : m_pNullObject;
}

void HRaTreeExt::makeNullObject() {
      m_pNullObject = (TObject*)fSourceCategory->getClass()->New();
}

ClassImp(HRaTreeExt)
