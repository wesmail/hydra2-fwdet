#ifndef HTOFDETECTOR_H
#define HTOFDETECTOR_H

#include "hdetector.h" 
#include "haddef.h"
#include "TArrayI.h"

class HTofDetector : public HDetector {
protected:

public:
  HTofDetector(void);
  ~HTofDetector(void);
  HTask *buildTask(const Text_t name[],const Option_t *opt="");
  HCategory *buildCategory(Cat_t cat);
  HCategory *buildMatrixCategory(const Text_t *classname,Float_t fillRate);
  Bool_t init(void) {return kTRUE;}
  void activateParIo(HParIo* io);
  Bool_t write(HParIo* io);
  
  ClassDef(HTofDetector,1) // TOF detector class
};


#endif /* !HTOFDETECTOR_H */









