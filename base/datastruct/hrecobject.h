#ifndef HRECOBJECT_H
#define HRECOBJECT_H

#include "TObject.h"
#include "TObjArray.h"
#include "haddef.h"
#include "hgeantdef.h"
#include "hlocation.h"

class HRecObject : public TObject
{
private:
public:
  HRecObject(void);
  ~HRecObject(void);
  HRecObject(HRecObject &aObj);
  ClassDef(HRecObject,1) // Data object obtained from other data objects
};

#endif /* !HRECOBJECT_H */
