#ifndef HPIONTRACKERDETECTOR_H
#define HPIONTRACKERDETECTOR_H

#include "hdetector.h"
#include "haddef.h"

class HPionTrackerDetector : public HDetector {
public:
   HPionTrackerDetector();
   ~HPionTrackerDetector();
   Bool_t init(void);
   Bool_t write(HParIo* io);
   void   activateParIo(HParIo* io);
   HCategory* buildMatrixCategory(const Text_t* name, Float_t fill = 1.0);
   HCategory* buildLinearCategory(const Text_t* name);
   HCategory* buildCategory(Cat_t);
   Int_t getMaxModInSetup(void);

   ClassDef(HPionTrackerDetector, 1) // Detector class for Pion Tracker
};

#endif  /* !HPIONTRACKERDETECTOR_H */















