#ifndef HITERATOR_H
#define HITERATOR_H

#include "TObject.h"
#include "TIterator.h"

class HLocation;

class HIterator : public TIterator {
protected:
  HIterator(void) {}
public:
  ~HIterator(void);
  virtual Bool_t gotoLocation(HLocation &aLoc)=0;
  virtual HLocation &getLocation(void)=0;
  ClassDef(HIterator,1) //Base iterator for categories
};

#endif /* !HITERATOR_H */
