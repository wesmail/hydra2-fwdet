#ifndef HLOCATEDDATAOBJECT_H
#define HLOCATEDDATAOBJECT_H

#include "TObject.h"
#include <hlocation.h>

class HLocatedDataObject : public TObject {
 public:
  HLocatedDataObject(void) {}
  ~HLocatedDataObject(void) {}
  virtual Int_t getNLocationIndex(void) {return 0;}
  virtual Int_t getLocationIndex(Int_t n) {return 0;}
  virtual HLocation* getLocation() {return NULL;};
    
  ClassDef(HLocatedDataObject,2) //Data object with localization data
};

#endif /* !HLOCATEDDATAOBJECT_H */
