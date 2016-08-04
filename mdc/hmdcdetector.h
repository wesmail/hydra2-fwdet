#ifndef HMDCDETECTOR_H
#define HMDCDETECTOR_H

#include "hdetector.h"
#include "haddef.h"
#include "TArrayI.h"

class HMdcDetector : public HDetector {
public:
  HMdcDetector();
  ~HMdcDetector();

  Bool_t init(void);
  void activateParIo(HParIo* io);
  Bool_t write(HParIo* io);
  HCategory* buildLinearCategory(const Text_t* className);
  HCategory* buildMatrixCategory(const Text_t* className,Float_t fillRate);
  HCategory* buildCategory(Cat_t cat);

  ClassDef(HMdcDetector,1) // Mdc detector class
};

#endif  /* !HMDCDETECTOR_H */
                                                            
