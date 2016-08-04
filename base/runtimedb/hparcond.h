#ifndef HPARCOND_H
#define HPARCOND_H

#include "hparset.h"

class HParamList;

class HParCond : public HParSet {
public:
  HParCond(const Char_t* name,const Char_t* title,const Char_t* context)
         : HParSet(name,title,context) {}
  virtual ~HParCond() {}
  virtual void putParams(HParamList*)=0;
  virtual Bool_t getParams(HParamList*)=0;
  virtual Bool_t init(HParIo*,Int_t*);
  virtual Int_t write(HParIo*);
  virtual void printParams();
protected:
  HParCond() {};
  ClassDef(HParCond,1) // Base class for condition containers
};

#endif  /* !HPARCOND_H */
