#ifndef HTOFRAWSIMFILTER_H
#define HTOFRAWSIMFILTER_H

#include "hfilter.h"
#include "htofrawsim.h"

class HTofRawSimFilter : public HFilter{

 public:
  HTofRawSimFilter(void){;}
  ~HTofRawSimFilter(void){;}
  Bool_t check(TObject* raw);

ClassDef(HTofRawSimFilter,1)   // Filter class for Tof digitizer
};
#endif
