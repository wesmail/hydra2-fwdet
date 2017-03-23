#ifndef HFWDETDETECTOR_H
#define HFWDETDETECTOR_H

#include "hdetector.h"

class HFwDetDetector : public HDetector
{
public:
    HFwDetDetector();
    virtual ~HFwDetDetector();

    void   activateParIo(HParIo* io);
    Bool_t write(HParIo* io);

    HCategory* buildCategory(Cat_t cat, Bool_t simulation = kFALSE);
    HCategory* buildMatrixCategory(const Text_t *, Int_t, Int_t, Float_t fillRate = 0.5);
    HCategory* buildMatrixCategory(const Text_t *, Int_t, Int_t, Int_t, Int_t, Float_t fillRate = 0.5);
    HCategory* buildMatrixCategory(const Text_t *, Int_t, Int_t, Int_t, Int_t, Int_t, Float_t fillRate = 0.5);
    HCategory* buildLinearCategory(const Text_t*, Int_t);

    Int_t getMaxModInSetup();

    ClassDef(HFwDetDetector, 0); // detector class for new forward detector
};

#endif /* !HFWDETDETECTOR_H */
