#ifndef HMATRIXCATITER_H
#define HMATRIXCATITER_H

#include "hiterator.h"
#include "TArrayI.h"
#include "hmatrixcategory.h"

class HMatrixCatIter : public HIterator {
private:
  HMatrixCatIter(void);
protected:
  HLocation fCurrentLoc; //Current location in the category
  HMatrixCategory *fCategory; //! Category being iterated
  TArrayI *fSizes; //! Sizes of the category
  Int_t fCursor; //Cursor for linear iteration
  Int_t fAntCursor; //Cursor of previous interation
  Bool_t fDir; //Iteration direction.
  Int_t fEnd; //Last possible value of fCursor
  Int_t fBegin; //First value of fCursor 
public:
  HMatrixCatIter(HMatrixCategory *cat,Bool_t dir=kIterForward);
  virtual ~HMatrixCatIter(void);
  TCollection *GetCollection(void) const;
  TObject *Next(void);
  void Reset(void);
  Bool_t gotoLocation(HLocation &loc);
  HLocation &getLocation(void);
  ClassDef(HMatrixCatIter,1) //Iterator for a HMatrixCategory
};

#endif /* !HMATRIXCATITER_H */
