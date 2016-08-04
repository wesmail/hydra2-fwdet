#ifndef __HRaTreeExt_H
#define __HRaTreeExt_H

#include "TObject.h"
#include "TObjArray.h"
#include "hlocation.h"
#include "hratree.h"

class HCategory;
class HLocatedDataObject;
class HIterator;
class TArrayI;

class HRaTreeExt : public HRaTree{
 public:
  HRaTreeExt(void);
  HRaTreeExt(HCategory *cat,TArrayI *sizes);
  HRaTreeExt(HCategory *cat,HLocation &aLoc,TArrayI *sizes);
  ~HRaTreeExt(void);

  TObject *getObject(HLocation &aLoc);
  TObject *getObject(Int_t i1=-1,Int_t i2=-1,Int_t i3=-1,Int_t i4=-1,
		     Int_t i5=-1,Int_t i6=-1,Int_t i7=-1,Int_t i8=-1,
		     Int_t i9=-1);


  void makeNullObject();  

  ClassDef(HRaTreeExt,1) //Random acces in a tree fashion.

private:
  TObject* m_pNullObject;
};

#endif
