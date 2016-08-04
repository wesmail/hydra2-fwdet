#ifndef HLINEARCATITER_H
#define HLINEARCATITER_H

#include "hiterator.h"
#include "hlinearcategory.h"

class HLinearCatIter : public HIterator {
private:
  HLinearCatIter(void);
protected:
  HLocation fCurrentLoc; //Current location in the category
  TIterator *fIterator; //! Iterator
  TClonesArray *fArray; //array being iterated
  TObject *fCurrent; //Current object
  Bool_t fLast,fLocated; //Iteration finished?
  Bool_t fDir;
  
public:
  HLinearCatIter(HLinearCategory *cat,Bool_t dir=kIterForward);
  virtual ~HLinearCatIter(void);
  TCollection *GetCollection(void) const;
  TObject *Next(void);
  void Reset(void);
  Bool_t gotoLocation(HLocation &loc);
  HLocation &getLocation(void);
  ClassDef(HLinearCatIter,1) //Iterator for a HMatrixCategory
};

#endif /* !HLINEARCATITER_H */
