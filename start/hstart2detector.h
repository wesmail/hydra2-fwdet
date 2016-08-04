#ifndef HSTART2DETECTOR_H
#define HSTART2DETECTOR_H

#include "haddef.h"
#include "hdetector.h"
#include "hstartdef.h"

class HParIo;

class HStart2Detector : public HDetector {
public:
   HStart2Detector();
   ~HStart2Detector();
   Bool_t init(void);
   void activateParIo(HParIo* io);
   Bool_t write(HParIo* io);
   HCategory* buildMatrixCategory(const Text_t* name, Float_t fill = 1.0);
   HCategory* buildLinearCategory(const Text_t* name);
   HCategory* buildCategory(Cat_t);
   Int_t getMaxModInSetup(void);

   ClassDef(HStart2Detector, 2) // Start2 detector using TRB for readout
};

#endif  /* !HSTART2DETECTOR_H */















