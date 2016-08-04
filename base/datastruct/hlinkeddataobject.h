#ifndef HLINKEDDATAOBJECT_H
#define HLINKEDDATAOBJECT_H

#include "hlocateddataobject.h"

class HLinkedDataObject : public HLocatedDataObject {
 public:
  Float_t sortVariable;  //! used to sort linked objects (non-persistent!)
 protected:
  Short_t nextHit;  // index of next hit object in cat (-1 if none)
 public:

  HLinkedDataObject(void);
  ~HLinkedDataObject(void);
  void setNextHitIndex(Int_t next) {nextHit = (Short_t)next;}
  Int_t getNextHitIndex(void) {return (Int_t)nextHit;}
  virtual Int_t getTrack() {return 0;}
  ClassDef(HLinkedDataObject,1)  //Data object with link index
};

#endif /* !HLINKEDDATAOBJECT_H */
