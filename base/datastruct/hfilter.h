#ifndef HFILTER_H
#define HFILTER_H

#include "TObject.h"

class HFilter : public TObject {
public:
  HFilter(void);
  ~HFilter(void);
  virtual Bool_t check(TObject *obj)=0;
  ClassDef(HFilter,1) //Base class for data filters
};

#endif /* !HFILTER */
