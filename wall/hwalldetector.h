#ifndef HWALLDETECTOR_H
#define HWALLDETECTOR_H

#include "hdetector.h" 

class HWallDetector : public HDetector {
public:
  HWallDetector(void);
  ~HWallDetector(void);
  HCategory* buildCategory(Cat_t);
  HCategory* buildLinearCategory(const Text_t*,Float_t);
  HCategory* buildMatrixCategory(const Text_t*,Float_t);
  Bool_t init(void) {return kTRUE;}
  void activateParIo(HParIo*);
  Bool_t write(HParIo*);
  Int_t getMaxModInSetup(void);
  
  ClassDef(HWallDetector,1) // Forward Wall detector class
};


#endif /* !HWALLDETECTOR_H */









