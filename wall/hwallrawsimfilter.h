#ifndef HWALLRAWSIMFILTER_H
#define HWALLRAWSIMFILTER_H

#include "hfilter.h"
#include "hwallrawsim.h"

class HWallRawSimFilter : public HFilter{

 public:
  HWallRawSimFilter(void){;}
  ~HWallRawSimFilter(void){;}
  Bool_t check(TObject* raw);

ClassDef(HWallRawSimFilter,1)   // Filter class for Wall digitizer
};
#endif
