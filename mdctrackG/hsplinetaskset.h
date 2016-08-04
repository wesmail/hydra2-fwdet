#ifndef HSPLINETASKSET_H
#define HSPLINETASKSET_H

#include "htaskset.h"

class HSplineTaskSet:public HTaskSet
{
 public:
  HSplineTaskSet();
  HSplineTaskSet(const Text_t name[],const Text_t title[]);
  ~HSplineTaskSet();
  HTask* make(const Char_t* sel="",const Option_t* opt="");
  
  void setSplinePar();
  
 private:
  void parseArguments(TString);
  static Bool_t isSpline;
  static Bool_t isClustering;
  static Bool_t isRunge;
  static Bool_t isRejectUnMatched;
  Bool_t isSplinePar;
  
  ClassDef(HSplineTaskSet,1)
};
#endif  /* ! HSPLINETASKSET_H */
